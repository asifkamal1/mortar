<?php

/*
 *This function runs after 'function mortar_send_reviewer_notification'.
 *If (SOP sned for issue by author)
 *
 * SOP Status are follwoing;
 * Draft = 6
 * Awaiting approvl = 13
 * Active Under revision = 4
 * Active Awaiting Approval = 41
 * Active = 3
 * 
 */
function mortar_send_controlled_document_for_approval(&$form, &$form_state, $form_id = 'controlled_document_node_form') {
    
    $error = FALSE;
    
    if($form_state['values']['mortar_field_notify_reviewer'] == 1) {
        $error = TRUE;
    }
    
    if($form_state['values']['mortar_field_notify_administrator_authorised'] == 1 ) {
        $error = TRUE;
    }
    
    if($form_state['values']['mortar_field_notify_administrator'] == 0) {
        $error = TRUE;
    }
    
    
    if($error) {
        return;
    }
    
    $controlled_document_node = node_load($form_state['nid']);
    
    if($form_state['values']['field_status']['und'][0]['target_id'] == 6) {
        $controlled_document_node -> field_status['und'][0]['target_id'] = 13;
    }
    else {
        $controlled_document_node -> field_status['und'][0]['target_id'] = 41;
    }
    
    node_save($controlled_document_node);
    
    global $user;
    
    $logged_user = $user -> name;
    $controlled_document_node = node_load($form_state['nid']);
    
    $controlled_document_address = drupal_get_path_alias('node/' . $controlled_document_node -> nid);
    $controlled_document_id = $controlled_document_node -> title;
    $controlled_document_title = $controlled_document_node -> field_title['und'][0]['value'];
    $controlled_document_author_id = $controlled_document_node -> uid;
    
    $administrator = user_load(113); //Joanna Black - 4 |113 - HoRAQ //NEEDS CHANGING
    $administrators_email = $administrator -> mail;
    
    $administrator = user_load(114); //QAA

    $administrator2 = user_load(1760); //Duncan Richards - 1760 (on test site)  //NEEDS CHANGING

    $administrators_email = $administrators_email . ',' . $administrator -> mail. ',' . $administrator2 -> mail;
    
    $headers = 'From: OCTRU MORTAR <octrutrialshub@ndorms.ox.ac.uk>';
    $message = '';
    
    if($form_state['values']['mortar_field_message']!='') {
        $author_message = 'Message sent by '. $logged_user . ": \r\n" . $form_state['values']['mortar_field_message'] . "\r\n";
    }
    

    $message = "Dear MORTAR Administrators ,\r\n";
    $message .= 'SOP ' . $controlled_document_title . "(". $controlled_document_id .") has been submitted for authorisation by ". $logged_user .". \r\n";
    $message .= "Please navigate to \r\n";
    $message .= 'http://mortar.octru.ox.ac.uk/'. $controlled_document_address ."\r\n";
    $message .= "to review the SOP. \r\n";
    
    if(isset($author_message)) {
        $message .= $author_message;
    }
    
    $message .= "Thanks,\n -- \n MORTAR";
    
    mail($administrators_email, $controlled_document_id . ' submitted for authorisation.', $message, $headers);
    drupal_set_message(t('Email successfully sent to the MORTAR administrators.'), 'status');
    
    if(isset($author_message)) {
        watchdog("Review email", "Email successfully sent to the MORTAR administrators. Controlled Document: <pre>" . $controlled_document_id . "</pre>");
    }
    else {
        watchdog("Review email", "Email successfully sent to the MORTAR administrators. Message: <pre>" . $author_message . "</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>");
    }
}

/*
 *This function runs after 'function mortar_send_reviewer_notification'. (Author send SOP to be aproved)
 *If 
 */
