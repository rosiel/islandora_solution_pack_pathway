<?php

/**
 * @file
 * Theme preprocessors for the pathway object display page.
 */

/**
 * Preprocess function for theme islandora_pathway_object.
 */
function islandora_pathway_preprocess_islandora_pathway_object(&$variables) {
  $object = $variables['object'];
  $variables['children'] = 'foo';

}
