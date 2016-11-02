<?php
/**
 * @file
 * Utitlity functions.
 */

/**
 * Load all MA Profiles for use in Object/User forms
 */
function get_islandora_managed_access_profiles() {
  $profiles = entity_load('islandora_managed_access_profile', FALSE);
  $profile_selects = array();
  foreach ($profiles as $profile) {
    $profile_selects[$profile->id] = $profile->hname;
  }
  return $profile_selects;
}

