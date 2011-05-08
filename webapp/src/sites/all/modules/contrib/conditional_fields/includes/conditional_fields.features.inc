<?php

/**
 * Implementation of hook_features_export_options().
 */
function conditional_fields_features_export_options() {
  $query = db_query("SELECT type FROM {conditional_fields}");
  while ($result = db_fetch_array($query)) {
    if (!isset($options['type'])) {
      $options[$result['type']] = t('@type conditions', array('@type' => node_get_types('name', $result['type'])));
    }
  }
  return $options;
}

/**
 * Implementation of hook_features_export()
 */
function conditional_fields_features_export($data, &$export, $module_name = '') {
  $export['dependencies'][] = 'conditional_fields';
  
  foreach ($data as $node_type) {
    $export['features']['conditional_fields'][$node_type] = $node_type;
  }
}

/**
 * Implmentation of hook_features_export_render()
 */
function conditional_fields_features_export_render($module_name = '', $data) {
  $code[] = '$items = array();';
  foreach ($data as $type) {
    $query = db_query("SELECT * FROM {conditional_fields} WHERE type='%s'", $type);

    while($result = db_fetch_array($query)) {    
      $row = $result; 
      $row['trigger_values'] = unserialize($row['trigger_values']);
      $code[] = '$items[] = ' . var_export($row, TRUE) . ';';
    }
  }
  $code[] = 'return $items;';
  $code = implode("\n", $code);

  return array('conditional_fields_default_fields' => $code);
}

/**
 * Implmentation of hook_features_rebuild()
 */
function conditional_fields_features_rebuild($module) {
   $defaults = module_invoke($module, 'conditional_fields_default_fields');
  if (!empty($defaults)) {
    $types = array();
    foreach ($defaults as $field) {
      if (!in_array($field['type'], $types)) {
        $types[] = $field['type'];
        db_query("DELETE FROM {conditional_fields} WHERE type='%s'", $field['type']);
      };
      conditional_fields_insert_field($field['type'], $field['field_name'], $field['control_field_name'], $field['trigger_values']);
    } 
  }
}

/**
 * Implementation of hook_features_revert().
 */
function conditional_fields_features_revert($module) {
  return conditional_fields_features_rebuild($module);
}
