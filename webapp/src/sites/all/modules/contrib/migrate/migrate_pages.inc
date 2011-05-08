<?php

/**
 * @file
 */

/**
 * Menu callback for dashboard page
 */
function migrate_dashboard() {
  return drupal_get_form('_migrate_dashboard_form');
}

/**
 * Form definition for dashboard page
 */
function _migrate_dashboard_form($form_state) {
  migrate_check_advanced_help();

  $header = array(
    array('data' => t('Clear')),
    array('data' => t('Import')),
    array('data' => t('Content Set')),
    array('data' => t('Total rows')),
    array('data' => t('Imported')),
    array('data' => t('Unimported')),
    array('data' => t('Last imported')),
    );
  $form['header'] = array('#type' => 'value', '#value' => $header);
  $sql = "SELECT *
          FROM {migrate_content_sets}
          ORDER BY weight, contenttype, view_name, view_args";
  $result = db_query($sql);

  $clearing = array();
  $importing = array();
  $rownum = 0;
  while ($row = db_fetch_object($result)) {
    $status = '';
    if ($row->mcsid) {
      $view = views_get_view($row->view_name);
      if (!$view) {
        drupal_set_message(t('View !view does not exist - either (re)create this view, or
            remove the content set !content_set',
          array('!view' => $row->view_name,
                '!content_set' => l($row->description, "admin/content/migrate/content_set/$row->mcsid"))));
        continue;
      }
      $clearing[$row->mcsid] = '';
      $importing[$row->mcsid] = '';
      $status = t('N/A');
      $maptable = migrate_map_table_name($row->mcsid);
      $sourcekey = $row->sourcekey;
      if (db_table_exists($maptable)) {
        $imported = db_result(db_query('SELECT COUNT(*) FROM {' . $maptable . '}'));
      }
      else {
        $imported = 0;
      }

      // If not caching counts, override the saved count with a fresh count
      if (!variable_get('migrate_cache_counts', 0)) {
        $row->rowcount = _migrate_get_view_count($view, $row->view_args);
      }

      $unimported = $row->rowcount - $imported;

      $msgtablename = migrate_message_table_name($row->mcsid);
      if (db_table_exists($msgtablename)) {
        if ($unimported > 0) {
          $messages = '';
          $numerrors = db_result(db_query("SELECT COUNT(DISTINCT sourceid)
                                           FROM {" . $msgtablename . "}
                                           WHERE level=%d",
                                          MIGRATE_MESSAGE_ERROR));
          if ($numerrors > 0) {
            if (module_exists('tw')) {
              $messages .= l(format_plural($numerrors, '1&nbsp;error', '@count&nbsp;errors'),
                "admin/content/tw/view/$msgtablename/" . MIGRATE_MESSAGE_ERROR,
                array('html' => TRUE));
            }
            else {
              $messages .= format_plural($numerrors, '1&nbsp;error', '@count&nbsp;errors');
            }
            $messages .= '<br />';
          }
          $numwarnings = db_result(db_query("SELECT COUNT(DISTINCT sourceid)
                                             FROM {" . $msgtablename . "}
                                             WHERE level=%d",
                                            MIGRATE_MESSAGE_WARNING));
          if ($numwarnings > 0) {
            if (module_exists('tw')) {
              $messages .= l(format_plural($numwarnings, '1&nbsp;warning', '@count&nbsp;warnings'),
                "admin/content/tw/view/$msgtablename/" . MIGRATE_MESSAGE_WARNING,
                array('html' => TRUE));
            }
            else {
              $messages .= format_plural($numwarnings, '1&nbsp;warning', '@count&nbsp;warnings');
            }
            $messages .= '<br />';
          }
          $numnotices = db_result(db_query("SELECT COUNT(DISTINCT sourceid)
                                             FROM {" . $msgtablename . "}
                                             WHERE level=%d",
                                            MIGRATE_MESSAGE_NOTICE));
          if ($numnotices > 0) {
            if (module_exists('tw')) {
              $messages .= l(format_plural($numnotices, '1&nbsp;notice', '@count&nbsp;notices'),
                "admin/content/tw/view/$msgtablename/" . MIGRATE_MESSAGE_NOTICE,
                array('html' => TRUE));
            }
            else {
              $messages .= format_plural($numnotices, '1&nbsp;notice', '@count&nbsp;notices');
            }
            $messages .= '<br />';
          }
          if ($messages) {
            $unimported = $messages . " $unimported&nbsp;total";
          }
        }
        if ($imported > 0) {
          $numinformational = db_result(db_query("SELECT COUNT(DISTINCT sourceid)
                                             FROM {" . $msgtablename . "}
                                             WHERE level=%d",
                                            MIGRATE_MESSAGE_INFORMATIONAL));
          if ($numinformational > 0) {
            $imported .= '<br />';
            if (module_exists('tw')) {
              $imported .=
                l(format_plural($numinformational, '1 informational message', '@count informational messages'),
                "admin/content/tw/view/$msgtablename/" . MIGRATE_MESSAGE_INFORMATIONAL,
                array('html' => TRUE));
            }
            else {
              $imported .= format_plural($numinformational, '1 informational message', '@count informational messages');
            }
            $imported .= '<br />';
          }
        }
      }
      else {
        $imported = '';
        $unimported = '';
      }
    }
    else {
      $imported = '';
      $unimported = '';
    }

    $form['data'][$rownum]['clearing'] = array('#value' => 0);
    $form['data'][$rownum]['importing'] = array('#value' => 0);
    $form['data'][$rownum]['description'] = array('#value' =>
      l($row->description, 'admin/content/migrate/content_set/' . $row->mcsid));
    $form['data'][$rownum]['importrows'] = array('#value' => check_plain($row->rowcount));
    $form['data'][$rownum]['imported'] = array('#value' => $imported);
    $form['data'][$rownum]['unimported'] = array('#value' => $unimported);
    $form['data'][$rownum]['lastimported'] = array('#value' => $row->lastimported);
    $form['data'][$rownum]['mcsid'] = array('#value' => check_plain($row->mcsid));
    $form['data'][$rownum]['status'] = array('#value' => $row->status);
    $rownum++;
  }

  $form['clearing'] = array(
    '#type' => 'checkboxes',
    '#options' => $clearing,
  );
  $form['importing'] = array(
    '#type' => 'checkboxes',
    '#options' => $importing,
  );
  if (user_access(MIGRATE_ACCESS_ADVANCED)) {
    // Open the fieldset if we're coming back from a run
    // TODO: Or from a Stop
    $collapsed = strpos(referer_uri(), 'batch') === FALSE;
    $form['interactive'] = array(
      '#type' => 'fieldset',
      '#title' => t('Execute'),
      '#collapsible' => TRUE,
      '#collapsed' => $collapsed,
      '#description' => t('<p>While large migration tasks are best run via
        <a href="http://drupal.org/project/drush">drush</a>,
        you may choose here to execute tasks directly. Check any tasks you want to run,
        and click the Run button. Note that the settings below will be applied to each
        task you run.</p>
        <p>Running tasks may be cleanly stopped by clicking <b>Stop all running tasks</b>.'),
    );
    $form['interactive']['update'] = array(
      '#type' => 'checkbox',
      '#title' => t('Update previously-imported content'),
      '#description' => t('If unchecked, import operations will only process source
        data which has not already been imported. If checked, they will also replace
        previously-imported destination objects with the current source information.'),
      '#default_value' => variable_get('migrate_update', 0),
    );
    $form['interactive']['limit'] = array(
      '#type' => 'textfield',
      '#title' => t('Sample size'),
      '#size' => 4,
      '#description' => t('Number of records to process. Leave
        blank to process all records in the content set.'),
      '#default_value' => variable_get('migrate_limit', ''),
    );
    $form['interactive']['idlist'] = array(
      '#type' => 'textfield',
      '#title' => t('Source IDs'),
      '#size' => 30,
      '#description' => t('Enter a comma-separated list of IDs from the incoming content set, to
        process only those records.'),
    );
    $form['interactive']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Run'),
      '#submit' => array('_migrate_dashboard_form_run'),
    );
    $form['interactive']['stop'] = array(
      '#type' => 'submit',
      '#value' => t('Stop all running tasks'),
      '#submit' => array('_migrate_dashboard_form_stop'),
    );
  }

  // Fetch information on available destinations
  $desttypes = migrate_invoke_all('types');

  $form['addset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add a content set'),
    '#collapsible' => TRUE,
    '#collapsed' => $rownum == 0 ? FALSE: TRUE,
  );

  $form['addset']['machine_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Content set name'),
    '#size' => 24,
    '#maxlength' => 50,
    '#description' => t('This is the unique name of the content set. It must contain
      only alphanumeric characters and underscores; it is used to identify the
      content set internally and to generate map and message tables related to
      the content set.'),
  );

  $form['addset']['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description of the content set'),
    '#description' => t('User-friendly description of the content set'),
  );

  $form['addset']['contenttype'] = array(
    '#type' => 'select',
    '#title' => t('Destination'),
    '#options' => $desttypes,
    '#description' => t('The type of Drupal content which will store the incoming records.'),
  );

  $views = views_get_all_views();
  foreach ($views as $name => $view) {
    if ($view->tag) {
      $options[$name] = $view->tag . ': ' . $name;
    }
    else {
      $options[$name] = $name;
    }
    if ($view->description) {
      if (drupal_strlen($view->description) > 60) {
        $view->description = drupal_substr($view->description, 0, 57) . '...';
      }
      $options[$name] .= " ($view->description)";
    }
  }
  asort($options);
  $form['addset']['view_name'] = array(
    '#type' => 'select',
    '#title' => t('Source view from which to import content'),
    '#options' => $options,
    '#description' => t('This View defines the records which are to be migrated.'),
  );

  $form['addset']['view_args'] = array(
    '#type' => 'textfield',
    '#title' => t('View arguments'),
    '#description' => t('Arguments to apply to the view when processing,
      separated with a / as though they were a URL path.'),
    '#maxlength' => 255,
    '#size' => 30,
  );
  $form['addset']['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#required' => TRUE,
    '#default_value' => 0,
    '#description' => t('The order in which content sets will be processed and displayed.'),
  );
  $form['addset']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#submit' => array('_migrate_content_set_form_submit'),
    '#validate' => array('_migrate_content_set_form_validate'),
  );
  return $form;
}

/**
 * Theme function for dashboard page
 */
function theme_migrate_dashboard($form) {
  $output = drupal_render($form['description']);
  if (isset($form['data']) && is_array($form['data'])) {
    foreach (element_children($form['data']) as $rownum) {
      $row = array();
      foreach (element_children($form['data'][$rownum]) as $colname) {
        if ($colname == 'clearing' || $colname == 'importing') {
          $row[] = drupal_render($form[$colname][$form['data'][$rownum]['mcsid']['#value']]);
          // Throw out the column contents
          drupal_render($form['data'][$rownum][$colname]);
          drupal_render($form['data'][$rownum]['mcsid']);
        }
        elseif ($colname == 'status' || $colname == 'mcsid') {
          drupal_render($form['data'][$rownum][$colname]);
        }
        else {
          $row[] = drupal_render($form['data'][$rownum][$colname]);
        }
      }
      // Highlight any process currently running
      if ($form['data'][$rownum]['status']['#value']) {
        $rows[] = array('data' => $row, 'class' => 'migrate-running');
      }
      else {
        $rows[] = $row;
      }
    }
  }

  $header = $form['header']['#value'];
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No data in the table.'), 'colspan' => count($header)));
  }

  $output .= theme('table', $header, $rows, array('class' => 'migrate-dashboard'));
  $output .= drupal_render($form);

  return $output;
}

