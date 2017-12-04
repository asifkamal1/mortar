<?php

/*
 *
 */
function mortar_traning_record_reminder_sent_data($data) {
    
    $tr_nid = $data -> nid;
    $query = "SELECT timestamp, variables FROM mortar_watchdog WHERE type = 'TR reminder email' ORDER BY timestamp DESC;";

    $result = db_query($query);
    $dates = array();
    
    foreach ($result as $record) {
        if(unserialize($record -> variables)['%id'] == $tr_nid) {
            $dates[] =  date('d M Y',$record -> timestamp);
        }
    }
    
    if(!empty($dates)) {
        return $dates;
    }
    else {
        return NULL;    
    }
    
}


/*
 *
 */
function mortar_views_recent_cd_tr_changes($data) {
    global $user;
    $mortar_user = user_load($user -> uid);
    
    //row update unix time stamp
    $time_stamp = $data -> node_changed;
    
    //user previous login
    
    //user last login
    if(isset($mortar_user -> field_current_login['und'][0]['value'])) {
        $current_login = strtotime($mortar_user -> field_current_login['und'][0]['value']);    
    }
    else {
        $current_login = strtotime("today");   
    }
    
    if(isset($mortar_user -> field_last_login['und'][0]['value'])) {
        //user last login
        $last_login = strtotime($mortar_user -> field_last_login['und'][0]['value']);
    }
    else {
        $last_login = $current_login - 60 * 60 * 24 * 7;
    }
    
    #debug($data);
    
    if($time_stamp < $last_login) {
        return TRUE;
    }
    
    switch ($data -> node_type) {
        
        case 'training_record':
            $node = node_load($data -> nid);
            
            if($node -> field_training_status['und'][0]['target_id'] == 33) {
                return TRUE;
            }
            
            if($node -> field_trainee['und'][0]['target_id'] == $user -> uid) {
                return FALSE;
            }
            /*
            if($node -> field_training_type['und'][0]['target_id'] == 24) {
                return FALSE;
            }
            
            if($mortar_user -> uid == $data -> node_uid ) {
                return FALSE;
            }
        
            if($node -> nid == $mortar_user -> uid) {
                return FALSE; 
            }
            */
            return TRUE;
            break;
        
        
        case 'controlled_document':
            
            $filter = mortar_views_cd_new_updated($data -> nid);
            
            if($filter == 'updated' || $filter == 'new') {
                return FALSE;
            }
            return TRUE;
            /*
            $node = node_load($data -> nid);
            
            if($node -> field_status['und'][0]['target_id'] == 13 || $node -> field_status['und'][0]['target_id'] == 6) {
                if(in_array('administrator', $user -> roles)) {
                    #debug($data);
                    return FALSE;
                }
                return TRUE;
            }
            
            return FALSE;
            */
            
            break;
        
    }
   
}

/*
$nids = array($data -> nid);
            $field_values = db_select('field_revision_field_trainee', 'f')
                ->fields('f', array('entity_id', 'field_trainee_target_id'))
                ->condition('entity_type', 'node')
                ->condition('bundle', 'training_record')
                ->condition('entity_id', $nids, 'IN')
                ->condition('deleted', 0)
                ->execute()
                ->fetchAllKeyed();
                
*/


/*
 *
 */
function mortar_views_cd_new_updated($nid) {
    
    $cd = node_load($nid);
    
    if(!$cd) {
        return 'false';
    }
    
   // $last_change = $cd -> changed;
   // $last_change =  $cd -> field_issue_date['und'][0]['value'];
    $last_change =  isset($cd -> field_issue_date['und'][0]['value']) ? $cd -> field_issue_date['und'][0]['value'] : "";
    $vid =  $cd -> vid;
    
    //get previous vid
    $previous_vid = db_query('SELECT MAX(vid) AS vid FROM {node_revision} WHERE vid < :vid AND nid = :nid', array(':vid' => $vid, ':nid' => $cd -> nid)) -> fetchField();
    
    if($last_change >= strtotime('-30 days')) {
        //if no previous version avaialbe it means it is new document
        if(!$previous_vid) {
            return 'new';
        }
        
        $current_node = node_load($nid);
        $prev_rev_node = node_load($nid, $previous_vid);
        
        //here we looking if current 'Active version number' and previous 'Active version number' are same or not
        if(($current_node -> field_active_version_number['und'][0]['tid'] != $prev_rev_node -> field_active_version_number['und'][0]['tid']) && $current_node -> field_active_version_number['und'][0]['tid']  != 8) {
            return 'updated';    
        } else {
            //In case if current 'Active version number' and previous 'Active version number' are same but they equal to 1 or 1.0 then they are new
            if($current_node -> field_active_version_number['und'][0]['tid']  == 8) {
                return 'new';
            } else {
                return 'updated';   
            }
        }
    }
    
    return 'false';
}



