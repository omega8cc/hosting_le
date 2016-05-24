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
function hosting_le_vhost_hosting_feature() {
  $features['hosting_le_vhost'] = array(
    'title' => t('Hosting LE Vhost'),
    'description' => t("Extends vhosts with ACME challenge paths for Let's Encrypt."),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_le_vhost',
    'group' => 'experimental',
  );
  return $features;
}
