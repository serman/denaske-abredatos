<?php

/**
 * @file
 * Implementation of node destination handling
 */

/**
 * Implementation of hook_migrate_types().
 */
function node_migrate_types() {
  static $nodetypes = NULL;

  if (!isset($nodetypes)) {
    $nodetypes = array();
    foreach (node_get_types() as $type => $info) {
      $nodetypes['node/' . $type] = t('Node: ') . $info->name;
    }
    asort($nodetypes);
  }
  return $nodetypes;
}

/**
 * Implementation of hook_migrate_delete_node().
 */
function node_migrate_delete_node($tblinfo, $nid) {
  // If we've created associations in {upload} between imported nodes
  // and files directly copied to the Drupal files directory (e.g. using
  // the auditfiles module), deleting the nodes deletes the files (not
  // what we typically want when going through clear/import cycles). We
  // short-circuit this behavior by explicitly deleting the {upload}
  // table entries for this node, so the upload module leaves the files alone.
  if (module_exists('upload') && db_table_exists('upload')) {
    db_query("DELETE FROM {upload} WHERE nid=%d", $nid);
  }
  node_delete($nid);
}

/**
 * Implementation of hook_migrate_import_node().
 */
function node_migrate_import_node($tblinfo, $row) {
  $node = new stdClass;
  $node->type = $tblinfo->desttype;
  // Handle an update operation
  if ($row->destid) {
    $node->nid = $row->destid;
    // Need to have the current vid present
    $node->vid = db_result(db_query("SELECT vid FROM {node} WHERE nid=%d", $row->destid));
  }
  // If a nid was provided via mapping, load the existing node
  else if (isset($tblinfo->fields['nid'])) {
    $nidname = $tblinfo->fields['nid']['srcfield'];
    $node = node_load($row->$nidname);
    if (!$node) {
      $node = new stdClass;
      $node->type = $tblinfo->desttype;
    }
  }

  // Data which might be useful for node hooks.
  $node->migrate_tblinfo = $tblinfo;

  // Make sure the default comment settings are applied first, so they
  // can be overridden
  $function = 'comment_nodeapi';
  if (function_exists($function)) {
    $function($node, "prepare");
  }

  foreach ($tblinfo->fields as $destfield => $values) {
    if ($values['srcfield'] && $row->$values['srcfield']) {
      $node->$destfield = $row->$values['srcfield'];
    }
    else {
      $node->$destfield = $values['default_value'];
    }
  }

  // Do default prep before letting other modules take a shot
  $type_info = node_get_types('type', $node->type);

  $errors = array();

  // User name and uid.
  global $user;
  if (isset($node->uid) && ($node->uid > 0)) {
    $account = user_load(array('uid' => $node->uid));
    $node->name = $account->name;
  }
  elseif (isset($node->name)) {
    // We have a mapped username.
    if ($account = user_load(array('name' => $node->name))) {
      $node->uid = $account->uid;
    }
    elseif ($account = user_load(array('mail' => $node->name))) {
      $node->uid = $account->uid;
    }
    // If no valid incoming user, the node owner will end up being the current user
  }
  elseif ($user->uid > 0) {
    $node->uid = $user->uid;
    $node->name = $user->name;
  }
  else {
    // Default to admin user
    $node->uid = 1;
    $account = user_load(array('uid' => $node->uid));
    $node->name = $account->name;
  }

  // Creation date.
  if (isset($node->created)) {
    // We have a mapped date.
    if (empty($node->created)) {
      unset($node->created);
    }
    elseif (($date = _migrate_valid_date($node->created)) > -1) {
      $node->created = $date;
    }
    else {
      migrate_message(t('The created date %date is not a valid date.',
        array('%date' => $node->created)));
      unset($node->created);
    }
  }

  // Last updated
  if (isset($node->changed)) {
    // We have a mapped changed date.
    if (empty($node->changed)) {
      unset($node->changed);
    }
    elseif (($date = _migrate_valid_date($node->changed)) > -1) {
      $node->changed = $date;
    }
    else {
      $errors[] = migrate_message(t('The changed date %date is not a valid date.',
        array('%date' => theme('placeholder', $node->changed))));
      unset($node->changed);
    }
  }

  // Options (published, promoted, sticky, moderated, new revision).
  if (!isset($options)) {
    $options = variable_get('node_options_'. $node->type, array('status', 'promote'));
  }
  $all_options = array('status', 'moderate', 'promote', 'sticky', 'revision');

  foreach ($all_options as $key) {
    if (isset($node->$key) && drupal_strlen($node->$key) > 0) {
      // If the field was mapped, use that value.
      $node->$key = ($node->$key ? 1 : 0);
    }
    else {
      // If not, use the global option.
      $node->$key = (isset($options[$key]) ? $options[$key] : 0);
    }
  }

  // Title.
  if ($type_info->has_title && (!isset($node->title) || empty($node->title))) {
    $node->title = t('{Empty title}');
  }

  // Strip tags from the title
  if (isset($node->title)) {
    $node->title = strip_tags($node->title);
  }

  // Body/teaser
  if ($type_info->has_body) {
    // If incoming data has a teaser and no body, copy the teaser into the body
    if (isset($node->teaser)) {
      $node->teaser = trim($node->teaser);
    }
    else {
      $node->teaser = '';
    }
    if (isset($node->body)) {
      $node->body = trim($node->body);
    }
    else {
      $node->body = '';
    }
    // If a teaser and body, and 'show summary in full view' disabled, add in
    // <!--break--> delimiter.
    // TODO: When updating an existing node (nid in incoming content set), the
    // break is unnecessarily prepended. How to avoid?
    if ($node->teaser && $node->body && (!isset($node->teaser_include) || !$node->teaser_include)) {
      $node->body = "\n<!--break-->\n" . $node->body;
    }
    elseif ($node->teaser && !$node->body) {
      $node->body = $node->teaser;
    }
    elseif ($node->body && !$node->teaser) {
      // Teaser not automatically generated
      $node->teaser = node_teaser($node->body);
    }
  }

  // Prepare the node for import.
  $errors = migrate_destination_invoke_all('prepare_node', $node, $tblinfo, $row);

  $success = TRUE;
  foreach ($errors as $error) {
    if ($error['level'] != MIGRATE_MESSAGE_INFORMATIONAL) {
      $success = FALSE;
      break;
    }
  }
  if ($success) {
    timer_start('node_save');
    // node_save() unconditionally sets the changed timestamp to time() - we need
    // to force the migrated value if present
    if (isset($node->changed)) {
      $changed = $node->changed;
    }
    node_save($node);
    if (isset($changed)) {
      $node->changed = $changed;
      drupal_write_record('node', $node, 'nid');
    }
    timer_stop('node_save');
    // Call completion hooks, for any processing which needs to be done after node_save
    timer_start('node completion hooks');
    $errors = array_merge($errors, migrate_destination_invoke_all('complete_node', $node, $tblinfo, $row));
    timer_stop('node completion hooks');

    $sourcekey = $tblinfo->sourcekey;
    migrate_add_mapping($tblinfo->mcsid, $row->$sourcekey, $node->nid);
  }
  return $errors;
}