/*
 * Implementation of hook_submit().
 */
function _migrate_dashboard_form_run($form, &$form_state) {
  $started = time();
  $values = $form_state['values'];
  $clearing = array();
  $importing = array();
  foreach ($values['importing'] as $mcsid => $value) {
    if ($value) {
      $importing[$mcsid] = $mcsid;
    }
    if ($values['clearing'][$mcsid]) {
      $clearing[$mcsid] = $mcsid;
    }
  }

  variable_set('migrate_limit', $values['limit']);
  variable_set('migrate_update', $values['update']);
  $batch = array(
    'operations' => array(
      array('migrate_content_process_batch',
            array($clearing,
                  $importing,
                  $values['limit'],
                  trim($values['idlist'])))),
    'title' => t('Migration processing'),
    'file' => drupal_get_path('module', 'migrate') . '/migrate_pages.inc',
    'init_message' => t('Starting migration process'),
    'progress_message' => t(''),
    'error_message' => t('An error occurred. Some or all of the migrate processing has failed.'),
    'finished' => '_migrate_content_process_finish',
  );
  batch_set($batch);
}

/*
 * Implementation of hook_submit().
 */
function _migrate_dashboard_form_stop($form, &$form_state) {
  db_query("UPDATE {migrate_content_sets} SET status=%d",
           MIGRATE_STATUS_IDLE);
  drupal_set_message(t('All migration tasks have been stopped.'));
}

