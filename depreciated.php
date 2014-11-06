<?php
/*
 *DEPRECIATED
 */


if(!isset($node ->nid)) {
$value = db_query("SELECT nid FROM {node} ORDER BY nid DESC LIMIT 1")->fetchField();
$number1 = strval($value);
$number2 = intval($number1)+1;
return 'Training Record ' . $number2;
}
else {
return 'Training Record ' . $node->nid;
}

 
 
/*
 *
 */
function mortar_training_record_user_connect(&$form, &$form_state, $form_id='') {
    
    $node_load = menu_get_object();
    
    #debug($node_load);
    
    if($node_load == NULL && arg(0) != 'node') {
        $form_state['values']['field_trainee']['und'][0]['target_id'] = $account -> uid;
    }
    else {
        if($form_state['values']['field_trainee']['und'][0]['target_id'] == NULL && $node_load -> type != 'controlled_document') {
            form_set_error('field_trainee', 'Please select trainee.');
            
        }
        else {
            if($node_load -> type == 'controlled_document') {
                $form_state['values']['uid'] = 1;
                $form_state['values']['name'] = 'admin';
                $form_state['values']['field_trainee']['und'][0]['target_id'] = $account -> uid;
                $form_state['values']['field_training_status']['und'][0]['target_id'] = 33;
                $form_state['values']['field_training_type']['und'][0]['target_id'] = 24;
                $form_state['values']['field_related_controlled_documen']['und'][0]['target_id'] = $node_load -> nid;
                $form_state['values']['field_controlled_doc_v_number']['und'][0]['target_id'] = $node_load -> field_active_version_number['und'][0]['tid'];
                $form_state['values']['field_date_completed']['und'][0]['value']['date'] = date("Y-m-j H:i:s");          
            }
            else {
                $form_state['values']['uid'] = $form_state['values']['field_trainee']['und'][0]['target_id'];
                $trainee = user_load($form_state['values']['field_trainee']['und'][0]['target_id']);
                $form_state['values']['name'] = $trainee -> name;
            }
        }
    }
}
 
 
function mortar_alter_controlled_doc_review_form(&$form, $node) {
    
    
    //if Controlled Doc review revision number is set load that revision and set $options for comment dropdown
    if(isset($node -> field_review_version_number['und'][0]['tid'])) {
        $controlled_document_rev_no = $node -> field_review_version_number['und'][0]['tid'];
        $term = taxonomy_term_load($controlled_document_rev_no);
        $options = array($term -> tid => $term -> name);
    }
    else {
        //if its not set - display 'None'
        $options = array('_none' => '- None -');
    }
    
    if($node -> type == 'controlled_document') {
        $form['field_controlled_document_id']['und']['#options'] = array($node -> nid => $node -> title);
    }
        
    $form['field_review_version']['und']['#options'] = $options;
    
    
}

 

function mortar_controlled_document_reviews_alter(&$view, $account) {
    //debug($view ->result[0]);
    
    for ($i=0; $i < sizeof($view->result); $i++) {
        if($view -> result[$i] -> users_node_uid != $account -> uid){
            unset($view -> result[$i]);
        }
    }
    
}



function mortar_comment_view_alter(&$build) {
    
    global $user;
    
    $controlled_doc_uid = $build['#node'] -> uid;
    $user_uid = $user -> uid;
    $is_mortar_admin = mortar_is_mortar_admin($user);    

    if($is_mortar_admin || $controlled_doc_uid == $user_uid) {
        return;
    }
    
    $comment = $build['#comment'];
    #debug($user_uid . ' ' .$comment -> uid);

    
    if($comment -> uid != $user_uid && $comment -> uid != $controlled_doc_uid) {
        $build['#post_render'][] = 'mortar_comment_private';
    }
    
    
}


function mortar_comment_private() {
  // return nothing if comment should not be visible.
}

/*
 *By default this function will unset comments unless current user is admin, author or reviewer
 */
function mortar_unset_comment($account, $node, &$build) {
    
    //checks if current user is admin or author
    if(in_array('mortar administrator', $account->roles)) {
        
        //set correct comment revision
        mortar_set_comment_review_version($account, $node, $build);
        
        //(does not unset comment)
        return;
    }
    
    if($account -> uid == $node -> uid && $node -> field_status['und'][0]['target_id'] != 13) {
        
        //set correct comment revision
        mortar_set_comment_review_version($account, $node, $build);
        
        //(does not unset comment)
        return;
        
    }
    
    //if status is not active and status is not awaiting approval check if current user is reviewer (at this stage we know that user is not admin nor author)
    if ($node -> field_status['und'][0]['target_id'] != 3 && $node -> field_status['und'][0]['target_id'] != 13) {
         
        //checks if user is reviewer   
        $is_reviewer = mortar_is_reviewer($node, $account);
        
        if($is_reviewer) {
            
            //if user is reviewer set correct comment version number
            mortar_set_comment_review_version($account, $node, $build);
            return;
        }
  
    }
    
    $compliance_reviewer = mortar_is_compliance_reviewer($node, $account);
    
    if(mortar_is_compliance_reviewer($node,$account) && $node -> field_status['und'][0]['target_id'] == 13) {
        
        //set correct comment revision
        mortar_set_comment_review_version($account, $node, $build);
        
        //(does not unset comment)
        return;
    }
    
    //unsets comments
    unset($build['comments']);
    return;
}


