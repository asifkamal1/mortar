<?php

/*
 * changes CD node
 */
function mortar_controlled_document_node_view_alter(&$build, $account, $node) {
    
    //if CD active
    if( ($node -> field_status['und'][0]['target_id'] == 41 ||
        $node -> field_status['und'][0]['target_id'] == 3 ||
        $node -> field_status['und'][0]['target_id'] == 4 ) &&
        $node -> field_type['und'][0]['target_id'] == 19
       ) {
        
        //this will make sure that 'Add to Traning Record' form is displayed if node is CD and test has not been set
        if($build['#node'] -> type == 'controlled_document' && !isset($node -> field_cd_test['und'][0]['target_id'])) {
            
            //need to make sure that this form is added to SOP only and not fors and tempaltes
            if($build['#node'] -> field_type['und'][0]['target_id'] == 19) {
                //function in cd-node-alter.inc
                mortar_training_record_form_display($build, $account, $node);
            }
        }
    }
    
    //if CD not active and is mortar admin
    if(mortar_is_mortar_admin($account) && $node -> field_status['und'][0]['target_id'] != 3) {
        
        //function in cd-node-alter.inc
        mortar_controlled_document_review_form_display($build, $account, $node);
        return;
    }
    
    //if author of the CD and CD not active and not awaiting approval
    if($account -> uid == $node -> uid &&
       $node -> field_status['und'][0]['target_id'] != 13 &&
       $node -> field_status['und'][0]['target_id'] != 3 &&
       $node -> field_status['und'][0]['target_id'] != 41
       ) {
        
        //function in cd-node-alter.inc
        mortar_controlled_document_review_form_display($build, $account, $node);
        return;
    }
    
    //if CD not active and not awaiting approval
    if ($node -> field_status['und'][0]['target_id'] != 3 &&
        $node -> field_status['und'][0]['target_id'] != 13 &&
        $node -> field_status['und'][0]['target_id'] != 41
        ) {
        
        //if reviewer of that CD
        //function in miscellaneous.inc
        if(mortar_is_reviewer($node, $account)) {
            
            //function in cd-node-alter.inc
            mortar_controlled_document_review_form_display($build, $account, $node);
            return;
        }
    }
    
    //if compliance reviwer of that CD
    ////function in miscellaneous.inc
    if(mortar_is_compliance_reviewer($node,$account) &&
       $node -> field_status['und'][0]['target_id'] == 13 &&
       $node -> field_status['und'][0]['target_id'] == 41
       ) {
        
        //function in cd-node-alter.inc
        mortar_controlled_document_review_form_display($build, $account, $node);
        return;
    }

}


/*
 * Assigns 'Add to my Training Record' form to CD $build
 */
function mortar_training_record_form_display(&$build, $account, $node) {
    
    //drupal API - need that to use node_add
    module_load_include('inc', 'node', 'node.pages');
    
    //load training record form
    $form = node_add('training_record'); 
    
    //node_add (drupal API) sets title. Need to reset it.
    drupal_set_title($node -> title);
    
    //make it pretty - build fieldset.
    $build['mortar_fieldset_training_record'] = array(
        '#type' => 'fieldset',
        '#title' => 'Training Record',
        '#weight' => 99,
        '#description' => t('I have read and fully understood the contents of the '. $node -> title . ' Controlled Document.'),
        
    );
    
    //add libraries to CD build
    $build['mortar_fieldset_training_record']['#attached']['library'][] = array('system', 'drupal.collapse');
    $build['mortar_fieldset_training_record']['#attributes']['class'][] = 'collapsible';
    
    //assign form to CD build
    $build['mortar_fieldset_training_record']['mortar_training_record_form'] = $form;
}


/*
 * Assigns 'CD Review' form to CD node
 */
function mortar_controlled_document_review_form_display(&$build, $account, $node) {
    
    module_load_include('inc', 'node', 'node.pages');
    $form = node_add('controlled_document_review'); 
    
    //DEPRECIATED
    #mortar_alter_controlled_doc_review_form($form, $node);
    
    drupal_set_title($node -> title);
    
    $build['mortar_fieldset_controlled_document_rev_history'] = array(
        '#type' => 'fieldset',
        '#title' => 'Review',
        '#weight' => 98,
        
    );
    
    $build['mortar_fieldset_controlled_document_rev_history']['#attached']['library'][] = array('system', 'drupal.collapse');
    $build['mortar_fieldset_controlled_document_rev_history']['#attributes']['class'][] = 'collapsible'; 
    $build['mortar_fieldset_controlled_document_rev_history']['mortar_controlled_document_review_form'] = $form;
}
 
 
/*
 * Alters CD Review Form
 */