function mortar_send_controlled_document_for_authorised(&$form, &$form_state, $form_id = 'controlled_document_node_form') {
    
    $error = FALSE;
    
    if($form_state['values']['mortar_field_notify_reviewer'] == 1) {
        $error = TRUE;
    }
    
    
    if($form_state['values']['mortar_field_notify_administrator'] == 1 ) {
        $error = TRUE;
    }
    

    if($form_state['values']['mortar_field_notify_administrator_authorised'] == 0) {
        $error = TRUE;
    }
    
    
    if($error) {
        return;
    }
    
    $controlled_document_node = node_load($form_state['nid']);
    
    if($form_state['values']['field_status']['und'][0]['target_id'] == 6) {
        $controlled_document_node -> field_status['und'][0]['target_id'] = 13;
    }
    else {
        $controlled_document_node -> field_status['und'][0]['target_id'] = 41;
    }
    
    node_save($controlled_document_node);
    
    global $user;
    
    $logged_user = $user -> name;
    $controlled_document_node = node_load($form_state['nid']);
    
    $controlled_document_address = drupal_get_path_alias('node/' . $controlled_document_node -> nid);
    $controlled_document_id = $controlled_document_node -> title;
    $controlled_document_title = $controlled_document_node -> field_title['und'][0]['value'];
    $controlled_document_author_id = $controlled_document_node -> uid;
    
    $administrator = user_load(113); //Joanna Black - 4 |113 - HoRAQ //NEEDS CHANGING
    $administrators_email = $administrator -> mail;
    
    global $base_url;
    
    if (strpos($base_url,'test') !== false) {
         $administrator = user_load(907); // Vicki Barber - 10 | OCTRU Hub Manager - test site(907)
    }
    else {
         $administrator = user_load(947); // Vicki Barber - 10 | OCTRU Hub Manager - live site(947)
    }
   
    $administrators_email = $administrators_email . ',' . $administrator -> mail;
    
    $headers = 'From: OCTRU MORTAR <octrutrialshub@ndorms.ox.ac.uk>';
    $message = '';
    
    if($form_state['values']['mortar_field_message']!='') {
        $author_message = 'Message sent by '. $logged_user . ": \r\n" . $form_state['values']['mortar_field_message'] . "\r\n";
    }
    

    $message = "Dear SOP Authoriser ,\r\n";
    $message .= 'SOP ' . $controlled_document_title . "(". $controlled_document_id .") has been submitted for authorisation by ". $logged_user .". \r\n";
    $message .= "Please navigate to \r\n";
    $message .= 'http://mortar.octru.ox.ac.uk/'. $controlled_document_address ."\r\n";
    $message .= "to review the SOP. \r\n";
    
    if(isset($author_message)) {
        $message .= $author_message;
    }
    
    $message .= "Thanks,\n -- \n MORTAR";
    
    mail($administrators_email, $controlled_document_id . ' submitted for authorisation.', $message, $headers);
    drupal_set_message(t('Email successfully sent to the SOP Authoriser.'), 'status');
    
    if(isset($author_message)) {
        watchdog("Review email", "Email successfully sent to the SOP Authoriser . Controlled Document: <pre>" . $controlled_document_id . "</pre>");
    }
    else {
        watchdog("Review email", "Email successfully sent to the SOP Authoriser . Message: <pre>" . $author_message . "</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>");
    }
}

