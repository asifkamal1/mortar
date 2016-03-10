<?php

/*
 * Implements hook form_FORM_ID_node_form_alter (API)
 */
function mortar_form_training_record_node_form_alter(&$form, &$form_state, $form_id) {
    
    //(API) global user
    global $user;
    
    //load user (API)
    $account = user_load($user -> uid); 
    
    //function in tr-node-alter.inc
    mortar_training_record_unset_options($form);
    
    //change name of the submit button
    $form['actions']['submit']['#value'] = 'Save Training Record';
    
    // ensuring the ajax upload still has all the includes
    $files = (isset($form_state['build_info']['files'])) ? $form_state['build_info']['files'] : array();
    $files[] = drupal_get_path('module', 'node') . '/node.pages.inc';
    $form_state['build_info']['files'] = $files;
    
    if(!isset($form_state['field_url_arg_0']['und']['#value'])) {
        $form_state['field_url_arg_0']['und']['#value'] = arg(0);
    }
    if(!isset($form_state['field_url_arg_1']['und']['#value'])) {
        $form_state['field_url_arg_1']['und']['#value'] = arg(1);
    }
    if(!isset($form_state['field_url_arg_2']['und']['#value'])) {
        $form_state['field_url_arg_2']['und']['#value'] = arg(2);
    }
    
    //load node from current page
    if($node_load = menu_get_object()) {
        $node = node_load($node_load -> nid);
        $node_type = $node -> type;
        #debug($form);
    }
    
    //prepare 'Create Training Request' from for Operational Lead
    if($form_state['field_url_arg_0']['und']['#value'] == 'node' &&
       ($form_state['field_url_arg_1']['und']['#value'] == 'add' ||
        $form_state['field_url_arg_2']['und']['#value'] == 'edit' )
       ){
        //function in tr-node-alter.inc
        mortar_training_record_main_form_user_connect($form, $form_state, $form_id);
    }
    
    //set title from 'Create Training Request'
    if($form_state['field_url_arg_1']['und']['#value'] == 'add') {
        drupal_set_title(t('Create Training Request'));
    }
    
    //prepare CD training record form 
    if(isset($node_type) && $node_type == 'controlled_document') {
        //function in tr-node-alter.inc
        mortar_training_record_controlled_doc_form_user_connect($form, $form_state, $form_id);
    }
    
    //prepare user training record form
    if($form_state['field_url_arg_0']['und']['#value'] == 'user') {
        //function in tr-node-alter.inc
        mortar_training_record_user_form_user_connect($form, $form_state, $form_id);
    } 

    
    #$form['#validate'][] = 'mortar_training_record_user_connect';
    #$form['actions']['submit']['#submit'][] = 'mortar_training_record_redirect_handler';
    
}


/*
 * Alters TR form displayed in user account screen
 */
function mortar_training_record_user_form_user_connect(&$form, &$form_state, $form_id = '') {
    
    //dont want the to be able to select CD as custom training
    unset($form['field_training_type']['und']['#options'][24]);
    
    //unset not relavant fields for the form
    $form['field_controlled_doc_v_number']['und']['#required'] = FALSE;
    $form['field_related_controlled_documen']['und']['#required'] = FALSE;
    $form['field_multiple_trainees']['und']['#required'] = FALSE;
    $form['field_multi_related_contr_doc']['und']['#required'] = FALSE;
    $form['field_controlled_doc_v_number']['#access'] = FALSE;
    $form['field_related_controlled_documen']['#access'] = FALSE;
    $form['revision_information']['#access'] = FALSE;
    $form['field_multiple_trainees']['#access'] = FALSE;
    $form['field_multi_related_contr_doc']['#access'] = FALSE;
    $form['field_trainee']['#access'] = FALSE;
    
    //add validation handler
    //function in tr-node-alter.inc
    $form['#validate'][] = 'mortar_training_record_user_user_connect';
    
    //add submission handler
    //function in tr-node-alter.inc
    $form['actions']['submit']['#submit'][] = 'mortar_training_record_redirect_handler';
}


/*
 * Alters TR form displayed in CD screen
 */
