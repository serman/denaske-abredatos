<?php

/**
 * Request an accesstoken.
 *
 * @param string $token
 * @param string $token_secret
 * @return array
 */
function serviceconnect_hyves_call_auth_accesstoken($token, $token_secret) {
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'auth.accesstoken';
  $params['ha_version'] = '2.0';
  $url = variable_get(SERVICECONNECT_HYVES_VAR_ACCESS_TOKEN_URL, SERVICECONNECT_HYVES_VAR_ACCESS_TOKEN_URL_DEFAULT);
  if (!$response = _serviceconnect_hyves_request($params, $url, $token, $token_secret)) {
    throw new Exception('Could not get access token');
  }
  
  $response->formatOutput = TRUE;
  $values = _serviceconnect_hyves_values_from_xml($response);
  
  return array(
    'token' => $values['oauth_token'],
    'token_secret' => $values['oauth_token_secret'],
    'service_user_id' => $values['userid'],
    'expire_date' => (int) $values['expiredate'] - (int) $session->timestamp_difference
  );
}