<?php

/*
 *Adds field control optins to SOP node
 */
function mortar_field_access($op, $field, $entity_type, $entity, $account) {
    
    //hide SOP status field for all but mortar administrator
    if(($field['field_name'] == 'field_status'  || $field['field_name'] == 'field_active_version_word_doc') && $op == 'edit') {
        
        if (in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
        
        /*
        if($field['field_name'] == 'field_status' && $op == 'view') {
            return TRUE;
        }
        */
        return FALSE;
    
    }
    
    if($field['field_name'] == 'field_add_training_record' && $op == 'edit') {
        return FALSE;
    }
    
    
    /*
    //hide SOP status field for all but mortar administrator
    if($field['field_name'] == 'field_cd_test' && ($entity -> field_status['und'][0]['target_id'] != 3 || $entity -> field_status['und'][0]['target_id'] != 41 || $entity -> field_status['und'][0]['target_id'] != 4)) {
        
        if (in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
    
        return FALSE;
    
    }
    */
    
    /*
    //in user edit, hide ctu field for all but admin
    if(($field['field_name'] == 'field_ctu' || $field['field_name'] == 'field_controlled_doc_attributes')  &&  $op == 'edit') {

        if (in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
    
        #return FALSE;
    }
    */
    //in user edit, hide effective date field for all but admin
    if($field['field_name'] == 'field_effective_date'  &&  $op == 'edit') {

        if (in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
    
        return FALSE;
    }
    
    //in user edit, hide issue date field for all but admin
    if($field['field_name'] == 'field_issue_date'  &&  $op == 'edit') {

        if (in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
    
        return FALSE;
    }
    
    //in user edit, hide next review date field for all but admin
    if($field['field_name'] == 'field_next_review_date'  &&  $op == 'edit') {

        if (in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
    
        return FALSE;
    }
    
    //in user edit, hide job title field for all but admin
    if($field['field_name'] == 'field_job_title'  &&  $op == 'edit') {

        if (in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
    
        return FALSE;
    }
    
    //in SOP edit, hide active version number field for all but admin
    if($field['field_name'] == 'field_active_version_number'  &&  $op == 'edit') {

        if (in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
    
        return FALSE;
    }
    
    //in SOP edit, hide active version field document for all but admin
    if($field['field_name'] == 'field_active_version_document' &&  $op == 'edit' ) {

        if (in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
    
        return FALSE;
    }
    
    
    if($field['field_name'] == 'field_review') {

        if(in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
        
        if($entity -> field_status['und'][0]['target_id'] == 3) {
            return FALSE;
        }
    
    }
    
    //in user edit, hide mortar silo field for all but admin
    if($field['field_name'] == 'field_mortar_silo' &&  $op == 'edit') {

        if(in_array('administrator', $account->roles) || in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
        
        return FALSE;
    }
    
    
    //in SOP view, hide review version field document for all but admin if status is active
    if(($field['field_name'] == 'field_review_version_document' || $field['field_name'] == 'field_review_version_number') &&  $op == 'view' ) {
        
        if(in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
        $is_reviewer = mortar_is_reviewer($entity,$account);
        $compliance_reviewer = mortar_is_compliance_reviewer($entity, $account);
        
        if(isset($entity -> uid)) {
            
            if (($entity -> uid == $account -> uid ||
                in_array('administrator', $account->roles) ||
                $compliance_reviewer ||
                $is_reviewer ||
                in_array('mortar administrator', $account->roles)) &&
                $entity -> field_status['und'][0]['target_id'] != 3 &&
                $entity -> field_status['und'][0]['target_id'] != 41 &&
                $entity -> field_status['und'][0]['target_id'] != 13) {
                
                return TRUE;
            }
        }
        
        return FALSE;
    }
    
    /*
    //in SOP view, hide review version field number for all but admin if status is active
    if($field['field_name'] == 'field_review_version_number' &&  $op == 'view' ) {
        
        if(in_array('mortar administrator', $account->roles)) {
            return TRUE;
        }
        
        #$entity -> uid == $account -> uid ||
        if(isset($entity -> uid)) {
            if ($entity -> uid == $account -> uid || in_array('administrator', $account->roles) ||  $compliance_reviewer ||
                in_array('mortar administrator', $account->roles) && $entity -> field_status['und'][0]['target_id'] == 3) {
                return TRUE;
            }
        }
        
        $is_reviewer = mortar_is_reviewer($entity,$account);

        if($is_reviewer && $entity -> field_status['und'][0]['target_id'] != 3 && $entity -> field_status['und'][0]['target_id'] != 13) {
            return TRUE;
        }
        return FALSE;
    }
    */

}