function mortar_training_record_controlled_doc_form_user_connect(&$form, &$form_state, $form_id = '') {
    #unset($form['field_training_type']['und']['#options'][24]);
    
    //unset not relavant fields for the form
    $form['field_training_type']['und']['#required'] = FALSE;
    $form['field_training_status']['und']['#required'] = FALSE;
    $form['field_training_details']['und']['#required'] = FALSE;
    $form['field_controlled_doc_v_number']['und']['#required'] = FALSE;
    $form['field_related_controlled_documen']['und']['#required'] = FALSE;
    $form['field_multi_related_contr_doc']['und']['#required'] = FALSE;
    $form['field_multiple_trainees']['und']['#required'] = FALSE;
    $form['field_training_type']['#access'] = FALSE;  
    $form['field_training_status']['#access'] = FALSE;
    $form['field_training_details']['#access'] = FALSE;   
    $form['field_controlled_doc_v_number']['#access'] = FALSE;
    $form['field_related_controlled_documen']['#access'] = FALSE; 
    $form['field_date_completed']['#access'] = FALSE;
    $form['field_training_document']['#access'] = FALSE;
    $form['revision_information']['#access'] = FALSE;
    $form['field_multiple_trainees']['#access'] = FALSE;
    $form['field_multi_related_contr_doc']['#access'] = FALSE;
    $form['actions']['submit']['#value'] = 'Add to my Training Record';
    $form['field_trainee']['#access'] = FALSE;
    
    #$form['#validate'][] = 'mortar_training_record_controlled_doc_user_connect';
    
    //removes default submit handler and adds custom one
    //function in tr-node-alter.inc
    $form['actions']['submit']['#submit'] = array(0 => 'mortar_training_record_controlled_doc_user_connect');
}


/*
 * Alters TR form displayed in 'Create Training Request' screen
 */
function mortar_training_record_main_form_user_connect(&$form, &$form_state, $form_id ='') {
    
    //loads user account (API)
    global $user;
    $account = user_load($user -> uid);
    
    if($form_state['field_url_arg_2']['und']['#value']  == 'edit') {
        
        $training_type = $form['field_training_type']['und']['#entity'] -> field_training_type['und'][0]['target_id'];
        unset($form['field_training_type']['und']['#options'][24]);
        
        if($training_type == 24) {
            $form['field_multi_related_contr_doc']['#access'] = FALSE;
            $form['field_multiple_trainees']['#access'] = FALSE;
            $form['field_training_details']['#access'] = FALSE;
            $form['field_training_details']['und']['#required'] = FALSE;
            $form['field_training_type']['#access'] = FALSE;
            $form['field_training_type']['und']['#required'] = FALSE;
        }
        else {
            if($training_type == 35 || $training_type == 36 || $training_type == 37) { //CV, JD, GCP array(35, 36, 37);
                
                
                $form['field_training_type']['#access'] = FALSE;
                $form['field_training_type']['und']['#required'] = FALSE;
                
                if($training_type != 37) {
                    $form['field_training_details']['#access'] = FALSE;
                    $form['field_training_details']['und']['#required'] = FALSE;
                }
            }
            
            if(!in_array('administrator', $account->roles)) {
                $form['field_trainee']['#access'] = FALSE;
            }
            $form['field_multiple_trainees']['#access'] = FALSE;
            $form['field_multiple_trainees']['und']['#required'] = FALSE;
            $form['field_multi_related_contr_doc']['#access'] = FALSE;
            $form['field_multi_related_contr_doc']['und']['#required'] = FALSE;
            $form['field_controlled_doc_v_number']['und']['#required'] = FALSE;
            $form['field_related_controlled_documen']['und']['#required'] = FALSE;
            $form['field_controlled_doc_v_number']['#access'] = FALSE;
            $form['field_related_controlled_documen']['#access'] = FALSE;
        }
        $form['actions']['submit']['#submit'][] = 'mortar_training_record_redirect_handler';
    }
    else {
        #$form['field_training_type']['und']['#options'][24] = '  Controlled Document ';
        $form['field_training_status']['und']['#required'] = FALSE;
        $form['field_controlled_doc_v_number']['und']['#required'] = FALSE;
        $form['field_related_controlled_documen']['und']['#required'] = FALSE; 
        $form['field_training_status']['#access'] = FALSE; 
        $form['field_controlled_doc_v_number']['#access'] = FALSE;
        $form['field_related_controlled_documen']['#access'] = FALSE; 
        $form['field_date_completed']['#access'] = FALSE;
        $form['field_training_document']['#access'] = FALSE;
        $form['revision_information']['#access'] = FALSE;
        $form['actions']['submit']['#value'] = 'Add Training Requests';
        $form['field_trainee']['#access'] = FALSE;
        
        $form['mortar_field_no_email'] =
        array(
                '#title' => t('Do NOT send email notification.'),
                '#description' => t('If this is checked email notifications will not be sent.'),
                '#type' => 'checkbox',
                '#options' => t('Enable'),
                '#weight' => 17, 
                                               
            );
        
        //dont display HoRAQ and QAA
        if(isset($form['field_multiple_trainees']['und']['#options'][113])) {
            unset($form['field_multiple_trainees']['und']['#options'][113]);
        }
        if(isset($form['field_multiple_trainees']['und']['#options'][114])){
            unset($form['field_multiple_trainees']['und']['#options'][114]);
        }
        
        $form['actions']['submit']['#submit'] = array(0 => 'mortar_multi_training_record_submit');
        
        
    }
    
    $form['#validate'][] = 'mortar_training_record_main_user_connect';
}


