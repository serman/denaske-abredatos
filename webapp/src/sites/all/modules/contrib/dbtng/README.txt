
CONTENTS OF THIS FILE
---------------------

 * Requirements
 * Configuration
 * API
 * More Information

REQUIREMENTS
------------

This module requires the Autoload module http://drupal.org/project/autoload.


CONFIGURATION
-------------

This module will automatically load any database information you have in your
settings.php and use that for DBTNG connections. To set the active database, 
use the key from the $db_url in your settings.php in dbtng_set_active('FOO'), 
where FOO is the key of the database you want to use.

API
___

This module uses an identical API to the Drupal 7 database layer, with the
exception that some functions have been renamed to use dbtng_ as a prefix instead
of db_.


MORE INFORMATION
----------------

For more information on using the API read its docs in the handbook,
http://drupal.org/node/310069 or at the Drupal API website, 
http://api.drupal.org/api/group/database/7 .