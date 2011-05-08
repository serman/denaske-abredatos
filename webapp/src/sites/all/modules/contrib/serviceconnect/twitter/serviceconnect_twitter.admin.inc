<?php

function serviceconnect_twitter_settings_form($form_state) {
  $form = array();
  
  $form['auth'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authentication settings'),
  );
  
  $form['auth'][SERVICECONNECT_TWITTER_VAR_CONSUMER_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth consumer key'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_CONSUMER_KEY, ''),
    '#description' => t('Available on %url', array('%url' => 'http://dev.twitter.com/apps')),
  );
  
  $form['auth'][SERVICECONNECT_TWITTER_VAR_CONSUMER_SECRET] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth consumer secret'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_CONSUMER_SECRET, ''),
    '#description' => t('Available on %url', array('%url' => 'http://dev.twitter.com/apps')),
  );
  
  $form['auth'][SERVICECONNECT_TWITTER_VAR_REQUEST_TOKEN_URL] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth request token url'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_REQUEST_TOKEN_URL, SERVICECONNECT_TWITTER_VAR_REQUEST_TOKEN_URL_DEFAULT),
    '#description' => t('See %url', array('%url' => 'http://dev.twitter.com/doc')),
  );
  
  $form['auth'][SERVICECONNECT_TWITTER_VAR_USER_AUTHORIZATION_URL] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth user authorization url'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_USER_AUTHORIZATION_URL, SERVICECONNECT_TWITTER_VAR_USER_AUTHORIZATION_URL_DEFAULT),
    '#description' => t('See %url', array('%url' => 'http://dev.twitter.com/doc')),
  );
  
  $form['auth'][SERVICECONNECT_TWITTER_VAR_ACCESS_TOKEN_URL] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth access token url'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_ACCESS_TOKEN_URL, SERVICECONNECT_TWITTER_VAR_ACCESS_TOKEN_URL_DEFAULT),
    '#description' => t('See %url', array('%url' => 'http://dev.twitter.com/doc')),
  );
  
  $form['legal'] = array(
    '#type' => 'fieldset',
    '#title' => t('Legal settings'),
  );
  
  $form['legal'][SERVICECONNECT_TWITTER_VAR_TERMS] = array(
    '#type' => 'textarea',
    '#title' => t('Terms and conditions for Twitter login'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_TERMS, ''),
  );
  
  $form['profile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Profile synchronization settings'),
  );
  
  $options = array(
    '' => 'do not download profilepicture',
    'normal' => 'normal (48x48)',
    'bigger' => 'bigger (73x73)',
  );
  $form['profile'][SERVICECONNECT_TWITTER_VAR_PROFILEPICTURE_SIZE] = array(
    '#type' => 'select',
    '#title' => t('Profilepicture size'),
    '#options' => $options,
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_PROFILEPICTURE_SIZE, SERVICECONNECT_TWITTER_VAR_PROFILEPICTURE_SIZE_DEFAULT),
    '#description' => t('Choose which size to download from Twitter'),
  );

  $form['profile'][SERVICECONNECT_TWITTER_VAR_PROFILEPICTURE_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Avatar'),
    '#options' => serviceconnect_get_profile_fields('file'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_PROFILEPICTURE_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_TWITTER_VAR_LINK_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Link to profile'),
    '#options' => serviceconnect_get_profile_fields('link'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_LINK_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_TWITTER_VAR_NAME_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Name'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_NAME_PROFILE_FIELD, ''),
  );
  
  $form['profile'][SERVICECONNECT_TWITTER_VAR_LOCATION_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Location'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_LOCATION_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_TWITTER_VAR_DESCRIPTION_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Description'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_DESCRIPTION_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_TWITTER_VAR_WEBSITE_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Website'),
    '#options' => serviceconnect_get_profile_fields('link'),
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_WEBSITE_PROFILE_FIELD, ''),
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
  $form['profile'][SERVICECONNECT_TWITTER_VAR_FRIENDS_RELATIONSHIP_TYPE] = array(
    '#type' => 'select',
    '#title' => t('Friends relationship type'),
    '#options' => $options,
    '#default_value' => variable_get(SERVICECONNECT_TWITTER_VAR_FRIENDS_RELATIONSHIP_TYPE, ''),
  );
  
  return system_settings_form($form);
}

/**
 * Menu callback.
 *
 * Generates the test connection page.
 */
function serviceconnect_twitter_admin_test_page() {
  global $_serviceconnect_twitter_last_response;
  
  $form = array();
  $params = array();
//  $params['oauth_callback'] = 'http://serviceconnect.nl/login-via-twitter';
  $url = variable_get(SERVICECONNECT_TWITTER_VAR_REQUEST_TOKEN_URL, SERVICECONNECT_TWITTER_VAR_REQUEST_TOKEN_URL_DEFAULT);
  if (!_serviceconnect_twitter_request($params, $url)) {
    drupal_set_message(t('Failed to connect to Twitter'), 'warning');
  }
  else {
    drupal_set_message(t('Succesfully connected to Twitter'));
  }
  
  if (!$_serviceconnect_twitter_last_response) {
    $form['info'] = array(
      '#value' => '<p>' . t('No response') . '</p>',
    );
  }
  else {
    switch ($_serviceconnect_twitter_last_response->code) {
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
        $message = $_serviceconnect_twitter_last_response->code;
        break;
    }
    $form['info'] = array(
      '#value' => '<p>' . t('HTTP response code is: %code', array('%code' => $message)) . '</p>',
    );
    $form['data'] = array(
      '#type' => 'textarea',
      '#title' => t('Response data'),
      '#value' => $_serviceconnect_twitter_last_response->data,
    );
  }
  
  return $form;
}