<?php

function drush_provision_hosting_le_post_provision_install() {
  drush_provision_hosting_le_post_provision_verify();
}

// We put the generation here because it will be executed as the aegir user
function drush_provision_hosting_le_post_provision_verify() {
  if (d()->type == 'site' && d()->ssl_enabled) {

    $le_root = d('@server_master')->aegir_root . "/tools/le";
    $le_cert = d('@server_master')->aegir_root . "/tools/le/certs";
    $le_acme = d('@server_master')->aegir_root . "/tools/le/.acme-challenges";
    $le_ctrl = d('@server_master')->aegir_root . "/tools/le/.ctrl";
    $le_exec = d('@server_master')->aegir_root . "/tools/le/letsencrypt.sh";

    provision_file()->create_dir($le_root, dt('[provision_hosting_le] LE root'), 0711);
    provision_file()->create_dir($le_cert, dt('[provision_hosting_le] LE certs'), 0700);
    provision_file()->create_dir($le_acme, dt('[provision_hosting_le] LE challenges'), 0711);
    provision_file()->create_dir($le_ctrl, dt('[provision_hosting_le] LE ctrl'), 0711);

    if (!provision_file()->exists($le_exec)->status()) {
      drush_log('[provision_hosting_le] Please upload letsencrypt.sh to ' . $le_exec, 'warning');
      drush_log('[provision_hosting_le] URL: https://raw.githubusercontent.com/lukas2511/letsencrypt.sh/master/letsencrypt.sh', 'warning');
      return FALSE;
    }

    // Filter out the '@'
    $sitename = substr(d()->name, 1);
    drush_log('[provision_hosting_le] sitename is ' . $sitename);

    if (preg_match("/\.(?:host8|boa)\.(?:biz|io)$/", $sitename) ||
        preg_match("/\.(?:dev|devel|temp|tmp|temporary)\./", $sitename)) ||
        preg_match("/\.(?:test|testing|stage|staging)\./", $sitename)) ||
        $sitename == 'hostmaster') {
      drush_log('[provision_hosting_le] Skipping LE setup for ' . $sitename);
      return FALSE;
    }

    $cert_dir = $le_cert . "/" . $sitename;
    drush_log('[provision_hosting_le] This cert_dir is ' . $cert_dir);

    $immutable = $le_ctrl . "/dont-overwrite-" . $sitename;
    drush_log('[provision_hosting_le] The optional immutable flag is ' . $immutable);

    $alt_names = implode(' -d ', str_replace('/', '.', d()->aliases));
    drush_log('[provision_hosting_le] LE certificate for ' . $sitename);
    drush_log('[provision_hosting_le] ALT names: -d ' . $alt_names);

    if (provision_file()->exists($immutable)->status() &&
        provision_file()->exists($cert_dir)->status()) {
      drush_log("[provision_hosting_le] Immutable protection mode detected for this domain: " . $cert_dir, 'warning');
      drush_log("[provision_hosting_le] SSL Certificate for this domain already exists in: " . $cert_dir, 'warning');
      drush_log("[provision_hosting_le] You can replace it with any other certificate since it will be left here as-is forever.", 'warning');
      drush_log("[provision_hosting_le] To re-activate LE auto-renewals please delete this file: " . $immutable, 'warning');
      drush_log("[provision_hosting_le] NOTE: On hosted Aegir service you need to contact your host support for further assistance.", 'warning');
    }
    else {
      drush_log("[provision_hosting_le] To stop the LE Certificate auto-renewals please create an empty ctrl file.", 'info');
      drush_log("[provision_hosting_le] Path to use for this site specific empty ctrl file: " . $immutable, 'info');
      drush_log("[provision_hosting_le] You could then replace existing cert with any other cert since it will be left here as-is forever.", 'info');
      drush_log("[provision_hosting_le] NOTE: On hosted Aegir service you need to contact your host support for further assistance.", 'info');
      $output = '';
      exec("/bin/bash " . $le_exec . " -c -d " . $sitename . " -d " . $alt_names, $output);
      $acme_result = implode(' ', $output);
      drush_log('[provision_hosting_le] ACME Output: ' . $acme_result);
      if (!provision_file()->exists($cert_dir)->status()) {
        drush_log("[provision_hosting_le] Error: The LE Certificate was not generated. This is probably a permissions issue.", 'error');
        return FALSE;
      }
      else {
        if (preg_match("/unchanged.*Skipping/i", $acme_result)) {
          drush_log("[provision_hosting_le] The existing LE Certificate is up to date in " . $cert_dir, 'success');
        }
        elseif (preg_match("/Forcing.*renew/i", $acme_result) && preg_match("/Creating.*fullchain/i", $acme_result)) {
          drush_log("[provision_hosting_le] The LE Certificate has been successfully updated in " . $cert_dir, 'success');
        }
        elseif (preg_match("/Forcing.*renew/i", $acme_result) && !preg_match("/Creating.*fullchain/i", $acme_result)) {
          drush_log("[provision_hosting_le] The LE Certificate attempted update looks incomplete in " . $cert_dir, 'warning');
          drush_log("[provision_hosting_le] Make sure that all aliases have valid DNS names pointing to your instance IP address.", 'warning');
        }
        else {
          drush_log("[provision_hosting_le] The LE Certificate has been successfully [re]generated in " . $cert_dir, 'success');
        }
      }
    }

    $ssl_symlinks[] = d('@server_master')->aegir_root . "/config/ssl.d/" . $sitename;
    $ssl_symlinks[] = d('@server_master')->aegir_root . "/config/server_master/ssl.d/" . $sitename;

    foreach ($ssl_symlinks as $symlink) {
      if (provision_file()->exists($symlink)->status()) {
        drush_log('[provision_hosting_le] File exists: ' . $symlink);

        if (!is_link($symlink)) {
          drush_log('[provision_hosting_le] Moving original directory out of the way: ' . $symlink);

          // This will overwrite symlink.bak if necessary, so we don't end up
          // with dozens of backups of unused certificates.
          rename ($symlink, $symlink . ".bak");
        }
        else {
          drush_log('[provision_hosting_le] SSL certificate already symlinked: ' . $symlink, 'success');
          continue;
        }
      }

      drush_log('[provision_hosting_le] Creating symlink at ' . $symlink);

      if (symlink($cert_dir, $symlink)) {
        drush_log('[provision_hosting_le] Symlinked cert directory to ' . $symlink, 'success');
      }
      else {
        drush_log('[provision_hosting_le] Could not symlink cert directory to ' . $symlink, 'warning');
      }
    }

    drush_log('[provision_hosting_le] Replacing openssl symlinks.');

    $filenames = array(
      'openssl.crt' => 'cert.pem',
      'openssl.csr' => 'cert.csr',
      'openssl.key' => 'privkey.pem',
      'openssl_chain.crt' => 'fullchain.pem',
    );

    $success = TRUE;
    foreach ($filenames as $original => $target) {
      // Remove current symlink or file (this would have been generated by
      // Aegir AFTER the original dir symlinking, meaning it's self-generated
      // and therefore unimportant.
      unlink($cert_dir . "/" . $original);

      $success = ($success && symlink($cert_dir . "/" . $target, $cert_dir . "/" . $original));
    }

    if ($success) {
      drush_log('[provision_hosting_le] Successfully replaced all symlinks.', 'success');
    }
    else {
      drush_log('[provision_hosting_le] Could not replace one or more symlinks. Check ' . $certdir, 'warning');
    }

    $pid = $le_ctrl . "/" . $sitename . ".pid";

    if ($sitename != 'hostmaster' && file_exists($cert_dir) && !file_exists($pid)) {
      provision_file()->file_put_contents($pid, $sitename)
        ->succeed('[provision_hosting_le] Created pid ' . $pid)
        ->fail('[provision_hosting_le] Could not create pid ' . $pid);
      // We will not run the secondary Verify if pid file doesn't exist,
      // to avoid verify-inside-verify loop which could overload the system.
      if (provision_file()->exists($pid)->status()) {
        drush_log('[provision_hosting_le] Running Verify again to reload web server once openssl_chain.crt is present in the vhost');
        $local_uri_verify = '@' . $sitename;
        provision_backend_invoke($local_uri_verify, 'provision-verify');
        // We could run it via frontend but it is not needed currently.
        //provision_backend_invoke('@hostmaster', 'hosting-task', array($local_uri_verify, 'verify'), array('force' => TRUE));
        sleep(5); // A small trick to avoid high load and race conditions.
      }
    }

    drush_log('[provision_hosting_le] Running nginx service graceful reload');
    shell_exec('sudo /etc/init.d/nginx reload');

  }
}
