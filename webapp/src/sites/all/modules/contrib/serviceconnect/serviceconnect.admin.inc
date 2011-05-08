<?php

/**
 * Admin settings form.
 */
function serviceconnect_settings_form($form_state) {
  $form = array();
  
  $form[SERVICECONNECT_VAR_REDIRECT_AFTER_FIRST_USE] = array(
    '#type' => 'checkbox',
    '#title' => t('Redirect user to edit profile after first login'),
    '#default_value' => variable_get(SERVICECONNECT_VAR_REDIRECT_AFTER_FIRST_USE, SERVICECONNECT_VAR_REDIRECT_AFTER_FIRST_USE_DEFAULT),
    '#description' => t('Asks user to fill in missing profile information (including e-mail).'),
  );

  $form[SERVICECONNECT_VAR_COLLECT_STATISTICS] = array(
    '#type' => 'checkbox',
    '#title' => t('Collect statistics'),
    '#default_value' => variable_get(SERVICECONNECT_VAR_COLLECT_STATISTICS, SERVICECONNECT_VAR_COLLECT_STATISTICS_DEFAULT),
    '#description' => t('Count number of logins, new users registered etc.'),
  );
  
  return system_settings_form($form);
}

/**
 * Statistics page callback.
 */
function serviceconnect_statistics_page() {
  $header = array(
    t('event'),
    t('count'),
  );
  $rows = array();
  $sql = 'SELECT name, count FROM {serviceconnect_counter} ORDER BY name';
  $res = db_query($sql);
  while ($rec = db_fetch_array($res)) {
    $rows[] = array($rec['name'], $rec['count']);
  }
  return theme('table', $header, $rows);
}