<?php

/**
 * Register our directory as a place to find provision classes.
 */
function hosting_le_vhost_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    $list = drush_commandfile_list();
    $provision_dir = dirname($list['provision']);
    if (is_readable($provision_dir . '/provision.inc')) {
      include_once($provision_dir . '/provision.inc');
      include_once($provision_dir . '/provision.service.inc');
      if (function_exists('provision_autoload_register_prefix')) {
        provision_autoload_register_prefix('Provision_', dirname(__FILE__));
      }
    }
  }
}

/**
 * Implements hook_drush_init().
 */
function hosting_le_vhost_drush_init() {
  hosting_le_vhost_register_autoload();
}

/**
 *  Implements hook_provision_services().
 */
function hosting_le_vhost_provision_services() {
  hosting_le_vhost_register_autoload();
  return array('Le' => NULL);
}

/*
 * Implementation of hook_provision_nginx_vhost_config()
 */
function hosting_le_vhost_provision_nginx_vhost_config($uri, $data) {

  $aegir_root = d('@server_master')->aegir_root;

  if (d()->type == 'site' && d()->ssl_enabled) {

    $lines = array();

    $lines[] = "";
    $lines[] = "  ###";
    $lines[] = "  ### Allow access to letsencrypt.org ACME challenges directory.";
    $lines[] = "  ###";
    $lines[] = "  location ^~ /.well-known/acme-challenge {";
    $lines[] = "    alias $aegir_root/tools/le/.acme-challenges;";
    $lines[] = "    try_files \$uri 404;";
    $lines[] = "  }";
    $lines[] = "\n";

    return implode("\n", $lines);
  }

  return '';
}

/*
 * Implementation of hook_provision_apache_vhost_config()
 */
function hosting_le_vhost_provision_apache_vhost_config($uri, $data) {

  $aegir_root = d('@server_master')->aegir_root;

  if (d()->type == 'site' && d()->ssl_enabled) {

    $lines = array();

    $lines[] = "";
    $lines[] = "  Alias /.well-known/acme-challenge $aegir_root/tools/le/.acme-challenges";
    $lines[] = "";
    $lines[] = "  # Allow access to letsencrypt.org ACME challenges directory.";
    $lines[] = "  <Directory \"$aegir_root/tools/le/.acme-challenges\">";
    $lines[] = "    Require all granted";
    $lines[] = "  </Directory>";
    $lines[] = "\n";

    return implode("\n", $lines);
  }

  return '';
}
