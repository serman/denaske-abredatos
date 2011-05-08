<?php

function serviceconnect_twitter_call_oauth_access_token($token, $token_secret) {
  $params = array();
  $url = variable_get(SERVICECONNECT_TWITTER_VAR_ACCESS_TOKEN_URL, SERVICECONNECT_TWITTER_VAR_ACCESS_TOKEN_URL_DEFAULT);
  if (!$response = _serviceconnect_twitter_request($params, $url, 'POST', $token, $token_secret)) {
    throw new Exception('Could not get access token');
  }
  $variables = array();
  $response = explode('&', $response);
  foreach ($response as $variable) {
    list($name, $value) = explode('=', $variable);
    $variables[urldecode($name)] = urldecode($value);
  }
  return array(
    'token' => $variables['oauth_token'],
    'token_secret' => $variables['oauth_token_secret'],
    'service_user_id' => $variables['user_id'],
  );
}