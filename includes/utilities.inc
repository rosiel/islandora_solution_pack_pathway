<?php
/**
 * @file
 * Helper functions for islandora_pathway module.
 */

/**
 * Helper function to get the first child pid.
 */
function _islandora_pathway_get_first_child(AbstractObject $object) {
  if (isset($object['PATHWAY'])) {
    $pathway = json_decode($object['PATHWAY']->content);
    if (!empty($pathway->children)) {
      return $pathway->children[0]->id;
    }
  }
  return NULL;
}

/**
 * Helper function to determine if a pid is in the pathway of a Fedora object.
 */
function _islandora_pathway_is_pid_in_pathway($pid, AbstractObject $object) {
  if (isset($object['PATHWAY'])) {
    $pathway = json_decode($object['PATHWAY']->content);
    return _islandora_pathway_is_pid_in_pathway_object($pid, $pathway);
  }
  return FALSE;
}

/**
 * Recursively searches for $pid in the pathway object.
 */
function _islandora_pathway_is_pid_in_pathway_object($pid, stdClass $path_object) {
  if ($path_object->id == $pid) {
    return TRUE;
  }
  else {
    foreach ($path_object->children as $child) {
      if (_islandora_pathway_is_pid_in_pathway_object($pid, $child)) {
        return TRUE;
      }
    }
  }
  return FALSE;
}