/**
 * Implementation of hook_migrate_xlat_node().
 */
function node_migrate_xlat_node($nid) {
  return "node/$nid";
}

/**
 * Implementation of hook_migrate_fields_node().
 */
function node_migrate_fields_node($type) {
  $fields = array();
  if (user_access('administer nodes')) {
    $fields = array(
      '[nid]' => t('Node: ID of pre-existing node to update'),
      'name' => t('Node: Authored by (username)'),
      'uid' => t('Node: Authored by (uid)'),
      'created' => t('Node: Authored on'),
      'changed' => t('Node: Last updated on'),
      'status' => t('Node: Published'),
      'moderate' => t('Node: In moderation queue'),
      'promote' => t('Node: Promoted to front page'),
      'sticky' => t('Node: Sticky at top of lists'),
      'revision' => t('Node: Create new revision'),
      'format' => t('Node: Input format'),
      'language' => t('Node: Language (fr, en)'),
    );
  }

  $type_info = node_get_types('type', $type);

  if ($type_info->has_title) {
    $fields['title'] = t('Node: ') . $type_info->title_label;
  }
  if ($type_info->has_body) {
    $fields['body'] = t('Node: ') . $type_info->body_label;
    $fields['teaser'] = t('Node: Teaser');
    $fields['teaser_include'] = t('Node: Show summary in full view');
  }

  if (module_exists('path') && (user_access('create url aliases') || user_access('administer url aliases'))) {
    $fields['path'] = t('Path: URL Alias');
  }

  return $fields;
}
