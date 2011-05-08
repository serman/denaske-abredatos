<?php

/**
 * @file
 * Implementation of user destination handling
 */

/**
 * Implementation of hook_migrate_types().
 */
function user_migrate_types() {
  $types = array('user' => t('User'), 'role' => t('Role'));
  return $types;
}

/**
 * Implementation of hook_migrate_fields_user().
 */
function user_migrate_fields_user($type) {
  $fields = array(
    'uid' => t('User: Existing user ID'),
    'name' => t('User: Username'),
    'pass' => t('User: Password'),
    'mail' => t('User: Email address'),
    'created' => t('User: Account created'),
    'access' => t('User: Account last accessed'),
    'login' => t('User: Last logged in'),
    'roles' => t('User: Roles'),
    'status' => t('User: Status'),
  );

  return $fields;
}

/**
 * Implementation of hook_migrate_delete_user().
 */
function user_migrate_delete_user($tblinfo, $uid) {
  timer_start('user_delete');
  user_delete(array(), $uid);
  timer_stop('user_delete');
}

/**
 * Implementation of hook_migrate_import_user().
 */
function user_migrate_import_user($tblinfo, $row) {
  $sourcekey = $tblinfo->sourcekey;
  // Begin building user object...
  $newuser = array();

  // Handle an update operation
  if ($row->destid) {
    $newuser['uid'] = $row->destid;
  }
  // If a uid was provided via mapping, load the existing user
  else if (isset($tblinfo->fields['uid'])) {
    $uidname = $tblinfo->fields['uid']['srcfield'];
    $newuser = (array)user_load($row->$uidname);
    // The existing md5 password will have another md5 applied to it if
    // we're not careful - save it here and reset it later if no one else
    // has set a password
    $original_password = $newuser['pass'];
  }
  // Data which might be useful for user hooks.
  $newuser['migrate_tblinfo'] = $tblinfo;

  foreach ($tblinfo->fields as $destfield => $values) {
    // Ignore CCK fields - we assume another module such as content_profile will deal with them
    if (!preg_match('/^field_/', $destfield)) {
      if ($values['srcfield'] && isset($row->$values['srcfield'])) {
        $newvalue = $row->$values['srcfield'];
      }
      else {
        $newvalue = $values['default_value'];
      }

      // Make sense of timestamp fields
      if ($destfield == 'created' || $destfield == 'access' || $destfield == 'login') {
        $newvalue = _migrate_valid_date($newvalue);
      }
      elseif ($destfield == 'roles') {
        static $user_roles;
        if (!isset($user_roles)) {
          $user_roles = array_flip(user_roles(TRUE));
        }
        // Break out roles
        $roles = explode(',', $newvalue);
        $newvalue = array();

        // Accept either role IDs or role names (which need to be converted to role IDs)
        // Note that a role name containing a comma will be broken up - in this case, you
        // must use the role ID instead of the name
        foreach ($roles as $role) {
          $rid = $user_roles[trim($role)];
          if ($rid) {
            $newvalue[$rid] = $rid;
          }
          else {
            $newvalue[$role] = $role;
          }
        }
      }
      $newuser[$destfield] = $newvalue;
    }
  }

  // Prepare the user for import.
  $errors = migrate_destination_invoke_all('prepare_user', $newuser, $tblinfo, $row);

  $success = TRUE;
  foreach ($errors as $error) {
    if ($error['level'] != MIGRATE_MESSAGE_INFORMATIONAL) {
      $success = FALSE;
      break;
    }
  }

  if ($success) {
    // Generate a random username if none was provided (presumably we
    // are migrating from a system using email addresses as account names)
    if (!isset($newuser['name']) || !$newuser['name']) {
      // There is a tiny risk that the generated name will not be unique
      $newuser['name'] = user_password();
    }

    // This field appears to be obsolete, but little cost to setting it...
    if (!isset($newuser['init'])) {
      $newuser['init'] = $newuser['mail'];
    }

    // Default to enabled
    if (!isset($newuser['status'])) {
      $newuser['status'] = 1;
    }

    // If we loaded an existing (MD5) password and no one overwrote it, we'll
    // need to set it directly in the DB
    if (isset($original_password) && ($original_password == $newuser['pass'])) {
      $reset_password = TRUE;
    }
    else {
      $reset_password = FALSE;
    }

    timer_start('user_save');
    $account = user_save((object)$newuser, $newuser);
    timer_stop('user_save');

    if ($reset_password) {
      $sql = "UPDATE {users} SET pass='%s' WHERE uid=%d";
      db_query($sql, $original_password, $account->uid);
    }

    // Call completion hooks, for any processing which needs to be done after user_save
    timer_start('user completion hooks');
    $errors = array_merge($errors, migrate_destination_invoke_all('complete_user', $account, $tblinfo, $row));
    timer_stop('user completion hooks');

    if ($account) {
      migrate_add_mapping($tblinfo->mcsid, $row->$sourcekey, $account->uid);
    }
    else {
      // User record wasn't created, but we weren't given back a reason
      if (empty($errors)) {
        $errors[] = migrate_message(t('user_save failed: Unknown reason'));
      }
    }
  }
  return $errors;
}

