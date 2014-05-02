<?php
function private_msg_messages_menu(){
  
    $output = '<div class="messages-menus">'."<p class='inbox-link'>".
                 l("inbox","user-inbox/messages")."</p><p class='sent-link'>".
                 l("sent","user-sent/messages")."</p><p class='draft-link'>".
                 l("Drafts","user-draft/messages")."</p><p class='favorite-link'>".
                 l("Favorites","user-favorite/messages")."</p><p class='write-link'>".  
                 l("write new messages","user-write/new/messages",array("attributes"=>array("title"=>"Compose")))."</p>".  
          '</div>';
  
  return $output;
}

function private_msg_sent_messages_form(){
    
      $output['form_two'] = drupal_get_form('private_msg_inboxpages_form');
  
  return $output;
}


//==================== Send Messages form ============================================//

function private_msg_inboxpages_form($form, &$form_state){
    
    $form['#attributes'] = array('enctype' => "multipart/form-data");
  
    $form['username'] = array(
    '#id' => drupal_html_id('user_search_message'),   
    '#type' => 'textfield',
    '#maxlength' => 255,
    '#required' => TRUE,
    '#autocomplete_path' => 'customize/user/autocomplete',
    '#description' => t('Enter comma(,) seperated name for multiple users.'),
    '#attributes' => array(
          'class' => array('Messages'),
          'placeholder' => array('User name')),    
  );
  
  $form['subject'] = array(
        '#type' => 'textfield',
        '#required' => TRUE,
        '#attributes' => array(
            'class' => array('subject'),
            'placeholder' => array('Subject')),
  );  
    
  $form['messages'] = array(
        '#type' => 'textarea',
        '#required' => TRUE,
        '#attributes' => array(
          'class' => array('Messages'),
          'placeholder' => array('Messages')),
      );
    
  $form['uploadfiles'] = array(
     '#type' => 'file', 
     '#title' => t('File Upload'), 
    );
	 
  $form['actions']['send'] = array(
        '#type' => 'submit', 
        '#value' => t('Send'),
        '#submit' => array('custom_submit_send_button'),
        );
  
   $form['actions']['draft'] = array(
        '#type' => 'submit', 
        '#value' => t('Save as Draft'),
        '#submit' => array('custom_submit_draft_button'),
        );
   
   return $form;
    
}

//==================== Send Submiit button code ===================================//


function custom_submit_send_button($form, &$form_state){
    
    global $user;

    $file = file_save_upload("uploadfiles", array() ,'public://',FILE_EXISTS_RENAME);
   
        $messages = check_plain($form_state['values']['messages']);
        $subject = check_plain($form_state['values']['subject']);
         
         //========== Store Sender information for "{private_msg_inbox_data}" =============//
     $username = $form_state['values']['username'];
      $username1 = explode(",",$username); 
      for($i=0;$i<count($username1);$i++)
      {
          
        @$username2 = explode("(",$username1[$i]);
        @$username3 = explode(")",$username2[1]);
        $userdata[] = user_load_by_name($username3[0]);
      }   

        if(@$userdata[0]->name){
           try{
                db_insert('private_msg_inbox_data')
                    ->fields(array(
                        'sent_uid' => $user->uid,
                        'is_delete' => 0, 
                        'mess_subject' => $subject, 
                        'messages' => $messages, 
                        'mess_type' => 1,     // 1 =sent, 2= draft , form inbox use another table 
                        'mess_file' => (@$file->uri)?@$file->uri:'',     
                        'draft_users' => 0,    
                        'sent_date' => date("Y-m-d h:m:s"),    
                        'draft_date' => 0,)    
                        )
                    ->execute();
               }
                catch(Exception $e){
                    drupal_set_message($e);
                }
        try{
         $last_id = db_query('SELECT MAX(mid) FROM {private_msg_inbox_data}')->fetchField();
        } catch(Exception $e){
                    drupal_set_message($e);
        }
      for($i=0;$i<count($username1);$i++)
      {
        if(@$userdata[$i]->name){
           
            //========== Store inbox messages information for "{private_msg_inbox_userdata}" =============//
            try{
                db_insert('private_msg_inbox_userdata')
                    ->fields(array(
                        'mid' => $last_id,
                        'recv_uid' => $userdata[$i]->uid, 
                        'is_delete' => 0, 
                        'mess_read' => 0,)    
                        )
                    ->execute();
                 //=============== send  mail after insert record ===============//
                 _custom_rss_contact_mail_send($userdata[$i]->uid,$subject,$message); // global api message send make in custom_rss_feed.module file.                 
                }
                catch(Exception $e){
                    drupal_set_message($e);
                }
             }
          }
       drupal_set_message(t("your message has been successfully sent."));
       }
       else{
           drupal_set_message(t("choose valid user for sent messages"),"error");
      }
      
}

