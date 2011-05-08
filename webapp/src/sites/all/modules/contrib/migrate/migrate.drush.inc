<?php

/**
 * @file
 * Drush support for the migrate module
 */

/**
 * Implementation of hook_drush_help().
 */
function migrate_drush_help($section) {
  switch ($section) {
    case 'drush:migrate-clear':
      return dt('Clear a given migration content set');
    case 'drush:migrate-import':
      return dt('Import a given migration content set');
    case 'drush:migrate-stop':
      return dt('Stop an active operation');
    case 'drush:migrate-status':
      return dt('List all content sets with current status');
    case 'drush:migrate-wipe':
      return dt('Delete all nodes from specified content types.');
  }
}

/**
 * Implementation of hook_drush_command().
 */
function migrate_drush_command() {
  $migration_options = array(
    '--itemlimit' => 'The maximum number of items to migrate. If unspecified, all are migrated',
    '--feedback' => 'Frequency of progress messages, in seconds or items processed',
    '--idlist' => 'A comma delimited list of ids to import or clear. If unspecified, migrate imports all pending items or clears all items for the content set.',
    '--all' => 'Process all content sets',
  );
  $items['migrate-status'] = array(
    'description' => 'List all content sets with current status.',
  );
  $items['migrate-clear'] = array(
    'description' => 'Clear migrated data for the specified content set',
    'options' => $migration_options,
    // We will bootstrap to login from within the command callback.
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'arguments' => array(
      'content_set' => 'Name or mcsid of content set to clear',
    ),
    'examples' => array(
      'migrate-clear 4' => 'Clear the content set with mcsid 4',
      'migrate-clear 4 --idlist=4,9' => 'Clear two items in mcsid 4. The ids refer to the value of the primary key in base table',
      'migrate-clear Articles --itemlimit=50' =>
        'Clear up to 50 items from the content set named Articles',
      'migrate-clear Users --feedback="60 seconds"' => 'Display a progress message every 60 seconds or less',
      'migrate-clear 2 --feedback="1000 items"' => 'Display a progress message every 1000 processed items or less',
    ),
  );
  $migration_options['--update'] = 'In addition to processing unimported items from the source, update previously-imported items with new data';
  $items['migrate-import'] = array(
    'description' => 'Import a given migration content set',
    'options' => $migration_options,
    'arguments' => array(
      'content_set' => 'Name or mcsid of content set to import',
    ),
    'examples' => array(
      'migrate-import 4' => 'Import new items in the content set with mcsid 4',
      'migrate-import 4 --update' => 'Import new items, and also update previously-imported items',
      'migrate-import 4 --idlist=4,9' => 'Import two items in mcsid 4. The ids refer to the value of the primary key in base table',
      'migrate-import Articles --itemlimit=50' =>
        'Import up to 50 items from the content set named Articles',
      'migrate-import Users --feedback="60 seconds"' => 'Display a progress message every 60 seconds or less',
      'migrate-import 2 --feedback="1000 items"' => 'Display a progress message every 1000 processed items or less',
    ),
  );
  $items['migrate-stop'] = array(
    'description' => 'Stop an active migration operation',
    'options' => array('--all' => 'Stop all active migration operations'),
    'arguments' => array(
      'content_set' => 'Name or mcsid of content set to stop',
    ),
    'examples' => array(
      'migrate-stop Articles' => 'Stop any active operation on the Articles content set',
      'migrate-stop --all' => 'Stop all active migration operations',
    ),
  );

  $items['migrate-wipe'] = array(
    'description' => 'Delete all nodes from specified content types.',
    'examples' => array(
       "migrate-wipe story article" => 'Delete all story and article nodes.',
    ),
    'arguments' => array(
      'type' => 'A space delimited list of content type machine readable Ids.',
    ),
    // We will bootstrap to login from within the command callback.
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  return $items;
}

/**
 * Get the value of all migrate related options. Used when spawning a subshell.
 *
 * @return
 *   An array of command specific options and their values.
 */
function drush_migrate_get_options() {
  $options = array();
  $command = drush_parse_command();
  foreach ($command['options'] as $key => $value) {
    // Strip leading --
    $key = ltrim($key, '-');
    $value = drush_get_option($key);
    if (isset($value)) {
      $options[$key] = $value;
    }
  }
  return $options;
}

/*
 * Spawn a subshell which runs the same command we are currently running.
 */
function drush_migrate_backend_invoke() {
  $args = drush_get_arguments();
  $options = drush_migrate_get_options();
  // If we leave the update option in, it will reupdate everything
  unset($options['update']);
  // @todo: use drush_backend_invoke_args() as per http://drupal.org/node/658420.
  drush_backend_invoke(implode(' ', $args), $options);
}

/**
 * A simplified version of the Process (dashboard) page
 */
function drush_migrate_status() {
  $table = array();
  $table[] = array(dt('Status'), dt('Name'), dt('Total'), dt('Imported'), dt('Unimported'));
  $sql = "SELECT *
          FROM {migrate_content_sets}
          ORDER BY weight, contenttype, view_name, view_args";
  $result = db_query($sql);
  while ($row = db_fetch_object($result)) {
    if ($row->status == MIGRATE_STATUS_CLEARING) {
      $status = dt('Clearing');
    }
    elseif ($row->status == MIGRATE_STATUS_IMPORTING) {
      $status = dt('Importing');
    }
    else {
      $status = dt('');
    }
    $view = views_get_view($row->view_name);
    if (!$view) {
      drush_set_error(NULL, dt('View !view does not exist - either (re)create this view, or
        remove the content set using it.', array('!view' => $row->view_name)));
      continue;
    }
    $maptable = migrate_map_table_name($row->mcsid);
    $imported = db_result(db_query('SELECT COUNT(*) FROM {' . $maptable . '}'));
    // If not caching counts, override the saved count with a fresh count
    if (!variable_get('migrate_cache_counts', 0)) {
      $total = _migrate_get_view_count($view, $row->view_args);
    }
    else {
      $total = $row->rowcount;
    }

    $table[] = array($status, $row->description, $total, $imported, $total-$imported);
  }
  drush_print_table($table, TRUE);
}

/**
 * Clear one specified content set
 *
 * @param $content_set
 *  The mcsid or the name of the content set
 */
function drush_migrate_clear($content_set = NULL) {
  // node_delete() has silly perm requirements in d5/d6.
  drush_set_option('user', 1);
  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_LOGIN);

  drush_migrate_set_verbose();
  if (drush_get_option('all')) {
    $all = TRUE;
    if (isset($content_set)) {
      drush_log(dt('You must specify either a content set or --all, not both'), 'warning');
      return;
    }
  }
  else {
    $all = FALSE;
    if (!$content_set) {
      drush_log(dt('You must specify either a content set or the --all option'), 'warning');
      return;
    }
    if (is_numeric($content_set)) {
      $row = db_fetch_object(db_query("SELECT mcsid,description FROM {migrate_content_sets}
                                       WHERE mcsid=%d", $content_set));
    }
    else {
      $row = db_fetch_object(db_query("SELECT mcsid,description FROM {migrate_content_sets}
                                       WHERE LOWER('%s') = LOWER(machine_name)", $content_set));
    }
    $mcsid = $row->mcsid;
    $description = $row->description;
  }

  $options = array();
  if ($idlist = drush_get_option('idlist', FALSE)) {
    $options['idlist'] = $idlist;
  }
  if ($itemlimit = drush_get_option('itemlimit', FALSE)) {
    $options['itemlimit'] = $itemlimit;
  }
  $options['feedback'] = array(
    'function' => 'drush_log',
  );
  $feedback = drush_get_option('feedback');
  if ($feedback) {
    $parts = explode(' ', $feedback);
    $options['feedback']['frequency'] = $parts[0];
    $options['feedback']['frequency_unit'] = $parts[1];
    if ($options['feedback']['frequency_unit'] != 'seconds' &&
        $options['feedback']['frequency_unit'] != 'items') {
      drush_set_error(NULL, dt("Invalid feedback frequency unit '!unit'",
        array('!unit' => $options['feedback']['frequency_unit'])));
      return;
    }
  }
  if ($all) {
    $sql = "SELECT mcsid,description FROM {migrate_content_sets} ORDER BY weight DESC";
    $result = db_query($sql);
    while ($row = db_fetch_object($result)) {
      _drush_migrate_do_clear($row->mcsid, $row->description, $options);
    }
  }
  else {
    _drush_migrate_do_clear($mcsid, $description, $options);
  }
}

function _drush_migrate_do_clear($mcsid, $description, $options) {
  drush_log(dt("Clearing content set '!description'",
               array('!description' => $description)));
  $result = migrate_content_process_clear($mcsid, $options);
  switch ($result) {
    case MIGRATE_RESULT_IN_PROGRESS:
      drush_log(dt('Content set !content_set has an operation already in progress.',
        array('!content_set' => $description)), 'error');
      break;
    case MIGRATE_RESULT_STOPPED:
      drush_log(dt('Clearing of content set !content_set stopped.',
                   array('!content_set' => $description)), 'notice');
      break;
    case MIGRATE_RESULT_INCOMPLETE:
      // Most likely we are near the php memory_limit. Spawn a sub shell.
      drush_migrate_backend_invoke();
      break;
    case MIGRATE_RESULT_COMPLETED:
      break;
  }
}

/**
 * Import one specified content set
 *
 * @param $content_set
 *  The mcsid or the name of the content set
 */
function drush_migrate_import($content_set = NULL) {
  drush_migrate_set_verbose();
  if (drush_get_option('all')) {
    $all = TRUE;
    if ($content_set) {
      drush_set_error(NULL, dt('You must specify either a content set or --all, not both'));
      return;
    }
  }
  else {
    $all = FALSE;
    if (!$content_set) {
      drush_set_error(NULL, dt('You must specify either a content set or the -all option'));
      return;
    }
    if (is_numeric($content_set)) {
      $row = db_fetch_object(db_query("SELECT mcsid,description FROM {migrate_content_sets}
                                 WHERE mcsid=%d", $content_set));
    }
    else {
      $row = db_fetch_object(db_query("SELECT mcsid,description FROM {migrate_content_sets}
                                 WHERE LOWER('%s') = LOWER(machine_name)", $content_set));
    }
    $mcsid = $row->mcsid;
    $description = $row->description;
  }

  $options = array();
  if ($idlist = drush_get_option('idlist', FALSE)) {
    $options['idlist'] = $idlist;
  }
  if ($itemlimit = drush_get_option('itemlimit', FALSE)) {
    $options['itemlimit'] = $itemlimit;
  }
  $options['feedback'] = array(
    'function' => 'drush_log',
  );
  $feedback = drush_get_option('feedback');
  if ($feedback) {
    $parts = explode(' ', $feedback);
    $options['feedback']['frequency'] = $parts[0];
    $options['feedback']['frequency_unit'] = $parts[1];
    if ($options['feedback']['frequency_unit'] != 'seconds' &&
        $options['feedback']['frequency_unit'] != 'items') {
      drush_set_error(NULL, dt("Invalid feedback frequency unit '!unit'",
        array('!unit' => $options['feedback']['frequency_unit'])));
      return;
    }
  }
  if ($all) {
    $sql = "SELECT mcsid,description FROM {migrate_content_sets} ORDER BY weight";
    $result = db_query($sql);
    while ($row = db_fetch_object($result)) {
      _drush_migrate_do_import($row->mcsid, $row->description, $options);
    }
  }
  else {
    _drush_migrate_do_import($mcsid, $description, $options);
  }
}

function _drush_migrate_do_import($mcsid, $description, $options) {
  drush_log(dt("Importing content set '!description'",
               array('!description' => $description)));
  if (drush_get_option('update')) {
    migrate_content_set_update($mcsid);
  }
  $result = migrate_content_process_import($mcsid, $options);
  switch ($result) {
    case MIGRATE_RESULT_IN_PROGRESS:
      drush_log(dt('Content set !content_set has an operation already in progress.',
        array('!content_set' => $description)), 'error');
      break;
    case MIGRATE_RESULT_STOPPED:
      drush_log(dt('Importing of content set !content_set stopped.',
                   array('!content_set' => $description)), 'notice');
      break;
    case MIGRATE_RESULT_INCOMPLETE:
      // Most likely we are near the php memory_limit. Spawn a sub shell.
      drush_migrate_backend_invoke();
      break;
    case MIGRATE_RESULT_COMPLETED:
      break;
  }
}

/**
 * Stop clearing or importing a given content set.
 *
 * @param $content_set
 *  The mcsid or the name of the content set
 */
function drush_migrate_stop($content_set = NULL) {
  if (drush_get_option('all')) {
    $all = TRUE;
    if (isset($content_set)) {
      drush_set_error(NULL, dt('You must specify either a content set or --all, not both'));
      return;
    }
  }
  else {
    $all = FALSE;
    if (!isset($content_set)) {
      drush_set_error(NULL, dt('You must specify either a content set or the -all option'));
      return;
    }
    if (is_numeric($content_set)) {
      $row = db_fetch_object(db_query("SELECT mcsid,description,status FROM {migrate_content_sets}
                                 WHERE mcsid=%d", $content_set));
    }
    else {
      $row = db_fetch_object(db_query("SELECT mcsid,description,status FROM {migrate_content_sets}
                                 WHERE LOWER('%s') = LOWER(machine_name)", $content_set));
    }
    $mcsid = $row->mcsid;
    $description = $row->description;
    $status = $row->status;
  }
  if ($all) {
    $sql = "SELECT mcsid,description,status FROM {migrate_content_sets}
            WHERE status <> %d";
    $result = db_query($sql, MIGRATE_STATUS_IDLE);
    while ($row = db_fetch_object($result)) {
      drush_log(dt('Stopping !type operation on "!description"',
                array('!type' => $row->status == MIGRATE_STATUS_CLEARING ? 'clearing' : 'importing',
                      '!description' => $row->description)));
      db_query("UPDATE {migrate_content_sets} SET status=%d WHERE mcsid=%d",
               MIGRATE_STATUS_IDLE, $row->mcsid);
    }
  }
  else {
    if ($status != MIGRATE_STATUS_IDLE) {
      drush_log(dt('Stopping !type operation on "!description"',
                array('!type' => $status == MIGRATE_STATUS_CLEARING ? 'clearing' : 'importing',
                      '!description' => $description)));
      db_query("UPDATE {migrate_content_sets} SET status=%d WHERE mcsid=%d",
               MIGRATE_STATUS_IDLE, $mcsid);
    }
  }
}

/**
 * A drush command callback.
 */
function drush_migrate_wipe() {
  // node_delete() has silly perm requirements in d5/d6.
  drush_set_option('user', 1);
  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_LOGIN);

  $types = func_get_args();
  $placeholders = db_placeholders($types, 'varchar');
  $sql = "SELECT nid FROM {node} WHERE type IN ($placeholders)";
  $result = db_query($sql, $types);
  while ($row = db_fetch_object($result)) {
    node_delete($row->nid);

    // Quickly wipe node_load() cache to avoid memory bloat.
    node_load('invalid_condition', NULL, TRUE);

    // Check for closeness to memory limit
    $usage = memory_get_usage();
    $memory_limit = _migrate_memory_limit();
    $pct_memory = $usage/$memory_limit;
    if ($pct_memory > MIGRATE_MEMORY_THRESHOLD) {
      // Low on memory. Spawn a subshell.
      drush_migrate_backend_invoke();
    }
  }
}

/**
 * Set the verbose context if 'feedback' is requested.
 */
function drush_migrate_set_verbose() {
  if (drush_get_option('feedback')) {
    drush_set_context('DRUSH_VERBOSE',  TRUE);
  }
}
