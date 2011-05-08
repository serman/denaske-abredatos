<?php

/**
 * @file
 * Rules condition for conditional fields
 */

/**
* Implementation of hook_rules_condition_info().
*/
function conditional_fields_rules_condition_info() {
  return array(
    'conditional_fields_rules_condition_is_triggered' => array(
      'label' => t('Controlled field is triggered'),
      'arguments' => array(
        'node' => array('type' => 'node', 'label' => t('Node')),
      ),
      'help' => t('Make sure that the field you select is a controlled field. If not, the condition will always return FALSE.'),
      'module' => 'CCK',
    ),
  );
}

/**
 * Condition: check if the controlled field is triggered
 */
function conditional_fields_rules_condition_is_triggered($node, $settings) {
  if (!$data = conditional_fields_load_data($node->type)) {
    return FALSE;
  }
  foreach ($data as $field) {
    if ($settings['field_name'] == $field['field_name']) {
      if (!conditional_fields_is_triggered($node->$field['control_field_name'], $field['trigger_values'])) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

/**
 * Condition form: add field selection element
 */
function conditional_fields_rules_condition_is_triggered_form($settings, &$form, &$form_state) {
  $form['settings']['field_name'] = array(
    '#type' => 'select',
    '#title' => t('Field'),
    '#options' => content_rules_get_field_names_by_type(),
    '#default_value' => $settings['field_name'],
    '#description' => t('Select the machine-name of the field.'),
    '#required' => TRUE,
  );
}