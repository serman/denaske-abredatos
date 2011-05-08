<?php

function serviceconnect_facebook_settings_form($form_state) {
  $form = array();
  
  $form['auth'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authentication settings'),
  );
  
  $form['auth'][SERVICECONNECT_FACEBOOK_VAR_API_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('API key'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_API_KEY, ''),
    '#description' => t('Available on %url', array('%url' => 'http://www.facebook.com/developers/apps.php')),
  );
  
  $form['auth'][SERVICECONNECT_FACEBOOK_VAR_APPLICATION_SECRET] = array(
    '#type' => 'textfield',
    '#title' => t('Application secret'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_APPLICATION_SECRET, ''),
    '#description' => t('Available on %url', array('%url' => 'http://www.facebook.com/developers/apps.php')),
  );
  
  $form['auth'][SERVICECONNECT_FACEBOOK_VAR_APPLICATION_ID] = array(
    '#type' => 'textfield',
    '#title' => t('Application ID'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_APPLICATION_ID, ''),
    '#description' => t('Available on %url', array('%url' => 'http://www.facebook.com/developers/apps.php')),
  );
  
  $form['auth'][SERVICECONNECT_FACEBOOK_VAR_GRAPH_API_URL] = array(
    '#type' => 'textfield',
    '#title' => t('Graph API URL'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_GRAPH_API_URL, SERVICECONNECT_FACEBOOK_VAR_GRAPH_API_URL_DEFAULT),
  );
  
  $form['auth'][SERVICECONNECT_FACEBOOK_VAR_LOGIN_URL] = array(
    '#type' => 'textfield',
    '#title' => t('Login url'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_LOGIN_URL, SERVICECONNECT_FACEBOOK_VAR_LOGIN_URL_DEFAULT),
  );
  
  $form['legal'] = array(
    '#type' => 'fieldset',
    '#title' => t('Legal settings'),
  );
  
  $form['legal'][SERVICECONNECT_FACEBOOK_VAR_TERMS] = array(
    '#type' => 'textarea',
    '#title' => t('Terms and conditions for Facebook login'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_TERMS, ''),
  );
  
  $form['profile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Profile synchronization settings'),
  );

  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_LINK_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Link to profile'),
    '#options' => serviceconnect_get_profile_fields('link'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_LINK_PROFILE_FIELD, ''),
  );
  
  $options = array(
    '' => 'do not download profilepicture',
    'square' => 'square (50x50)',
    'small' => 'small (50px width)',
    'large' => 'large (200px width)',
  );
  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_PROFILEPICTURE_SIZE] = array(
    '#type' => 'select',
    '#title' => t('Profilepicture size'),
    '#options' => $options,
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_PROFILEPICTURE_SIZE, SERVICECONNECT_FACEBOOK_VAR_PROFILEPICTURE_SIZE_DEFAULT),
    '#description' => t('Choose which size to download from Facebook'),
  );
  
  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_PROFILEPICTURE_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Avatar'),
    '#options' => serviceconnect_get_profile_fields('file'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_PROFILEPICTURE_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_NAME_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Name'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_NAME_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_FIRST_NAME_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('First name'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_FIRST_NAME_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_LAST_NAME_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Last name'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_LAST_NAME_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_BIRTHDAY_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Birthdate'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_BIRTHDAY_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_GENDER_PROFILE_FIELD] = array(
    '#type' => 'select',
    '#title' => t('Gender'),
    '#options' => serviceconnect_get_profile_fields('text'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_GENDER_PROFILE_FIELD, ''),
  );

  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_GENDER_PROFILE_FIELD_MALE] = array(
    '#type' => 'textfield',
    '#title' => t('Text to use for male'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_GENDER_PROFILE_FIELD_MALE, 'male'),
  );

  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_GENDER_PROFILE_FIELD_FEMALE] = array(
    '#type' => 'textfield',
    '#title' => t('Text to use for female'),
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_GENDER_PROFILE_FIELD_FEMALE, 'female'),
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
  $form['profile'][SERVICECONNECT_FACEBOOK_VAR_FRIENDS_RELATIONSHIP_TYPE] = array(
    '#type' => 'select',
    '#title' => t('Friends relationship type'),
    '#options' => $options,
    '#default_value' => variable_get(SERVICECONNECT_FACEBOOK_VAR_FRIENDS_RELATIONSHIP_TYPE, ''),
  );
  
  return system_settings_form($form);
}
