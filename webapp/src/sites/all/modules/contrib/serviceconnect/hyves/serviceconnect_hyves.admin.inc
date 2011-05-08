<?php

/**
 * Admin settings form.
 */
function serviceconnect_hyves_settings_form($form_state) {
  $form = array();
  
  $form['auth'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authentication settings'),
  );
  
  $form['auth'][SERVICECONNECT_HYVES_VAR_CONSUMER_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth consumer key'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_CONSUMER_KEY, ''),
    '#description' => t('Available on %url', array('%url' => 'http://www.hyves.nl/api/manage')),
  );
  
  $form['auth'][SERVICECONNECT_HYVES_VAR_CONSUMER_SECRET] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth consumer secret'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_CONSUMER_SECRET, ''),
    '#description' => t('Available on %url', array('%url' => 'http://www.hyves.nl/api/manage')),
  );
  
  $form['auth'][SERVICECONNECT_HYVES_VAR_REQUEST_TOKEN_URL] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth request token url'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_REQUEST_TOKEN_URL, SERVICECONNECT_HYVES_VAR_REQUEST_TOKEN_URL_DEFAULT),
    '#description' => t('See %url', array('%url' => 'http://trac.hyves-api.nl/wiki/APIUserAuthorization')),
  );
  
  $form['auth'][SERVICECONNECT_HYVES_VAR_USER_AUTHORIZATION_URL] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth user authorization url'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_USER_AUTHORIZATION_URL, SERVICECONNECT_HYVES_VAR_USER_AUTHORIZATION_URL_DEFAULT),
    '#description' => t('See %url', array('%url' => 'http://trac.hyves-api.nl/wiki/APIUserAuthorization')),
  );
  
  $form['auth'][SERVICECONNECT_HYVES_VAR_ACCESS_TOKEN_URL] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth access token url'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_ACCESS_TOKEN_URL, SERVICECONNECT_HYVES_VAR_ACCESS_TOKEN_URL_DEFAULT),
    '#description' => t('See %url', array('%url' => 'http://trac.hyves-api.nl/wiki/APIUserAuthorization')),
  );
  
  $form['auth'][SERVICECONNECT_HYVES_VAR_API_URL] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth API url'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_API_URL, SERVICECONNECT_HYVES_VAR_API_URL_DEFAULT),
    '#description' => t('See %url', array('%url' => 'http://trac.hyves-api.nl/wiki/APIUserAuthorization')),
  );
  
  $options = array(
    'default' => 'Default token, valid for 1 hour',
    'infinite' => 'Infinite token, if User accepts valid for 2 years',
    'user' => 'The User chooses the expiredate for the token',
  );
  $form['auth'][SERVICECONNECT_HYVES_VAR_EXPIRATION_TYPE] = array(
    '#type' => 'radios',
    '#title' => t('OAuth access token expiration type'),
    '#options' => $options,
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_EXPIRATION_TYPE, SERVICECONNECT_HYVES_VAR_EXPIRATION_TYPE_DEFAULT),
    '#description' => t('See %url', array('%url' => 'http://trac.hyves-api.nl/wiki/APIMethods/1.0/auth.requesttoken')),
  );
  
  $form['legal'] = array(
    '#type' => 'fieldset',
    '#title' => t('Legal settings'),
  );
  
  $form['legal'][SERVICECONNECT_HYVES_VAR_TERMS] = array(
    '#type' => 'textarea',
    '#title' => t('Terms and conditions for Hyves login'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_TERMS, ''),
  );
  
  $form['profile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Profile synchronization settings'),
  );

  $form['profile'][SERVICECONNECT_HYVES_VAR_LINK_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Link to profile'),
    '#options' => serviceconnect_get_profile_fields('link'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_LINK_PROFILE_FIELD, ''),
  );
  
  $options = array(
    '' => 'do not download profilepicture',
    'icon_small' => 'icon_small (max 50x50)',
    'icon_medium' => 'icon_medium (max 75x75)',
    'icon_large' => 'icon_large (max 120x120)',
    'icon_extralarge' => 'icon_extralarge (max 200x200)',
    'image' => 'image',
    'image_fullscreen' => 'image_fullscreen',
    'square_large' => 'square_large (75x75)',
    'square_extralarge' => 'square_extralarge (120x120)',
  );
  $form['profile'][SERVICECONNECT_HYVES_VAR_PROFILEPICTURE_SIZE] = array(
    '#type' => 'select',
    '#title' => t('Profilepicture size'),
    '#options' => $options,
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_PROFILEPICTURE_SIZE, SERVICECONNECT_HYVES_VAR_PROFILEPICTURE_SIZE_DEFAULT),
    '#description' => t('Choose which size to download from Hyves'),
  );

  $form['profile'][SERVICECONNECT_HYVES_VAR_PROFILEPICTURE_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Avatar'),
    '#options' => serviceconnect_get_profile_fields('file'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_PROFILEPICTURE_PROFILE_FIELD, ''),
  );
  
  $form['profile'][SERVICECONNECT_HYVES_VAR_FIRSTNAME_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Firstname'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_FIRSTNAME_PROFILE_FIELD, ''),
  );
  
  $form['profile'][SERVICECONNECT_HYVES_VAR_LASTNAME_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Lastname'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_LASTNAME_PROFILE_FIELD, ''),
  );
  
  $form['profile'][SERVICECONNECT_HYVES_VAR_GENDER_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Gender'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_GENDER_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_HYVES_VAR_GENDER_PROFILE_FIELD_MALE] = array(
    '#type' => 'textfield',
    '#title' => t('Text to use for male'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_GENDER_PROFILE_FIELD_MALE, 'male'),
  );

  $form['profile'][SERVICECONNECT_HYVES_VAR_GENDER_PROFILE_FIELD_FEMALE] = array(
    '#type' => 'textfield',
    '#title' => t('Text to use for female'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_GENDER_PROFILE_FIELD_FEMALE, 'female'),
  );

  $form['profile'][SERVICECONNECT_HYVES_VAR_BIRTHDAY_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Birthday'),
    '#options' => serviceconnect_get_profile_fields('date'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_BIRTHDAY_PROFILE_FIELD, ''),
  );
  
  $form['profile'][SERVICECONNECT_HYVES_VAR_CITYNAME_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('City'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_CITYNAME_PROFILE_FIELD, ''),
  );
  
  $options = array('' => t('Do not synchronize friends'));
  if (module_exists('user_relationships_api')) {
    $sql = 'SELECT urt.rtid, urt.name
    FROM {user_relationship_types} urt
    ORDER BY urt.name ASC';
    $res = db_query($sql);
    while ($rec = db_fetch_array($res)) {
      $options[$rec['rtid']] = $rec['name'];
    }
  }
  
  $form['profile'][SERVICECONNECT_HYVES_VAR_FRIENDS_RELATIONSHIP_TYPE] = array(
    '#type' => 'select',
    '#title' => t('Friends relationship type'),
    '#options' => $options,
    '#default_value' => variable_get(SERVICECONNECT_HYVES_VAR_FRIENDS_RELATIONSHIP_TYPE, ''),
  );
  
  return system_settings_form($form);
}

