<?php

/**
 * Implements hook_form().
 */
function mortar_admin_setting_form($node, &$form_state) {
  $form = array();

  $form['overview'] = array(
    '#markup' => t('This interface allows administrators to manage general MORTAR Settings'),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );

  $form['user_log_settings'] = array(
    '#title' => t('User Log Settings'),
    '#description' => t('Settings Logging Changes to User'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
   $form['user_log_settings']['user_field_log_list'] = array(
    '#title' => t('List user fields to log changes'),
    '#description' => t('List all the User fields you want to log any changes. Seperate fields using |'),
    '#type' => 'textfield',
    '#default_value' => variable_get('user_field_log_list'),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}


function mortar_admin_setting_form_submit($form, &$form_state) {

    variable_set('mortar_user_field_log_list', $form_state['values']['user_field_log_list']['value']);
    
}

