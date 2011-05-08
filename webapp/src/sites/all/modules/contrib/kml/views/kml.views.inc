<?php
// $Id: kml.views.inc,v 1.1.2.1 2010/03/05 19:06:22 tmcw Exp $

/**
 * @file
 * Views plugins definition for views_kml
 */

/**
 * Implementation of hook_views_style_plugins(). 
 * Adds view types to views UI interface.
 */
function kml_views_plugins() {
  return array(
    'style' => array(
      'kml_feed' => array(
        'title' => t('KML Feed'),
        'help' => t('Outputs a KML formatted feed'),
        'handler' => 'views_plugin_style_kml',
        'theme' => 'views_view_kml',
        'theme file' => 'kml_views.theme.inc',
        'theme path' => drupal_get_path('module', 'kml') .'/views',
        'path' => drupal_get_path('module', 'kml') .'/views',
        'uses row plugin' => FALSE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'uses grouping' => FALSE,
        'type' => 'feed',
      ),
    ),
  );
}