/**
 * Menu callback.
 *
 * Generates the test connection page.
 */
function serviceconnect_hyves_admin_test_page() {
  global $_serviceconnect_hyves_last_response;
  
  $form = array();
  
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'auth.requesttoken';
  $params['ha_version'] = '1.2';
  $params['methods'] = 'users.get,friends.get';
  $url = variable_get(SERVICECONNECT_HYVES_VAR_ACCESS_TOKEN_URL, SERVICECONNECT_HYVES_VAR_ACCESS_TOKEN_URL_DEFAULT);
  if (!_serviceconnect_hyves_request($params, $url)) {
    drupal_set_message(t('Failed to connect to Hyves'), 'warning');
  }
  else {
    drupal_set_message(t('Succesfully connected to Hyves'));
  }
  
  if (!$_serviceconnect_hyves_last_response) {
    $form['info'] = array(
      '#value' => '<p>' . t('No response') . '</p>',
    );
  }
  else {
    switch ($_serviceconnect_hyves_last_response->code) {
      case '200':
        $message = '200 OK';
        break;
      case '400':
        $message = '400 Bad Request';
        break;
      case '401':
        $message = '401 Unauthorised';
        break;
      case '404':
        $message = '404 Not Found (request token url is invalid)';
        break;
      default:
        $message = $_serviceconnect_hyves_last_response->code;
        break;
    }
    $form['info'] = array(
      '#value' => '<p>' . t('HTTP response code is: %code', array('%code' => $message)) . '</p>',
    );
    $form['data'] = array(
      '#type' => 'textarea',
      '#title' => t('Response data'),
      '#value' => $_serviceconnect_hyves_last_response->data,
    );
  }
  
  return $form;
}