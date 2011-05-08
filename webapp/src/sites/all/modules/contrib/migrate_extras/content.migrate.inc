<?php
// $Id: content.migrate.inc,v 1.1.2.3 2010/03/25 19:39:29 textplease Exp $

/**
 * @file
 * Hooks for handling CCK fields on destination nodes
 */

/**
 *  CCK data can be imported into any column of a CCK field.
 *  Most fields have a 'value' column, but there are others that
 *  can be used, for example:
 *   - Textareas have a 'value' and a 'format'
 *   - Date fields have a 'value', if date-specific timezones are
 *       used they also have a 'timezone' and 'offset'
 *   - Nodereference fields have a 'nid' column
 *   - Userreference fields have a 'uid' column
 *
 *  For nodereference and userreference fields you can either import the
 *  numeric value for a nid or uid, or a text value for the title or
 *  name. If a numeric value is imported, it is used as is.
 *  If a title or name is imported, the module will search for
 *  the nid or uid that goes with that title or name.
 *
 *  Date values can be imported as unix timestamps, as ISO values, or as
 *  any text that can be interpreted by the php strtotime function.
 *  The date 'timezone' column expects a full timezone name like 'US/Central',
 *  timezone abbreviations are not unique and will not work.
 *  The date 'offset' column is the plus or minus value in seconds.
 */


/**
 * Implementation of hook_migrate_fields_node().
 */
function content_migrate_fields_node($type) {
  $types = (array) content_types();
  $fields = array();

  if (isset($types[$type])) {
    $content_type = $types[$type];
    foreach ($content_type['fields'] as $field) {
      // filefield has own implementation of hook_migrate_fields
      if ($field['type'] != 'filefield') {
        // Create a potential input field for each field column by creating a dummy name for each column.
        $db_info = content_database_info($field);
        foreach ($db_info['columns'] as $column => $info) {
          $fields[$field['field_name'] . '_' . $column] = t('CCK: ') . $field['widget']['label'] .
            ' ' . $column;
        }
      }
    }
  }
  return $fields;
}

/**
 * Implementation of hook_migrate_prepare_node().
 */
function content_migrate_prepare_node(&$node, $tblinfo, $row) {
  static $types, $fieldinfo = array();
  $errors = array();
  if (!isset($types)) {
    $types = (array) content_types();
  }
  if (isset($types[$node->type])) {
    $content_type = $types[$node->type];
    foreach ($content_type['fields'] as $field) {
      $field_name = $field['field_name'];
      // Find the column names for this field type and process each.
      if (!isset($fieldinfo[$node->type][$field_name])) {
        $fieldinfo[$node->type][$field_name] = content_database_info($field);
      }
      $db_info = $fieldinfo[$node->type][$field_name];
      foreach ($db_info['columns'] as $column => $info) {
        $dummy_name = $field['field_name'] .'_'. $column;

        // Force a node value for missing data.
        if (empty($node->$dummy_name)) {
          $values = array();
        }
        else {
          // Explode multiple values to create the $delta and $value for each.
          if ($field['multiple']) {
            $values = explode($tblinfo->multiple_separator, $node->$dummy_name);
          }
          else {
            $values = array(0 => $node->$dummy_name);
          }
        }

        foreach ($values as $delta => $value) {
          switch ($field['type']) {
            case 'date':
            case 'datestamp':
            case 'datetime':
              // Get properly formatted date for value column.
              // Assume timezone and offset values are correct as input.
              if ($column == 'value' || $column == 'value2') {
                if ($field['type'] == 'date' || $field['type'] == 'datetime') {
                  // A value like "1957" could be either a Unix timestamp
                  // or an ISO year-only date... Favor the ISO interpreation

                  // If the imported date is a (partial) ISO date, convert it
                  // to date's ISO date.
                  // TODO: Need to pass granularity for partial dates to work
                  if (date_is_valid($value, DATE_ISO)) {
                    $value = date_convert($value, DATE_ISO, DATE_ISO);
                  }
                  // Is the imported date a unix timestamp? If so convert it to ISO date.
                  elseif (is_numeric($value) && date_is_valid($value, DATE_UNIX)) {
                    $value = date_convert($value, DATE_UNIX, DATE_ISO);
                  }
                  // Otherwise, use strtotime and unix2iso to convert
                  // partial ISO or text date into full ISO.
                  else {
                    $value = date_convert(strtotime($value), DATE_UNIX, DATE_ISO);
                  }
                }
                else {
                  // Is this already a unix timestamp? If not try strototime.
                  if (!is_numeric($value) || !date_is_valid($value, DATE_UNIX)) {
                    $value = strtotime($value);
                  }
                }
              }
              break;

            case 'nodereference':
              // If $value is numeric use it, otherwise
              // find a node title that matches and get nid.
              if (intval($value) > 0) {
                $value = intval($value);
              }
              break;

            case 'userreference':
              // If $value is numeric use it, otherwise
              // find a user name that matches and get uid.
              if (intval($value) > 0) {
                $value = intval($value);
              }
              else {
                if (($account = user_load(array('name' => $author['name'])))) {
                  $value = $account->uid;
                }
              }
              break;

            case 'number_integer':
              // Make sure numeric data is brought in as numeric values.
              $value = intval($value);
              break;

            case 'number_decimal':
              $value = floatval($value);
              break;
          }
          // Set the correct node field values.
          if (isset($field_name) && isset($node->$field_name) && is_array($node->$field_name)) {
            if (isset($node->{$field_name}[$delta])) {
              $node->{$field_name}[$delta] += array($column => $value);
            }
            else {
              $node->$field_name += array($delta => array($column => $value));
            }
          }
          else {
            $node->$field_name = array($delta => array($column => $value));
          }
        }
      }

      // Unset the dummy column value.
      if (isset($node->$dummy_name)) {
        unset($node->$dummy_name);
      }
    }
    // Handle defaulting of date "to" from "from"
  }

  return $errors;
}
