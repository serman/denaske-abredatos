<?php
// $Id: filefield.migrate.inc,v 1.1.2.1 2010/03/25 19:35:28 textplease Exp $

/**
 * @file
 * Hooks for handling FileFields in destination nodes.
 */

/**
 * Implementation of hook_migrate_prepare_node().
 */
function filefield_migrate_prepare_node(&$node, $tblinfo, $row) {
  $errors = array();
  module_load_include('inc', 'filefield', 'field_file');

  foreach (filefield_migrate_fields_node($node->type) as $source_key => $label) {
    if (isset($node->$source_key)) {
      $source_file = $node->$source_key;
      $field_name = preg_replace('/_sourcefile$/', '', $source_key);

      $field = content_fields($field_name, $tblinfo->desttype);
      $validators = filefield_widget_upload_validators($field);
      if ($field['widget']['type'] == 'imagefield_widget') {
        $validators = array_merge($validators, imagefield_widget_upload_validators($field));
      }
      $files_path = _migrate_widget_files_directory($field);

      if ($file = field_file_save_file($source_file, $validators, $files_path)) {
        if (!isset($node->$field_name)) {
          $node->$field_name = array();
        }
        array_push($node->$field_name, $file);
      }
      else {
        $errors[] = migrate_message(t('File save failed for %file', array('%file' => $source_file)));
      }
    }
  }
  return $errors;
}

/**
 * Determine the widget's files directory
 *
 * @param $field CCK field
 * @return files directory path.
 */
function _migrate_widget_files_directory($field) {
  $widget_file_path = $field['widget']['file_path'];
  if (module_exists('token')) {
    global $user;
    $widget_file_path = token_replace($widget_file_path, 'user', $user);
  }
  return file_directory_path() .'/'. $widget_file_path;
}

/**
 * Implementation of hook_migrate_fields_node().
 */
function filefield_migrate_fields_node($type) {
  $types = (array) content_types();
  $fields = array();

  if (isset($types[$type])) {
    $content_type = $types[$type];
    foreach ($content_type['fields'] as $field) {
      if ($field['type'] == 'filefield') {
        $fields[$field['field_name'] .'_sourcefile'] = t('FileField: @label source file path', array('@label' => $field['widget']['label']));
      }
    }
  }
  return $fields;
}
