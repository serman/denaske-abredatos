<?php
// $Id: userpoints.migrate.inc,v 1.1.2.1 2010/01/19 21:39:54 textplease Exp $

/**
 * @file
 * Integrates userpoints module with the migrate module.
 * 
 * This will import individual user point transactions.
 * 
 * Still waiting on a patch to userpoints so that we can get txn_id.
 * #441678: Use drupal_write_record(). Needed to be able to import 
 * transactions with migrate_extras. http://drupal.org/node/441678  
 */

/**
 * Implementation of hook_migrate_types().
 */
function userpoints_migrate_types() {
  //$types = userpoints_get_categories();
  $types = array('userpoints' => 'User Points: Transactions')
  
  return $types;
}

/**
 * Implementation of hook_migrate_fields().
 */
function userpoints_migrate_fields_userpoints($type) {

  $fields = array(
     'points' => t('Points'),# of points (int) (required)
     'moderate' => t('Is Moderated'), //TRUE/FALSE 
     'uid' => t('Points UID'), 
     'time_stamp' => t('Updated timestamp'), //(custom), patch added to accept this.
     'operation' => t('Operation'),//'published' 'moderated' etc.
     'tid' => t('Category ID'), 
     'expirydate' => t('Expires Timestamp'), //timestamp or 0, 0 = non-expiring; NULL = site default
     'description' => t('description'),
     'reference' => t('reference'),//reserved for module specific use
     'display' => t('display'), //whether or not to display "points awarded" message
     'txn_id' => t('Transaction ID'), //Transaction ID of points, If present an UPDATE is performed
     'entity_id' => t('Entity ID'), //ID of an entity in the Database. ex. $node->id or $user->uid
     'entity_type' => t('Entity Type') //string of the entity type. ex. 'node' or 'user' NOT 'node-content-custom'
      
    );
    //dpm($fields);
  return $fields;
}

/**
 * Implementation of hook_migrate_prepare().
 */
/*
function userpoints_migrate_prepare_userpoints(&$ur, $tblinfo, $row) {
  //not used.
}
*/

/**
 * Implementation of hook_migrate_import().
 */
function userpoints_migrate_import_userpoints($tblinfo, $row) {
  $txn = (object)array();
    
  foreach ($tblinfo->fields as $destfield => $values) {

    if ($values['srcfield'] && $row->$values['srcfield']) {
       //dpm($destfield);
       //dpm($row->$values['srcfield']);
      $txn->$destfield = $row->$values['srcfield'];
    } else {
      $txn->$destfield = $values['default_value'];
    }
  }
    
  timer_start('prepare_userpoints');
    $errors = migrate_destination_invoke_all('prepare_userpoints', $txn, $tblinfo, $row);
  timer_stop('prepare_userpoints');
  
  
  $success = TRUE;
  foreach ($errors as $error) {
    if ($error['level'] != MIGRATE_MESSAGE_INFORMATIONAL) {
      $success = FALSE;
      break;
    }
  }
  if ($success) {
    //dpm("pre_save");
    //dpm($txn);
    timer_start('save_userpoints');
      // function expects an array
      $txn = (array)$txn;
      $ret = userpoints_userpointsapi($txn);
    timer_stop('save_userpoints');
    //dpm("post_save");
    //dpm($txn);
    if(!$ret['status']) {
      $errors[] = migrate_message($ret['reason']);
    }
    //switch back to object for consistency
    $txn = (object)$txn;
    
    // Call completion hooks, for any processing which needs to be done after node_save
    timer_start('userpoints completion hooks');
      $errors = array_merge($errors, migrate_destination_invoke_all('complete_userpoints', $txn, $tblinfo, $row));
    timer_stop('userpoints completion hooks');
    
    $sourcekey = $tblinfo->sourcekey;
    
    //map the relationship id
    if ($txn->txn_id) {
      $sourcekey = $tblinfo->sourcekey;
      migrate_add_mapping($tblinfo->mcsid, $row->$sourcekey, $txn->txn_id);
    }
  }
  return $errors;
}

/**
 * Implementation of hook_migrate_complete().
 */
/*
function userpoints_migrate_complete_userpoints($txn_id) {
  // unused
}
*/

/**
 * Implementation of hook_migrate_delete().
 */
/*
function userpoints_migrate_delete_userpoints($txn_id) {
  //todo
}
*/


