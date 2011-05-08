<?php

function serviceconnect_hyves_call_tips_getcategories($token = NULL, $token_secret = NULL) {
  if (empty($token) || empty($token_secret)) {
    if (!$token_info = serviceconnect_hyves_get_active_token()) {
      throw new Exception('Unable to get valid token');
    }
    extract($token_info);
  }
  
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'tips.getCategories';
  $params['ha_version'] = '2.0';
  $url = variable_get(SERVICECONNECT_HYVES_VAR_API_URL, SERVICECONNECT_HYVES_VAR_API_URL_DEFAULT);
  if (!$response = _serviceconnect_hyves_request($params, $url, $token, $token_secret)) {
    global $_serviceconnect_hyves_last_response;
    throw new Exception('Error from hyves_request()');
  }
  $categories = array();
  $root = $response->documentElement;
  if (!$root->hasChildNodes()) {
    return FALSE;
  }
  $child = $root->firstChild;
  do {
    if (!$child->nodeName == 'tipcategory') {
      continue;
    }
    if (!$child->hasChildNodes()) {
      return;
    }
    $tipcategoryid = NULL;
    $name = NULL;
    $category_child = $child->firstChild;
    do {
      switch ($category_child->nodeName) {
        case 'tipcategoryid':
          $tipcategoryid = $category_child->textContent;
          break;
        case 'name':
          $name = $category_child->textContent;
          break;
      }
    } while ($category_child = $category_child->nextSibling);
    if (!empty($tipcategoryid) && !empty($name)) {
      $categories[$tipcategoryid] = $name;
    }
  } while ($child = $child->nextSibling);
  return $categories;
}