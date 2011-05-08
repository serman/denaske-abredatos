<?php
// $Id: kml_views.theme.inc,v 1.1.2.4 2010/10/28 02:44:14 tmcw Exp $

/**
 * @file
 * Theming functions for KML module views output.
 */

function template_preprocess_views_view_kml(&$vars) {
  $view = $vars['view'];
  
  $points = $view->style_plugin->map_rows($vars['rows']);

  $style = theme('kml_style', $points);

  foreach ($points as $point) {
    $rows .= theme('kml_placemark', $point, $points);
  }
     
  $vars['rows'] = $rows;
  $vars['style'] = $style;
  $vars['viewtitle'] = $view->display_handler->get_option('title');
    
  // Checks if filename is manually specified in view style options otherwise
  // it sets it as the name of the view.
  if (!empty($view->style_options['filename'])) {
    $filename = $view->style_options['filename'] . '.kml';
  }
  else {
    $filename = $vars['viewtitle'] . '.kml';
  }

  drupal_set_header('Content-Type: application/vnd.google-earth.kml+xml');
  drupal_set_header("Content-Disposition: attachment; filename=$filename");
}

/**
 * Preprocess for theme('kml_placemark').
 */
function template_preprocess_kml_placemark(&$vars) {  
  $vars['name'] = filter_xss_admin($vars['point']['name']);
  $vars['description'] = filter_xss_admin($vars['point']['description']);
  $vars['coords'] = check_plain($vars['point']['point']);
  $vars['styleUrl'] = check_plain($vars['point']['styleUrl']);
}

/**
 * Theme function for kml feed icon.
 */
function theme_kml_feed_icon($url, $title, $icon) {
  if ($image = theme('image', $icon, t('Download KML Feed'), $title)) {
    return '<a href="'. check_url($url) 
      .'" class="feed-icon">'. $image .'</a>';
  }
}
