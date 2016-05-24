Summary
=======

This module replaces self-generated Aegir certificates with Let's Encrypt ones.

Initial version developed by `@gboudrias` for Advisor Websites. Improved by `@omega8cc`.

Requirements
============

This module works with Apache and Nginx. You must have the third-party `letsencrypt.sh` library uploaded (see Usage).

Usage
=====

This module uses [letsencrypt.sh](https://github.com/lukas2511/letsencrypt.sh) and automates almost everything:

* Aegir and its config may live in any non-standard directory or in the canonical `/var/aegir`
* The certificate's name in Aegir (and thus its directory) is the same as the site's name
* Your `letsencrypt.sh` can be found at `[aegir_root]/tools/le/letsencrypt.sh`
* All site's aliases must have valid DNS names pointing to your server IP address
* Aliases redirection must be disabled if all aliases are expected to be included as SAN names
* If aliases redirection is enabled, the certificate created will list only the main site name
* This module will ignore Hostmaster site and all sites with special keywords in their names

Let's Encrypt API limits
========================

Let's Encrypt API for live, real certificates has its own requirements and limits you should be aware of.

Please visit [Let's Encrypt website](https://community.letsencrypt.org/t/rate-limits-for-lets-encrypt/6769) for details.

Demo mode
=========

It is recommended to test this module in Let's Encrypt demo mode, so it will not hit limits enforced for live, real Let's Encrypt SSL certificates. To enable demo mode, please create an empty control file: `[aegir_root]/tools/le/.ctrl/ssl-demo-mode.pid`

To switch the mode to live, simply delete `[aegir_root]/tools/le/.ctrl/ssl-demo-mode.pid` and run Verify task on the SSL enabled site again.

You could switch it back and forth to demo/live mode by adding and deleting the control file, and it will re-register your system via Let's Encrypt API, but we have not tested how it may affect already generated live certificates.

This module will create all required directories it needs to operate on the first attempt to run site Verify task with SSL option enabled, but you may want to create at least `[aegir_root]/tools/le/.ctrl/` before running it for the first time, so the demo mode will be active on the first attempt.

Read the task log lines which start with `[hosting_le]` prefix for more information.

Exceptions
==========

The list of keywords to use in the site's main name to have the site ignored by this module:

  `.(dev|devel|temp|tmp|temporary|test|testing|stage|staging).`

Examples: `foo.temp.bar.org`, `foo.test.bar.org`, `foo.dev.bar.org`

Custom certificates
===================

This module will regenerate existing symlinks and existing Let's Encrypt key and certificate, if needed, like when you will update site's aliases or the certificate will have less than 30 days to expiration date. However, you can safely use another third-party's certificate if you so choose, replacing the key and certificates created by Let's Encrypt integration library, if you will create an empty control file: `[aegir_root]/tools/le/.ctrl/dont-overwrite-example.com.pid`

Likewise, you can simply put your private certificate in `[aegir_root]/tools/le/certs/example.com/`, though you should know Aegir expects `openssl.key` and `openssl.crt` (or a symlink with these filenames) to exist, and this module will generate them for you.

Certificates updates and renewals
=================================

To renew or update the Let's Encrypt certificate, with all aliases added as Subject Alternative Names (SAN), it is enough to run the Verify task on the site. It's planned to automate this procedure using different methods (implemented in BOA via automated sites Verify tasks).

To know more about `letsencrypt.sh` usage, please visit: https://github.com/lukas2511/letsencrypt.sh

General Notes
=============

The main important file here is `drush/provision_hosting_le.drush.inc` , which allows us to run code after the verify as the Aegir user. The `www-data` group must not be granted access to `letsencrypt.sh` and its base directory `[aegir_root]/tools/le/` for security reasons.

The file `hosting.feature.le.inc` must be present for Aegir to load the Provision (Drush) extension, and will do so only if the module is enabled.

License
=======

This project is licensed under the GNU GENERAL PUBLIC LICENSE Version 2, see LICENSE.txt for details.
