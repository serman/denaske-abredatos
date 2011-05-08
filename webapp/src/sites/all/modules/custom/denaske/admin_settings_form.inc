<?php

function denaske_settings_form(){
  
  $form = array();

$form['title'] = array(

      '#type' => 'textfield', 

      '#title' => t('Subject'), 

      '#default_value' => $node->title, 

      '#size' => 60, 

      '#maxlength' => 128, 

      '#required' => TRUE,

    );
    
  $form['denaske-distancia'] = array(
    '#type' => 'textfield',
    '#title' => 'Radio distancia',
    //'#default_value' => variable_get('denaske_distancia', 0),
    '#required' => TRUE,
    
  );
 //return 'hola';
  return system_settings_form($form);
  
}