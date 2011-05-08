<?php

/**
 * Get list of friends.
 *
 * @param string $token
 * @param string $token_secret
 * @return array
 */
function serviceconnect_hyves_call_friends_get($token, $token_secret) {
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'friends.get';
  $params['ha_version'] = '2.0';
  $url = variable_get(SERVICECONNECT_HYVES_VAR_API_URL, SERVICECONNECT_HYVES_VAR_API_URL_DEFAULT);
  if (!$response = _serviceconnect_hyves_request($params, $url, $token, $token_secret)) {
    throw new Exception('Error from hyves_request()');
  }
  
  // Get profile information from response
  $friends = array();
  $child = $response->documentElement->firstChild;
  do {
    if ($child->nodeName != 'userid') {
      continue;
    }
    $friends[] = $child->textContent;
  } while ($child = $child->nextSibling);
  return $friends;
}