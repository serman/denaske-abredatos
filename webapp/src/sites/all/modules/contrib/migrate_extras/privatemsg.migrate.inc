<?php
// $Id: privatemsg.migrate.inc,v 1.1 2010/01/19 19:41:18 textplease Exp $

/**
 * @file
 * Integrates privatemsg module with the migrate module
 * 
 */

/**
 * Implementation of hook_migrate_types().
 */
function privatemsg_migrate_types() {
  $types = array('privatemsg' => t('Private Message'));
  return $types;
}

/**
 * Implementation of hook_migrate_fields().
 */
function privatemsg_migrate_fields_privatemsg($type) { 
 
  $fields = array(
      // 'mid' => t('Message ID'), //this is to be set on the fly, don't need
      'author' => t('Author UID'), // sender of msg
      'recipient' => t('Recipient UID '), //recipient of msg 
      'subject' => t('Subject'), // subject
      'body' => t('Message Body'), // the actual message
      'format' => t('Message Input format ID'), //the input format ID 
      'timestamp' => t('Timestamp'), //timestamp
      'is_new' => t('Is New?'), // 1 or 0
      'author_deleted' => t('Author Deleted?'), // 1 or 0       
      'recipient_deleted' => t('Recipient Deleted?'), // 1 or 0
      'thread_id' => t('Thread Message ID'), //mid of original msg ro threaded discussion (optional)
    );
  return $fields;
}

/**
 * Implementation of hook_migrate_prepare().
 */
function privatemsg_migrate_prepare_privatemsg(&$msg, $tblinfo, $row) {
  //we can't just pass the $msg object into privatemsg_new_thread() as is, so lets fix it up.
  
  //get full user objects for author and recipients, seems like a lot of overhead?
  $author = user_load(array('uid' => $msg->author));
  $recipient = user_load(array('uid' => $msg->recipient));  
  $new_thread->recipients = array($recipient);
  
  //set options
  $options['author'] = $author;
  $options['timestamp'] = $msg->timestamp;
  $options['format'] = $msg->format;
  $new_thread->options = $options;
  $new_thread->message = $msg->body;
  $new_thread->subject = $msg->subject;
  
  //add new items to msg (while keeping the old for later)
  $msg->new_thread = $new_thread;
}

/**
 * Implementation of hook_migrate_import().
 */
function privatemsg_migrate_import_privatemsg($tblinfo, $row) {
  $msg = (object)array();
  foreach ($tblinfo->fields as $destfield => $values) {

    if ($values['srcfield'] && $row->$values['srcfield']) {
      $msg->$destfield = $row->$values['srcfield'];
    } else {
      $msg->$destfield = $values['default_value'];
    }
  }
  
  timer_start('relationship_prepare hooks');
    $errors = migrate_invoke_all('prepare_privatemsg', $msg, $tblinfo, $row);
  timer_stop('relationship_prepare hooks');
  
  $success = TRUE;
  foreach ($errors as $error) {
    if ($error['level'] != MIGRATE_MESSAGE_INFORMATIONAL) {
      $success = FALSE;
      break;
    }
  }
  
  $new_thread = $msg->new_thread;
  if ($success) {

    timer_start('privatemsg_save');
      $msg = (object) array_merge((array)$msg, privatemsg_new_thread($new_thread->recipients, $new_thread->subject, $new_thread->message, $new_thread->options));
    timer_stop('privatemsg_save');

    if(!$msg->success) {
      $errors[] = migrate_message("Message didn't save");
    }
    // Call completion hooks, for any processing which needs to be done after node_save
    timer_start('privatemsg_completion hooks');
      $errors = array_merge($errors, migrate_invoke_all('complete_privatemsg', $msg, $tblinfo, $row));
    timer_stop('privatemsg_completion hooks');
    
  }
  return $errors;
}

/**
 * Implementation of hook_migrate_complete().
 */
function privatemsg_migrate_complete_privatemsg($msg, $tblinfo, $row) {
  $errors = array();
  if ($msg->message['mid']) {
   
    if ($msg->author_deleted) {
      //delete the author record
      privatemsg_message_change_delete($msg->message['mid'], 1, $msg->message['author']);
    }
    if ($msg->recipient_deleted) {
      //delete the author record
      privatemsg_message_change_delete($msg->message['mid'], 1, $msg->message['recipients'][0]);
    }

    if (!$msg->is_new) {
      privatemsg_thread_change_status($msg->message['mid'], PRIVATEMSG_READ, $msg->message['recipients'][0]);
    }
    $sourcekey = $tblinfo->sourcekey;
    migrate_add_mapping($tblinfo->mcsid, $row->$sourcekey, $msg->message['mid']);
  }
  else {
    $errors[] = migrate_message(t('Message not saved'));
  }
  return $errors;
}

/**
 * Implementation of hook_migrate_delete().
 */
function privatemsg_migrate_delete_privatemsg($mid) {
  //privatemsg_thread_change_delete($mid, 1, $)
  db_query("DELETE FROM {pm_index} WHERE mid = %d", $mid);
  db_query("DELETE FROM {pm_message} WHERE mid = %d", $mid);
}
