<?php

/*
 *
 */
function mortar_form_webform_client_form_alter(&$form, &$form_state) {
    $node = menu_get_object();
    
    #if($node -> type == 'controlled_document') {
    mortar_controlled_doc_test_webform_alter($form, $form_state, $node);
    #}
    $form['#node'] -> title = '';
    
}


/*
 *
 */
function mortar_controlled_doc_test_webform_alter(&$form, &$form_state, $node) {
    
    $answers = mortar_controlled_doc_test_webform_get_form_correct_answers($form, $form_state);
    
    $form['answers'] = array(
        '#type' => 'value',
        '#value' => $answers
    );
    
    $form['cd_node'] = array(
        '#type' => 'value',
        '#value' => $node
    );
    
    $form['#validate'][] = 'mortar_controlled_doc_test_evaluate';
    $form['#submit'][] = 'mortar_controlled_doc_test_redirect';
}


/*
 *
 */
function mortar_controlled_doc_test_redirect($form, &$form_state) {
        $cd_node = $form_state['values']['cd_node'];
        $form_state['redirect'] = 'node/' . $cd_node -> nid;
}


/*
 *
 */
function mortar_controlled_doc_test_evaluate($form, &$form_state) {
    $error = form_get_errors();
    if (!empty($error)){
	return;
    }
    
    $cd_node = $form_state['values']['cd_node'];
    $correct_answers = $form_state['values']['answers'];
    $user_answers = $form_state['values']['submitted'];
    $questions_title = $form_state['values']['questions_title'];
    $incorrect_answer = array();
    
    //check which question has been answered incorrectly
    foreach($correct_answers as $answer_key => $answer) {
        if(isset($user_answers[$answer_key])) {
            if(is_array($user_answers[$answer_key])) {
                $result = array_diff_assoc($correct_answers[$answer_key], $user_answers[$answer_key]);
                if(!empty($result)) {
                    $incorrect_answer[$answer_key] = $user_answers[$answer_key];
                }
            }
            else {
                if($user_answers[$answer_key] !== $correct_answers[$answer_key]) {
                    $incorrect_answer[$answer_key] = $user_answers[$answer_key];
                }
            }
        }
        else {
            $incorrect_answer[$answer_key] = $user_answers[$answer_key];
        }
    }
    
    $form_state['values']['submitted']['test_result'] = NULL;
    
    if(empty($incorrect_answer)) {
        drupal_set_message(t('Congratulations, you have passed the test. A training record will be added to your account.'), 'status');
        
        if($cd_node -> type == 'controlled_document') {
            mortar_training_record_controlled_doc_user_connect($form, $form_state, $form_id = NULL);    
        }
        $form_state['values']['submitted']['test_result'] = '1';
    }
    else {
        drupal_set_message(t('Sorry you have not yet achieved a 100% score on this test and therefore you will not receive a training record.  Please re-read the SOP and take the test again. The following questions have been answered incorrectly: <br /><br />'), 'warning');
        foreach($incorrect_answer as $inc_key => $inc_ans) {
           drupal_set_message(t($questions_title[$inc_key]), 'warning'); 
        }
        
        $form_state['values']['submitted']['test_result'] = '2';
    }
}


/*
 *
 */
function mortar_controlled_doc_test_webform_get_form_correct_answers(&$form, &$form_state) {
    
    $answers = array();
    
    foreach ($form['submitted'] as $key => &$item) {
        if(isset($item['#options']['answer'])){

            $questions_title[$item['#webform_component']['form_key']] = $item['#title'];
            
            if($item['#type'] == 'checkboxes') {
                $q_answers = explode(',',$item['#options']['answer']);
                foreach($item['#options'] as $option_key => $option) {
                    if($option_key != 'answer') {
                        $answers[$item['#webform_component']['form_key']][$option_key] = 0;
                    }
                }
                foreach($q_answers as $answer) {
                    if(isset($answers[$item['#webform_component']['form_key']][$answer])) {
                        $answers[$item['#webform_component']['form_key']][$answer] = $answer;    
                    }
                    
                }
            }
            else {
                $answers[$item['#webform_component']['form_key']] = $item['#options']['answer']; 
            }
            
            //show correct answers 
            if(arg(4) != 'edit') {
                unset($item['#options']['answer']);    
            }
        }
        if(arg(4) != 'edit') {
            if($key == 'test_result') {
                $form['submitted'][$key]['#access'] = FALSE;
            }
        }
        
    }
    
    $form['questions_title'] = array(
        '#type' => 'value',
        '#value' => $questions_title
    );
    
    return $answers;
}