<?php

/**
 * @file
 * Handler for 'finder_views_php_array_finder' style.
 */
class finder_views_plugin_style_php_array_finder extends views_plugin_style {

  function render() {
    $results = array();
    // Group the rows according to the grouping field, if specified.
    $sets =
      $this->render_grouping($this->view->result, $this->options['grouping']);
    $base_field = $this->view->base_field;
    $options = $this->display->display_options['finder_views_options'];
    foreach ($sets as $title => $records) {
      foreach ($records as $label => $row) {
        $result = $row;
        $result->base_table = $options['base_table'];
        $result->base_field = $base_field;
        $result->set = $title;
        if ($options['mode'] == 'choices') {
          foreach ($options['field_info'][$options['feid']]
            as $key => $field) {
            $id = 'finder_element_'. $options['feid'] .'_'. $field['table'] .'_'. $field['field'];
            $alias = $this->view->field[$id]->field_alias;
            $result->$id = $row->$alias;
          }
        }
        elseif ($options['mode'] == 'results') {
          $result->rendered = $this->row_plugin->render($row);
          foreach ($options['field_info'] as $feid => $info) {
            foreach ($info as $key => $field) {
              $id = 'finder_element_'. $feid .'_'. $field['table'] .'_'. $field['field'];
              // TO DO: find out if this if statement about $alias is still relevant?
              if ($alias = $this->view->field[$id]->field_alias) {
                $result->$id = $row->$alias;
              }
              $result->field_names[$feid] = $id;
            }
          }
        }
        $results[] = $result;
      }
    }
    return $results;
  }

}