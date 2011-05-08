<?php
// $Id: location.migrate.inc,v 1.1 2010/01/15 17:44:05 textplease Exp $

/**
 * @file
 * Integrates location module with the migrate module
 * 
 */

/**
 * Implementation of hook_migrate_types().
 */
function location_migrate_types() {
    $types = array('location' => t('Node Location'));
  return $types;
}

/**
 * Implementation of hook_migrate_fields().
 */
function location_migrate_fields_location($type) { 
  $fields = array(
      '[lid]' => t('Location ID'), // Primary Key: Unique location ID.
      //'locpick' => t('Location Chosen By User?'),
      'nid' => t('NID'), // nid
      'vid' => t('VID'), // vid
      //'uid' => t('User ID'),  only used for location_user? (manual ones have uid of 0)
      'genid' => t('Generic reference key'), // Generic reference key.
      'name' => t('Location Name'), // Place Name.
      'street' => t('Street Address'), // Street address, line 1.
      'additional' => t('Additonal info'), // Street address, line 2.      
      'city' => t('City'), // City.
      'province' => t('Province/State'), // State / Province code.
      'postal_code' => t('Postal Code'), // Postal / ZIP code.
      'country' => t('Country (2 letter code)'), // Two letter ISO country code.
      'latitude' => t('Latitude'), // Location latitude (decimal degrees).
      'longitude' => t('Longitude'), // Location longitude (decimal degrees).
      'source' => t('Source'), // Source of the latitude and longitude data (Geocoder, user entered, invalid, etc.)
      'is_primary' => t('Primary Location? (depricated)'), // Is this the primary location of an object? (unused, civicrm legacy field?)
      'inhibit_geocode' => t('Inhibit Geocoding?'), //
    );
  return $fields;
}


/**
 * Implementation of hook_migrate_prepare().
 */
function location_migrate_prepare_location(&$loc, $tblinfo, $row) {
  $errors = array();
  
  //can't load vid from views, so if it's not present, we'll load it.
  if(!isset($loc->vid) && isset($loc->nid)) {
    $sql = "SELECT vid from {node} WHERE nid = %d";
    if($vid = db_result(db_query($sql, $loc->nid))) {
      $loc->vid = $vid;
    }
    else{
      $errors[] = migrate_message('Could not find a VID for the Supplied NID');
    }
  }
  
  //note, we could probably do the same for uid here too and not have to have the extra table info stuff in matadormove
  // on second thought, it looks like uid is only used for location_user!
  
  $context = array(
      'nid' => $loc->nid,
      'vid' => $loc->vid,
      'uid' => $loc->uid,
      'genid' => $loc->genid,
 );
 
 unset($loc->nid, $loc->vid, $loc->uid, $loc->genid);
 $loc->context = $context;

}

/**
 * Implementation of hook_migrate_import().
 */
function location_migrate_import_location($tblinfo, $row) {
  $loc = (object)array();
    
  foreach ($tblinfo->fields as $destfield => $values) {

    if ($values['srcfield'] && $row->$values['srcfield']) {

      $loc->$destfield = $row->$values['srcfield'];
    } else {
      $loc->$destfield = $values['default_value'];
    }
  }  
  
  timer_start('location_prepare hooks');
    $errors = migrate_destination_invoke_all('prepare_location', $loc, $tblinfo, $row);
  timer_stop('location_prepare hooks');
  
  $context = $loc->context;
  unset($loc->context);
    
  $success = TRUE;
  foreach ($errors as $error) {
    if ($error['level'] != MIGRATE_MESSAGE_INFORMATIONAL) {
      $success = FALSE;
      break;
    }
  }
  //change loc to array for location_save().
  //$loc = array(0 => (array)$loc);
  $loc = (array)$loc;
  
  if ($success) {

    timer_start('location_save');
      $not_empty = location_save($loc, FALSE, $context);
     //$success = location_save_locations($loc, $context);
    timer_stop('location_save');
   
    if (!$not_empty) {
      $errors[] = migrate_message("Empty Location - location module considers this location 'empty' so it will not be saved. It probably matches the default location form.");
    }
    
    //change loc back to object
     $loc = (object)$loc;
     $loc->context = $context;
     $loc->not_empty = (bool)$not_empty;
    
    // Call completion hooks, for any processing which needs to be done after node_save
    timer_start('location_completion hooks');
      $errors = array_merge($errors, migrate_destination_invoke_all('complete_location', $loc, $tblinfo, $row));
    timer_stop('location_completion hooks');
  }
  return $errors;
}

