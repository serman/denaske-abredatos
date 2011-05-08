<?php

/**
 * @file
 * Implementation of comment destination handling
 */

/**
 * Implementation of hook_migrate_types().
 */
function comment_migrate_types() {
  $types = array('comment' => t('Comment'));
  return $types;
}

/**
 * Implementation of hook_migrate_fields_comment().
 */
function comment_migrate_fields_comment($type) {
  $fields = array(
    'cid' => t('Comment: Existing comment ID'),
    'subject' => t('Comment: Title'),
    'comment' => t('Comment: Body'),
    'timestamp' => t('Comment: Post date'),
    'nodesourceid' => t('Comment: Node by source id'),
    'nid' => t('Comment: Node by Drupal nid'),
    'name' => t('Comment: Author by name'),
    'usersourceid' => t('Comment: Author by source id'),
    'uid' => t('Comment: Author by Drupal uid'),
    'commentsourceid' => t('Comment: Parent by source id'),
    'pid' => t('Comment: Parent by Drupal cid'),
    'homepage' => t("Comment: Author's website"),
    'mail' => t("Comment: Author's email"),
    'hostname' => t("Comment: Author's hostname"),
    'status' => t('Comment: Status'),
  );

  return $fields;
}

/**
 * Implementation of hook_migrate_delete_comment().
 */
function comment_migrate_delete_comment($tblinfo, $cid) {
  $path = drupal_get_path('module', 'comment') . '/comment.admin.inc';
  include_once($path);
  // Backdoor deletion - query stolen from comment_delete()
  $form = array();
  $form_state = array();
  $form['#comment'] = db_fetch_object(db_query(
    'SELECT c.*, u.name AS registered_name, u.uid
     FROM {comments} c
     INNER JOIN {users} u ON u.uid = c.uid
     WHERE c.cid = %d',
    $cid));
  comment_confirm_delete_submit($form, $form_state);
}

/**
 * Implementation of hook_migrate_import_comment().
 */
function comment_migrate_import_comment($tblinfo, $row) {
  $errors = array();
  $comment = array();
  // Handle an update operation
  if ($row->destid) {
    $comment['cid'] = $row->destid;
  }
  // If a cid was provided via mapping, load the existing comment
  else if (isset($tblinfo->fields['cid'])) {
    $cidname = $tblinfo->fields['cid']['srcfield'];
    $comment = _comment_load($row->$cidname);
  }
  // Data which might be useful for comment hooks.
  $comment['migrate_tblinfo'] = $tblinfo;

  $sourcekey = $tblinfo->sourcekey;

  foreach ($tblinfo->fields as $destfield => $values) {
    if (isset($values['srcfield']) && $values['srcfield'] && $row->$values['srcfield']) {
      $comment[$destfield] = $row->$values['srcfield'];
    }
    else {
      $comment[$destfield] = $values['default_value'];
    }
  }

  if (isset($comment['nodesourceid']) && $comment['nodesourceid'] && !isset($comment['nid'])) {
    $nid = _migrate_xlat_get_new_id('node', $comment['nodesourceid']);
    if ($nid) {
      $comment['nid'] = $nid;
    }
    else {
      $errors[] = migrate_message(t('Could not determine node ID for !id',
        array('!id' => $comment['nodesourceid'])),
        MIGRATE_MESSAGE_WARNING);
    }
    unset($comment['nodesourceid']);
  }
  if (isset($comment['usersourceid']) && $comment['usersourceid'] && !isset($comment['uid'])) {
    $uid = _migrate_xlat_get_new_id('user', $comment['usersourceid']);
    if ($uid) {
      $comment['uid'] = $uid;
    }
    else {
      $errors[] = migrate_message(t('Could not determine user ID for !id',
        array('!id' => $comment['usersourceid'])),
        MIGRATE_MESSAGE_WARNING);
    }
    unset($comment['usersourceid']);
  }
  if (isset($comment['commentsourceid']) && $comment['commentsourceid'] && !isset($comment['pid'])) {
    $pid = _migrate_xlat_get_new_id('comment', $comment['commentsourceid']);
    if ($pid) {
      $comment['pid'] = $pid;
    }
    else {
      $errors[] = migrate_message(t('Could not determine comment ID for !id',
        array('!id' => $comment['commentsourceid'])),
        MIGRATE_MESSAGE_WARNING);
    }
    unset($comment['commentsourceid']);
  }

  // Prepare the comment for import.
  $errors = array_merge($errors, migrate_destination_invoke_all('prepare_comment', $comment, $tblinfo, $row));

  /*
   * Validation: should probably create a validation hook instead
   */
  if (!isset($comment['nid'])) {
    $errors[] = migrate_message(t('No node found'));
  }
  if (!isset($comment['name'])) {
    $commentuser = user_load(array('uid' => $comment['uid']));
    $comment['name'] = $commentuser->name;
  }
  if (!isset($comment['name'])) {
    $errors[] = migrate_message(t('No user id or name found'));
  }

  // Do our best to interpret timestamps
  if (isset($comment['timestamp'])) {
    $comment['timestamp'] = _migrate_valid_date($comment['timestamp']);
    if ($comment['timestamp'] <= 0) {
      $errors[] = migrate_message(t('Provided timestamp is invalid'));
    }
  }

  //TODO: we should probably check actual errors, some may be info messages.
  if (count($errors) == 0) {
    $comment['cid'] = comment_save($comment);
  }

  $errors = array_merge($errors, migrate_destination_invoke_all('complete_comment', $comment, $tblinfo, $row));

  return $errors;
}

/**
 * Implementation of hook_migrate_complete_comment().
 */
function comment_migrate_complete_comment(&$comment, $tblinfo, $row) {
  $errors = array();
  if ($comment['cid']) {
    // comment_save() always saves time() as the timestamp, override if we have a value
    if ($comment['timestamp']) {
      db_query("UPDATE {comments}
                SET timestamp=%d, uid=%d, hostname='%s'
                WHERE cid=%d",
               $comment['timestamp'], $comment['uid'], $comment['hostname'], $comment['cid']);
    }
    $sourcekey = $tblinfo->sourcekey;
    migrate_add_mapping($tblinfo->mcsid, $row->$sourcekey, $comment['cid']);
  }
  else {
    $errors[] = migrate_message(t('Comment not saved'));
  }
  return $errors;
}
