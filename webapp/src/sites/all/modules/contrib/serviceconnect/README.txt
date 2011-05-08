
PREFACE
-------
This module enable users to login with 3th party services.

The following services are supported:

* Facebook
* Twitter
* Hyves

This module is able to fetch profile information and store this inforamtion in
the user profile (via profile or content_profile module).

CONTENTS
--------
1. Installation
2. Configuration
2.1. Facebook
2.2. Twitter
2.3. Hyves
3. Manage services
4. Statistics

1. Installation
--------------------------------------------------------------------------------
Copy Service Connect into your modules directory (i.e. sites/all/modules).
Before installing Service Connect, you need to download the OAuth library.

You can do this with the following commands (from the module directory):

  wget http://oauth.googlecode.com/svn/code/php/OAuth.php
  mv OAuth.php oauth.lib.php

Or with curl:

  curl -O http://oauth.googlecode.com/svn/code/php/OAuth.php
  mv OAuth.php oauth.lib.php

Alternatively, you may download this file by hand from:

  http://oauth.googlecode.com/svn/code/php/

Only download OAuth.php and rename it to oauth.lib.php, so that you have:

  sites/all/modules/serviceconnect/oauth.lib.php

The module directory may be different.

Enable Service Connect on admin/build/modules or use "drush en serviceconnect".
You should also enable the submodules for each service you want to use:

* serviceconnect_facebook
* serviceconnect_twitter
* serviceconnect_hyves

Go to admin/user/permissions and enable to following permissions:

* administer serviceconnect
  For roles which may change the serviceconnect settings.
* manage all linked services
  For roles which may see and unlink services for all users.
* manage own linked services
  For roles which may see and unlink their own services.
* link facebook profile
* link twitter profile
* link hyves profile
  For roles which may link this service to their profile. Recommended to enable
  for authenticated users.
* login via facebook
* login via twitter
* login via hyves
  For roles which may login using this service. Recommended to enable for all
  roles, or only for anonymous roles if you want to force users to logout before
  logging in with another account.

2. Configuration
--------------------------------------------------------------------------------
The global settings page is available at admin/settings/serviceconnect. Each
service will have their own tab available on this page.

2.1. Facebook
-------------
Register your application on Facebook before configuring the Service Connect
module. You can do this on:

  http://www.facebook.com/developers/apps.php

When creating a new application you have to fill in the following details:

* Name
  The name of your application
* Contact Email
  Your e-mail address
* Site URL (on the 'Website' tab)
  URL to your homepage ('http://example.com/')
* Site Domain (on the 'Website' tab)
  Domainname of your website ('example.com')
* White serverlist (on the 'Advanced' tab)
  Fill in the IP's of your servers. This may be your own IP when testing on
  your local machine.

You will find your application keys on the 'My Applications' page. Go to
admin/settings/serviceconnect/facebook to configure the Drupal module. Fill in
the API key, Application secret and Application ID. Leave the URL settings
as-is. Optionally you may fill in terms and conditions and configure the profile
import settings.

You may now login using Facebook on /login-via-facebook or linking your Facebook
profile with the currently logged in user by going to /link-facebook-profile.

2.2. Twitter
------------
Register your application on Twitter before configuring the Service Connect
module. You can do this on:

  http://dev.twitter.com/apps

When creating a new application you have to fill in the following details:

* Application name
* Description
* Application website
  URL to your homepage ('http://example.com')
* Organization
* Application Type
  Set to 'Browser'
* Callback URL
  Use the URL to your homepage. The Drupal module will override this setting.
* Default Access type
  Service Connect does not require write permission. Set this to 'Read-only' if
  no other modules are requiring write access.

You will find your application keys on the 'Twitter applications' page (click
on 'My apps'). Go to admin/settings/serviceconnect/twitter to configure the
Drupal module. Fill in the consumer key and consumer secret. Leave the URL
settings as-is. Optionally you may fill in terms and conditions and configure
the profile import settings.

Now click the 'Test connection' tab. This page should say 'Succesfully
connected to Twitter'.

You may now login using Twitter on /login-via-twitter or linking your Twitter
profile with the currently logged in user by going to /link-twitter-profile.

2.3. Hyves
----------
Register your application on Hyves before configuring the Service Connect
module. You can do this on:

  http://www.hyves.nl/developer/applications/

Click on 'New Data-API application'. Fill in the following details:

* Type
  Set to 'Web'
* Name
* Website/url
  URL to your frontpage ('http://example.com')
* IP adressses
  Fill in the IP's of your servers. This may be your own IP when testing on
  your local machine.

You will find your application keys on the 'Applications' page. Go to
admin/settings/serviceconnect/hyves to configure the Drupal module. Fill in the
application key and application secret. Leave the URL settings as-is. Optionally
you may fill in terms and conditions and configure the profile import settings.

Now click the 'Test connection' tab. This page should say 'Succesfully
connected to Hyves'.

You may now login using Hyves on /login-via-hyves or linking your Hyves
profile with the currently logged in user by going to /link-hyves-profile.

3. Manage services
------------------
On the user profile page you will find a new tab 'External logins'. This page
shows a list of all 3th party profiles linked to this user.

You may unlink the 3th party profile from this user by clicking on 'unlink'.

This page is only available when the logged in user has permission to use this
page (see §1).

4. Statistics
-------------
There are basic statistics available at admin/settings/socialconnect/statistics.