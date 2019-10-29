<?php
/*
 *!!!!!!!!!!! DEPRECATED !!!!!!!!!!!!
 */
function mortar_ctu_access($node, $op, $account) {
    
    $user_load = user_load($account -> uid);
    $has_access = mortar_has_ctu_access($node, $user_load);
    
    if(!$has_access) {
        #return NODE_ACCESS_DENY;
    }
    
    return;
}


/*
 * !!!!!!!!!!! DEPRECATED !!!!!!!!!!!!
 */
function mortar_training_record_access($node, $op, $account) {
    
    $user_load = user_load($account -> uid);
    
    if(arg(0) == 'user') {
        return NODE_ACCESS_IGNORE;
    }

    $has_access = mortar_has_training_record_access($node, $user_load);
    if(!$has_access) {
        #debug($node -> nid);
        #return NODE_ACCESS_DENY;
   
    }
    
    return;
}


/*
 * checks if current user has access to CD
 */
function mortar_controlled_document_access($node, $op, $account) {

    //checks if current user is the review of the current SOP node
    $is_reviewer = mortar_is_reviewer($node, $account);
    
    //checks if current user is mortar administrator
    $is_mortar_admin = mortar_is_mortar_admin($account);
    
    $compliance_reviewer = mortar_is_compliance_reviewer($node, $account);

    //if SOP node withdrwan deny access
    if ($node -> field_status['und'][0]['target_id'] == 238) {
        drupal_set_message(t('Sorry - You do not have access to this SOP <b>['.$node -> title.']</b>. This SOP is no longer active'), 'warning');
        return NODE_ACCESS_DENY;
    }
   
    //if SOP node not active and user not involved deny access
    if ($node -> uid != $account -> uid &&
        $node -> field_status['und'][0]['target_id'] != 3 &&
        $node -> field_status['und'][0]['target_id'] != 41 &&
        $node -> field_status['und'][0]['target_id'] != 4 &&
        $node -> field_status['und'][0]['target_id'] != 238 &&
        !$is_reviewer && !$is_mortar_admin && !$compliance_reviewer) {
        drupal_set_message(t('Sorry - you do not currently have access to this SOP. <pre>'.$node -> field_status['und'][0]['target_id'].'</pre> You are not a reviewer of this SOP.'), 'warning');
        return NODE_ACCESS_DENY;
    }
    
    //if SOP 'awaiting approval' deny edit access for all but mortar admin
    if ($node -> field_status['und'][0]['target_id'] == 13 && $op != 'view' && !$is_mortar_admin && !$compliance_reviewer) {
        drupal_set_message(t($node -> title .' SOP awaiting approval. You cannot make any changes until it has been reviewed by administrator.'), 'warning');
        return NODE_ACCESS_DENY;
    }
    
     //if SOP 'active' or 'active - awaiting approval' deny edit access for all but mortar admin
    if ($node -> field_status['und'][0]['target_id'] == 3 || $node -> field_status['und'][0]['target_id'] == 41 ) {
        
        if($op != 'view' && !$is_mortar_admin) {
            return NODE_ACCESS_DENY;
        }
        
    }
}