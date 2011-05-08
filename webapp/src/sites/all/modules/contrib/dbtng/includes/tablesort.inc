<?php

/**
 * @file
 * Functions to aid in the creation of sortable tables.
 *
 * All tables created with a call to theme('table') have the option of having
 * column headers that the user can click on to sort the table by that column.
 */


/**
 * Query extender class for tablesort queries.
 */
class TableSort extends SelectQueryExtender {

  /**
   * The array of fields that can be sorted by.
   *
   * @var array
   */
  protected $header = array();

  public function __construct(SelectQueryInterface $query, DatabaseConnection $connection) {
    parent::__construct($query, $connection);

    // Add convenience tag to mark that this is an extended query. We have to
    // do this in the constructor to ensure that it is set before preExecute()
    // gets called.
    $this->addTag('tablesort');
  }

  /**
   * Order the query based on a header array.
   *
   * @see theme_table()
   * @param $header
   *   Table header array.
   * @return SelectQueryInterface
   *   The called object.
   */
  public function orderByHeader(Array $header) {
    $this->header = $header;
    $ts = $this->init();
    if (!empty($ts['sql'])) {
      // Based on code from db_escape_table(), but this can also contain a dot.
      $field = preg_replace('/[^A-Za-z0-9_.]+/', '', $ts['sql']);

      // Sort order can only be ASC or DESC.
      $sort = drupal_strtoupper($ts['sort']);
      $sort = in_array($sort, array('ASC', 'DESC')) ? $sort : '';
      $this->orderBy($field, $sort);
    }
    return $this;
  }

  /**
   * Initialize the table sort context.
   */
  protected function init() {
    $ts = $this->order();
    $ts['sort'] = $this->getSort();
    $ts['query'] = $this->getQueryParameters();
    return $ts;
  }

  /**
   * Determine the current sort direction.
   *
   * @param $headers
   *   An array of column headers in the format described in theme_table().
   * @return
   *   The current sort direction ("asc" or "desc").
   */
  protected function getSort() {
    if (isset($_GET['sort'])) {
      return ($_GET['sort'] == 'desc') ? 'desc' : 'asc';
    }
    // User has not specified a sort. Use default if specified; otherwise use "asc".
    else {
      foreach ($this->header as $header) {
        if (is_array($header) && isset($header['sort'])) {
          return $header['sort'];
        }
      }
    }
    return 'asc';
  }

  /**
   * Compose a URL query parameter array to append to table sorting requests.
   *
   * @return
   *   A URL query parameter array that consists of all components of the current
   *   page request except for those pertaining to table sorting.
   *
   * @see tablesort_get_query_parameters()
   */
  protected function getQueryParameters() {
    return tablesort_get_query_parameters();
  }

  /**
   * Determine the current sort criterion.
   *
   * @param $headers
   *   An array of column headers in the format described in theme_table().
   * @return
   *   An associative array describing the criterion, containing the keys:
   *   - "name": The localized title of the table column.
   *   - "sql": The name of the database field to sort on.
   */
  protected function order() {
    $order = isset($_GET['order']) ? $_GET['order'] : '';
    foreach ($this->header as $header) {
      if (isset($header['data']) && $order == $header['data']) {
        return array('name' => $header['data'], 'sql' => isset($header['field']) ? $header['field'] : '');
      }

      if (isset($header['sort']) && ($header['sort'] == 'asc' || $header['sort'] == 'desc')) {
        $default = array('name' => $header['data'], 'sql' => isset($header['field']) ? $header['field'] : '');
      }
    }

    if (isset($default)) {
      return $default;
    }
    else {
      // The first column specified is initial 'order by' field unless otherwise specified
      $headers = array_values($this->header);
      $header = $headers[0];
      if (is_array($header)) {
        $header += array('data' => NULL, 'field' => NULL);
        return array('name' => $header['data'], 'sql' => $header['field']);
      }
      else {
        return array('name' => $header);
      }
    }
  }
}

/**
 * Compose a URL query parameter array for table sorting links.
 *
 * @return
 *   A URL query parameter array that consists of all components of the current
 *   page request except for those pertaining to table sorting.
 */
function tablesort_get_query_parameters() {
  return drupal_get_query_parameters($_GET, array('q', 'sort', 'order'));
}