function mortar_form_controlled_document_review_node_form_alter(&$form, &$form_state, $form_id) {
    
    // ensuring the ajax upload still has all includes
    $files = (isset($form_state['build_info']['files'])) ? $form_state['build_info']['files'] : array();
    $files[] = drupal_get_path('module', 'node') . '/node.pages.inc';
    $form_state['build_info']['files'] = $files;
    
    
    //to do - allow admins to add reviews straight from add review screen without a need of controlled doc view??
    //ajax file upload breaks form_alter
    #if($node_load -> type != 'controlled_document_review') {

        $form['field_controlled_document_id']['#access'] = FALSE;
        $form['field_review_version']['#access'] = FALSE;    
    #}
    
    
    //add new submistion handler
    $form['actions']['submit']['#submit'][] = 'mortar_controlled_doc_review_redirect_handler';
    
    //add new validation handler
    //function in cd-node-alter.inc
    $form['#validate'][] = 'mortar_controlled_doc_review_node_connect';
    
    //change name of submit button
    $form['actions']['submit']['#value'] = 'Submit Review';

    
}


/*
 * Connects CD Review to the CD
 */
function mortar_controlled_doc_review_node_connect($form, &$form_state) {
    
    //Since review form can be added only from CD page, we use API to load current node.
    if($node_load = menu_get_object()) {
        $node = node_load($node_load -> nid);
    }
    //if trying to add review directly from admin/node/add
    else {
        //set error (API)
        form_set_error('', 'Sorry - cannot add review from this screen');
        return;
    }
    
    //assign nid if the CD to CD Review
    if($node -> type == 'controlled_document') {    
        $form_state['values']['field_controlled_document_id']['und'][0]['target_id'] = $node -> nid;
    }
    //When edit CD Review reassign nid
    else {
        $form_state['values']['field_controlled_document_id']['und'][0]['target_id'] = $node -> field_controlled_document_id['und'][0]['target_id'];
    }
    
    //set review version if applicable
    if(isset($node -> field_review_version_number['und'][0]['tid'])){
        $form_state['values']['field_review_version']['und'][0]['target_id'] = $node -> field_review_version_number['und'][0]['tid'];
    }
    
}


/*
 * Redirects back to CD page
 */
function mortar_controlled_doc_review_redirect_handler($form, &$form_state) {
    
    //get nid from form
    $node_load = $form_state['values']['field_controlled_document_id']['und'][0]['target_id'];
    
    //load node (API)
    $node = node_load($node_load);
    
    //get autopath address (API)
    $node_address = drupal_get_path_alias('node/' . $node -> nid);
    
    //set form redirect address (API)
    $form_state['redirect'] = $node_address;
}

/*
 *Adds checkboxes for notifying reviewers and administrators. This is done that way so value is not saved to the db.
 */