/*
 *Send email to author for further review of SOP after failing the authorisation (SOP send by authoriser - site adminstrator or HORAQ as user with site admin role)
*/
function mortar_send_controlled_document_authorisation_outcome_fail(&$form, &$form_state, $form_id = 'controlled_document_node_form') {
    
    $error = FALSE;
    
    if($form_state['values']['mortar_field_notify_author_authorised_fail'] == 0) {
        drupal_set_message(t('Email notification has NOT been sent. If you wish to send an email please choose receiver by selecting
                                "Send Controlled Document to author as the Authorisation Failed".'), 'warning');
        $error = TRUE;
    }
    
    #debug($form_state['values']);
    
     if($error) {
        return;
    }
    

    $controlled_document_node = node_load(menu_get_object()->nid);

    if($controlled_document_node->field_status['und'][0]['target_id'] == 13) {
        $controlled_document_node -> field_status['und'][0]['target_id'] = 6;
    }
    else {
        $controlled_document_node -> field_status['und'][0]['target_id'] = 4;
    }
    
    node_save(node_load(menu_get_object()->nid));

    global $user;
    
    $logged_user = $user -> name;
    $controlled_document_node = node_load(menu_get_object()->nid);
    
    $controlled_document_address = drupal_get_path_alias('node/' . $controlled_document_node -> nid);
    $controlled_document_id = $controlled_document_node -> title;
    $controlled_document_title = $controlled_document_node -> field_title['und'][0]['value'];
    $controlled_document_author_id = $controlled_document_node -> uid;
    
    $author = user_load($controlled_document_author_id);
    
    $headers = 'From: OCTRU MORTAR <octrutrialshub@ndorms.ox.ac.uk>';
    $message = '';
    
    if($form_state['values']['mortar_field_message']!='') {
        $author_message = 'Message sent by '. $logged_user . ": \r\n" . $form_state['values']['mortar_field_message'] . "\r\n";
    }
    

    $message = 'Dear ' . $author -> name . ",\r\n";
    $message .= 'SOP ' . $controlled_document_title . " (". $controlled_document_id .") has now been reviewed by the Authoriser. This has failed the Authorisation process and requires further work. \r\n";
    $message .= 'http://mortar.octru.ox.ac.uk/'. $controlled_document_address ."\r\n";
    
    if(isset($author_message)) {
        $message .= $author_message;
    }
    
    $message .= "Thanks,\n -- \n MORTAR";
    
    mail($author -> mail, $controlled_document_id . ' Authoriser review outcome available.', $message, $headers);
    drupal_set_message(t('Email successfully sent to the author:<pre>'.$author -> name.'</pre>'), 'status');
    
    if(isset($author_message)) {
        watchdog("Review email", "Email successfully sent to the Authoriser: <pre>". $author -> name .' - '. $author -> mail."</pre> Message: <pre>" . $author_message . "</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>"); 
    }
    
    #debug($controlled_document_node)
}


/*
 *Send email to author for further review of SOP after successfully pass the authorisation  (SOP send by authoriser - site adminstrator or HORAQ as user with site admin role)
*/
function mortar_send_controlled_document_authorisation_outcome_pass(&$form, &$form_state, $form_id = 'controlled_document_node_form') {
    
    $error = FALSE;
    
    if($form_state['values']['mortar_field_notify_author_authorised_pass'] == 0) {
        drupal_set_message(t('Email notification has NOT been sent. If you wish to send an email please choose receiver by selecting
                             "Send Notification to author: Authorisation Pass. Send Controlled Document for Issue.'), 'warning');
        $error = TRUE;
    }
    
    #debug($form_state['values']);
    
     if($error) {
        return;
    }
    
    /*
     * Email to administrator that SOP is ready to be issue
    */
    $controlled_document_node = node_load(menu_get_object()->nid);
    
    if($form_state['values']['field_status']['und'][0]['target_id'] == 13 || $form_state['values']['field_status']['und'][0]['target_id'] == 6) {
        $controlled_document_node -> field_status['und'][0]['target_id'] = 13;
    }
    else {
        $controlled_document_node -> field_status['und'][0]['target_id'] = 41;
    }
    
    node_save($controlled_document_node);
    
    global $user;
    
    $logged_user = $user -> name;
    $controlled_document_node = node_load(menu_get_object()->nid);
    
    $controlled_document_address = drupal_get_path_alias('node/' . $controlled_document_node -> nid);
    $controlled_document_id = $controlled_document_node -> title;
    $controlled_document_title = $controlled_document_node -> field_title['und'][0]['value'];
    $controlled_document_author_id = $controlled_document_node -> uid;
   
    $administrator = user_load(113); //Joanna Black - 4 |113 - HoRAQ //NEEDS CHANGING
    $administrators_email = $administrator -> mail;
    
    $administrator = user_load(114); //QAA
    $administrators_email = $administrators_email . ',' . $administrator -> mail;
    
    $headers = 'From: OCTRU MORTAR <octrutrialshub@ndorms.ox.ac.uk>';
    $message = '';
    
    if($form_state['values']['mortar_field_message']!='') {
        $author_message = 'Message sent by '. $logged_user . ": \r\n" . $form_state['values']['mortar_field_message'] . "\r\n";
    }
    
    $message = "Dear MORTAR Administrators ,\r\n";
    $message .= 'SOP ' . $controlled_document_title . "(". $controlled_document_id .") has been submitted for authorisation by ". $logged_user .". \r\n";
    $message .= "Please navigate to \r\n";
    $message .= 'http://mortar.octru.ox.ac.uk/'. $controlled_document_address ."\r\n";
    $message .= "to review the SOP. \r\n";
    
    if(isset($author_message)) {
        $message .= $author_message;
    }
    
    $message .= "Thanks,\n -- \n MORTAR";
 
    mail($administrators_email, $controlled_document_id . ' submitted for authorisation.', $message, $headers);
    drupal_set_message(t('Email successfully sent to the MORTAR administrators.'), 'status');
    
    if(isset($author_message)) {
        watchdog("Review email", "Email successfully sent to the MORTAR administrators. Controlled Document: <pre>" . $controlled_document_id . "</pre>");
    }
    else {
        watchdog("Review email", "Email successfully sent to the MORTAR administrators. Message: <pre>" . $author_message . "</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>");
    }
    
    /*
     * Email to author that SOP is send to be issued by administrator 
    */
    
    $author = user_load($controlled_document_author_id);

    $message_for_author = '';

    $message_for_author = 'Dear ' . $author -> name . ",\r\n";
    $message_for_author .= 'SOP ' . $controlled_document_title . " (". $controlled_document_id .") has now been reviewed by the Authoriser and has passed the Authorisation process.
    The SOP has now been sent for issue. No further action required. \r\n";
    //$message_for_author .= 'http://mortar.octru.ox.ac.uk/'. $controlled_document_address ."\r\n";
    
    if(isset($author_message)) {
        $message_for_author .= $author_message;
    }
   
    $message_for_author .= "Thanks,\n -- \n MORTAR";
    
    mail($author -> mail, $controlled_document_id . ' Authoriser review outcome available.', $message_for_author, $headers);
    drupal_set_message(t('Email successfully sent to the author:<pre>'.$author->name.'</pre>'), 'status');
    
    if(isset($author_message)) {
        watchdog("Review email", "Email successfully sent to the Authoriser: <pre>". $author->name .' - '. $author -> mail."</pre> Message: <pre>" . $author_message . "</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>"); 
    }
    
    #debug($controlled_document_node)
}


function mortar_send_controlled_document_approval_outcome(&$form, &$form_state, $form_id = 'controlled_document_node_form') {
    
    $error = FALSE;
    
    if($form_state['values']['mortar_field_notify_author'] == 0) {
        drupal_set_message(t('Approval response email notification has NOT been sent. If you wish to do so please,
                         press Edit below, check "Send Controlled Document approval response" box and press Save.'), 'warning');
        $error = TRUE;
    }
    
    if($error) {
        return;
    }
    
    global $user;
    
    $logged_user = $user -> name;
    $controlled_document_node = node_load($form_state['nid']);
    
    $controlled_document_address = drupal_get_path_alias('node/' . $controlled_document_node -> nid);
    $controlled_document_id = $controlled_document_node -> title;
    $controlled_document_title = $controlled_document_node -> field_title['und'][0]['value'];
    $controlled_document_author_id = $controlled_document_node -> uid;
    
    $author = user_load($controlled_document_author_id);
    
    $headers = 'From: OCTRU MORTAR <octrutrialshub@ndorms.ox.ac.uk>';
    $message = '';
    
    if($form_state['values']['mortar_field_message']!='') {
        $author_message = 'Message sent by '. $logged_user . ": \r\n" . $form_state['values']['mortar_field_message'] . "\r\n";
    }
    

    $message = 'Dear ' . $author -> name . ",\r\n";
    $message .= 'SOP ' . $controlled_document_title . " (". $controlled_document_id .") has now been reviewed by the administrator. \r\n";
    $message .= 'http://mortar.octru.ox.ac.uk/'. $controlled_document_address ."\r\n";
    
    if(isset($author_message)) {
        $message .= $author_message;
    }
    
    $message .= "Thanks,\n -- \n MORTAR";
    
    mail($author -> mail, $controlled_document_id . ' administrative review outcome available.', $message, $headers);
    drupal_set_message(t('Email successfully sent to the author:<pre>'.$author -> name .'</pre>'), 'status');
    
    if(isset($author_message)) {
        watchdog("Review email", "Email successfully sent to the reviewer: <pre>". $author -> name .' - '. $author -> mail."</pre> Message: <pre>" . $author_message . "</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>"); 
    }
    else {
        watchdog("Review email", "Email successfully sent to the reviewer: <pre>". $author -> name .' - '. $author -> mail."</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>");
    }
    
    #debug($controlled_document_node);

}

function mortar_send_reviewer_notification(&$form, &$form_state, $form_id = 'controlled_document_node_form') {
    
    $error = FALSE;
    
    if($form_state['values']['mortar_field_notify_administrator'] == 1 ) {
        $error = TRUE;
    }
    
    if($form_state['values']['mortar_field_notify_administrator_authorised'] == 1 ) {
        $error = TRUE;
    }
    
    if($error) {
        return;
    }
    
    if($form_state['values']['mortar_field_notify_reviewer'] == 0) {
        drupal_set_message(t('Email notification has NOT been sent. If you wish to do so please,
                             press Edit below, check "Send notification to reviewers" box and press Save.'), 'warning');
        $error = TRUE;
    }
    
    #debug($form_state['values']);
    
    if($error) {
        return;
    }
    
    global $user;
    
    $logged_user = $user -> name;
    
    $reviewers = $form_state['values']['field_reviewers']['und'];


    $controlled_document_node = node_load($form_state['nid']);
    
    $controlled_document_address = drupal_get_path_alias('node/' . $controlled_document_node -> nid);
    $controlled_document_id = $controlled_document_node -> title;
    $controlled_document_title = $controlled_document_node -> field_title['und'][0]['value'];
    
    $headers = 'From: OCTRU MORTAR <octrutrialshub@ndorms.ox.ac.uk>';
    $message = '';
    
    if($form_state['values']['mortar_field_message']!='') {
        $author_message = 'Message sent by '. $logged_user . ": \r\n" . $form_state['values']['mortar_field_message'] . "\r\n";
    }
    
    foreach($reviewers as $reviewer) {
        $user_details = user_load($reviewer['target_id']);
        $user_email = $user_details -> mail;
        $message = 'Dear ' . $user_details -> name . ",\r\n";
        $message .= 'You have been selected as a reviewer of '. $controlled_document_title . ' (' . $controlled_document_id . ") SOP that is now ready to be reviewed. Please navigate to\r\n";
        $message .= 'http://mortar.octru.ox.ac.uk/'. $controlled_document_address ."\r\n";
        $message .= "to review the SOP. \r\n";
        
        if(isset($author_message)) {
            $message .= $author_message;
        }
        
        $message .= "Thanks,\n -- \n MORTAR";
        
        mail($user_email, $controlled_document_id . ' ready for review.', $message, $headers);
        drupal_set_message(t('Email successfully sent to the reviewer:<pre>'.$user_details -> name .'</pre>'), 'status');
        
        if(isset($author_message)) {
            watchdog("Review email", "Email successfully sent to the reviewer: <pre>". $user_details -> name .' - '. $user_email."</pre> Message: <pre>" . $author_message . "</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>");    
        }
        else {
            watchdog("Review email", "Email successfully sent to the reviewer: <pre>". $user_details -> name .' - '. $user_email."</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>");    
        }
        
        
    }
    
    #debug($controlled_document_node);

}


/*
 *
 */
function mortar_views_send_mail_queued($message, $view, $row_id) {

    //add watchdog to get reminders in OL Training Record
    $nid = $view -> result[$row_id] -> nid;
    watchdog('TR reminder email', 'Traning Record email reminder sent. Training Record ID: %id', array('%id' => $nid), WATCHDOG_NOTICE, $link = NULL);

}


/*
 *
 */
function mortar_send_controlled_document_for_compliance_review(&$form, &$form_state, $form_id = 'controlled_document_node_form') {
    
    $error = FALSE; 
    
    if($form_state['values']['mortar_field_notify_compliance_reviewer'] == 0) {
        $error = TRUE;
    }
    
    if($error) {
        return;
    }
    
    global $user;
    
    $logged_user = $user -> name;
    
    $reviewers = $form_state['values']['field_compliance_reviewers']['und'];


    $controlled_document_node = node_load($form_state['nid']);
    
    $controlled_document_address = drupal_get_path_alias('node/' . $controlled_document_node -> nid);
    $controlled_document_id = $controlled_document_node -> title;
    $controlled_document_title = $controlled_document_node -> field_title['und'][0]['value'];
    
    $headers = 'From: OCTRU MORTAR <octrutrialshub@ndorms.ox.ac.uk>';
    $message = '';
    
    if($form_state['values']['mortar_field_message']!='') {
        $author_message = 'Message sent by '. $logged_user . ": \r\n" . $form_state['values']['mortar_field_message'] . "\r\n";
    }
    
    foreach($reviewers as $reviewer) {
        $user_details = user_load($reviewer['target_id']);
        $user_email = $user_details -> mail;
        $message = 'Dear ' . $user_details -> name . ",\r\n";
        $message .= 'You have been selected as a compliance reviewer of '. $controlled_document_title . ' (' . $controlled_document_id . ") SOP that is now ready to be reviewed. Please navigate to\r\n";
        $message .= 'http://mortar.octru.ox.ac.uk/'. $controlled_document_address ."\r\n";
        $message .= "to review the SOP. \r\n";
        
        if(isset($author_message)) {
            $message .= $author_message;
        }
        
        $message .= "Thanks,\n -- \n MORTAR";
        
        mail($user_email, $controlled_document_id . ' ready for review.', $message, $headers);
        drupal_set_message(t('Email successfully sent to the compliance reviewer:<pre>'.$user_details -> name .'</pre>'), 'status');
        
        if(isset($author_message)) {
            watchdog("Review email", "Email successfully sent to the complaince reviewer: <pre>". $user_details -> name .' - '. $user_email."</pre> Message: <pre>" . $author_message . "</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>");    
        }
        else {
            watchdog("Review email", "Email successfully sent to the complaince reviewer: <pre>". $user_details -> name .' - '. $user_email."</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>");
        }
    }

}


/*
 *Send email to author for further review of SOP
*/
function mortar_send_author_further_review_notification(&$form, &$form_state, $form_id = 'controlled_document_node_form') {
    
    $error = FALSE;
    
    if($form_state['values']['mortar_field_notify_administrator'] == 1 ) {
        $error = TRUE;
    }
    
    if($error) {
        return;
    }
    
    if($form_state['values']['mortar_fieldset_notify_author_review'] == 0) {
        drupal_set_message(t('Email notification has NOT been sent. If you wish to do so please,
                             press Edit below, check "Send notification to author for further review" box and press Save.'), 'warning');
        $error = TRUE;
    }
    
    #debug($form_state['values']);
    
     if($error) {
        return;
    }
    
    global $user;
    
    $logged_user = $user -> name;
    $controlled_document_node = node_load($form_state['nid']);
    
    $controlled_document_address = drupal_get_path_alias('node/' . $controlled_document_node -> nid);
    $controlled_document_id = $controlled_document_node -> title;
    $controlled_document_title = $controlled_document_node -> field_title['und'][0]['value'];
    $controlled_document_author_id = $controlled_document_node -> uid;
    
    $author = user_load($controlled_document_author_id);
    
    $headers = 'From: OCTRU MORTAR <octrutrialshub@ndorms.ox.ac.uk>';
    $message = '';
    
    if($form_state['values']['mortar_field_message']!='') {
        $author_message = 'Message sent by '. $logged_user . ": \r\n" . $form_state['values']['mortar_field_message'] . "\r\n";
    }
    

    $message = 'Dear ' . $author -> name . ",\r\n";
    $message .= 'SOP ' . $controlled_document_title . " (". $controlled_document_id .") has now been reviewed by the administrator. \r\n";
    $message .= 'http://mortar.octru.ox.ac.uk/'. $controlled_document_address ."\r\n";
    
    if(isset($author_message)) {
        $message .= $author_message;
    }
    
    $message .= "Thanks,\n -- \n MORTAR";
    
    mail($author -> mail, $controlled_document_id . ' administrative review outcome available.', $message, $headers);
    drupal_set_message(t('Email successfully sent to the author:<pre>'.$author -> name .'</pre>'), 'status');
    
    if(isset($author_message)) {
        watchdog("Review email", "Email successfully sent to the reviewer: <pre>". $author -> name .' - '. $author -> mail."</pre> Message: <pre>" . $author_message . "</pre> Controlled Document: <pre>" . $controlled_document_id . "</pre>"); 
    }
    
    #debug($controlled_document_node)
}
