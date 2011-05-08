<?php

/**
 * @file
 * Handler for 'finder_views' display.
 */
class finder_views_plugin_display_finder extends views_plugin_display {

  function execute() {
    return $this->view->render($this->display->id);
  }

  function render() {
    return !empty($this->view->result) || !empty($this->view->style_plugin->definition['even empty']) ? $this->view->style_plugin->render($this->view->result) : '';
  }

  function query() {

    $options = $this->get_option('finder_views_options');

    // Add group by's for choices - this is actually screwed thanks to
    // views_handler_field.inc adding the base field in add_additional_fields().

    if (!empty($options['groups']) && is_array($options['groups'])) {
      foreach ($options['groups'] as $id) {
        $this->view->query->add_groupby($this->view->field[$id]->field_alias);
      }
    }

    if (!empty($options['keywords'])) {
      // Create a where group called 'finder', for element combination.
      $this->view->query->set_where_group($options['combination'], 'finder');
      if (empty($options['build_info'])) {
        // Conjunction or Disjunction element combination.
        foreach ($options['keywords'] as $feid => $keyword_array) {
          $match = $options['mode'] == 'results' ? $options['results_match'][$feid] : $options['match'];
          $keyword_array = (array)$keyword_array;
          if (!empty($keyword_array)) {
            $clauses = array();
            $clause_args = array();
            foreach ($options['field_info'][$feid] as $key => $field) {
              $alias = NULL;
              $alias = $this->view->query->ensure_table($field['table']);
              if (!$alias) {
                // This is a hack to get fields present via a relationship to work.
                // Doesn't work if you haven't added the field to the view via UI though.
                $field_info = $this->view->field[$field['field']];
                $table = $field_info->definition['table'];
                $alias = $field_info->table_alias;
                // Alternate idea that might work without adding the field to the view:
                // $table_data = views_fetch_data($field['table']);
              }
              else {
                $join_data = $this->view->query->get_join_data($alias, $options['base_table']);
                $table = $join_data->table ? $join_data->table : $field['table'];
              }
              $placeholder = finder_placeholder($match, $table, $field['field']);
              foreach ($keyword_array as $keyword_position => $keyword) {
                $outer_key = $options['nesting_order'][$feid] ? $key : $keyword_position;
                $clauses[$outer_key][] = $alias .'.'. $field['field'] . $placeholder;
                $clause_args[$outer_key][] = $keyword;
              }
            }
            if (!empty($clauses)) {
              $inner_operator = $options['nesting_order'][$feid] ? $options['value_combination'][$feid] : $options['field_combination'][$feid];
              $outer_operator = $options['nesting_order'][$feid] ? $options['field_combination'][$feid] : $options['value_combination'][$feid];
              $inner_clauses = array();
              $inner_clause_args = array();
              foreach ($clauses as $clause) {
                $inner_clauses[] = '('. implode(' ' . $inner_operator . ' ', $clause) .')';
                foreach ($clause_args as $clause_arg) {
                  foreach ($clause_arg as $inner_clause_arg) {
                    $inner_clause_args[] = $inner_clause_arg;
                  }
                }
              }
              $this->view->query->add_where('finder', '('. implode(' ' . $outer_operator . ' ', $inner_clauses) .')', $inner_clause_args);
            }
          }
        }
      }
      else {
        // Intersect element combination.
        // Use build_info to make the queries
        $alias = $this->view->query->ensure_table($options['base_table']);
        $table = $alias .'.'. $options['base_field'];
        foreach ($options['build_info'] as $build_info) {
          $this->view->query->add_where('finder', '('.  $table .' IN ('. $build_info['query'] .'))', $build_info['query_args']);
        }
      }
    }
  }


}