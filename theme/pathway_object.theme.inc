<?php

/**
 * @file
 * Theme preprocessors for the pathway object display page.
 */

/**
 * Preprocess function for theme islandora_pathway_object.
 */
function islandora_pathway_preprocess_islandora_pathway_object(&$variables) {
  module_load_include('inc', 'islandora_pathway', 'includes/utilities');
  drupal_add_css(drupal_get_path('module', 'islandora_pathway') . '/css/pathway.css');
  $object = $variables['object'];
  if (!isset($object['PATHWAY'])) {
    $variables['children'] = t('This object does not have any waypoints yet.');
  }
  else {
    $pathway = json_decode($object['PATHWAY']->content);
    if (empty($pathway->children)) {
      $variables['children'] = t('This object does not have any waypoints yet.');
    }
    else {
      $children = array();
      // Need to make this hierarchical.
      foreach ($pathway->children as $child) {
        $child_obj = islandora_object_load($child->id);
        $children[] = array(
          'title' => $child_obj->label,
          'href' => "islandora/object/$object->id/pathway/$child->id",
          'attributes' => array('class' => array('waypoint-link')),
        );
      }
      $render_array = array(
        '#theme' => 'links',
        '#links' => $children,
        'attributes' => array('class' => array('child-elements')),
      );
      $variables['children'] = drupal_render($render_array);
    }
  }

  // Render current waypoint.
  $current_waypoint = $variables['current_waypoint'];
  if (!$current_waypoint) {
    $current_waypoint_pid = _islandora_pathway_get_first_child($object);
    $current_waypoint = islandora_object_load($current_waypoint_pid);
  }

  $viewable = islandora_object_access_callback(ISLANDORA_VIEW_OBJECTS, $current_waypoint);
  if ($viewable) {
    $content = islandora_view_object($current_waypoint);
  }
  else {
    $content = '';
  }

  $variables['waypoint_content'] = drupal_render($content);

}
