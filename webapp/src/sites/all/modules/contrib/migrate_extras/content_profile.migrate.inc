<?php
// $Id: content_profile.migrate.inc,v 1.1 2010/01/13 04:04:28 mikeryan Exp $

/**
 * @file
 * Hooks to support content_profile when migrating users
 */

/**
 * Implementation of hook_migrate_fields_user().
 */
function content_profile_migrate_fields_user($type) {
  // Get any CCK fields attached to content profiles
  $cp_types = content_profile_get_types();
  $fields = array();
  foreach ($cp_types as $type => $info) {
    $profile_fields = migrate_invoke_all('fields_node', $type);
    foreach ($profile_fields as $key => $value) {
      if (preg_match('/^field_/', $key) || $key == 'title' || $key == 'body') {
        $profile_fields[$type . '|' . $key] = $info->name . ' ' . $value;
      }
      unset($profile_fields[$key]);
    }
    $fields = array_merge($fields, $profile_fields);
  }
  return $fields;
}

/**
 * Implementation of hook_migrate_complete_user().
 */
function content_profile_migrate_complete_user(&$account, $tblinfo, $row) {
  // Initialize all related profile nodes
  $cp_types = content_profile_get_types();
  $cp_fields = array();
  foreach ($cp_types as $type => $info) {
    $cp_fields[$type] = content_migrate_fields_node($type);
    $node[$type] = new StdClass;
    $node[$type]->type = $type;
  }

  // Process all CCK fields in the input as content_profile
  foreach ($tblinfo->fields as $destfield => $values) {
    unset($type);
    if (strpos($destfield, '|') !== FALSE) {
      $pieces = explode('|', $destfield);
      $type = $pieces[0];
      $destfield = $pieces[1];
    }
    if ($values['srcfield'] && isset($row->$values['srcfield'])) {
      $newvalue = $row->$values['srcfield'];
    }
    else {
      $newvalue = $values['default_value'];
    }
    if (isset($type)) {
      $node[$type]->$destfield = $newvalue;
    }
  }

  // For each profile type, create the node attached to this user
  foreach ($cp_types as $type => $name) {
    $node[$type]->uid = $account->uid;
    if (!isset($node[$type]->title)) {
      $node[$type]->title = $account->name;
    }
    $node[$type]->name = $account->name;
    // Create the node.
    // Prepare the node for import. We could have written the following loop
    // as: module_invoke_all('node_import_prepare', $node, $preview > 0);
    // but unfortunately module_invoke_all passes all argumens by value.
    foreach (module_list() as $module_name) {
      $function = $module_name . '_migrate_prepare_node';
      if (function_exists($function)) {
        $errors = array_merge((array)$errors, (array)$function($node[$type], $tblinfo, $row));
      }
    }
    $node[$type] = node_submit($node[$type]);
    node_save($node[$type]);
  }

  return $errors;
}
