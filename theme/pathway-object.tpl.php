<?php

/**
 * @file
 * Variables available:
 *
 * $object
 *   The fedora object for this page.
 *
 * $children
 *   The rendered menu of children objects.
 *
 * $waypoint_content
 *   The rendered display of the current islandora waypoint.
 */
?>
<div class="islandora-pathway-object islandora">
  <div class="pathway-menu">
    <?php print ($children); ?>
  </div>
  <div class="islandora-waypoint-content">
    <?php print ($waypoint_content); ?>
  </div>
</div>
