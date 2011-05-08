IMPORTANT: This is the final release of Migrate V1 - there will be no more support for
this version of Migrate going forward.


The Migrate module provides a flexible framework for migrating content into Drupal from other
sources (e.g., when converting a web site from another CMS to Drupal). Out-of-the-box, support 
for creating Drupal nodes, fields, comments, url aliases, users+profiles and roles are included. 
Hooks permit migration of other types of content.

The combination of the Table Wizard and Migrate modules is designed to support a migration process 
along these lines:

 * Import - Table Wizard can be used to import external data into tables in your Drupal database. 
   Any tables in your Drupal database, or in other databases on the same server, can be made 
   available to views for review and migration.

 * Analysis - Table Wizard performs analysis of table data (ranges, sizes, etc.), and supports 
   collaborative documentation of table fields.

 * Content sets - Create Views which represent the different source records that you wish to 
   migrate. The Views utilize the Views integration you just created with Table Wizard.

 * Migration - Migrate takes rows from the content sets creates Drupal objects from them. Migration 
   is reversible - migrated data can be quickly backed out and migrated again. A dashboard is provided to easily manage the process. Background processing via cron is supported for large migrations.

 * Auditing - Errors and other conditions flagged during migration are flagged and can be reviewed 
   in Views). Also, Migrate maintains mapping tables from the source data to the resulting Drupal 
   objects, enabling the creation of comparison views.

Acknowledgements 
----------------

Much of the Migrate module functionality was sponsored by Cyrve, for its clients GenomeWeb 
(http://www.genomeweb.com) and The Economist (http://www.economist.com). The original code was 
based on node_import.

Author
------

Mike Ryan - http://drupal.org/user/4420
