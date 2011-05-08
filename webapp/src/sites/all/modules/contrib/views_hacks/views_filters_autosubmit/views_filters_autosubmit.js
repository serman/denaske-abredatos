// $Id: views_filters_autosubmit.js,v 1.1.2.3 2010/07/03 16:53:27 kratib Exp $
(function ($) {
// START jQuery

Drupal.vfas = Drupal.vfas || {};

Drupal.behaviors.vfas = function(context) {
  $('form#'+Drupal.settings.vfas.form_id+':not(.vfas-processed)', context).each(function() {
    var self = this;
    var exceptions = Drupal.settings.vfas.exceptions;
    if (exceptions) {
      exceptions = ':not('+Drupal.settings.vfas.exceptions+')';
    }    
    else {
      exceptions = '';
    }
    $(self).addClass('vfas-processed');
    $('#'+Drupal.settings.vfas.submit_id, self).hide();
    $('div.views-exposed-widget input'+exceptions, self).change(function() {
      $(self).submit();
    });
    $('div.views-exposed-widget select'+exceptions, self).change(function() {
      $(self).submit();
    });
  });
}

// END jQuery
})(jQuery);