/**
 * Batch API finished callback - report results
 *
 * @param $success
 *  Ignored
 * @param $results
 *  List of results from batch processing
 * @param $operations
 *  Ignored
 */
function _migrate_content_process_finish($success, $results, $operations) {
  foreach ($results as $result) {
    drupal_set_message($result);
  }
}

/**
 * Common validation function for content sets
 */
function _migrate_content_set_validate($form_state) {
  $machine_name = trim($form_state['values']['machine_name']);
  if (!$machine_name) {
    form_set_error('machine_name', t('Content set name is required'));
  }
  else {
    $mcsid = db_result(db_query("SELECT mcsid FROM {migrate_content_sets}
                                 WHERE machine_name='%s'", $machine_name));
    if ($mcsid && $mcsid != $form_state['values']['mcsid']) {
      form_set_error('machine_name', t('The content set name must be unique.'));
    }
    else if (preg_match('/[^a-zA-Z0-9_]/', $machine_name)) {
      form_set_error('machine_name', t('Content set name must be alphanumeric or underscores only.'));
    }
  }
  $description = trim($form_state['values']['description']);
  if (!$description) {
    form_set_error('description', t('Content set description is required'));
  }
  $weight = trim($form_state['values']['weight']);
  if (!is_numeric($weight) || (((int)$weight) != $weight)) {
    form_set_error('weight', t('Weight must be an integer value (positive or negative)'));
  }
}

/**
 * Implementation of hook_validate().
 */
function _migrate_content_set_form_validate($form, $form_state) {
  _migrate_content_set_validate($form_state);
}

/**
 * Implementation of hook_submit().
 */
function _migrate_content_set_form_submit($form, &$form_state) {
  $content_set = array();
  $content_set['view_name'] = $form_state['values']['view_name'];
  $content_set['view_args'] = trim($form_state['values']['view_args']);
  $content_set['machine_name'] = $form_state['values']['machine_name'];
  $content_set['description'] = $form_state['values']['description'];
  $destination = explode('/', $form_state['values']['contenttype']);
  $content_set['contenttype'] = $destination[0];
  if (isset($destination[1])) {
    $content_set['desttype'] = $destination[1];
  }
  $content_set['weight'] = $form_state['values']['weight'];
  $mcsid = migrate_save_content_set($content_set);
  // Go straight to the mapping form
  $form_state['redirect'] = "admin/content/migrate/content_set/$mcsid";
}

/**
 * Menu callback function for content set edit page.
 */
function migrate_content_set_mappings($form_state, $mcsid) {
  $row = db_fetch_object(db_query("SELECT * FROM {migrate_content_sets}
                                   WHERE mcsid=%d",
                                  $mcsid));
  $desttype = $row->desttype;
  $view_name = $row->view_name;
  $view_args = $row->view_args;
  $sourcekey = $row->sourcekey;
  $machine_name = $row->machine_name;
  $description = $row->description;
  $contenttype = $row->contenttype;
  $desttype = $row->desttype;
  $weight = $row->weight;
  $multiple_separator = $row->multiple_separator;

  drupal_set_title(check_plain($description));

  // Fetch information on available destinations
  $desttypes = migrate_invoke_all('types');
  $destfields = migrate_invoke_all("fields_$contenttype", $desttype);

  $form['mcsid'] = array(
    '#type' => 'value',
    '#value' => $mcsid,
  );

  $form['machine_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Content set name'),
    '#size' => 24,
    '#maxlength' => 50,
    '#default_value' => $machine_name,
    '#description' => t('This is the unique name of the content set. It must contain
      only alphanumeric characters and underscores; it is used to identify the
      content set internally and to generate map and message tables related to
      the content set.'),
  );

  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description of the content set'),
    '#default_value' => $description,
  );
  $form['show_view_name'] = array(
    '#prefix' => '<div>',
    '#value' => t('<strong>Source view:</strong> ') . l($view_name, 'admin/build/views/edit/' . $view_name),
    '#suffix' => '</div>',
  );
  $form['view_args'] = array(
    '#type' => 'textfield',
    '#title' => t('View arguments'),
    '#description' => t('Arguments to apply to the view when processing,
      separated with a / as though they were a URL path.'),
    '#default_value' => $view_args,
  );

  $form['view_name'] = array(
    '#type' => 'value',
    '#value' => $view_name,
  );
  if ($desttype) {
    $destination = $desttypes["$contenttype/$desttype"];
  }
  else {
    $destination = $desttypes[$contenttype];
  }
  $form['show_contenttype'] = array(
    '#prefix' => '<div>',
    '#value' => t('<strong>Destination:</strong> ') . $destination,
    '#suffix' => '</div>',
  );
  $form['contenttype'] = array(
    '#type' => 'value',
    '#value' => $contenttype,
  );
  $form['desttype'] = array(
    '#type' => 'value',
    '#value' => $desttype,
  );

  $form['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#description' => t('The order in which content sets will be processed and displayed.'),
    '#default_value' => $weight,
  );

  $form['multiple_separator'] = array(
    '#type' => 'textfield',
    '#title' => t('Separator'),
    '#description' => t('Separator for fields potentially holding multiple values, e.g. taxonomy terms.'),
    '#size' => 2,
    '#maxlength' => 8,
    '#default_value' => $multiple_separator,
  );

  $form['header'] = array(
    '#type' => 'value',
    '#value' => array(
      array('data' => t('Source field')),
      array('data' => t('Default value')),
      array('data' => t('Destination field')),
    ),
  );

  $view = views_get_view($view_name);
  if (!$view) {
    drupal_set_message(t('View !view does not exist - either (re)create a view with
      this name, or delete this content set.', array('!view' => $view_name)));
  }
  else {
    // Need to fill in the query, to find out the aliases that will be returned by the
    // query
    if ($view_args) {
      $view->set_arguments(explode('/', $view_args));
    }
    $view->build();
    $fields = $view->get_items('field');
    $srcoptions = array();
    foreach ($view->query->fields as $fieldalias => $field) {
      $fieldname = $field['field'];
      $fieldtable = $field['table'];
      // The field name can be ambiguous (e.g., two map tables in the view), so
      // we can't just do $fields[$fieldname] - we need to iterate and match the
      // table as well
      foreach ($fields as $viewfieldname => $viewfield) {
        if ($viewfield['field'] == $fieldname && $viewfield['table'] == $fieldtable) {
          $srcoptions[$fieldalias] = $viewfield['label'];
          break;
        }
      }
      if (!isset($srcoptions[$fieldalias])) {
        $srcoptions[$fieldalias] = $fieldtable . '.' . $fieldname;
      }
    }

    $form['sourcekey'] = array(
      '#type' => 'select',
      '#options' => $srcoptions,
      '#default_value' => $sourcekey,
      '#title' => t('Primary key of source view'),
    );

    $mappings = array();
    $defaults = array();

    $srcoptions = array_merge(array('' => t('<none>')), $srcoptions);
    foreach ($destfields as $destfield => $destname) {
      $matches = array();
      if (preg_match('/^\[([^\]]*)\]$/', $destfield, $matches)) {
        $primary_key = $matches[1];
        $destfield = $primary_key;
        $destfields[$primary_key] = $destname;
        unset($destfields[$destfield]);
      }
      else {
        $primary_key = '';
      }
      $sql = "SELECT *
              FROM {migrate_content_mappings}
              WHERE mcsid=%d AND destfield='%s'";
      $result = db_query($sql, $mcsid, $destfield);
      $row = db_fetch_object($result);
      $cols[] = $destfield;
      $form['srcfield'][$destfield] = array(
        '#type' => 'select',
        '#options' => $srcoptions,
        '#default_value' => isset($row->srcfield) ? $row->srcfield : '',
      );
      $form['default_value'][$destfield] = array(
        '#type' => 'textfield',
        '#default_value' => isset($row->default_value) ? $row->default_value : '',
        '#size' => 25,
        '#maxlength' => 255,
      );
      $form['destfield'][$destfield] = array('#value' => $destname);
    }

    $form['cols'] = array(
      '#type' => 'value',
      '#value' => $cols,
    );

    $form['primary_key'] = array(
      '#type' => 'value',
      '#value' => $primary_key,
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit changes'),
    );
  }
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  $form['#tree'] = TRUE;
  return $form;
}

/**
 * Theme function for content set edit page.
 */
function theme_migrate_content_set_mappings($form) {
  $output = drupal_render($form['machine_name']);
  $output .= drupal_render($form['description']);
  $output .= drupal_render($form['show_view_name']);
  $output .= drupal_render($form['view_args']);
  $output .= drupal_render($form['show_contenttype']);
  $output .= drupal_render($form['desttype']);
  $output .= drupal_render($form['sourcekey']);
  $output .= drupal_render($form['weight']);
  $output .= drupal_render($form['multiple_separator']);

  if (isset($form['destfield']) && is_array($form['destfield'])) {
    foreach (element_children($form['destfield']) as $destfield) {
      $row = array();
      $row[] = drupal_render($form['srcfield'][$destfield]);
      $row[] = drupal_render($form['default_value'][$destfield]);
      $row[] = drupal_render($form['destfield'][$destfield]);
      $rows[] = $row;
    }
  }

  $header = $form['header']['#value'];
  if (!$rows) {
    $rows[] = array(array('data' => t('No data in the table.'), 'colspan' => count($header)));
  }
  $output .= t('Map fields in the Source recordset to properties in a Drupal object. More complex fields such as multiple value elements need to be handled in code using the prepare hook.');
  $output .= theme('table', $header, $rows);
  $output .= drupal_render($form['submit']);
  $output .= drupal_render($form);

  // Support dynamic visualization of already-picked fields
  //drupal_add_js(drupal_get_path('module', 'migrate') .'/migrate.js');

  return $output;
}

/**
 * Implementation of hook_validate().
 */
function migrate_content_set_mappings_validate($form, $form_state) {
  _migrate_content_set_validate($form_state);
}

/**
 * Implementation of hook_submit().
 */
function migrate_content_set_mappings_submit($form, &$form_state) {
  $mcsid = $form_state['values']['mcsid'];
  if ($form_state['clicked_button']['#parents'][0] == 'delete') {
    migrate_delete_content_set($mcsid);
    drupal_set_message(t('Content set deleted'));
  }
  else {
    // Get list of current mappings, so we know if any need deletion
    $sql = "SELECT mcmid,destfield FROM {migrate_content_mappings} WHERE mcsid=%d";
    $result = db_query($sql, $mcsid);
    $prevdests = array();
    while ($row = db_fetch_object($result)) {
      $prevdests[$row->destfield] = $row->mcmid;
    }
    $sourcekey = $form_state['values']['sourcekey'];
    foreach ($form_state['values']['cols'] as $key => $destfield) {
      $mapping = new stdClass;
      $mapping->mcsid = $mcsid;
      $mapping->srcfield = $form_state['values']['srcfield'][$destfield];
      $mapping->destfield = $destfield;
      $mapping->default_value = $form_state['values']['default_value'][$destfield];
      $mapping->mcmid = db_result(db_query(
        "SELECT mcmid
         FROM {migrate_content_mappings}
         WHERE mcsid=%d AND destfield='%s'",
        $mcsid, $destfield));
      if ($form_state['values']['primary_key'] == $destfield) {
        $mapping->primary_key = TRUE;
      }
      else {
        $mapping->primary_key = FALSE;
      }
      migrate_save_content_mapping($mapping);
      // This mapping is currently valid, so remove from the deletion list
      unset($prevdests[$mapping->destfield]);
    }
    // Delete any mappings we didn't save
    foreach ($prevdests as $mcmid) {
      migrate_delete_content_mapping($mcmid);
    }
    migrate_save_content_set($form_state['values']);
    drupal_set_message('Changes saved');
  }
  $form_state['redirect'] = 'admin/content/migrate/dashboard';
}

/**
 * Menu callback for settings page.
 */
function migrate_settings() {
  return drupal_get_form('_migrate_settings_form');
}

/**
 * Form definition for settings page.
 */
function _migrate_settings_form($form_state) {
  $max_execution_time = ini_get('max_execution_time');
  $max_input_time = ini_get('max_input_time');
  $memory_limit = _migrate_memory_limit();
  $description = '';
  if ($max_execution_time < 90 || $max_input_time < 90) {
    $description .= t(
      '<p>Each batch of a migration performed interactively will last
        a few seconds less than the less of <em>max_execution_time</em>
        and <em>max_input_time</em>. It is recommended that you set these
        to a higher value (say, 240) in .htaccess.</p>'
    );
  }
  if ($memory_limit < 128*1024*1024) {
    $description .= t(
      '<p>Large migration operations can take substantial memory, particularly
        if there is cached information growing with each iteration. It is
        recommended that you set <em>memory_limit</em> higher (at least
        256M if you can).</p>'
    );
  }
  $description .= t('<p>Current PHP configuration options:
    <table>
      <tr><td>max_execution_time</td><td>!max_execution_time</td></tr>
      <tr><td>max_input_time</td><td>!max_input_time</td></tr>
      <tr><td>memory_limit</td><td>!memory_limit</td></tr>
    </table>
    </p>',
    array('!max_execution_time' => $max_execution_time,
          '!max_input_time' => $max_input_time,
          '!memory_limit' => ini_get('memory_limit')
         )
  );
  $form['description'] = array(
    '#prefix' => '<div>',
    '#value' => $description,
    '#suffix' => '</div>',
  );
  $form['display_timers'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display timers when processing'),
    '#description' => t('To diagnose performance bottlenecks, turn this toggle
      on - at the completion of a processing round, cumulative times of
      tasks will be displayed.'),
    '#default_value' => variable_get('migrate_display_timers', 0),
  );

  $form['hide_help_message'] = array(
    '#type' => 'checkbox',
    '#title' => t('Ignore missing advanced help module'),
    '#description' => t('Migrate uses the Advanced Help module to provide help text; if this module is not present Migrate will complain, unless this setting is checked.'),
    '#default_value' => variable_get('migrate_hide_help_message', FALSE),
  );

  $form['cache_counts'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cache content set counts'),
    '#description' => t('With large and/or complex content sets, getting the <strong>Total
      Rows</strong> value on the dashboard page can be time-consuming. Enable caching to
      store the last known count in the database.'),
    '#default_value' => variable_get('migrate_cache_counts', 0),
  );

  if (variable_get('migrate_cache_counts', 0)) {
    $form['refresh_counts'] = array(
      '#type' => 'checkbox',
      '#title' => t('Refresh cached content set counts'),
      '#description' => t('Update the cached content set counts to reflect their current values.'),
      '#default_value' => 0,
    );
  }
  $form['integrations'] = array('#tree' => TRUE);
  $form['integrations']['description'] = array(
    '#prefix' => '<hr /><div>',
    '#value' => t('Modules implementing the Migrate API are listed below. Most modules
                   other than the Migrate module itself simply support themselves, but
                   some (notably <a href="http://drupal.org/project/migrate_extras">Migrate Extras</a>)
                   may provide support on behalf of other modules. Support can be
                   disabled by unchecking the box for a particular line.'),
    '#suffix' => '</div>',
  );

  foreach (migrate_get_module_apis() as $module => $info) {
    $form['integrations'][$module] = array(
      '#type' => 'fieldset',
      '#title' => t('Migration support implemented in the @module module',
                    array('@module' => $module)),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    foreach ($info['integration modules'] as $intmod => $intmod_details) {
       $form['integrations'][$module][$intmod] = array(
         '#type' => 'checkbox',
         '#title' => t($intmod_details['description']),
         '#default_value' => $intmod_details['status'],
       );
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t("Submit"),
  );
  return $form;
}

/**
 * Theme function for settings form.
 */
function theme_migrate_settings($form) {
  $output = '';
  $output .= drupal_render($form['description']);
  $output .= drupal_render($form['display_timers']);
  $output .= drupal_render($form['hide_help_message']);
  $output .= drupal_render($form['cache_counts']);
  $output .= drupal_render($form['refresh_counts']);
  $output .= drupal_render($form['integrations']);
  $output .= drupal_render($form);

  return $output;
}

/*
 * Implementation of hook_submit().
 */
function _migrate_settings_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  variable_set('migrate_display_timers', $values['display_timers']);
  variable_set('migrate_hide_help_message', $values['hide_help_message']);
  $original_cache = variable_get('migrate_cache_counts', 0);
  variable_set('migrate_cache_counts', $values['cache_counts']);

  variable_set('migrate_integration_settings', $values['integrations']);
  if (isset($values['refresh_counts']) || ($values['cache_counts'] && !$original_cache)) {
    $sql = "SELECT mcsid,view_name,view_args FROM {migrate_content_sets}";
    $result = db_query($sql);
    while ($row = db_fetch_object($result)) {
      $rowcount = _migrate_get_view_count($row->view_name, $row->view_args);
      $sql = "UPDATE {migrate_content_sets}
              SET rowcount=%d WHERE mcsid=%d";
      db_query($sql, $rowcount, $row->mcsid);
    }
    if ($values['refresh_counts']) {
      drupal_set_message(t('Content set counts refreshed'));
    }
    else {
      drupal_set_message(t('Content set counts saved'));
    }
  }
  drupal_set_message(t('Settings saved'));
}

/**
 * Menu callback function.
 */
function migrate_export_content_set($form_state, $mcsid) {
  $row = db_fetch_array(db_query("SELECT * FROM {migrate_content_sets}
                                   WHERE mcsid=%d",
                                  $mcsid));
  drupal_set_title(check_plain($row['description']));
  // TODO: Generate Table Wizard calls?
  $code = '  $content_set = new stdClass;' . "\n";
  unset($row['mcsid']);
  foreach ($row as $field => $value) {
    // Yes, we're potentially quoting integer values - it all works out
    $code .= '  $content_set->' . $field . "= '" . $value . "';\n";
  }
  $code .= '  migrate_save_content_set($content_set);' . "\n";
  $code .= "// when used in hook_install(), use this line instead\n";
  $code .= "// migrate_save_content_set(\$content_set, array('base_table' => 'table_name'));\n";
  $code .= '  $mcsid = $content_set->mcsid;' . "\n\n";

  $sql = "SELECT * FROM {migrate_content_mappings} WHERE mcsid=%d";
  $result = db_query($sql, $mcsid);
  while ($row = db_fetch_array($result)) {
    unset($row['mcmid']);
    unset($row['mcsid']);
    $code .= '  $mapping = new stdClass;' . "\n";
    $code .= '  $mapping->mcsid = $mcsid;' . "\n";
    foreach ($row as $field => $value) {
      $code .= '  $mapping->' . $field . "= '" . $value . "';\n";
    }
    $code .= '  migrate_save_content_mapping($mapping);' . "\n\n";
  }
  $lines = substr_count($code, "\n");

  $form['description'] = array(
    '#prefix' => '<div>',
    '#value' => t('Copy this code to the the clipboard and paste it into an
      install or update hook. This enables you to maintain content sets
      programmatically, and to put their definitions under source control.'),
    '#suffix' => '</div>',
  );

  $form['export'] = array(
    '#title' => t('Export data'),
    '#type' => 'textarea',
    '#value' => $code,
    '#rows' => $lines,
  );
  return $form;
}
