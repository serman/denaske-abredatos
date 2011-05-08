<?php

/**
 * Search in friends
 *
 * @param string $searchterms Basic search query
 * @param string $token
 * @param string $token_secret
 * @return array Array with Associative arrays containing profile information
 * 
 * @see http://trac.hyves-api.nl/wiki/APIMethods/2.0/users.searchInFriends
 * 
 * Keys available in output:
 *  userid
 *  displayname
 *  firstname
 *  lastname
 *  gender
 *  friendscount
 *  service_profile_url
 */
function serviceconnect_hyves_call_users_searchInFriends($searchterms, $token = NULL, $token_secret = NULL) {
  if (empty($token) || empty($token_secret)) {
    if (!$token_info = serviceconnect_hyves_get_active_token()) {
      throw new Exception('Unable to get valid token');
    }
    extract($token_info);
  }
  
  if (strlen($searchterms) <= 2) {
    // Prevent "illegal searchterms" error from Hyves
    return array();
  }
  
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'users.searchInFriends';
  $params['ha_version'] = '2.0';
  $params['searchterms'] = $searchterms;
  $url = variable_get(SERVICECONNECT_HYVES_VAR_API_URL, SERVICECONNECT_HYVES_VAR_API_URL_DEFAULT);
  if (!$response = _serviceconnect_hyves_request($params, $url, $token, $token_secret)) {
    /**
     * @todo Check error code for "invalid searchterms"
     * Instead of throwing an exception, we just return 0 results to prevent
     * the common error "invalid searchterms", by example when you search on "$$$".
     */
    return array();
  }
  
  $profiles = array();
  $root_child = $response->documentElement->firstChild;
  do {
    if ($root_child->nodeName != 'user') {
      continue;
    }
    $profile = array();
    $user_child = $root_child->firstChild;
    do {
      switch ($user_child->nodeName) {
        case 'userid':
        case 'displayname':
        case 'firstname':
        case 'lastname':
        case 'gender':
        case 'friendscount':
          $profile[$user_child->nodeName] = $user_child->textContent;
          break;
        case 'url':
          $profile['service_profile_url'] = $user_child->textContent;
          break;
      }
    } while ($user_child = $user_child->nextSibling);
    $profiles[] = $profile;
  } while ($root_child = $root_child->nextSibling);
  return $profiles;
}