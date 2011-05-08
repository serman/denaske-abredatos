<?php
// $Id: email_registration.migrate.inc,v 1.1 2010/01/13 04:04:28 mikeryan Exp $

/**
 * @file
 * Hooks to suport email_registration during user migration
 */

/**
 * Implementation of hook_email_registration_name().
 *
 * If email_registration is in use, it overwrites any explicit username
 * with one generated from the email address. If you install the patch at
 * http://drupal.org/node/247717, this hook will short-circuit that and
 * force the username set in the migration process (possibly randomly
 * generated below).
 *
 * @param $edit
 *  The values we passed to user_save().
 * @param $account
 *  The user account as created by user_save().
 * @return
 *  The username we've already set - returning this prevents
 *  email_registration from overwriting it.
 */
function migrate_email_registration_name($edit, $account) {
  return $edit['name'];
}

/**
 * Implementation of hook_migrate_prepare_user().
 */
function email_registration_migrate_prepare_user(&$newuser, $tblinfo, $row) {
  // Generate a random username if none was provided
  if (!$newuser['name']) {
    // There is a tiny risk that the generated name will not be unique
    $newuser['name'] = user_password();
  }
}