function custom_submit_draft_button($form, &$form_state){
    
 global $user;
    $userdata1 = '';
        $messages = check_plain($form_state['values']['messages']);
        $subject = check_plain($form_state['values']['subject']);
        
        $file = file_save_upload("uploadfiles", array() ,'public://',FILE_EXISTS_RENAME); 
        
        $username = $form_state['values']['username'];
      $username1 = explode(",",$username); 
      for($i=0;$i<count($username1);$i++)
      {
          
        $username2 = explode("(",$username1[$i]);
        @$username3 = explode(")",$username2[1]);
        $userdata = user_load_by_name($username3[0]);
        
        if($userdata->name){
            $userdata1 .= $userdata->uid.",";
            $userdata2[] = $userdata->uid;
        }
      } 
         //========== Store Sender Draft information for "{private_msg_inbox_data}" =============//
      if(@$userdata2){
            db_insert('private_msg_inbox_data')
                ->fields(array(
                    'sent_uid' => $user->uid,
                    'is_delete' => 0, 
                    'mess_subject' => $subject, 
                    'messages' => $messages, 
                    'mess_type' => 2,     // 1 =sent, 2= draft , form inbox use another table 
                    'mess_file' => (@$file->uri)?@$file->uri:'',     
                    'draft_users' => $userdata1,    
                    'sent_date' => 0,    
                    'draft_date' => date("Y-m-d h:m:s"),)    
                     )
                ->execute(); 
        drupal_set_message(t("your message has been successfully Saved."));   
      }
      else{
           drupal_set_message(t("choose valid user for Draft messages"),"error"); 
      }
}

//======================== Search Messages Form ========================================//

function private_msg_search_messages_form() {

if(arg(0) == "user-inbox"){ $searchform = drupal_get_form('private_msg_search_form_inbox'); }
if(arg(0) == "user-sent") { $searchform = drupal_get_form('private_msg_search_form_sent'); }
if(arg(0) == "user-draft"){ $searchform = drupal_get_form('private_msg_search_form_draft'); }
if(arg(0) == "user-favorite"){ $searchform = drupal_get_form('private_msg_search_form_favorite'); }
if(@$_GET['search']){     
    if(arg(0) == "user-inbox-message"){ $searchform = drupal_get_form('private_msg_search_form_inbox'); }
    if(arg(0) == "user-sent-message") { $searchform = drupal_get_form('private_msg_search_form_sent'); }
    if(arg(0) == "user-draft-message"){ $searchform = drupal_get_form('private_msg_search_form_draft'); }
    if(arg(0) == "user-favorite-message"){ $searchform = drupal_get_form('private_msg_search_form_favorite'); }
}

$output = drupal_render($searchform);
  return $output;
}

function private_msg_search_form_inbox($form, &$form_state){
    global $base_url;
    
    $form['#method'] = 'get';
    $form['#action'] = $base_url.'/user-inbox-message/results';
    $form['search'] = array(
    '#type' => 'textfield',
    '#size' => 20,
    '#default_value' => @$_GET['search'],   
    '#attributes'=>array(
      'class' => array('mymod-search'),
      'placeholder' => array('Search'),
      'autocomplete' => 'off')
  );
  return $form;
}

function private_msg_search_form_sent($form, &$form_state){
    global $base_url;
    
    $form['#method'] = 'get';
    $form['#action'] = $base_url.'/user-sent-message/results';
    $form['search'] = array(
    '#type' => 'textfield',
    '#size' => 20,
    '#default_value' => @$_GET['search'],
    '#attributes'=>array(
      'class' => array('mymod-search'),
      'placeholder' => array('Search'),
      'autocomplete' => 'off')
  );
  return $form;
}

function private_msg_search_form_draft($form, &$form_state){
    global $base_url;
    
    $form['#method'] = 'get';
    $form['#action'] = $base_url.'/user-draft-message/results';
    $form['search'] = array(
    '#type' => 'textfield',
    '#size' => 20,
    '#default_value' => @$_GET['search'],
    '#attributes'=>array(
      'class' => array('mymod-search'),
      'placeholder' => array('Search'),
      'autocomplete' => 'off')
  );
  return $form;
}

function private_msg_search_form_favorite($form, &$form_state){
    global $base_url;
    
    $form['#method'] = 'get';
    $form['#action'] = $base_url.'/user-favorite-message/results';
    $form['search'] = array(
    '#type' => 'textfield',
    '#size' => 20,
    '#default_value' => @$_GET['search'],   
    '#attributes'=>array(
      'class' => array('mymod-search'),
      'placeholder' => array('Search'),
      'autocomplete' => 'off')
  );
  return $form;
}