/*
 * Checking the version of SOP documnet realted to a training test is updated or same
 */
function mortar_views_cd_check($nid, $created) {
   
    $cd = node_load($nid);
    
    if(!$cd) {
        return 'false';
    }
    
   // $last_change = $cd -> changed;
   // $last_change =  $cd -> field_issue_date['und'][0]['value'];
    $last_change =  isset($cd -> field_issue_date['und'][0]['value']) ? $cd -> field_issue_date['und'][0]['value'] : "";
    $vid =  $cd -> vid;
    
    //get previous vid
    $previous_vid = db_query('SELECT MAX(vid) AS vid FROM {node_revision} WHERE vid < :vid AND nid = :nid', array(':vid' => $vid, ':nid' => $cd -> nid)) -> fetchField();
    

    if($last_change >= $created) {
        //if no previous version avaialbe it means it is new document
        if(!$previous_vid) {
            return 'same';
        }
        
        $current_node = node_load($nid);
        $prev_rev_node = node_load($nid, $previous_vid);
    
        //here we looking if current 'Active version number' and previous 'Active version number' are same or not
        if(($current_node -> field_active_version_number['und'][0]['tid'] > $prev_rev_node -> field_active_version_number['und'][0]['tid']) && $current_node -> field_active_version_number['und'][0]['tid']  != 8) {
            return 'updated';    
        } else {
            //In case if current 'Active version number' and previous 'Active version number' are same but they equal to 1 or 1.0 then they are new
            if($current_node -> field_active_version_number['und'][0]['tid']  == 8) {
                return 'same';
            } else {
                return 'updated';   
            }
        }
    }    
    
    return 'false';
}


/*
 * Checking the related Training record of the provided sop for a specfied user
 */
function mortar_views_tr_related_cd_report($user_uid, $recieved_nid, $related_cd, $active_version) {
    // active version tid of cd
    //$cd = node_load($recieved_nid);
    //$tidv = $cd->field_active_version_number['und'][0]['tid'];
    $query_list_check = new EntityFieldQuery();
    $query_list_check
    
    ->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'training_record')
    ->fieldCondition('field_trainee', 'target_id', $user_uid)
    ->propertyOrderBy('nid', 'ASC');
    
    // Execute, returning an array of arrays.
    $result_other = $query_list_check->execute();

    if(isset($result_other['node'])) {

        foreach($result_other['node'] as $nid => $stub_node) {
     
            //Load the full node and wrap it with entity_metadata_wrapper().
            $node = node_load($nid);

            $values = array();
            //the the active version of the TR related control document
            //$cd_related_control_doc = node_load($node->field_related_controlled_documen['und'][0]['target_id']);

             //if the Recvied NID is equall to the NID og the TR related document also the active version need to be same to the related control doc version number of the user TR record (as we looking for active CD) finally need exclude all the TR which are Superseded
            if(((isset($node->field_related_controlled_documen['und'][0]['target_id']) && $node->field_related_controlled_documen['und'][0]['target_id'] == $recieved_nid) && (isset($node->field_controlled_doc_v_number['und'][0]['target_id']) && $node->field_controlled_doc_v_number['und'][0]['target_id'] == $active_version)) && (isset($node->field_training_status['und'][0]['target_id']) && $node->field_training_status['und'][0]['target_id'] !== '211')) {
                $values[0] = $node->field_training_status;
                $values[1] = "Assigned";
                $values[2] = "$nid"; 
              
                return $values;
            
            }

        }
    }

    return;
}


/*
 * Checking the related Training record of the provided sop for a specfied user for full history view
 */
