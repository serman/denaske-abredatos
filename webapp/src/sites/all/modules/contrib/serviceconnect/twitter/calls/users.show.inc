<?php

function serviceconnect_twitter_call_users_show($user_id, $token, $token_secret) {
  $params = array();
  $params['user_id'] = $user_id;
  $url = 'http://api.twitter.com/1/users/show.json';
  if (!$response = _serviceconnect_twitter_request($params, $url, 'GET', $token, $token_secret)) {
    throw new Exception('Unable to retreive user profile');
  }
  $variables = array();
  $response = json_decode($response);
  return array(
    'friends_count' => $response->friends_count,
    'profile_image_url' => $response->profile_image_url,
    'screen_name' => $response->screen_name,
    'name' => $response->name,
    'statuses_count' => $response->statuses_count,
    'id' => $response->id,
    'followers_count' => $response->followers_count,
    'website' => $response->url,
    'description' => $response->description,
    'location' => $response->location,
  );
}