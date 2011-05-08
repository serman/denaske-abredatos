<?php

/**
 * @file
 * Implementation of profile destination handling
 */

/**
 * Implementation of hook_migrate_fields_user().
 */
function profile_migrate_fields_user($type) {
  $fields = array();

  $profile = db_query("SELECT title, name FROM {profile_fields}");
  while ($field = db_fetch_object($profile)) {
    $fields[$field->name] = t('Profile: @title', array('@title' => $field->title));
  }

  return $fields;
}

/**
 * Implementation of hook_migrate_prepare_user().
 * Because user_save() on a new user record only saves those profile fields
 * with register=1, we pull the profile fields out and stash them in the row
 * object to be saved later.
 */
function profile_migrate_prepare_user(&$account, $tblinfo, &$row) {
  // Cache categories for each field
  static $cats;
  if (!isset($cats)) {
    $cats = array();
    $sql = "SELECT name,category FROM {profile_fields}";
    $result = db_query($sql);
    while ($catrow = db_fetch_object($result)) {
      $cats[$catrow->name] = $catrow->category;
    }
  }
  foreach ($account as $fieldname => $value) {
    if (!strncmp($fieldname, 'profile_', 8)) {
      $cat = $cats[$fieldname];
      $row->profile_fields[$cat][$fieldname] = $value;
      unset($account[$fieldname]);
    }
  }
  return array();
}

/**
 * Implementation of hook_migrate_complete_user().
 */
function profile_migrate_complete_user($account, $tblinfo, $row) {
  if (isset($row->profile_fields) && is_array($row->profile_fields)) {
    foreach ($row->profile_fields as $category => $fields) {
      profile_save_profile($fields, $account, $category);
    }
  }
}
