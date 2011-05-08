<?php

function serviceconnect_hyves_call_messages_sendToUser($title, $body, $target_userid, $token = NULL, $token_secret = NULL) {
  if (empty($token) || empty($token_secret)) {
    if (!$token_info = serviceconnect_hyves_get_active_token()) {
      throw new Exception('Unable to get valid token');
    }
    extract($token_info);
  }
  
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'messages.sendToUser';
  $params['ha_version'] = '1.2.1';
  $params['title'] = $title;
  $params['body'] = $body;
  $params['target_userid'] = $target_userid;
  $url = variable_get(SERVICECONNECT_HYVES_VAR_API_URL, SERVICECONNECT_HYVES_VAR_API_URL_DEFAULT);
  if (!$response = _serviceconnect_hyves_request($params, $url, $token, $token_secret)) {
    throw new Exception('Error from hyves_request()');
  }
  
  return TRUE;
}