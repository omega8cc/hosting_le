<?php

// This file is necessary for the drush file to be detected.

/**
 * Register a hosting feature with Aegir.
 *
 * This will be used to generate the 'admin/hosting' page.
 *
 * @return array
 *   associative array indexed by feature key.
 */
function hosting_le_hosting_feature() {
  $features['hosting_le'] = array(
    'title' => t('Hosting LE SSL'),
    'description' => t("Replaces self-signed certificates with Let's Encrypt ones."),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_le',
    'group' => 'experimental',
  );
  return $features;
}
