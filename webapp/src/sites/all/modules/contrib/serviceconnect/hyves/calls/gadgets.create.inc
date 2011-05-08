<?php

/**
 * Create a gadget.
 *
 * @param string $title
 * @param string $html
 * @param string $visibility
 * @param bool $maycopy
 * @param string $token
 * @param string $token_secret
 * @return bool
 */
function serviceconnect_hyves_call_gadgets_create($title, $html, $visibility = 'friend', $maycopy = TRUE, $token = NULL, $token_secret = NULL) {
  if (empty($token) || empty($token_secret)) {
    if (!$token_info = serviceconnect_hyves_get_active_token()) {
      throw new Exception('Unable to get valid token');
    }
    extract($token_info);
  }
  
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'gadgets.create';
  $params['ha_version'] = '2.0';
  $params['title'] = $title;
  $params['html'] = $html;
  $params['visibility'] = $visibility;
  $params['maycopy'] = $maycopy ? 'TRUE' : 'FALSE';
  $url = variable_get(SERVICECONNECT_HYVES_VAR_API_URL, SERVICECONNECT_HYVES_VAR_API_URL_DEFAULT);
  if (!$response = _serviceconnect_hyves_request($params, $url, $token, $token_secret)) {
    //global $_serviceconnect_hyves_last_response;
    //var_dump($_serviceconnect_hyves_last_response);
    //exit;
    throw new Exception('Error from hyves_request()');
  }
  
  return TRUE;
}