
if (!Drupal.ConditionalFields) {
  Drupal.ConditionalFields = {};
}

Drupal.ConditionalFields.switchField = function(id, values, onPageReady) {
  // Check each controlled field
  if (Drupal.settings.ConditionalFields.controlling_fields == undefined || Drupal.settings.ConditionalFields.controlling_fields[id] == undefined) {
    return;
  }
  $.each(Drupal.settings.ConditionalFields.controlling_fields[id], function(i, controlledField) {
    var triggers = Drupal.ConditionalFields.checkTriggered(controlledField, values);
    // If the field was not triggered, hide it
    if (!triggers) {
      Drupal.ConditionalFields.doAnimation(controlledField, 'hide', onPageReady);
    }
    // Else check other controlling fields: if any one doesn't trigger, hide the field and stop checking
    else {
      var otherTriggers = true;
      $.each(Drupal.settings.ConditionalFields.controlling_fields, function(ii, maybeControllingField) {
        if (ii != id) {
          $.each(maybeControllingField, function(iii, maybeControlledField) {
            if (maybeControlledField.field_id == controlledField.field_id) {
              otherTriggers = Drupal.ConditionalFields.checkTriggered(maybeControlledField, Drupal.ConditionalFields.findValues($(ii)));
              if (!otherTriggers) {
                return false;
              }
            }
          });
        }
        if (!otherTriggers) {
          Drupal.ConditionalFields.doAnimation(controlledField, 'hide', onPageReady);
          return false;
        }
      });
      if (otherTriggers) {
        Drupal.ConditionalFields.doAnimation(controlledField, 'show', onPageReady);
      }
    }
  });
}

Drupal.ConditionalFields.checkTriggered = function(controlledField, selectedValues) {
  var triggers = false;
  $.each(controlledField.trigger_values, function(ii, val) {
    if (jQuery.inArray(val, selectedValues) !== -1) {
      triggers = true;
      return false;
    }
  });
  return triggers;
}

Drupal.ConditionalFields.doAnimation = function(fieldSettings, showOrHide, onPageReady) {
  /* Multiple fields are enclosed in a wrapper */
  if ($(fieldSettings.field_id).parents('#' + fieldSettings.field_id.substring(13) + '-add-more-wrapper').length == 1) {
    var toSwitch = $('#' + fieldSettings.field_id.substring(13) + '-add-more-wrapper');
  } else {
    var toSwitch = $(fieldSettings.field_id);
  }

  if (Drupal.settings.ConditionalFields.ui_settings == 'disable') {
    var disabled = '';
    if (showOrHide == 'hide') {
      disabled = 'disabled';
    }
    toSwitch.find('textarea, input, select').attr('disabled', disabled);
  }
  /* Avoid flickering */
  else if (onPageReady == true) {
    /* Setting css instead of simply hiding to avoid interference from collapse.js */
    showOrHide == 'show' ? toSwitch.show() : toSwitch.css('display', 'none');
  }
  else {
    switch (Drupal.settings.ConditionalFields.ui_settings.animation) {
      case 0:
        showOrHide == 'show' ? toSwitch.show() : toSwitch.hide();
        break;
      case 1:
        /* Don't double top and bottom margins while sliding. */
        var firstChild = toSwitch.children(':first-child');
        var marginTop = firstChild.css('margin-top');
        var marginBottom = firstChild.css('margin-bottom');
        firstChild.css('margin-top', '0').css('margin-bottom', '0');
        if (showOrHide == 'show') {
          toSwitch.slideDown(Drupal.settings.ConditionalFields.ui_settings.anim_speed, function() {
            firstChild.css('margin-top', marginTop).css('margin-bottom', marginBottom);
          });
        }
        else {
          toSwitch.slideUp(Drupal.settings.ConditionalFields.ui_settings.anim_speed, function() {
            firstChild.css('margin-top', marginTop).css('margin-bottom', marginBottom);
          });
        }
        break;
      case 2:
        showOrHide == 'show' ? toSwitch.fadeIn(Drupal.settings.ConditionalFields.ui_settings.anim_speed) :
                               toSwitch.fadeOut(Drupal.settings.ConditionalFields.ui_settings.anim_speed);
    }
  }
}

Drupal.ConditionalFields.findValues = function(field) {
  var values = [];
  field.find('option:selected, input:checked, input:text, textarea').each( function() {
    values[values.length] = this.value;
  });
  return values;
}       

Drupal.ConditionalFields.fieldChange = function() {
  var values = Drupal.ConditionalFields.findValues($(this));
  var id = '#' + $(this).attr('id');
  Drupal.ConditionalFields.switchField(id, values, false);
}

Drupal.behaviors.ConditionalFields = function (context) {
  $('.conditional-field.controlling-field:not(.conditional-field-processed)').addClass('conditional-field-processed').each(function () {
    /* Set default state */
    Drupal.ConditionalFields.switchField('#' + $(this).attr('id'), Drupal.ConditionalFields.findValues($(this)), true);
    if ($(this).find('option, input:not(:text)').length > 0) {
      /* Apparently, Explorer doesn't catch the change event? */
      $.browser.msie == true ? $(this).click(Drupal.ConditionalFields.fieldChange) : $(this).change(Drupal.ConditionalFields.fieldChange);
    }
    else if ($(this).find('textarea, input:text').length > 0) {
      $(this).keyup(Drupal.ConditionalFields.fieldChange);
    }
  });
};