/*
 * 
 */
function mortar_training_record_user_user_connect(&$form, &$form_state, $form_id='') {
    global $user;
    $account = user_load($user -> uid);
    
    $form_state['values']['field_trainee']['und'][0]['target_id'] = $account -> uid;
    
    $trainee_id = $account -> uid;
    $training_type = $form_state['values']['field_training_type']['und'][0]['target_id'];
    $controlled_doc_id = NULL;
    $controlled_doc_v = NULL;
    $training_name = NULL;
    
    if($form_state['values']['field_training_type']['und'][0]['target_id'] == 25) {
        $training_name = $form_state['values']['field_training_details']['und'][0]['value'];
    }
    
    $has_training = mortar_has_training_record($trainee_id, $training_type, $training_name, $controlled_doc_id, $controlled_doc_v);
    
    if($has_training) {
        form_set_error('', '');
    }
}


/*
 *
 */
function mortar_training_record_controlled_doc_user_connect(&$form, &$form_state, $form_id = '') {
    
    $node_load = menu_get_object();
    
    global $user;
    $account = user_load($user -> uid);
    
    /*
    $form_state['values']['uid'] = 1;
    $form_state['values']['name'] = 'admin';
    $form_state['values']['field_trainee']['und'][0]['target_id'] = $account -> uid;
    $form_state['values']['field_training_status']['und'][0]['target_id'] = 33;
    $form_state['values']['field_training_type']['und'][0]['target_id'] = 24;
    $form_state['values']['field_related_controlled_documen']['und'][0]['target_id'] = $node_load -> nid;
    $form_state['values']['field_controlled_doc_v_number']['und'][0]['target_id'] = $node_load -> field_active_version_number['und'][0]['tid'];
    $form_state['values']['field_date_completed']['und'][0]['value']['date'] = date("Y-m-j H:i:s");
    */
    
    $trainee_id = $account -> uid;
    $training_type = 24;
    $controlled_doc_id = $node_load -> nid;
    $controlled_doc_v = $node_load -> field_active_version_number['und'][0]['tid'];
    
    $has_training = mortar_has_training_record($trainee_id, $training_type, $training_name = NULL, $controlled_doc_id, $controlled_doc_v);
    
    if($has_training == NULL) {
        
        $node = new stdClass(); // We create a new node object
        $node -> type = "training_record"; // Or any other content type you want
        $node -> language = LANGUAGE_NONE; // Or any language code if Locale module is enabled. More on this below *
        
        node_object_prepare($node); // Set some default values.
        
        $node -> uid = 1; // Or any id you wish
        $node -> field_training_type[$node->language][0]['target_id'] = 24;
        $node -> field_related_controlled_documen[$node->language][0]['target_id'] = $controlled_doc_id;
        $node -> field_training_status[$node->language][0]['target_id'] = 33;
        $node -> field_controlled_doc_v_number[$node->language][0]['target_id'] = $controlled_doc_v;
        $node -> field_trainee[$node->language][0]['target_id'] = $trainee_id;
        $node -> field_date_completed[$node->language][0]['value']['date'] = date("Y-m-j H:i:s");
        
        $node = node_submit($node);
        node_save($node);
        drupal_set_message(t('Controlled Document '.$node_load -> title.' added to your Training Record.'), 'status');
    }
    else {
        if($has_training['tr_status'] == 33) {
            return;
        }
        
        $tr_node = node_load($has_training['tr_id']);
        $tr_node -> field_training_status['und'][0]['target_id'] = 33;
        $tr_node -> field_date_completed['und'][0]['value']['date'] = date("Y-m-j H:i:s");
        node_save($tr_node);
        drupal_set_message(t('Controlled Document '. $has_training['tr_cd_title'] . ' added to your Training Record.'), 'status');
    }
}


