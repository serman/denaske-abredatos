<?php

/**
 * Request a requesttoken.
 *
 * @return array
 */
function serviceconnect_hyves_call_auth_requesttoken() {
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'auth.requesttoken';
  $params['ha_version'] = '2.0';
  $params['methods'] = 'users.get,users.getByUsername,users.searchInFriends,friends.get,wwws.create,tips.create,tips.getCategories,gadgets.create';
  if (variable_get(SERVICECONNECT_HYVES_FRAGMENTS_VAR_ENABLE_SENDAFRIEND, SERVICECONNECT_HYVES_FRAGMENTS_VAR_ENABLE_SENDAFRIEND_DEFAULT)) {
    $params['methods'] .= ',messages.sendToUser';
  }
  $params['expirationtype'] = variable_get(SERVICECONNECT_HYVES_VAR_EXPIRATION_TYPE, SERVICECONNECT_HYVES_VAR_EXPIRATION_TYPE_DEFAULT);
  $url = variable_get(SERVICECONNECT_HYVES_VAR_REQUEST_TOKEN_URL, SERVICECONNECT_HYVES_VAR_REQUEST_TOKEN_URL_DEFAULT);
  if (!$response = _serviceconnect_hyves_request($params, $url)) {
    throw new Exception('Unable to get requesttoken');
  }
  return _serviceconnect_hyves_values_from_xml($response);
}