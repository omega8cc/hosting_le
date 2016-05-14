Summary
=======

This module replaces self-generated Aegir certificates with Let's Encrypt ones.

Developped for Advisor Websites. Use at your own risk.

Requirements
============

This module only works with nginx\_ssl. You must have the letsencrypt.sh library (see Usage).

Usage
=====

This module uses [letsencrypt.sh](https://github.com/lukas2511/letsencrypt.sh) and makes several assumptions:

* Aegir and its config live at /var/aegir and /var/aegir/config respectively
* The certificate's name in Aegir (and thus its directory) has the canonical site's name,
  eg. example.com has a certificate in Aegir named example.com.
* A site that uses SSL wants to use Let's Encrypt, or at least a certificate located at /var/aegir/config/acme/letsencryptsh/certs/example.com
* Your letsencrypt.sh can be found at /var/aegir/config/acme/letsencryptsh/letsencrypt.sh

Note that this module will NOT regenerate an existing symlink OR an existing Let's Encrypt certificate, meaning you can safely use another third-party's certificate if you so choose, simply symlink both SSL directories (/var/aegir/config/ssl.d/example.com and /var/aegir/config/server\_master/ssl.d/example.com).

Likewise, you can simply put your private certificate in /var/aegir/config/acme/letsencryptsh/certs/example.com, though you should know Aegir expects openssl.key and openssl.crt (or a symlink with these filenames) to exist, and this module will try to generate them for you.

To know more about letsencrypt.sh usage, go to https://github.com/lukas2511/letsencrypt.sh

General Notes
=============

The main important file here is drush/provision\_awforce\_ssl.drush.inc , which allows us to run code after the verify as the Aegir user. The www-data must not be granted access to letsencrypt.sh for security reasons.

The file awforce\_ssl.hosting.feature.awforce\_ssl.inc must be present for Aegir to load the provision (drush) extension, and will do so only if the module is enabled.

License
=======

This project is licensed under the GNU GENERAL PUBLIC LICENSE Version 2, see LICENSE.txt for details.