/*
 *Sets correct comment review number by checking current SOP review number
 */
function mortar_set_comment_review_version($account, $node, &$build) {
    
    //if SOP review revision number is set load that revision and set $options for comment dropdown
    if(isset($node -> field_review_version_number['und'][0]['tid'])) {
        $controlled_document_rev_no = $node -> field_review_version_number['und'][0]['tid'];
        $term = taxonomy_term_load($controlled_document_rev_no);
        $options = array($term -> tid => $term -> name);
    }
    else {
        //if its not set - display 'None'
        $options = array('_none' => '- None -');
    }
    
    $build['mortar_fieldset_controlled_document_rev_history'] = array(
        '#type' => 'fieldset',
        '#title' => 'Review History',
        '#weight' => 99,
        
    );
    
    $compliance_reviewer = mortar_is_compliance_reviewer($node, $account);
    
    $build['mortar_fieldset_controlled_document_rev_history']['#attached']['library'][] = array('system', 'drupal.collapse');
    $build['mortar_fieldset_controlled_document_rev_history']['#attributes']['class'][] = 'collapsible';
    
    if($node -> field_status['und'][0]['target_id'] == 13 && !$compliance_reviewer || $node -> field_status['und'][0]['target_id'] == 3 ) {
        $build['mortar_fieldset_controlled_document_rev_history']['#attributes']['class'][] = 'collapsed';
    }
    
    $build['mortar_fieldset_controlled_document_rev_history']['comments'] = $build['comments'];
    $build['mortar_fieldset_controlled_document_rev_history']['comments']['comment_form']['field_comment_review_version']['und']['#options'] = $options;
    
    if($node -> field_status['und'][0]['target_id'] == 3){
        unset($build['mortar_fieldset_controlled_document_rev_history']['comments']['comment_form']);
    }

    mortar_comment_post_access($account, $node, $build);
    
    
    unset($build['comments']);
    
}


/*
 *
 */
function mortar_comment_post_access($account, $node, &$build) {
    
    #debug($build['#node']);
    #$build['mortar_fieldset_controlled_document_comments']['comments']['comments'][35]['field_comment_review_version_doc']['#object'] -> status = 0;
    #debug($build['mortar_fieldset_controlled_document_comments']['comments']['comment_form']);
    #$node->comment = 0;
}


/*
 *
 */
function mortar_views_pre_build(&$view) {
    
    if ($view->name == "training_record") {

    }
}

function mortar_views_post_build(&$view) {
    /*
    $view->display_handler->options['filters']['field_ctu_target_id']['exposed'] = FALSE;
    
    #$view->display_handler->options['filters']['field_ctu_target_id']['value'] = array();
    
    $view->display_handler->options['filters']['field_ctu_target_id']['value']['value'] = 168;
    $view->display_handler->options['filters']['field_ctu_target_id']['value']['min'] = '';
    $view->display_handler->options['filters']['field_ctu_target_id']['value']['max'] = '';
    debug($view->display_handler->options['filters']['field_ctu_target_id']);
   # debug($view->display_handler->options['filters']['field_ctu_target_id']);
   #[]['value']
   */
}

/*
 *
 */
function mortar_views_post_execute(&$view) {
    
    global $user;
    global $pager_total_items;
    
    #debug(count($view -> result));
    
    $account = user_load($user -> uid);
    
    if($node_load = menu_get_object()) {
        $node = node_load($node_load -> nid);
    }
    
    
    if($view -> name == 'training_record') {
        mortar_training_record_view_alter($view, $account, $node = NULL, $pager_total_items);
    }
    
    global $params;
    
    #debug($params);
    
    #debug($pager_total_items);
    
    #debug(count($view -> result));
    #$view->display_handler->set_option('items_per_page',2);
    
    $view->query->pager->total_items = $pager_total_items[0];
    #$view->query->pager->options['items_per_page'] = 2;
    $view->query->pager->update_page_info();
    
    #$view_raw = views_get_view_result('training_record');
        
        #$view_raw -> init_display();
        #$view_raw -> preview();
        #$view_raw -> is_cacheable = FALSE;
        #$view_raw -> pre_execute();
        #$view_raw -> post_execute();
        #$view_raw -> execute();
        
    
    /*
    $pager_total_items[0] = 7;
    debug($pager_total);
    debug($pager_page_array);
    $view->query->pager->update_page_info();
    debug(count($view -> result));
    $view->query->pager->options['quantity'] = '8';
    $view->query->pager->total_items = 9;
    $view->query->pager->update_page_info(); // update the pager object witht the correct number of items
    debug($view->query->pager->options);
    
    $view_results = $view -> result;
    
    $view->query->pager->total_items = 9;// reset the count of items
    $view->query->pager->update_page_info(); // update the pager object witht the correct number of items
    $view->result = $view_results; // replace the old results with the results without duplicates
    debug($view->query->pager->total_items);
    */
}


function mortar_training_record_view_alter(&$view, $account, $node, &$pager_total_items){
    
    
    
    foreach ($view -> result as $key => &$result) {
        #debug($result -> _field_data['nid']['entity']);
        #$result_data = $result -> _field_data['nid']['entity'];
        #debug($result -> field_field_ctu);
        if(!isset($result -> field_field_ctu[0])) {
            #unset($view -> result[$key]);
        }  
    } 
}