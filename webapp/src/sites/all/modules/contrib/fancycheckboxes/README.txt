
Fancy Checkboxes module for Drupal.

-- SUMMARY --

Replaces default checkboxes with fancy (iOS-like) on/off switches under
configurable pages.

I was just trying for a good way to make checkboxes easier to click on
(see Fitt's Law) but came up with this. Original jQuery code taken from
http://github.com/tdreyno/iphone-style-checkboxes

For a full description of the module, visit the project page:
  http://drupal.org/project/fancycheckboxes

To submit bug reports and feature suggestions, or to track changes:
  http://drupal.org/project/issues/fancycheckboxes


-- REQUIREMENTS --

None.


-- INSTALLATION --

* Install as usual, see http://drupal.org/node/70151 for further information.

* If you want to specify PHP code to determine the pages to enable the checkbox
  replacement, enable the core PHP Filter module.


-- CONFIGURATION --

By default, after enabling most checkboxes in administration pages are converted
to Fancy checkboxes (notable exceptions are checkboxes in content and people
listings).

Optionally, you can configure the module to change all checkboxes on a per-page
basis (exactly like the block per-page visibility settings). For this, go to:

 * Administer >> Site Configurarion >> Fancy Checkboxes

where you can specify a list of paths where to activate/not activate the
checkbox replacement. If you have the core PHP Filter module enabled, you can
also specify PHP code.