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
function awforce_ssl_hosting_feature() {
  $features['awforce_ssl'] = array(
    'title' => t('AWForce SSL'),
    'description' => t("Replaces self-signed certificates with Let's Encrypt ones."),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'awforce_ssl',
    'group' => 'experimental',
  );
  return $features;
}