/*
 *
 */
function mortar_training_record_main_user_connect(&$form, &$form_state, $form_id = '') {
    /*
    if(arg(2)=='edit') {
        if($form['field_training_type']['und']['#entity'] -> field_training_type['und'][0]['target_id'] == 24) {
            $form_state['values']['field_training_type']['und'][0]['target_id'] = 24;
        }
        return;
    }
    */
    
    #debug($form_state['values']['mortar_field_no_email']);
}


/*
 *
 */
function mortar_multi_training_record_submit(&$form, &$form_state, $form_id = '') {
    
    $training_record_counter = 0;
    $send_no_emails = $form_state['values']['mortar_field_no_email'];
    
    $training_type = $form_state['values']['field_training_type']['und'][0]['target_id'];
    
    foreach($form_state['values']['field_multiple_trainees']['und'] as $key_tr => $trainee) {
        
        if($form_state['values']['field_training_type']['und'][0]['target_id'] == 24) {
            foreach($form_state['values']['field_multi_related_contr_doc']['und'] as $key_cd => $controlled_doc){
                #debug($controlled_doc);
                #debug($trainee);
                
                $node_laod = node_load($controlled_doc['target_id']);
                
                $trainee_id = $trainee['target_id'];
                $training_type = $form_state['values']['field_training_type']['und'][0]['target_id'];
                $controlled_doc_id = $controlled_doc['target_id'];
                $controlled_doc_v = $node_laod -> field_active_version_number['und'][0]['tid'];
                $trainging_message = $form_state['values']['field_add_message']['und'][0]['value']; // Add a new message box - asif kamal
                
                $has_training = mortar_has_training_record($trainee_id, $training_type, $training_name = NULL, $controlled_doc_id, $controlled_doc_v);
                
                if($has_training == NULL) {
                    
                    $trainee_emails[$trainee_id] = $trainee_id;
                    
                    $node = new stdClass(); // We create a new node object
                    $node -> type = "training_record"; // Or any other content type you want
                    $node -> language = LANGUAGE_NONE; // Or any language code if Locale module is enabled.
                    
                    node_object_prepare($node); // Set some default values.
                    
                    $node -> uid = 1; // Or any id you wish
                    $node -> field_training_type[$node->language][0]['target_id'] = 24;
                    $node -> field_related_controlled_documen[$node->language][0]['target_id'] = $controlled_doc['target_id'];
                    $node -> field_training_status[$node->language][0]['target_id'] = 34;
                    $node -> field_controlled_doc_v_number[$node->language][0]['target_id'] = $node_laod -> field_active_version_number['und'][0]['tid'];
                    $node -> field_trainee[$node->language][0]['target_id'] = $trainee['target_id'];
                    $node -> field_add_message[$node->language][0]['value'] = $trainging_message; // Add a new message box - asif kamal
                    
                    $node = node_submit($node);
                    node_save($node);
                    //mail($user_email, $controlled_document_name . ' ready for review.', $message, $headers);
                    
                    $account = user_load($trainee_id);
                    
                    $controlled_doc_node = node_load($controlled_doc_id);
                    
                    $headers = 'From: OCTRU MORTAR <octrutrialshub@ndorms.ox.ac.uk>';
            
                    $message = 'Dear ' . $account -> name . ",\r\n";
                    $message .= "New Training Request has been added to your account. Please navigate to\r\n";
                    $message .= "http://mortar.octru.ox.ac.uk/user \r\n";
                    $message .= "to undertake your training and complete training requirement(s). \r\n";
                    $message .= "Thanks,\n -- \n MORTAR";
                    
                    if($send_no_emails != 1) {
                        mail($account -> mail, 'Training Record request. ' . $controlled_doc_node -> title . ' ('.$controlled_doc_node -> field_title['und'][0]['value'].')', $message, $headers);
                        watchdog("Training Record", "Email successfully sent to: <pre>". $account -> name .' - '. $account -> mail.' - '. $controlled_doc_node -> title . "</pre>");
                    }
                    $training_record_counter += 1;
                
                }
                
            }
        }
        else {
            $trainee_id = $trainee['target_id'];
            $training_type = $form_state['values']['field_training_type']['und'][0]['target_id'];
            $controlled_doc_id = NULL;
            $controlled_doc_v = NULL;
            $training_name = NULL;
            $trainging_message = $form_state['values']['field_add_message']['und'][0]['value']; // Add a new message box - asif kamal
            
            if($training_type == 25 || $training_type == 47 || $training_type == 46 ||
               $training_type == 48 || $training_type == 49 || $training_type == 56 ||
               $training_type == 94)
            { // 25, 47, 46, 48, 49, 56
                $training_name = $form_state['values']['field_training_details']['und'][0]['value'];
            }
            
            $has_training = mortar_has_training_record($trainee_id, $training_type, $training_name, $controlled_doc_id, $controlled_doc_v);
            
            if($has_training == NULL) {
                
                $trainee_emails[$trainee_id] = $trainee_id;
                
                $node = new stdClass(); // We create a new node object
                $node -> type = "training_record"; // Or any other content type you want
                $node -> language = LANGUAGE_NONE; // Or any language code if Locale module is enabled. More on this below *
                
                node_object_prepare($node); // Set some default values.
                
                $node -> uid = $trainee['target_id']; // Or any id you wish
                $node -> field_training_type[$node->language][0]['target_id'] = $form_state['values']['field_training_type']['und'][0]['target_id'];
                $node -> field_add_message[$node->language][0]['value'] = $trainging_message; // Add a new message box - asif kamal
                
                if($training_name != NULL) {
                    $node -> field_training_details[$node->language][0]['value'] = $training_name;
                }
                
                $node -> field_training_status[$node->language][0]['target_id'] = 34;
                $node -> field_trainee[$node->language][0]['target_id'] = $trainee['target_id'];
                $node = node_submit($node);
                node_save($node);
                
                
                
                $training_record_counter += 1;
            }
        }
    }
    
    if($training_record_counter != 0) {
        drupal_set_message(t($training_record_counter . ' Training Request(s) added.'), 'status');
    }
    else {
        drupal_set_message(t($training_record_counter . ' Training Requests added.'), 'warning');
    }
    
    if(!isset($trainee_emails) || $send_no_emails == 1 ) {
        drupal_set_message(t('No emails have been sent.'), 'status');
        return; 
    }
    elseif($training_type != 24) {
        #debug($trainee_emails);
        foreach($trainee_emails as $key => $trainee_email) {
            $account = user_load($key);
            
            $headers = 'From: OCTRU MORTAR <octrutrialshub@ndorms.ox.ac.uk>';
            
            $message = 'Dear ' . $account -> name . ",\r\n";
            $message .= "New Training Request(s) has/have been added to your account. Please navigate to\r\n";
            $message .= "http://mortar.octru.ox.ac.uk/user \r\n";
            $message .= "to undertake your training and complete training requirement(s). \r\n";
            $message .= "Thanks,\n -- \n MORTAR";
            
            mail($account -> mail, 'Training Record request.', $message, $headers);
            drupal_set_message(t('Email successfully sent to:<pre>'.$account -> name .'</pre>'), 'status');
            watchdog("Training Record", "Email successfully sent to: <pre>". $account -> name .' - '. $account -> mail."</pre>");
            
        }
    }
    elseif($training_type == 24) {
        drupal_set_message(t('Email(s) successfully sent.'), 'status');
    }
    
    
}


