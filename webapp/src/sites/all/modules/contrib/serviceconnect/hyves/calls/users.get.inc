<?php

/**
 * Get profile information for are given Hyves profile
 *
 * @param string $user_id Internal Hyves user id
 * @param string $token
 * @param string $token_secret
 * @return array Associative array with profile information
 * 
 * Keys available in output:
 *  displayname
 *  firstname
 *  lastname
 *  gender
 *  birthday
 *  friendscount
 *  service_profile_url
 *  cityname
 *  profilepicture_mediaid
 *  profilepicture_title
 *  profilepicture_description
 *  profilepicture_width
 *  profilepicture_height
 *  profilepicture_src
 */
function serviceconnect_hyves_call_users_get($user_id, $token, $token_secret) {
  $params = array();
  $params['ha_fancylayout'] = 'FALSE';
  $params['ha_format'] = 'xml';
  $params['ha_method'] = 'users.get';
  $params['ha_version'] = '2.0';
  $params['ha_responsefields'] = 'profilepicture,cityname';
  $params['userid'] = $user_id;
  $url = variable_get(SERVICECONNECT_HYVES_VAR_API_URL, SERVICECONNECT_HYVES_VAR_API_URL_DEFAULT);
  if (!$response = _serviceconnect_hyves_request($params, $url, $token, $token_secret)) {
    throw new Exception('Error from hyves_request()');
  }
  
  // Get profile information from response
  $profile = array();
  $root_child = $response->documentElement->firstChild;
  do {
    if ($root_child->nodeName != 'user') {
      continue;
    }
    $user_child = $root_child->firstChild;
    do {
      switch ($user_child->nodeName) {
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
  } while ($root_child = $root_child->nextSibling);
  return $profile;
}