//$Id$

(function($) {
  var D = window.Drupal = window.Drupal || {};
  var A = window.ajax_gmap = window.ajax_gmap || {};
  $(document).ready(function() {
    ajaxGMBuild && ajaxGMBuild($, A, D);
    A.initiate();
  });
})(jQuery);