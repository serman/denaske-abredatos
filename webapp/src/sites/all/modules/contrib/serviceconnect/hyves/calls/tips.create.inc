<?php

function serviceconnect_hyves_call_tips_create($title, $body, $rating, $tipcategoryid = '00be7fa4a7eb16282922bb1236a139cfe3', $token = NULL, $token_secret = NULL) {
  if (empty($token) || empty($token_secret)) {
    if (!$token_info = serviceconnect_hyves_get_active_token()) {
      throw new Exception('Unable to get valid token');
    }
    extract($token_info);
  }
  
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'tips.create';
  //$params['ha_method'] = 'tips.getCategories';
  $params['ha_version'] = '2.0';
  $params['title'] = $title;
  $params['body'] = $body;
  $params['tipcategoryid'] = $tipcategoryid;
  $params['rating'] = $rating;
  $url = variable_get(SERVICECONNECT_HYVES_VAR_API_URL, SERVICECONNECT_HYVES_VAR_API_URL_DEFAULT);
  if (!$response = _serviceconnect_hyves_request($params, $url, $token, $token_secret)) {
    throw new Exception('Error from hyves_request()');
  }
  return TRUE;
}