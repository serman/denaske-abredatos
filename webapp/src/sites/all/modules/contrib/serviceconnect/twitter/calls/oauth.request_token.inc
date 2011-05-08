<?php

function serviceconnect_twitter_call_oauth_request_token($callback = NULL) {
  $params = array();
  if ($callback) {
    $params['oauth_callback'] = $callback;
  }
  $url = variable_get(SERVICECONNECT_TWITTER_VAR_REQUEST_TOKEN_URL, SERVICECONNECT_TWITTER_VAR_REQUEST_TOKEN_URL_DEFAULT);
  if (!$response = _serviceconnect_twitter_request($params, $url, 'POST')) {
    throw new Exception('Unable to get requesttoken');
  }
  $variables = array();
  $response = explode('&', $response);
  foreach ($response as $variable) {
    list($name, $value) = explode('=', $variable);
    $variables[urldecode($name)] = urldecode($value);
  }
  return $variables;
}