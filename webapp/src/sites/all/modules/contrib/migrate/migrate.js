
/** 
 * @file
 * @author dasd
 * Migrate module content set assignment screen UI.
 * Source fields already used are moved to the bottom of the select list, and displayed with
 * the number of uses
 */

var migrate_old_selected_dd_val = ''; // to store the previously selected value when user put focus on a dropdown

$('document').ready(function() {
  var src_flds = $("select[id^='edit-srcfield']"); 

  $.each(src_flds, function() {
    $(this).change(function(){      
      processSelects($(this));
    });
    $(this).focus(function() {      
      migrate_old_selected_dd_val=$(this).find('option:selected').val();
    });
    processSelects($(this)); // initial load call
  });
});

/*
 * The following function takes a source field select element as an argument and sets its text
 * based on the number of the times the source field is used.
 */
function processSelects(current_element) {
  var val = current_element.find('option:selected').val();  
  var old_val = migrate_old_selected_dd_val;
  
  var src_flds = $("select[id^='edit-srcfield']");
  
  $.each(src_flds, function() {
    var options = $(this).children('option');
    $.each(options, function() {
      if ($(this).val() == val && val != '') {
        var option_text = addDF($(this).text());
        $(this).text(option_text);
      }
      if ($(this).val() == old_val && old_val!='') {
        var option_text = removeDF($(this).text());
        $(this).text(option_text);        
      }      
    });
    sortOptions($(this));
    addSeparator($(this));
  });
}

/*
 * Adding the additional text
 */
function addDF(textStr) {  
  var out = splitNoText(textStr);
  return out.str + ' (' + (out.no+1) + ')';   
}

/*
 * Removing the additional text
 */
function removeDF(textStr) {
  var out = splitNoText(textStr);
  if (out.no){
    var no_str = out.no-1;
    if(no_str == 0){
      return out.str;
    }
    else{
      return out.str + ' (' + no_str + ')';
    }
    
  }
  else{
    return out.str; 
  }
  
}

/*
 * Return output as an object with str & no properties by splitting the text
 * received.
 */
function splitNoText(textStr) {
  var output = {str:'',no:''};
  var regexp = /\(\d+\)/;
  if (regexp.test(textStr)) {
    var ar = regexp.exec(textStr);
    output.no = parseInt(ar[0].replace(/\(/,'').replace(/\)/,''));
    output.str = textStr.split('(')[0]; 
  }
  else {
    output.str=textStr;
  }
  return output;
}

function sortOptions(elem){
  var options = elem.children();
  var selected_value = elem.val();
  
  var option_html=$.makeArray(options).sort(function(a,b) {    
    var a_no=splitNoText(a.text).no;
    var b_no=splitNoText(b.text).no;
    return a_no == b_no? 0: a_no < b_no ? -1:1;
  });
  elem.html(option_html);
  elem.val(selected_value);
}

function addSeparator(elem) {
  elem.children().removeClass('migrate-option-separator');
  $.each(elem.children(), function(){
    var first_out = splitNoText($(this).text()) 
    if(first_out.no != ''){
      $(this).prev().addClass('migrate-option-separator');
      return false;
    }
  });
}

