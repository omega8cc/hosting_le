Summary
=======

This module replaces self-generated Aegir certificates with Let's Encrypt ones.

Initial version developed by `@gboudrias` for Advisor Websites.
Improved by `@omega8cc` and `@helmo`.

Requirements
============

This module works with Apache and Nginx.
You must have the third-party `letsencrypt.sh` library uploaded (see Usage).

Quick Start
===========

While it is [included in BOA by default](https://github.com/omega8cc/boa/blob/master/docs/SSL.txt),
you should be able to add it easily on any Aegir 3.x vanilla system:

1. Upload [hosting_le module](https://github.com/omega8cc/hosting_le) to
   profiles/hostmaster/modules/contrib/

2. Create expected directory tree, download the script, and enable demo mode:

```
$ su -s /bin/bash - aegir
$ mkdir -p /var/aegir/tools/le/.ctrl
$ touch /var/aegir/tools/le/.ctrl/ssl-demo-mode.pid
$ cd /var/aegir/tools/le/
$ wget https://raw.githubusercontent.com/omega8cc/letsencrypt.sh/master/letsencrypt.sh
$ chmod 0700 letsencrypt.sh
```

3. Enable apache_ssl or nginx_ssl on the server node at /hosting/c/server_master

4. Enable "Hosting LE SSL" and "Hosting LE Vhost" at /admin/hosting (in Experimental)

5. Edit a hosted site node to set SSL to "Enabled" (NOT to "Required", until you
   enable live LE mode) and make sure to fill the "New encryption key" field
   with the name of your site, e.g. example.com

6. Wait for the Verify task to run. If it's not all-green, check the task log
   lines which start with `[hosting_le]` prefix for more information.

Usage
=====

This module depends on the [forked letsencrypt.sh script](https://raw.githubusercontent.com/omega8cc/letsencrypt.sh/master/letsencrypt.sh)
and automates almost everything:

* The [letsencrypt.sh](https://raw.githubusercontent.com/omega8cc/letsencrypt.sh/master/letsencrypt.sh)
  script expected path is: `[aegir_root]/tools/le/letsencrypt.sh`
* The `[aegir_root]/tools/le/` directory must be writable by your Aegir
  system user, typically `aegir`
* Aegir and its config may live in any non-standard directory
  or in the canonical `/var/aegir`
* The certificate's name in Aegir (and thus its directory) must be the same
  as the site's name
* Avoid renaming SSL-enabled sites; move aliases between site's clones instead
* Before you rename a site, disable SSL first; then re-enable once it's renamed
* Many useful details can be found also in the [BOA specific docs](https://github.com/omega8cc/boa/blob/master/docs/SSL.txt)

Caveats
=======

* Let's Encrypt leverages TLS/SNI, which works only with modern browsers
* Let's Encrypt doesn't support wildcard names
* Let's Encrypt doesn't support IDN names (for now)
* All site's aliases must have valid DNS names pointing to your server IP address
* This module will ignore sites with special keywords in their names,
  or in their redirection target (see Exceptions further below)

Caveats for non-BOA Aegir systems only
======================================

* All site's aliases must have valid DNS names pointing to your server IP address,
  unless redirection is used, and the target has a valid DNS name
* Aliases redirection must be disabled if all aliases are expected
  to be included as SAN names
* If aliases redirection is enabled, the certificate created will list only
  the redirection target name

Let's Encrypt API limits
========================

Let's Encrypt API for live, real certificates has its own requirements and
limits you should be aware of.

Please visit [Let's Encrypt website](https://letsencrypt.org/docs/rate-limits/)
for details.

Demo mode
=========

It is recommended to test this module in Let's Encrypt demo mode, so it will not
hit limits enforced for live, real Let's Encrypt SSL certificates. To enable
demo mode, please create an empty control file:
`[aegir_root]/tools/le/.ctrl/ssl-demo-mode.pid`

To switch the mode to live, delete `[aegir_root]/tools/le/.ctrl/ssl-demo-mode.pid`
and run Verify task on the SSL enabled site again.

You could switch it back and forth to demo/live mode by adding and deleting
the control file, and it will re-register your system via Let's Encrypt API,
but we have not tested how it may affect already generated live certificates
once you will run the switch many times, so please try not to abuse this feature.

It is important to remember that once you will switch the Let's Encrypt mode
to demo from live, or from live to demo, by adding or removing
the `[aegir_root]/tools/le/.ctrl/ssl-demo-mode.pid` control file, it will not
replace all previously issued certificates instantly, because certificates
are updated, if needed, only when you (or the BOA system for you during its
daily maintenance, if used) will run Verify tasks on SSL enabled sites.

These BOA specific Verify tasks are normally scheduled to run weekly,
between Monday and Sunday, depending on the first character in the site's
main name, so both live and demo certificates may still work in parallel
for SSL enabled sites until it will be their turn to run Verify and update
the certificate according to currently set Let's Encrypt mode.

This module will create all required directories it needs to operate on the
first attempt to run site Verify task with SSL option enabled, but you may
want to create at least `[aegir_root]/tools/le/.ctrl/` before running it for
the first time, so the demo mode will be active on the first attempt.
This is default behaviour in BOA, which always starts the integration in
demo mode, until the control file is removed.

Read the task log lines which start with `[hosting_le]` prefix for more information.

Exceptions
==========

The list of keywords to use in the site's main name to have the site ignored
by this module:

  `.(dev|devel|temp|tmp|temporary|test|testing|stage|staging).`

Examples: `foo.temp.bar.org`, `foo.test.bar.org`, `foo.dev.bar.org`

Custom certificates
===================

This module will regenerate existing symlinks and existing Let's Encrypt key and
certificate, if needed, like when you will update site's aliases or the certificate
will have less than 30 days to expiration date. However, you can safely use another
third-party's certificate if you choose so, replacing the key and certificates
created by Let's Encrypt integration library, if you will create an empty
control file: `[aegir_root]/tools/le/.ctrl/dont-overwrite-example.com.pid`

Likewise, you can simply put your custom, non-LE certificate in
`[aegir_root]/tools/le/certs/example.com/`, though you should know that Aegir
expects `openssl.key` and `openssl.crt` (or symlinks with these filenames)
to exist, and this module will generate them for you.

Certificates updates and renewals
=================================

To renew or update the Let's Encrypt certificate, with all aliases added
as Subject Alternative Names (SAN), it is enough to run the Verify task
on the SSL enabled site. It's planned to automate this procedure using
different methods (implemented in BOA via automated sites Verify tasks).

To know more about `letsencrypt.sh` usage, please visit:
https://github.com/lukas2511/letsencrypt.sh

General Notes
=============

The main important file here is `drush/provision_hosting_le.drush.inc`,
which allows us to run code after the Verify as the Aegir user. The `www-data`
group must not be granted access to `letsencrypt.sh` and its base directory
`[aegir_root]/tools/le/` for security reasons.

The file `hosting.feature.le.inc` must be present for Aegir to load
the Provision (Drush) extension, and will do so only if the module is enabled.

License
=======

This project is licensed under the GNU GENERAL PUBLIC LICENSE Version 2,
see LICENSE.txt for details.
