<?php

/**
 * @file
 * Manage pathway objects.
 */

/**
 * Create the manage interface.
 */
function islandora_pathway_manage_object(AbstractObject $object) {

  // List existing objects.
  // Let the user add an object.
  // Let the user delete an object.
  // Let the user re-arrange the objects.
  // Let the user re-arrange the objects to be hierarchical.
  // Make a pretty version of the add an object, with autocomplete or views or something.

  $children = drupal_get_form('islandora_pathway_existing_waypoints_form', $object);
  $add_form = drupal_get_form('islandora_pathway_add_waypoint_form', $object);
  return array($children, $add_form);
}


/**
 * Define the show children form.
 */
function islandora_pathway_existing_waypoints_form(array $form, array &$form_state, AbstractObject $object) {
  $form_state['storage']['object'] = $object;
  $form = array();
  $rows = array();
  $row_elements = array();

  $form['waypoints_table'] = array(
    '#type' => 'fieldset',
    '#title' => 'Waypoints',
  );

  // Get the existing waypoints, if they exist.
  $children = array();
  if ($object['PATHWAY']) {
    $pathway = json_decode($object['PATHWAY']->content);
    if (isset($pathway->children)) {
      $i = 0;
      foreach ($pathway->children as $child) {
        $child_obj = islandora_object_load($child->id);
        if ($child_obj) {
          $children[$child_obj->id] = array(
            'enable' => TRUE,
            'weight' => $i,
            'pid' => $child_obj->id,
            'label' => $child_obj->label,
          );
          $i++;
        }
      }
    }
  }

  foreach ($children as $pid => $child) {
    // Build the table rows.
    $rows[$pid] = array(
      'data' => array(
        // First column: drag and drop.
        array('class' => array('entry-cross')),
        // Weight column.
        array('data' => array(
          '#type' => 'weight',
          '#title' => 'Weight',
          '#title_display' => 'invisible',
          '#default_value' => $child['weight'],
          '#parents' => array('waypoints_table', $pid, 'weight'),
          '#attributes' => array(
            'class' => array('entry-order-weight'),
          ),
        )),
        // PID column.
        array('data' => array(
          //'#type' => 'textfield',
          //'#size' => 10,
          '#parents' => array('waypoints_table', $pid, 'pid'),
          //'#disabled' => TRUE,
          '#markup' => $child['pid'],
        )),
      ),
      'class' => array('draggable'),
    );

    // Build rows of the form elements in teh table.
    $row_elements[$pid] = array(
      'weight' => &$rows[$pid]['data'][1]['data'],
      'pid' => &$rows[$pid]['data'][2]['data'],
    );
  }

  $form['waypoints_table']['table'] = array(
    '#theme' => 'table',
    'elements' => $row_elements,
    '#header' => array(
      array('data' => NULL, 'colspan' => 2),
      t('PID'),
    ),
    '#rows' => $rows,
    '#empty' => 'There are no waypoints present.',
    '#attributes' => array('id' => 'entry-order'),

  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  drupal_add_tabledrag('entry-order', 'order', 'sibling', 'entry-order-weight');
  return $form;
}

/**
 * Submit handler for re-arranging form.
 */
function islandora_pathway_existing_waypoints_form_submit($form, &$form_state) {
  $root_object = $form_state['storage']['object'];
  $pathway_obj = _get_blank_pathway_object($root_object->id);
  foreach($form_state['input']['waypoints_table'] as $child_pid => $child_row) {
    $child_object = islandora_object_load($child_pid);
    if (!$child_object) {
      // ERROR!
      continue;
    }
    $pathway_obj->children[] = _get_blank_pathway_object($child_pid);
  }
  if (!isset($root_object['PATHWAY'])) {
    $path_ds = $root_object->constructDatastream('PATHWAY', 'M');
    $path_ds->label = "Pathway";
    $path_ds->mimetype = 'application/json';
    $root_object->ingestDatastream($path_ds);
  }
  $root_object['PATHWAY']->setContentFromString(json_encode($pathway_obj));
}


/**
 * The add a new waypoint form.
 */
function islandora_pathway_add_waypoint_form(array $form, array &$form_state, AbstractObject $object) {
  $form_state['redirect'] = "islandora/object/{$object->id}/manage/pathway";
  $form_state['storage']['object'] = $object;
  $form = array();
  $form['new_waypoint'] = array(
    '#title' => t('Add a new waypoint.'),
    '#type' => 'textfield',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 * Submit handler for add form.
 */
function islandora_pathway_add_waypoint_form_submit($form, &$form_state) {
  $pid = $form_state['values']['new_waypoint'];
  if ($pid) {
    $new_waypoint = islandora_object_load($pid);
    if (!$new_waypoint) {
      drupal_set_message("Object $pid does not exist.");
      return;
    }
    else {
      $object = $form_state['storage']['object'];
      _add_new_waypoint($object, $new_waypoint);
    }
  }
}

function _add_new_waypoint(AbstractObject $object, AbstractObject $new_waypoint) {
  if (!$object['PATHWAY']) {
    $obj = _get_blank_pathway_object($object->id);
    $path_ds = $object->constructDatastream('PATHWAY', 'M');
    $path_ds->label = "Pathway";
    $path_ds->mimetype = 'application/json';
    $path_ds->content = json_encode($obj);
    $object->ingestDatastream($path_ds);
  }
  $pathway = json_decode($object['PATHWAY']->content);

  $pathway->children[] = _get_blank_pathway_object($new_waypoint->id);

  $object['PATHWAY']->setContentFromString(json_encode($pathway));

}

function _get_blank_pathway_object($base_pid) {
  return (object) array('id' => $base_pid, 'children' => array());
}