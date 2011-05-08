<?php

/**
 * Get profile information for are given Hyves profile
 *
 * @param string $usernames Comma serparated list of usernames
 * @param string $token
 * @param string $token_secret
 * @return array Array with Associative arrays containing profile information
 * 
 * Keys available in output:
 *  userid
 *  displayname
 *  firstname
 *  lastname
 *  gender
 *  birthday
 *  friendscount
 *  service_profile_url
 */
function serviceconnect_hyves_call_users_getByUsername($usernames, $token = NULL, $token_secret = NULL) {
  if (empty($token) || empty($token_secret)) {
    if (!$token_info = serviceconnect_hyves_get_active_token()) {
      throw new Exception('Unable to get valid token');
    }
    extract($token_info);
  }
  
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'users.getByUsername';
  $params['ha_version'] = '2.0';
  $params['username'] = str_replace(' ', '', $usernames);
  $url = variable_get(SERVICECONNECT_HYVES_VAR_API_URL, SERVICECONNECT_HYVES_VAR_API_URL_DEFAULT);
  if (!$response = _serviceconnect_hyves_request($params, $url, $token, $token_secret)) {
    throw new Exception('Error from hyves_request()');
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
        case 'cityname':
          $profile[$user_child->nodeName] = $user_child->textContent;
          break;
        case 'url':
          $profile['service_profile_url'] = $user_child->textContent;
          break;
        case 'birthday':
          $birthday = array();
          $child = $user_child->firstChild;
          do {
            $birthday[$child->nodeName] = $child->textContent;
          } while ($child = $child->nextSibling);
          $profile['birthday'] = $birthday['year']
             . '-' . str_pad($birthday['month'], 2, '0', STR_PAD_LEFT)
             . '-' . str_pad($birthday['day'], 2, '0', STR_PAD_LEFT);
          break;
        case 'profilepicture':
          $child = $user_child->firstChild;
          do {
            switch ($child->nodeName) {
              case 'mediaid':
              case 'title':
              case 'description':
                $profile['profilepicture_' . $child->nodeName] = $child->textContent;
                break;
              default:
                if ($child->nodeName == variable_get(SERVICECONNECT_HYVES_VAR_PROFILEPICTURE_SIZE, SERVICECONNECT_HYVES_VAR_PROFILEPICTURE_SIZE_DEFAULT)) {
                  $profilepicture_child = $child->firstChild;
                  do {
                    $profile['profilepicture_' . $profilepicture_child->nodeName] = $profilepicture_child->textContent;
                  } while ($profilepicture_child = $profilepicture_child->nextSibling);
                }
                break;
            }
          } while ($child = $child->nextSibling);
          break;
      }
    } while ($user_child = $user_child->nextSibling);
    $profiles[] = $profile;
  } while ($root_child = $root_child->nextSibling);
  return $profiles;
}