/*
 *
 */
function mortar_training_record_redirect_handler(&$form, &$form_state, $form_id = '') {
    
    global $user;
    $account = user_load($user -> uid);
    
    $node_load = menu_get_object();
    
    if($node_load == NULL && arg(0) != 'node') {
        $form_state['redirect'] = 'user/' . $account -> uid;
    }
    
    if($node_load != NULL) {
        if($node_load -> type == 'controlled_document') {
            $form_state['redirect'] = 'node/' . $node_load -> nid;
        }
    }
    
    if(arg(2) == 'edit' && $form_state['values']['field_training_type']['und'][0]['target_id'] != 24) {
        $form_state['redirect'] = 'user/' . $account -> uid;
    }

}


/*
 *
 */
function mortar_training_record_unset_options(&$form) {
    
    global $user;
    $account = user_load($user -> uid);
    
    if(!in_array('administrator', $account->roles)) {
        unset($form['field_training_type']['und']['#options'][35]);
        unset($form['field_training_type']['und']['#options'][36]);
        unset($form['field_training_type']['und']['#options'][37]); //CV, JD, GCP array(35, 36, 37);
    }
}


/*
 *
 */
function mortar_training_record_reminder_view_send_email(&$form, &$form_state, $form_id) {
    
    global $user;
    if(!in_array('administrator', $user -> roles)) {
        if(isset($form['actions']['submit']['#validate'][0])) {
            $form['actions']['submit']['#validate'][1] = $form['actions']['submit']['#validate'][0];
            $form['actions']['submit']['#validate'][0] = 'mortar_training_record_reminder_view_send_email_validate';
            $form['from']['#access'] = FALSE;
            $form['to']['#access'] = FALSE;
            $form['mail']['token']['#access'] = FALSE;
            $form['additional']['views_send_headers']['#access'] = FALSE;
            $form['views_send_direct']['#access'] = FALSE;
            $form['views_send_remember']['#access'] = FALSE;
            $form['views_send_carbon_copy']['#default_value'] = 0;
            
        }
        
        $form['mail']['views_send_subject']['#default_value'] =
                'To do: [views-send:field_training_type] [views-send:field_related_controlled_documen] [views-send:field_controlled_doc_v_number]';
    }
}