/**
 * Implementation of hook_migrate_xlat_user().
 */
function user_migrate_xlat_user($uid) {
  return "user/$uid";
}

/**
 * Implementation of hook_migrate_fields_role().
 */
function user_migrate_fields_role($type) {
  $fields = array(
    '[rid]' => t('Role: Existing role ID'),
    'name' => t('Role: Name'),
  );

  return $fields;
}

/**
 * Implementation of hook_migrate_delete_role().
 */
function user_migrate_delete_role($tblinfo, $rid) {
  timer_start('role_delete');

  db_query('DELETE FROM {users_roles} WHERE rid = %d', $rid);
  db_query('DELETE FROM {permission} WHERE rid = %d', $rid);
  db_query('DELETE FROM {role} WHERE rid = %d', $rid);

  timer_stop('role_delete');
}

/**
 * Implementation of hook_migrate_import_role().
 */
function user_migrate_import_role($tblinfo, $row) {
  $errors = array();
  $new_role = array();
  // Handle an update operation
  if ($row->destid) {
    $new_role['rid'] = $row->destid;
  }
  // If a rid was provided via mapping, load the existing role
  else if (isset($tblinfo->fields['rid'])) {
    $ridname = $tblinfo->fields['rid']['srcfield'];
    $new_role['rid'] = $row->$ridname;
    $sql = "SELECT name FROM {role} WHERE rid=%d";
    $new_role['name'] = db_result(db_query($sql, $new_role['rid']));
  }

  foreach ($tblinfo->fields as $destfield => $values) {
    if ($values['srcfield'] && isset($row->$values['srcfield'])) {
      $newvalue = $row->$values['srcfield'];
    }
    else {
      $newvalue = $values['default_value'];
    }
    $new_role[$destfield] = $newvalue;
  }

  // Prepare the role for import
  $errors = migrate_destination_invoke_all('prepare_role', $new_role, $tblinfo, $row);

  $role_name = $new_role['name'];

  if ($role_name) {
    if (isset($new_role['rid'])) {
      db_query("UPDATE {role} SET name='%s' WHERE rid=%d", $role_name, $new_role['rid']);
    }
    else {
      db_query("INSERT INTO {role} (name) VALUES ('%s')", $role_name);
    }
    $sql = "SELECT rid FROM {role} WHERE name='%s'";
    $rid = db_result(db_query($sql, $role_name));
    $new_role['rid'] = $rid;
    // Call completion hooks, for any additional role-related processing
    // (such as assigning permissions)
    timer_start('role completion hooks');
    $errors = migrate_destination_invoke_all('complete_role', $new_role, $tblinfo, $row);
    timer_stop('role completion hooks');

    $sourcekey = $tblinfo->sourcekey;
    migrate_add_mapping($tblinfo->mcsid, $row->$sourcekey, $rid);
  }
  return $errors;
}
