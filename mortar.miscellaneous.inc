<?php
/*
 * checks if curret user is reviewer of the CD
 */
function mortar_is_reviewer($node, $account) {
    
    if(!isset($node -> field_reviewers['und'])) {
        return FALSE;
    }
    
    $reviewers = $node -> field_reviewers['und'];

            
    foreach($reviewers as $reviewer) {

        if($reviewer['target_id'] == $account -> uid) {  
            return TRUE;
        }
                
    }
    
    return FALSE;  
}



/*
 * !!!!!!!!!!! DEPRECATED !!!!!!!!!!!!
 * checks if user has CTU access 
 */
function mortar_has_ctu_access($node, $user_load) {
    
    if(!isset($user_load -> field_ctu['und'])) {
        return FALSE;
    }
    
    $ctus = $user_load -> field_ctu['und'];

            
    foreach($ctus as $ctu) {

        if($ctu['target_id'] == $node -> nid) {  
            return TRUE;
        }
                
    }
    
    return FALSE;  
}

/*
 * checks if curret user has training record access
 */
function mortar_has_training_record_access($node, $user_load) {
    
    if(!isset($user_load -> field_ctu['und'])) {
        return FALSE;
    }
    
    if(!isset($node -> field_trainee['und'])) {
        return FALSE;    
    }
    
    $trainee = user_load($node -> field_trainee['und'][0]['target_id'] );
    
    
    if(!isset($trainee -> field_ctu['und'])) {
        return FALSE;
    }
    
    $trainee_ctus = $trainee -> field_ctu['und'][0];
    $user_ctus = $user_load -> field_ctu['und'][0];
    
    $ctu_access = array_diff($trainee_ctus,$user_ctus);
    #debug($user_ctus);
    if(!isset($ctu_access['target_id'])) {
        #$ctu_access = TRUE;
        return TRUE;
    }
    
    
    
    return FALSE;  
}


/*
 * Checks if current user is compliance reviwer of the CD
 */
function mortar_is_compliance_reviewer($node, $account) {
    
    if(!isset($node -> field_compliance_reviewers['und'])) {
        return FALSE;
    }
    
    $reviewers = $node -> field_compliance_reviewers['und'];

            
    foreach($reviewers as $reviewer) {

        if($reviewer['target_id'] == $account -> uid) {  
            return TRUE;
        }
                
    }
    
    return FALSE;  
}


/*
 * checks if current user is mortar admin
 */
function mortar_is_mortar_admin($account) {
    
    if(in_array('mortar administrator', $account->roles)) {
        return TRUE;
    }
    return FALSE;
}


/*
 * checks if current user has training record
 */
function mortar_has_training_record($trainee_id, $training_type, $training_name, $controlled_doc_id, $controlled_doc_v) {
    
    if($training_type == 24) { //Controlled Document
        
        //(API) load nodes from DB for given conditions
        $training_object = new EntityFieldQuery();
        $training = $training_object
        ->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'training_record')
        ->propertyCondition('status', 1)
        ->fieldCondition('field_related_controlled_documen', 'target_id', $controlled_doc_id)
        ->fieldCondition('field_controlled_doc_v_number', 'target_id', $controlled_doc_v)
        ->fieldCondition('field_trainee', 'target_id', $trainee_id)
        ;
        
        $training_result = $training -> execute();
        #debug();
        if (isset($training_result['node'])) {
            #drupal_set_message('t','status');
            $controlled_doc_tr = node_load($controlled_doc_id);
            $user_tr = user_load($trainee_id);
            $version_doc_tr = taxonomy_term_load($controlled_doc_v);
            $node_tr = node_load(key($training_result['node']));
            
            //this is set because of the check when user completes the test. should be addjusted to let OL when they add new training request know that the training has been added but its not completed
            if($node_tr -> field_training_status['und'][0]['target_id'] == 33) {
                drupal_set_message(t('Training Record <pre>'.$user_tr -> name . ' -> '.$controlled_doc_tr -> title.' v' .$version_doc_tr -> name.'</pre>
                                    already exists. This has not been added.'), 'warning');
            }
            return array('tr_id' => key($training_result['node']), 'tr_cd_title' => $controlled_doc_tr -> title, 'tr_status' => $node_tr -> field_training_status['und'][0]['target_id']);
        }
        else {
            return NULL;
        }
    }
    else {
        //needs review
        if($training_type == 25 || $training_type == 47 || $training_type == 46 || $training_type == 48 || $training_type == 49 || $training_type == 56) { //Other type
            /*
            $training_object = new EntityFieldQuery();
            $training = $training_object
            ->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', 'training_record')
            ->propertyCondition('status', 1)
            ->fieldCondition('field_training_details', 'value', $training_name)
            ->fieldCondition('field_training_type', 'target_id', $training_type)
            ->fieldCondition('field_trainee', 'target_id', $trainee_id)
            ;
            
            $training_result = $training -> execute();
            
            if (isset($training_result['node'])) {
                
                $user_tr = user_load($trainee_id);
                $user_tr_name_other = taxonomy_term_load($training_name);
                $node_tr = node_load(key($training_result['node']));
                
                drupal_set_message(t('Training Record <pre>'.$user_tr -> name . ' -> '.$user_tr_name_other -> name.'</pre>
                                     already exists. This has not been added.'), 'warning');
                return key($training_result['node']); 
            }
            else {
                return NULL;
            }
            */
        }
        else {
            $training_object = new EntityFieldQuery();
            $training = $training_object
            ->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', 'training_record')
            ->propertyCondition('status', 1)
            ->fieldCondition('field_training_type', 'target_id', $training_type)
            ->fieldCondition('field_trainee', 'target_id', $trainee_id)
            ;
            
            $training_result = $training -> execute();
            
            if (isset($training_result['node'])) {
                
                $user_tr = user_load($trainee_id);
                $user_tr_name = taxonomy_term_load($training_type);
                
                drupal_set_message(t('Training Record <pre>'.$user_tr -> name . ' -> '.$user_tr_name -> name.'</pre>
                                     already exists. This has not been added.'), 'warning');
                return key($training_result['node']);
            }
            else {
                return NULL;
            }
            
        }
    }
    
    return NULL;
}