/**
 * Implementation of hook_migrate_complete().
 */
function location_migrate_complete_location(&$loc, $tblinfo, $row) {
  /**
   * Add the location instance. location_save_locations() will overwrite 
   * previous locations, so we need this to iteratively add multiple
   * locations to one node.  Code adapted from location_save_locations()
   */
  $criteria = $loc->context;

  //kind of complex, but we can use this one function for user and cck too.
  foreach (array('nid' => '%d', 'vid' => '%d', 'uid' => '%d', 'genid' => "'%s'") as $key => $placeholder) {
    if (isset($criteria[$key])) {
      $columns[] = $key;
      $placeholders[] = $placeholder;
      $args[] = $criteria[$key];
      $qfrags[] = "$key = $placeholder";
    }
  }
 
 
  $columns[] = 'lid';
  $placeholders[] = '%d';

  if ($loc->lid !== FALSE) {
    $args[] = $loc->lid;
    $query = 'INSERT INTO {location_instance} ('. implode(', ', $columns) .') VALUES ('. implode(', ', $placeholders) .')';
    db_query($query, $args);
  }
  
  // look like this was an empty location, let's map it anyway to zero, so we ignore it on future imports.
  // I decided against this later, the errors get reset and get lost. leaving in case we change our minds.
  /*if(!$loc->not_empty) {
    $loc->lid = 0;
  }*/
  
  // Add the mapping and check for errors
  $errors = array();
  //if ($loc->lid || !$loc->not_empty) {
  if ($loc->lid) {
    $sourcekey = $tblinfo->sourcekey;
    migrate_add_mapping($tblinfo->mcsid, $row->$sourcekey, $loc->lid);
  }
  
  //return an error if for some reason it wasn't empty but no lid was returned.
  if (!$loc->lid && $loc->not_empty) {
    $errors[] = migrate_message(t('Location not mapped - No LID returned'));
  }
  return $errors;
}

function location_migrate_delete_location($lid) {
  /* BIG problem: lids are not unique to each location instance. Only 
   * the pairing of lid and (uid, genid, or nid/vid) are unique. We would
   * somehow need to store two fields to get a unique id.
   */
  /*
  // this way wipes out ALL locations for a particular node if there are more than 1.
  $location = location_load_location($lid);
  dpm($location);
  $empty = array();
  location_save_locations($empty, array('nid' => $location->nid));
  */
  
  /**
   * So, we have to make an assumption, that location imports are following
   * the same order as the nodes that were imported, and that the delete process works 
   * last in last out. So we'll look for the highest node id associated with a 
   * lid, and assume that is the one that needs removing.
   */
  
  
  $result = db_query('SELECT nid, uid, genid FROM {location_instance} WHERE lid = %d AND nid <> 0 ORDER BY nid DESC', $lid);
  if($location_instance = db_fetch_array($result)) {
    //at least one node matches this location
    db_query('DELETE FROM {location_instance} WHERE lid = %d and nid = %d', array($lid, $location_instance['nid']));
  }
  
  //check to see if ths is the last instance, and if it is, remove the location.
  $count = db_result(db_query('SELECT COUNT(*) FROM {location_instance} WHERE lid = %d', $lid));
  if ($count !== FALSE && $count == 0) {
    $location = array('lid' => $lid);
    location_invoke_locationapi($location, 'delete');
    db_query('DELETE FROM {location} WHERE lid = %d', $location['lid']);
  }
}
