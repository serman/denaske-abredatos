$Id: README.txt,v 1.2 2008/07/24 14:32:35 kbahey Exp $

Copyright 2007 Khalid Baheyeldin and http://2bits.com

Description
-----------
This module intercepts all outgoing emails from a Drupal site and
reroutes them to a predefined configurable email address.

This is useful in case where you do not want email sent from a Drupal
site to reach the users. For example, if you copy a live site to a test
site for the purpose of development, and you do not want any email sent
to real users of the original site. Or you want to check the emails sent
for uniform formatting, footers, ...etc.

This is also a good demonstration of what hook_mail_alter(), available in
Drupal 5.x and later, can do.

Installation
------------
To install this module, do the following:

1. Extract the tar ball that you downloaded from Drupal.org.

2. Upload the entire directory and all its contents to your
   modules directory.

Configuration
-------------
To enable this module do the following:

1. Go to Admin -> Modules, and enable reroute email.

2. Go to Admin -> Settings -> Reroute email, and enter an email address
   to route all email to.

Bugs/Features/Patches:
----------------------
If you want to report bugs, feature requests, or submit a patch, please do so
at the project page on the Drupal web site.
http://drupal.org/project/reroute_email

Author
------
Khalid Baheyeldin (http://baheyeldin.com/khalid and http://2bits.com)

If you use this module, find it useful, and want to send the author
a thank you note, then use the Feedback/Contact page at the URL above.

The author can also be contacted for paid customizations of this
and other modules.
