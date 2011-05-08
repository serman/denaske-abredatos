<?php

/**
 * @file
 * Implementation of taxonomy destination handling
 */

/**
 * Implementation of hook_migrate_types().
 */
function taxonomy_migrate_types() {
  $types = array('term' => t('Taxonomy Term'));
  return $types;
}

/**
 * Implementation of hook_migrate_fields_term().
 */
function taxonomy_migrate_fields_term($type) {
  $fields = array(
    'tid' => t('Taxonomy: Existing term ID'),
    'vid' => t('Taxonomy: Vocabulary'),
    'name' => t('Taxonomy: Name'),
    'description' => t('Taxonomy: Description'),
    'weight' => t('Taxonomy: Weight'),
    'parent' => t('Taxonomy: Parent'),
    'synonyms' => t('Taxonomy: Synonyms'),
  );
  return $fields;
}

/**
 * Implementation of hook_migrate_fields_node().
 */
function taxonomy_migrate_fields_node($type) {
  $fields = array();
  foreach ((array)taxonomy_get_vocabularies($type) as $vocab) {
    $fields['migrate_taxonomy_'. $vocab->vid] = t('Taxonomy: %name', array('%name' => check_plain($vocab->name)));
  }
  return $fields;
}

/**
 * Implementation of hook_migrate_delete_term().
 */
function taxonomy_migrate_delete_term($tblinfo, $tid) {
  taxonomy_del_term($tid);
}

/**
 * Implementation of hook_migrate_import_term().
 */
