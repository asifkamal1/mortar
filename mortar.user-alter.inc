<?php

/*
 *
 */
function mortar_user_view($account, $view_mode, $langcode) {
    #debug('hello');
    global $user;
    $account_user = user_load($user -> uid);
    
    mortar_user_record_view_alter($account, $account_user);
}


/*
 *
 */
function mortar_user_record_view_alter($account, $account_user) {
    /*
    $account -> content['mortar_add_training_record_form'] = array(
        '#type' => 'fieldset',
        '#title' => 'Add Training Record',
        '#weight' => 99,
        
    );
    */
    
    module_load_include('inc', 'node', 'node.pages');
    $form = node_add('training_record');
    $account -> content['field_add_training_record'] = $form;
    drupal_set_title($account_user -> name);
    
    #debug($build['#account'] -> field_personal_training_record);
    #$build['#account'] -> field_personal_training_record['und'][0]['vname'] = 'training_record|mobile_display_user';
    #debug($build['#account'] -> field_personal_training_record);
    #$account -> content['mortar_add_training_record_form']['#attached']['library'][] = array('system', 'drupal.collapse');
    #$account -> content['mortar_add_training_record_form']['#attributes']['class'][] = 'collapsible collapsed'; 
    #$account -> content['mortar_add_training_record_form']['mortar_trainig_record_form'] = $form;
    if(in_array('administrator', $account_user -> roles)) {
        #debug($account -> content['field_add_training_record']);
    }
}

/*
 *
 */
function mortar_form_user_register_form_alter(&$form, &$form_state, $form_id) {
    $form['#submit'][] = 'mortar_user_request_default_training_records';
    #debug($form);
}


/*
 *
 */
function mortar_form_user_login_block_alter(&$form, &$form_state, $form_id) {
    #debug($form['links']['#markup']);
    #drupal_set_message(t('MORTAR is currently not accessible due to the maintenance. We should be back in few hours.'), 'warning');
    #$form['#submit'] = NULL;
    
    $form['links']['#markup'] = $form['links']['#markup'] .
    '<div class="item-list"><ul><li>
    To create an account please contact
    <a href="mailto:OCTRU<octrutrialshub@ndorms.ox.ac.uk>?Subject=MORTAR%20user%20account%20enquiry." target="_top">
    OCTRU</a>.</li></ul></div>';
}


/*
 *
 */
function mortar_form_user_login_alter(&$form, &$form_state, $form_id) {
    #$form['#submit'] = NULL;
    #drupal_set_message(t('MORTAR is currently not accessible due to the maintenance. We should be back in few hours.'), 'warning');
    $form['links']['#markup'] =
    '<div class="item-list"><ul><li>
    To create an account please contact
    <a href="mailto:OCTRU<octrutrialshub@ndorms.ox.ac.uk>?Subject=MORTAR%20user%20account%20enquiry." target="_top">
    OCTRU</a>.</li></ul></div>' . '
    <div class="item-list"><ul><li>
    If your "one-time login link" expired, please request new password <a href="/user/password" >here</a>.</li></ul></div>'
    ;
    
    
}


/*
 *
 */
function mortar_user_request_default_training_records (&$form, &$form_state, $form_id = ''){
    
    $trainee_id = $form_state['user'] -> uid;
    $training_requests = array(35, 36, 37); //CV, JD, GCP
    $training_record_counter = 0;
    
    foreach($training_requests as $training_request) {
        
        $training_type = $training_request;
        $has_training = mortar_has_training_record($trainee_id, $training_type, $training_name = NULL, $controlled_doc_id = NULL, $controlled_doc_v = NULL);
        
        if($has_training == NULL) {
            $node = new stdClass(); // We create a new node object
            $node -> type = "training_record"; // Or any other content type you want
            $node -> language = LANGUAGE_NONE; // Or any language code if Locale module is enabled. More on this below *
            
            node_object_prepare($node); // Set some default values.
            
            $node -> uid = $trainee_id; // Or any id you wish
            $node -> field_training_type[$node->language][0]['target_id'] = $training_type;
            $node -> field_training_status[$node->language][0]['target_id'] = 34;
            $node -> field_trainee[$node->language][0]['target_id'] = $trainee_id;
            $node = node_submit($node);
            node_save($node);
            
            $training_record_counter += 1;
        }
    }
    
    if($training_record_counter != 0) {
        drupal_set_message(t($training_record_counter . ' Training Request(s) added.'), 'status');
    }
    else {
        drupal_set_message(t($training_record_counter . ' Training Requests added.'), 'warning');
    }
}


/*
 *
 */
function mortar_user_profile_form_alter(&$form, &$form_state, $form_id, $user) {
    
    if(in_array('administrator', $form['#user'] -> roles)) {
        unset($form['account']);
        unset($form['actions']);
        unset($form['field_ctu']);
        unset($form['field_first_name']);
        unset($form['field_last_name']);
        unset($form['field_job_title']);
        #debug($form);
        drupal_set_message(t('You do not have permission to edit this account.'), 'error');
    }
    //unsets administrator role for none admin
    if(isset($form['account']['roles']['#options'][3])) {
        unset($form['account']['roles']['#options'][3]); 
    }
}
