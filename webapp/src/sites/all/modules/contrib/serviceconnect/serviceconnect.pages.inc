<?php

/**
 * Manage linked services page callback.
 */
function serviceconnect_manage_linked_services_page($account) {
  $output = '';

  $output .= '<div id="serviceconnect-manage-linked-services">';
  
  $header = array(
    t('Service'),
    t('Displayname'),
    t('Created'),
    t('Expires'),
    t('Last login'),
    t('Login attempts'),
    t('Actions'),
  );
  
  $rows = array();
  $sql = 'SELECT scpid, service, service_profile_url, displayname, created, expires, last_login, login_attempts
  FROM {serviceconnect_user}
  WHERE uid = %d
  ORDER BY last_login DESC';
  $res = db_query($sql, $account->uid);
  while ($rec = db_fetch_array($res)) {
    $rows[] = array(
      t($rec['service']),
      l($rec['displayname'], $rec['service_profile_url']),
      format_date($rec['created'], 'custom', 'j M Y'),
      $rec['expires'] > 2000000000 ? t('never') : format_date($rec['expires'], 'custom', 'j M Y'),
      format_date($rec['last_login'], 'custom', 'j M Y'),
      $rec['login_attempts'],
      l(t('unlink'), str_replace(array('%user', '%'), array($account->uid, $rec['scpid']), SERVICECONNECT_PATH_UNLINK_SERVICE)),
    );
  }

  if (!count($rows)) {
    $output .= '<p>' . t('There are no services linked yet.') . '</p>';
  }
  else {
    $output .= '<p>' . t('On this page you can manage the external services linked to your account.') . '</p>';
    $output .= theme('table', $header, $rows);
  }

  $output .= '</div>';

  return $output;
}

/**
 * Unlink service form.
 */
function serviceconnect_unlink_service_form($form_state, $account, $scpid) {
  $form = array();
  $form['#uid'] = $account->uid;
  $form['#scpid'] = $scpid;
  
  $form['info'] = array(
    '#value' => '<p>' . t('Are you sure you want to unlink the external service profile from this user?') . '</p>',
  );
  
  if (isset($account->internal_name) && preg_match('/^.+\\.[0-9a-f]{32,40}$/', $account->internal_name)) {
    // This account is created with an external service, check if this is the last linked service
    $sql = 'SELECT COUNT(*) FROM {serviceconnect_user} WHERE uid = %d';
    $linked_services = db_result(db_query($sql, $account->uid));
    if ($linked_services == 1) {
      $form['warning'] = array(
        '#value' => '<p>' . t('Warning: this is the only linked service to this user and this user does not have a username and password. This user cannot login anymore after unlinking this external profile!') . '</p>',
      );
    }
  }
  
  $form['buttons'] = array();
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Ok'),
  );
  $form['buttons']['cancel'] = array(
    '#value' => l(t('Cancel'), str_replace('%user', $account->uid, SERVICECONNECT_PATH_MANAGE_SERVICES)),
  );
  
  return $form;
}

/**
 * Unlink service form submit callback.
 */
function serviceconnect_unlink_service_form_submit($form, &$form_state) {
  $sql = 'DELETE FROM {serviceconnect_user} WHERE uid = %d AND scpid = %d';
  db_query($sql, $form['#uid'], $form['#scpid']);
  drupal_set_message(t('The external profile is now unlinked from this user.'));
  $form_state['redirect'] = str_replace('%user', $form['#uid'], SERVICECONNECT_PATH_MANAGE_SERVICES);
}