function taxonomy_migrate_import_term($tblinfo, $row) {
  static $vocabularies = NULL;
  if (!isset($vocabularies)) {
    $vocabularies = array();
    $sql = "SELECT vid,name FROM {vocabulary}";
    $result = db_query($sql);
    while ($vocabrow = db_fetch_object($result)) {
      $vocabularies[$vocabrow->name] = $vocabrow->vid;
    }
  }

  $sourcekey = $tblinfo->sourcekey;
  $errors = array();
  // Begin building term array.
  $term = array();

  // Handle an update operation
  if ($row->destid) {
    $term['tid'] = $row->destid;
  }
  // If a tid was provided via mapping, load the existing term
  else if (isset($tblinfo->fields['tid'])) {
    $term = taxonomy_get_term($tblinfo->fields['tid']);
  }

  // Data which might be useful for taxonomy hooks.
  $term['migrate_tblinfo'] = $tblinfo;

  foreach ($tblinfo->fields as $destfield => $values) {
    if ($values['srcfield'] && $row->$values['srcfield']) {
      $source_value = trim($row->$values['srcfield']);
    }
    else {
      $source_value = $values['default_value'];
    }
    switch ($destfield) {
      case 'vid':
        // Get vocabulary by name
        if (!is_numeric($source_value)) {
          if (isset($vocabularies[$source_value])) {
            $source_value = $vocabularies[$source_value];
          }
          else {
            $errors[] = migrate_message(t('Provided vocabulary "!vocab" not found',
              array('!vocab' => $source_value)));
          }
        }
        $term['vid'] = $source_value;
        break;
      case 'parent':
        // @TODO: what if parent term is not already loaded into database?
        // Interpret a numeric parent as the source ID of the parent term
        if (is_numeric($source_value)) {
          $parent = db_result(db_query("SELECT destid FROM {". $tblinfo->maptable ."} WHERE sourceid = %d", $source_value));
          $term[$destfield] = ($parent >= 0 ? $parent : 0);
        }
        elseif ($source_value) {
          // Interpret a string parent as the name of the parent term
          $matches = taxonomy_get_term_by_name($source_value);

          // No matches - maybe the parent isn't migrated yet
          if (count($matches) == 0) {
            $errors[] = migrate_message(t('Parent term "!parent" not found. You may need
              to change the ordering of your content set view so all parents are migrated
              before their children.', array('!parent' => $source_value)));
          }
          else {
            // If there's no explicit vocabulary for the term...
            if (!$term['vid']) {
              // ...use a single match...
              if (count($matches) == 1) {
                $term['parent'] = $matches[0]->tid;
              }
              // ...but deprecate ambiguity
              else {
                $errors[] = migrate_message(t('Multiple potential parent terms named "!parent"
                  were found.', array('!parent' => $source_value)));
              }
            }
            // If the term is tied to a vocabulary, look for the parent only in that vocab
            else {
              foreach ($matches as $parent_term) {
                if ($parent_term->vid == $term['vid']) {
                  $term['parent'] = $parent_term->tid;
                  break;
                }
              }
              // No parent found
              if (!isset($term['parent'])) {
                $errors[] = migrate_message(t('Parent term "!parent" not found in vocabulary !vid.
                  You may need to change the ordering of your content set view so all parents are
                  migrated before their children.',
                  array('!parent' => $source_value, '!vid' => $term['vid'])));
              }
            }
          }
        }
        break;
      default:
        $term[$destfield] = $source_value;
        break;
    }
  }

  // Prepare the term for import.
  $errors = array_merge($errors, migrate_destination_invoke_all('prepare_term', $term, $tblinfo, $row));

  $success = TRUE;
  foreach ($errors as $error) {
    if ($error['level'] != MIGRATE_MESSAGE_INFORMATIONAL) {
      $success = FALSE;
      break;
    }
  }
  if ($success) {
    timer_start('taxonomy_save');
    taxonomy_save_term($term);
    timer_stop('taxonomy_save');
    // Call completion hooks, for any processing which needs to be done after node_save
    timer_start('taxonomy completion hooks');
    $errors = migrate_destination_invoke_all('complete_term', $term, $tblinfo, $row);
    timer_stop('taxonomy completion hooks');

    // @TODO: Check first for existence, we may have updated an existing term - do we care about duplicates?
    migrate_add_mapping($tblinfo->mcsid, $row->$sourcekey, $term['tid']);
  }
  return $errors;
}

/**
 * Implementation of hook_migrate_prepare_node().
 */
function taxonomy_migrate_prepare_node(&$node, $tblinfo, $row) {
  static $vocabs;
  if (!isset($vocabs) || !isset($vocabs[$node->type])) {
    $vocabs[$node->type] = taxonomy_get_vocabularies($node->type);
  }
  if (!$vocabs[$node->type] || count($vocabs[$node->type]) < 1) return;

  $multiple_separator = $tblinfo->multiple_separator;

  $taxonomy = array(); // Here we will store the final taxonomy for this node.
  $errors = array();   // Here we will store the errors for this node.

  // It is possible there appeared some terms magically already.
  if (isset($node->taxonomy)) {
    $taxonomy = is_array($node->taxonomy) ? $node->taxonomy : array();
  }

  foreach ((array)$vocabs[$node->type] as $vocab) {
    $field = 'migrate_taxonomy_'. $vocab->vid;
    if (isset($field) && isset($node->$field)) {
      $value = trim($node->$field);
    }
    else {
      $value = '';
    }
    unset($node->$field);

    $vid = $vocab->vid;

    // Depending on the type of vocabulary, we need to handle this specially.
    if ($vocab->tags) {
      // 1. Free tagging vocabularies:
      //    $node->taxonomy['tags'] = array($vid1 => $text_value, $vid2 => $text_value, ...);
      //    note: we don't have to split the $text_value as taxonomy_node_save()
      //    will do that for us. So in this case, to specify multiple terms, you
      //    need to set it to "term 1, term 2, term 3" (separator = ',').
      $taxonomy['tags'] = isset($taxonomy['tags']) ? $taxonomy['tags'] : array();
      $taxonomy['tags'][$vid] = (!empty($global_value) && !empty($value)) ? ',' : '';
      $taxonomy['tags'][$vid] .= str_replace($multiple_separator, ',', $value);

      // Error if the vocabulary was required, but there are no terms.
      if ($vocab->required && empty($taxonomy['tags'][$vid])) {
        $errors[] = migrate_message(t('You need to assign at least one term of the vocabulary %name.',
          array('%name' => theme('placeholder', $vocab->name))));
      }
    }
    else {
      // 2. Other vocabularies:
      //    $node->taxonomy = array($tid1, $tid2, ...)
      //    or
      //    $node->taxonomy = array($vid1 => array($tid1, $tid2, ...), $vid2 => array(...), ...)
      //    We'll use the second form.
      $taxonomy[$vid] = isset($taxonomy[$vid]) ? $taxonomy[$vid] : array();
      if (isset($value) && !empty($value)) {
        // If the vocabulary allows multiple terms, explode the $value.
        if ($vocab->multiple) {
          $terms = array_map('trim', explode($multiple_separator, $value));
        }
        else {
          $terms = array($value);
        }

        // Now handle each term.
        foreach ($terms as $text) {
          if (!empty($text)) {
            $tid = _migrate_taxonomy_get_term($vocab, $text, 'warn');
            if ($tid >= 0) {
              // A $tid == 0 means that the term was not found, but will be created.
              // Because we check whether terms are assigned later on for required
              // vocabularies, we need to add it to the array.
              $taxonomy[$vid][] = $tid;
            }
            elseif ($tid < 0) {
              $errors[] = migrate_message(t('The term %term does not exist in the %name vocabulary.',
                array('%term' => theme('placeholder', $value), '%name' => theme('placeholder', $vocab->name))));
            }
          }
        }
      }


      // Error if the vocabulary was required, but there are no terms.
      if ($vocab->required && count($taxonomy[$vid]) == 0) {
        $errors[] = migrate_message(t('You need to assign at least one term of the %name vocabulary.',
          array('%name' => theme('placeholder', $vocab->name))));
      }

      // Make sure there are no duplicated entries and no '0' entries.
      $taxonomy[$vid] = array_filter(array_unique($taxonomy[$vid]));

      // If single select, the $taxonomy[$vid] should be an integer, not an array.
      if (!$vocab->multiple) {
        if (count($taxonomy[$vid]) == 1) {
          $taxonomy[$vid] = $taxonomy[$vid][0];
        }
        else {
          unset($taxonomy[$vid]);
        }
      }
    }
  }

  if (module_exists('category')) {
    $node->category = $taxonomy;
  }
  else {
    $node->taxonomy = $taxonomy;
  }

  return $errors;
}

/**
 * Implementation of hook_migrate_xlat_$contenttype().
 */
function taxonomy_migrate_xlat_term($tid) {
  return "taxonomy/term/$tid";
}

/**
 * Return a tid for a term (text).
 * @param $vocab
 *  Vocabulary object
 * @param $text
 *  Name of term searched for
 * @param $handler
 *  'add' to automatically add the term to the vocabulary, 'warn' to
 *  simply test for existence
 * @return
 *  The tid of the existing or added term, or -1 if $handler is 'warn'
 *  and the term does not exist.
 */
function _migrate_taxonomy_get_term($vocab, $text, $handler) {
  static $missing_terms = array();

  $vid = $vocab->vid;

  if (!isset($missing_terms[$vid])) {
    $missing_terms[$vid] = array();
  }

  // Bail out for empty text.
  if (empty($text)) {
    return -1;
  }

  // If we have found this $text already, return it.
  if (isset($missing_terms[$vid][$text])) {
    return $missing_terms[$vid][$text];
  }

  // Try to find a term with a matching name.
  $possibilities = taxonomy_get_term_by_name($text);
  foreach ($possibilities as $possibility) {
    if ($possibility->vid == $vid) {
      $missing_terms[$vid][$text] = $possibility->tid;
      return $possibility->tid;
    }
  }

  // Try to find a term with a matching tid.
  if (is_numeric($text) && ($term = taxonomy_get_term($text))) {
    $missing_terms[$vid][$text] = $term->tid;
    return $term->tid;
  }

  // If we arrive here, the term does not exist.
  switch ($handler) {
    case 'add':
      $edit = array('vid' => $vid, 'name' => $text);
      $status = taxonomy_save_term($edit);
      $tid = $edit['tid'];
      drupal_set_message(t('Added %term term to the %name vocabulary.', array('%term' => theme('placeholder', $text), '%name' => theme('placeholder', $vocab->name))));
      break;

    case 'warn':
      drupal_set_message(t('There is no %term term inside the %name vocabulary.', array('%term' => theme('placeholder', $text), '%name' => theme('placeholder', $vocab->name))));
      //Fall-through
    default: // which includes 'ignore' and 'no-import'
      $tid = -1;
      break;
  }

  $missing_terms[$vid][$text] = $tid;
  return $tid;
}