function mortar_form_controlled_document_node_form_alter(&$form, &$form_state, $form_id) {

    if($form['#node'] -> title == NULL) {
        $controlled_document_status = 'new';
        $controlled_document_status_value = NULL;
    }
    else {
        $controlled_document_status = 'update';
        if(isset($form['#node'] -> field_status['und'][0]['target_id'])) {
            $controlled_document_status_value = $form['#node'] -> field_status['und'][0]['target_id'];
        }
        else {
            $controlled_document_status_value = NULL;
        }
    }
    
    global $user;
    //loads user details
    
    $account = $user;
    
    //if not MA hide author
    if(!mortar_is_mortar_admin($user)) {
        $form['field_cd_author']['#access'] = FALSE;
    }
    
    //this is not used for now
    $form['field_compliance_reviewers']['#access'] = FALSE;
    #debug();
    
    //if new CD set author to logged in user
    if($form['nid']['#value'] == NULL) {
        $form['field_cd_author']['und']['#default_value'][0] = $account -> uid;
    }
    else {
        $form['field_cd_author']['und']['#default_value'][0] = $form['uid']['#value'];
    }
    
    
    if(isset($form['#node'] -> uid)) {
        unset($form['field_reviewers']['und']['#options'][$form['#node'] -> uid]);
    }
    else {
        unset($form['field_reviewers']['und']['#options'][$user -> uid]);
    }
    
    $form['mortar_field_message'] = array(
        '#title' => t('Message'),
        '#description' => t('Message to send via email.'),
        '#type' => 'textarea',
        '#weight' => 14,
    );
    
    $form['#validate'][] = 'mortar_validate_controlled_document';
    
    $form['#validate'][] = 'mortar_set_controlled_document_author';
    
    /*
     * when the SOP in 'active - awaiting approval' and the it is not ready to go Active the this will send an email
     * to author to further review the SOP.
    */
    if($controlled_document_status_value == 41) {
        $form['mortar_fieldset_notify_author_review'] = array(
            '#title' => t('Send Controlled Document approval response to author for further review'),
            '#description' => t('If this is checked and Controlled Document is saved,
                                author will be notified by email that the Controlled Document need further review.'),
            '#type' => 'checkbox',
            '#options' => t('Enable'),
            '#weight' => 12,
            
        );
        
        
        $form['actions']['submit']['#submit'][] = 'mortar_send_author_further_review_notification';
    
    } else if($controlled_document_status_value != 13) {
        
        $form['mortar_field_notify_reviewer'] = array(
            '#title' => t('Send notification to reviewers'),
            '#description' => t('If this is checked and Controlled Document is saved,
                                reviewers will be notified by email that changes have
                                been made to this Controlled Document and they are required to review it.'),
            '#type' => 'checkbox',
            '#options' => t('Enable'),
            '#weight' => 12,
        );
        
        $form['mortar_fieldset_notify_moderator_authorised'] = array(
            '#type' => 'fieldset',
            '#title' => 'SOP to be authorised',
            '#weight' => 13,
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
            '#tree' => FALSE,
        );
        
        $form['mortar_fieldset_notify_moderator_authorised']['mortar_field_notify_administrator_authorised'] = array(
            '#title' => t('Send Controlled Document to be authorised'),
            '#description' => t('If this is checked and Controlled Document is saved,
                                authoriser will be notified by email that the Controlled Document
                                is ready to be authorised. Once saved you will not be able to make any
                                changes until Controlled Document is reviewed by authoriser.'),
            '#type' => 'checkbox',
            '#options' => t('Enable'),
            '#weight' => 13,
        );
        
        
        $form['mortar_fieldset_notify_moderator'] = array(
            '#type' => 'fieldset',
            '#title' => 'SOP issue',
            '#weight' => 13,
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
            '#tree' => FALSE,
        );
        
        $form['mortar_fieldset_notify_moderator']['mortar_field_notify_administrator'] = array(
            '#title' => t('Send Controlled Document for issue'),
            '#description' => t('If this is checked and Controlled Document is saved,
                                administrator will be notified by email that the Controlled Document
                                is ready to be issued. Once saved you will not be able to make any
                                changes until Controlled Document is reviewed by administrator.'),
            '#type' => 'checkbox',
            '#options' => t('Enable'),
            '#weight' => 13,
        );
    
        
        $form['actions']['submit']['#submit'][] = 'mortar_send_reviewer_notification';
        $form['actions']['submit']['#submit'][] = 'mortar_send_controlled_document_for_authorised';
        $form['actions']['submit']['#submit'][] = 'mortar_send_controlled_document_for_approval';
    }
    
    //Show checbox if the SOP is in "Active Awaiting - Dreft" or "Active Awaiting - Approval" status
    if($controlled_document_status_value == 41 || $controlled_document_status_value == 13) {
            
        $form['mortar_fieldset_notify_author_authorised_fail'] = array(
            '#type' => 'fieldset',
            '#title' => 'SOP Authorisation',
            '#weight' => 12,
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
            '#tree' => FALSE,
        );
           
        $form['mortar_fieldset_notify_author_authorised_fail']['mortar_field_notify_author_authorised_fail'] = array(
            '#title' => t('Send Controlled Document to author as the Authorisation Failed'),
            '#description' => t('If this is checked and Controlled Document is saved,
                                author will be notified by email that the Controlled Document
                                failed the authorisation process and it is sent back to Document Under Review section for further review.'),
            '#type' => 'checkbox',
            '#options' => t('Enable'),
            '#weight' => 12,
        );
        
        $form['mortar_fieldset_notify_author_authorised_fail']['mortar_field_notify_author_authorised_pass'] = array(
            '#title' => t('Send Controlled Document to author as the Authorisation Pass'),
            '#description' => t('If this is checked and Controlled Document is saved,
                                author will be notified by email that the Controlled Document
                                pass the authorisation process and it is sent back to Document Under Review section ready to be issue.'),
            '#type' => 'checkbox',
            '#options' => t('Enable'),
            '#weight' => 12,
        );
        
        
        $form['actions']['submit']['#submit'][] = 'mortar_send_controlled_document_authorisation_outcome_fail';
        $form['actions']['submit']['#submit'][] = 'mortar_send_controlled_document_authorisation_outcome_pass';
    
    } 
    
    if($controlled_document_status == 'new' || $controlled_document_status_value != 13) {
        return;
    }
    
    $form['mortar_fieldset_notify_author'] = array(
        '#type' => 'fieldset',
        '#title' => 'Controlled Document approval response',
        '#weight' => 16,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#tree' => FALSE,
    );

    $form['mortar_fieldset_notify_author']['mortar_field_notify_author'] = array(
        '#title' => t('Send Controlled Document approval response'),
        '#description' => t('If this is checked and Controlled Document is saved,
                            author will be notified by email that the Controlled Document approval outcome is now available.'),
        '#type' => 'checkbox',
        '#options' => t('Enable'),
        '#weight' => 16,
    );
    
    $form['mortar_fieldset_notify_compliance_reviewer'] = array(
        '#type' => 'fieldset',
        '#title' => 'Controlled Document compliance review',
        '#weight' => 17,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#tree' => FALSE,
    );

    $form['mortar_fieldset_notify_compliance_reviewer']['mortar_field_notify_compliance_reviewer'] = array(
        '#title' => t('Send Controlled Document for compliance review'),
        '#description' => t('If this is checked and Controlled Document is saved,
                            compliance reviewer will be notified by email that the Controlled Document is now available for review.'),
        '#type' => 'checkbox',
        '#options' => t('Enable'),
        '#weight' => 17,
    );
    
    
    if(mortar_is_mortar_admin($account)) {
        $form['mortar_fieldset_notify_author']['#access'] = TRUE;
        $form['mortar_fieldset_notify_compliance_reviewer']['#access'] = TRUE;
        $form['actions']['submit']['#submit'][] = 'mortar_send_controlled_document_approval_outcome';
        $form['actions']['submit']['#submit'][] = 'mortar_send_controlled_document_for_compliance_review';
    }
    else{
        $form['mortar_fieldset_notify_author']['#access'] = FALSE;
        $form['mortar_fieldset_notify_compliance_reviewer']['#access'] = FALSE;
    }
}


/*
 *
 */
function mortar_set_controlled_document_author(&$form, &$form_state) {
    
    $account = user_load($form_state['values']['field_cd_author']['und'][0]['target_id']);
    
    $form_state['values']['name'] = $account -> name;
}


/*
 * CD validation
 */
function mortar_validate_controlled_document(&$form, &$form_state, $form_id = 'controlled_document_node_form') {
    
    if(isset($form_state['values']['mortar_field_notify_reviewer'])) {
        if($form_state['values']['mortar_field_message']!='' && $form_state['values']['mortar_field_notify_reviewer'] == 0 && $form_state['values']['mortar_field_notify_administrator'] == 0 && $form_state['values']['mortar_field_notify_administrator_authorised'] == 0) {
            form_set_error('mortar_field_notify_reviewer', t('If you wish to send an email please choose at least one receiver by selecting
                                                             "Send notification to reviewers" or "Send Controlled Document for approval" or "Send Controlled Document to be authorised" checkbox.')
                           );
        }
        
        if($form_state['values']['mortar_field_notify_administrator'] == 1 && $form_state['values']['mortar_field_notify_reviewer'] == 1) {
            form_set_error('mortar_field_notify_reviewer',t('You are trying to send this SOP for review and issue at the same time -
                                 please make sure that you select only one of these options.')
                           );
        }
        
        if($form_state['values']['mortar_field_notify_reviewer'] == 1 && $form_state['values']['mortar_field_notify_administrator_authorised'] == 1) {
            form_set_error('mortar_field_notify_reviewer',t('You are trying to send this SOP for review and to be authorised at the same time -
                                 please make sure that you select only one of these options.')
                           );
        }
        
        if($form_state['values']['mortar_field_notify_administrator'] == 1 && $form_state['values']['mortar_field_notify_administrator_authorised'] == 1) {
            form_set_error('mortar_field_notify_reviewer',t('You are trying to send this SOP for issue and to be authorised at the same time -
                                 please make sure that you select only one of these options.')
                           );
        }
        
        if($form_state['values']['field_reviewers']['und'][0]['target_id'] == NULL && $form_state['values']['mortar_field_notify_reviewer'] == 1) {
            form_set_error('field_reviewers',t('You are trying to send this SOP for review but have not selected the Reviewer. 
                                Please select at least one Reviewer.')
                            );
        }        
    }
    
    if(isset($form_state['values']['mortar_field_notify_author'])) {
        if($form_state['values']['mortar_field_message']!='' && $form_state['values']['mortar_field_notify_author'] == 0
           && $form_state['values']['mortar_field_notify_compliance_reviewer'] == 0 && $form_state['values']['mortar_field_notify_author_authorised_fail'] == 0
           && $form_state['values']['mortar_field_notify_author_authorised_pass'] == 0) {
            form_set_error('mortar_field_notify_reviewer', t('If you wish to send an email please choose at least one receiver by selecting
                                                             "Send Controlled Document approval response" or "Send Controlled Document for compliance review"
                                                             or "Send Controlled Document to author as the Authorisation Failed"
                                                             or "Send Controlled Document to author as the Authorisation Pass" checkbox.')
                           );
        }
        
        if($form_state['values']['mortar_field_notify_author'] == 1 && $form_state['values']['mortar_field_notify_compliance_reviewer'] == 1) {
            form_set_error('mortar_field_notify_author', t('You are trying to send this SOP for compliance review and send approval outcome at the same time -
                             please make sure that you select only one of these options.')
                           );
        }
        
        if($form_state['values']['mortar_field_notify_author'] == 1 && $form_state['values']['mortar_field_notify_author_authorised_fail'] == 1) {
            form_set_error('mortar_field_notify_author', t('You are trying to send this SOP for approval and send the authorisation failed outcome at the same time -
                             please make sure that you select only one of these options.')
                           );
        }
        
        
        if($form_state['values']['mortar_field_notify_author_authorised_fail'] == 1 && $form_state['values']['mortar_field_notify_compliance_reviewer'] == 1) {
            form_set_error('mortar_field_notify_author', t('You are trying to send this SOP for compliance review and send the authorisation failed outcome at the same time -
                             please make sure that you select only one of these options.')
                           );
        }
        
        if($form_state['values']['mortar_field_notify_author'] == 1 && $form_state['values']['mortar_field_notify_author_authorised_pass'] == 1) {
            form_set_error('mortar_field_notify_author', t('You are trying to send this SOP for approval and send the authorisation pass outcome at the same time -
                             please make sure that you select only one of these options.')
                           );
        }
        
        
        if($form_state['values']['mortar_field_notify_author_authorised_pass'] == 1 && $form_state['values']['mortar_field_notify_compliance_reviewer'] == 1) {
            form_set_error('mortar_field_notify_author', t('You are trying to send this SOP for compliance review and send the authorisation pass outcome at the same time -
                             please make sure that you select only one of these options.')
                           );
        }
        
        if($form_state['values']['mortar_field_notify_author_authorised_pass'] == 1 && $form_state['values']['mortar_field_notify_author_authorised_fail'] == 1) {
            form_set_error('mortar_field_notify_author', t('You are trying to send this SOP authorisation pass outcome  and send the authorisation failed outcome at the same time -
                             please make sure that you select only one of these options.')
                           );
        }
    }
    
    if(isset($form_state['values']['mortar_field_notify_compliance_reviewer'])) {
        if($form_state['values']['field_compliance_reviewers']['und'][0]['target_id'] == NULL && $form_state['values']['mortar_field_notify_compliance_reviewer'] == 1) {
            form_set_error('field_compliance_reviewers', t('You are trying to send this SOP for compliance review but have not selected Compliance Reviewer. 
                                Please select at least one Complience Reviewer.')
                           );
        }
    }
    
    if($form_state['values']['field_status']['und'][0]['target_id'] == 41 ||
       $form_state['values']['field_status']['und'][0]['target_id'] == 4 ||
       $form_state['values']['field_status']['und'][0]['target_id'] == 3
       ) {
        
        if(!isset($form_state['values']['field_active_version_number']['und'][0]['tid'])) {
            form_set_error('field_active_version_number', t('Active version number is required.')
                            );
        }
        
    }
    
}