/*
 *
 */
function mortar_training_record_reminder_view_send_email_validate(&$form, &$form_state, $form_id ='') {
    
    global $user;
    global $base_url;
    
    $account = user_load($user -> uid);
    
    $form_state['values']['views_send_from_name'] = $account -> field_first_name['und'][0]['value'] . ' ' . $account -> field_last_name['und'][0]['value'];
    $form_state['values']['views_send_from_mail'] = $account -> mail;
    
    if (strpos($base_url,'dev') !== false) {   
        //DEV setup
        $form_state['values']['views_send_to_name'] = 'field_first_name_pos_2';
        $form_state['values']['views_send_to_mail'] = 'users_field_data_field_trainee_mail_pos_0';
        $form_state['values']['views_send_direct'] = 1;
    }
    else {
        //LIVE SETUP
        $form_state['values']['views_send_to_name'] = 'field_first_name_pos_2';
        $form_state['values']['views_send_to_mail'] = 'users_field_data_field_trainee_mail_pos_0';
        $form_state['values']['views_send_direct'] = 1;
    }
    
}

/*
 * For All SOP view 
 */
function mortar_training_record_all_sop_view_send_email(&$form, &$form_state, $form_id) {
    
    global $user;
    if(!in_array('administrator', $user -> roles)) {
        if(isset($form['actions']['submit']['#validate'][0])) {
            $form['actions']['submit']['#validate'][1] = $form['actions']['submit']['#validate'][0];
            $form['actions']['submit']['#validate'][0] = 'mortar_training_record_all_sop_view_send_email_validate';
            $form['from']['#access'] = FALSE;
            $form['to']['#access'] = FALSE;
            $form['mail']['token']['#access'] = FALSE;
            $form['additional']['views_send_headers']['#access'] = FALSE;
            $form['views_send_direct']['#access'] = FALSE;
            $form['views_send_remember']['#access'] = FALSE;
            $form['views_send_carbon_copy']['#default_value'] = 0;
            
        }
       
        $form['mail']['views_send_subject']['#default_value'] =
                'To do: [views-send:field_controlled_doc_attributes] [views-send:title] [views-send:field_active_version_number]';
                
        $message = "Dear  ,\r\n";
        $message .= "New Training Request has been added to your account. Please navigate to\r\n";
        $message .= "http://mortar.octru.ox.ac.uk/user \r\n";
        $message .= "to undertake your training and complete training requirement(s). \r\n";
        $message .= "Thanks,\n -- \n MORTAR";
        
        $form['mail']['views_send_message']['#default_value'] = $message;       
    }
    
    /*
     * Check the selected SOP if  not assigned to user , can assign to user's trainging records
     */
    $values =& $form_state['values'];
    $view = $form_state['build_info']['args'][0];

    if(isset($form_state['input']['op']) && ($form_state['input']['op'] == 'Next')) {
        if (!empty($values['views_send_to_mail'])) {
            $to_mail_field = $values['views_send_tokens'][$values['views_send_to_mail']];
            
            $title = $values['views_send_tokens']['node_title_pos_1'];
            $version = $values['views_send_tokens']['field_active_version_number_pos_3'];
            $uid = $values['views_send_tokens']['views_php_4_pos_4'];
            $status = $values['views_send_tokens']['views_php_6_pos_6'];
            
            foreach ($form_state['selection'] as $row_id) {
                $user_mail = $view->style_plugin->get_field_value($row_id, $to_mail_field);
                $sop_title = $view->style_plugin->get_field_value($row_id, $title);
                $sop_version = $view->style_plugin->get_field_value($row_id, $version);
                $user_uid= $view->style_plugin->get_field_value($row_id, $uid);
                $tr_status = $view->style_plugin->get_field_value($row_id, $status);
                $user_account = user_load($user_uid);
                              
                $cd_nid= db_query('SELECT nid FROM {node} WHERE title = :mytitle', array(':mytitle' => $sop_title))->fetchField();
                $node_laod = node_load($cd_nid);

                if($tr_status === "Not-assigned") {                      
                        $node = new stdClass(); // We create a new node object
                        $node -> type = "training_record"; // Or any other content type you want
                        $node -> language = LANGUAGE_NONE; // Or any language code if Locale module is enabled.
                        
                        node_object_prepare($node); // Set some default values.
                        
                        $node -> uid = 1; // Or any id you wish
                        $node -> field_training_type[$node->language][0]['target_id'] = 24;
                        $node -> field_related_controlled_documen[$node->language][0]['target_id'] = $cd_nid;
                        $node -> field_training_status[$node->language][0]['target_id'] = 34;
                        $node -> field_controlled_doc_v_number[$node->language][0]['target_id'] = $node_laod -> field_active_version_number['und'][0]['tid'];
                        $node -> field_trainee[$node->language][0]['target_id'] = $user_uid;
                        
                        $node = node_submit($node);
                        node_save($node);
                        drupal_set_message(t('Controlled Document, ' . $node_laod -> title . ' - '. $node_laod -> field_title['und'][0]['value'] . ' added to '. $user_account -> name.' Training Record'), 'status');
                }
            }
        }
    }
}

/*
 * For All SOP view 
 */
function mortar_training_record_all_sop_view_send_email_validate(&$form, &$form_state, $form_id ='') {
    
    global $user;
    global $base_url;
    
    $account = user_load($user -> uid);
    
    $form_state['values']['views_send_from_name'] = $account -> field_first_name['und'][0]['value'] . ' ' . $account -> field_last_name['und'][0]['value'];
    $form_state['values']['views_send_from_mail'] = $account -> mail;
    
    if (strpos($base_url,'dev') !== false) {   
        //DEV setup
        $form_state['values']['views_send_to_name'] = 'field_first_name_pos_2';
        $form_state['values']['views_send_to_mail'] = 'views_php_5_pos_5';
        $form_state['values']['views_send_direct'] = 1;
    }
    else {
        //LIVE SETUP
        $form_state['values']['views_send_to_name'] = 'field_first_name_pos_2';
        $form_state['values']['views_send_to_mail'] = 'views_php_5_pos_5';
        $form_state['values']['views_send_direct'] = 1;
    }
    
}