function mortar_views_tr_related_cd_full_history_report($user_uid, $recieved_nid, $related_cd, $active_version) {
    global $values, $count;
    $values = array();
    // active version tid of cd
    //$cd = node_load($recieved_nid);
    //$tidv = $cd->field_active_version_number['und'][0]['tid'];
    $query_list_check = new EntityFieldQuery();
    $query_list_check
    
    ->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'training_record')
    ->fieldCondition('field_trainee', 'target_id', $user_uid)
    ->propertyOrderBy('nid', 'ASC');
    
    // Execute, returning an array of arrays.
    $result_other = $query_list_check->execute();

    if(isset($result_other['node'])) {
        $count = 0;
        foreach($result_other['node'] as $nid => $stub_node) {
     
            //Load the full node and wrap it with entity_metadata_wrapper().
            $node = node_load($nid);

            
            //the the active version of the TR related control document
            //$cd_related_control_doc = node_load($node->field_related_controlled_documen['und'][0]['target_id']);

             //if the Recvied NID is equall to the NID of the TR related document 
            if($node->field_related_controlled_documen['und'][0]['target_id'] == $recieved_nid) {
                
                $term_version = taxonomy_term_load($node->field_controlled_doc_v_number['und'][0]['target_id']);
                $term_status = taxonomy_term_load($node->field_training_status['und'][0]['target_id']);
                
                $values[$count]['version'] = $term_version->name;
                $values[$count]['status'] = $term_status->name;
                $values[$count]['nid'] = "$nid"; 
                $count++;
               // $values[ ] = 'Vesrion: '. $term_version->name .',  Status: '.$term_status->name;
            }

        }
        
        return $values;
    }

    return; 
}


/*
 *
 */
function mortar_views_pre_view(&$view, &$display_id, &$args) { 
    #debug($display_id);
    
    if($view->name == 'training_record') {
        
        $detect = mobile_detect_get_object();
        $is_tablet = $detect -> isTablet();
        $is_mobile = $detect -> isMobile();
        
        if($is_mobile || $is_tablet) {
            mortar_render_mobile_dev_training_record_display($view, $display_id, $args);
        }
        
        if($display_id == 'user_training_record_export') {
            mortart_training_record_export_data_access_fix();
        }
    }
    
    if($view->name == 'trainee_list') {
        mortar_alter_user_list_display($view, $display_id, $args);
    }
    
    if ($view->name == "operational_lead_user_list") {
        mortar_operational_lead_user_view_alter($view, $display_id, $args);
    }
    
}


/*
 *
 */
function mortar_alter_user_list_display(&$view, &$display_id, &$args) {
    
    global $user;
    #debug($user->roles);
    
    if(arg(1) == 'structure') {
        return;   
    }
    
    if(in_array('mortar operational lead', $user->roles)) {
        #$view->set_display('op_trainee_list');
    }
    
    if(in_array('mortar administrator', $user->roles)) {
        if($user -> uid == 1 && $display_id == 'ol_trainee_list') {
            //$display_id = 'ma_trainee_list';
            #unset($view->display['ol_trainee_list']);
            #$view->set_display('ma_trainee_list');    
        }
        
    }
    
    if(in_array('administrator', $user->roles)) {
        #$view->set_display('ma_trainee_list');
    }
}


/*
 *
 */
function mortart_training_record_export_data_access_fix() {
    global $user;
    
    if(in_array('administrator', $user->roles)) {
        return;
    }
    
    if(arg(1) != $user -> uid) {
        header('Location: /user');
    }
}


/*
 *
 */
function mortar_render_mobile_dev_training_record_display(&$view, &$display_id, &$args) {
    
    switch ($display_id) {
        case 'desktop_display_user':
            $view->set_display('mobile_display_user'); 
            break;
        case 'desktop_display_ol':
            $view->set_display('mobile_display_ol'); 
            break;
    }
}


/*
 *
 */
function mortar_views_query_alter(&$view, &$query) {
    
    global $user;
    $account = user_load($user -> uid);
    
    if($node_load = menu_get_object()) {
        $node = node_load($node_load -> nid);
    }
    
    
    if($view -> name == 'controlled_document_reviews' && isset($node)) {
        mortar_controlled_document_reviews_view_alter($view, $query, $account, $node);
    }

    if ($view->name == "training_record") {
        mortar_training_record_view_alter($view, $query, $account, $node = NULL);
    }
    
    if ($view->name == "operational_lead_user_list") {
        #mortar_operational_lead_user_view_alter($view, $query, $account, $node = NULL);
    }
    
    if ($view->name == "all_training_record_related_to_sops_report") {

        foreach($query->where[1]['conditions'] as $idx => $condition) {
            if (!is_object($condition['field']) && $condition['field'] == 'users_field_data_field_trainee.uid') {
               
                //change the vlaue of PHP field with the supplied user ID in the filter that can be use in the other PHP field to get the user TR record related to current SOP
                $supplied_user = $query->where[1]['conditions'][$idx]['value'][0];
                
                //check the current login user (operation lead) is allowed to see the profile of the supplied user (yes if the supplied user is in hi/her team)
                $supplied_user = $query->where[1]['conditions'][$idx]['value'][0];
                
                global $user, $values, $count, $team, $rec_team;
                
                $team = array();
                $rec_team = array();
                $values = array();
                 
                $mortar_user = user_load($user -> uid);
                
                if(in_array('administrator', $mortar_user -> roles) == 0  && in_array('mortar administrator', $mortar_user -> roles) == 0) {
                   
                    if(!empty($mortar_user->field_ctu)) {
                        for($a = 0; $a < count($mortar_user->field_ctu['und']); $a++){
                            $team[$a] = $mortar_user->field_ctu['und'][$a]['target_id'];
                        }
                    }
                    
                    $supplied_user_info = user_load($supplied_user);
                    
                    if(!empty($supplied_user_info->field_ctu)) {
                        for($a = 0; $a < count($supplied_user_info->field_ctu['und']); $a++){
                            $rec_team[$a] = $supplied_user_info->field_ctu['und'][$a]['target_id'];
                        }
                    }
                        
                    $matches_team = array_intersect($rec_team,$team);
                   
                    if(empty($matches_team)) {
                        drupal_set_message(t($supplied_user_info->name. ' is not in your Team.'), 'warning');
                        unset($query);  
                        retrurn;
                    }
                }
                
                $view->display_handler->handlers['field']['php_1']->options['php_output']   = '<?php echo '. $supplied_user .'; ?>';
                
                // or you can unset this condition and write down a new one (as we dont need it to get sop record)
                unset($query->where[1]['conditions'][$idx]);  
            }
        }
    }
    
    if ($view->name == "all_training_record_related_to_sops_full_history_report") {

        foreach($query->where[1]['conditions'] as $idx => $condition) {
            if (!is_object($condition['field']) && $condition['field'] == 'users_field_data_field_trainee.uid') {
                
                //change the vlaue of PHP field with the supplied user ID in the filter that can be use in the other PHP field to get the user TR record related to current SOP
                $supplied_user = $query->where[1]['conditions'][$idx]['value'][0];
                $view->display_handler->handlers['field']['php_1']->options['php_output']   = '<?php echo '. $supplied_user .'; ?>';
                
                // or you can unset this condition and write down a new one (as we dont need it to get sop record)
                unset($query->where[1]['conditions'][$idx]);  
            }
        }
    }
    
    if ($view->name == "individual_all_training_record_related_to_sops_report") {
                global $user;
                $supplied_user = $user -> uid;
                //change the vlaue of PHP field with the supplied user ID in the filter that can be use in the other PHP field to get the user TR record related to current SOP
                $view->display_handler->handlers['field']['php_1']->options['php_output']   = '<?php echo '. $supplied_user .'; ?>';
    }
}


function mortar_entity_view_mode_alter(&$view_mode, $context) {
  // For nodes, change the view mode when it is teaser.
  #if ($context['entity_type'] == 'view' && $view_mode == 'teaser') {
    #$view_mode = 'mobile_display_user';
    #debug('te');
  #}
}

/*
 *
 */
function mortar_operational_lead_user_view_alter(&$view, &$display_id, &$args) {
    
    #$view->set_display('ma_user_list'); 
    
    /*
    if(!mortar_is_mortar_admin($account)) {
        
        if(isset($account -> field_ctu['und'])) {
            $ctu = $account -> field_ctu['und'][0]['target_id'];
        }
        else {
            drupal_set_message(t('Sorry - it seems that you have not been assigned to any CTU. Please contract system administrator.'), 'error');
            $ctu = -1;
        }
        
        $query->add_where(
                  0,
                  db_or()
                  ->condition('field_data_field_ctu.field_ctu_target_id', $ctu, '=')
                  );
        
    }
    #debug($query);
    */
}


/*
 *
 */
function mortar_training_record_view_alter(&$view, &$query, $account, $node) {
    #debug($query);
    /*
    if(!mortar_is_mortar_admin($account) && arg(0) == 'training-record') {
        
        if(isset($account -> field_ctu['und'])) {
            $ctu = $account -> field_ctu['und'][0]['target_id'];
        }
        else {
            drupal_set_message(t('Sorry - it seems that you have not been assigned to any CTU. Please contract system administrator.'), 'error');
            $ctu = -1;
        }
        
        $query->add_where(
                  0,
                  db_or()
                  ->condition('og_membership_users.gid', $ctu, '=')
                  );
    }
    */
}


/*
 *
 */
function mortar_controlled_document_reviews_view_alter(&$view, &$query, $account, $node) {

    if(!isset($node)) {
        return;
    }
    
    if($node -> uid != $account -> uid && !mortar_is_mortar_admin($account)) {
        $query->add_where(
                          0,
                          db_or()
                          ->condition('node.uid', $account -> uid, '=')
                          #->condition('node.uid', 1, '=')
                          );
    }

}


/*
 *
 */
function mortar_views_post_render(&$view, &$output, &$cache) {
    #debug($view->display_handler->options);
    global $user;
    $account = $user;
    
    if($node_load = menu_get_object()) {
        $node = node_load($node_load -> nid);
    }
    
    
    if($view -> name == 'controlled_docs_under_review') {
        mortar_controlled_docs_under_review_view_alter($view, $output, $account, $node = NULL);
    }
    
    /*
    if($view -> name == 'my_controlled_documents' ||
       $view -> name == 'controlled_documents' ||
       $view -> name == 'controlled_docs_awaiting_approval'||
       $view -> name == 'compliance_check' ||
       $view -> name == 'global_controlled_documents'
       ) {
        mortar_my_controlled_docs_view_alter($view, $output, $account, $node = NULL);
    }
    */

}


/*
 *
 */
function mortar_my_controlled_docs_view_alter(&$view, &$output, $account, $node) {
    if($view->result == NULL) {
        return;
    }

    foreach ($view -> result as $key => &$result) {
        
        $replace = '<tr class="row-' . ($key+1);
        
        $result_status = $result -> _field_data['nid']['entity'] -> field_status['und'][0]['target_id'];
        
        switch($result_status) {
            case 6 :
                $output = str_replace($replace, '<tr class="row-' . ($key+1) . ' mortar-not-active', $output); 
                break;
            
            case 4: 
                $output = str_replace($replace, '<tr class="row-' . ($key+1) . ' mortar-active mortar-warning', $output);
                break;
            
            case 3: 
                $output = str_replace($replace, '<tr class="row-' . ($key+1) . ' mortar-active', $output);
                break;
            
            case 13: 
                $output = str_replace($replace, '<tr class="row-' . ($key+1) . ' mortar-not-active mortar-warning', $output);
                break;
            
            case 41: 
                $output = str_replace($replace, '<tr class="row-' . ($key+1) . ' mortar-active mortar-warning', $output);
                break;
        }

    }
}


/*
 *
 */
function mortar_controlled_docs_under_review_view_alter(&$view, &$output, $account, $node) {
        
    if($view->result == NULL) {
        return;
    }
    
    foreach ($view -> result as $key => &$result) {
        $view_result = NULL;
        $review_number = $result -> _field_data['nid']['entity'] -> field_review_version_number['und'][0]['tid'];
        $controlled_doc_rev = new EntityFieldQuery();
        $revisions = $controlled_doc_rev
        ->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'controlled_document_review')
        ->propertyCondition('uid', $account -> uid)
        ->propertyCondition('status', 1)
        ->fieldCondition('field_controlled_document_id', 'target_id', $result -> nid)
        ->fieldCondition('field_review_version', 'target_id', $review_number)
        ;
        
        $replace = '<tr class="row-' . ($key+1);
        
        $view_result = $revisions -> execute();
        if (isset($view_result['node'])) {
            $output = str_replace($replace, '<tr class="row-' . ($key+1) . ' mortar-not-active mortar-status', $output); 
        }
        else {
            $output = str_replace($replace, '<tr class="row-' . ($key+1) . ' mortar-not-active mortar-warning', $output);
        }
    }
}