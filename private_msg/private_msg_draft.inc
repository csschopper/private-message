<?php
function private_msg_draftpages(){
      $output = array();
  
   // Messages Menu  block are displaying throught Block and check block list page in admin section. 
  
  $output['form_one'] = drupal_get_form('private_msg_draftmessages_form');
  return $output;
}

//==================== Send Messages form ============================================//
    
function private_msg_draftmessages_form($form, &$form_state){
 
    global $user;
    global $base_url;
    
    drupal_add_js('jQuery(function(){
                            jQuery("#selectall").click(function () {
                                jQuery(".case").attr("checked", this.checked);
                            });
                            jQuery(".case").click(function(){

                                if(jQuery(".case").length == jQuery(".case:checked").length) {
                                    jQuery("#selectall").attr("checked", "checked");
                                } else {
                                    jQuery("#selectall").removeAttr("checked");
                                }

                            });
                        });
                        
jQuery("#edit-submit").click(function() {
  var count = jQuery("input[class*=\'case\']:checked").length;
  if(count==0) {
    alert("Please Select the Messages for delete.");
    return false;
  }
 if(count != 0){
    if(confirm("Are you sure you want to delete these messages? Cannot be undone.")){
     return true;
    }else{
     return false;
    }
  }
  
});
', array('type' => 'inline','scope' => 'footer'));
  
       $form["copy"] = array(
          '#type' => 'checkbox',
           '#id'  => "selectall",
          '#return_value'  => "",
          '#title' => t(''),
        );
       
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Delete')
        );
 
  //   $query = db_query("SELECT pmid.*   FROM {private_msg_inbox_data} as  pmid Where  pmid.sent_uid=:sent_uid AND pmid.is_delete=:is_delete AND pmid.draft_users<>:draft_users ORDER BY pmid.mid DESC",array(":sent_uid"=>$user->uid,":is_delete"=>0,":draft_users"=>0));
    $query = db_select("private_msg_inbox_data", 'pmid');
    $query->fields('pmid', array("mid",'sent_uid',"mess_subject","messages","mess_type","mess_file","is_delete"));
    $query->condition('pmid.sent_uid', $user->uid, '=');
    $query->condition('pmid.is_delete', 0, '=');
    $query->condition('pmid.mess_type', 2, '=');
    $query->condition('pmid.draft_users', 0, '<>');
    $query->orderBy('pmid.mid', 'DESC');
    //dpq($query);
     $query = $query->extend('PagerDefault')->limit(20);
     $query2 = $query->execute();
     $query1 = $query2->fetchAll();
    
     $output = '';
    if(count($query1)){
        for($i=0;$i<count($query1);$i++)
            {
                    
               $userdata = (array) user_load($query1[$i]->sent_uid);
               $userimage = (array) $userdata['picture']->uri;
//              echo "<pre>";
//              print_r($userdata);
               if($userimage[0]){ 
                 $userimage1 = "<a href='".$base_url."/user/".$userdata['uid']."'><img src='" .  file_create_url($userimage[0]) . "' height='64' width='65'/></a>";}
                 else{
                  $userimage1 = "<a href='".$base_url."/user/".$userdata['uid']."'><img src='" . $base_url . "/sites/default/files/nouser.jpg' height='64' width='65'/></a>"; 
                  }
                   if (@$userdata['field_name_first']['und'][0]['value']) {
                    $username = "<a href='".$base_url."/user/".$userdata['uid']."'>".$userdata['field_name_first']['und'][0]['value'] . " " .$userdata['field_name_last']['und'][0]['value']."</a>";
                } else {
                    $username = "<a href='".$base_url."/user/".$userdata['uid']."'>".$userdata['name']."</a>";
                }
             $userdesig =   @$userdata['field_current_designation']['und'][0]['value'].",";
           $userdetax =  '<div class="curdesig italc">';
                     @$texodata = taxonomy_term_load(@$userdata['field_user_specialist']['und'][0]['tid']);
            $userdetax .= @$texodata->name."</div>";
                    $alter = array(
                        'max_length' => 410,
                        'html' => false,
                        'ellipsis' => TRUE,
                            );
                    
               $form["copy_".$query1[$i]->mid] = array(
                '#type' => 'checkbox',
                '#return_value'  => $query1[$i]->mid,
                '#prefix' => '<div class="single-inbox-messages">',
                '#attributes' => array(
                'class' => array('case')),
                 '#title' => '<div class="messages-part"><div class="umess-image">'.$userimage1                     .
                    "</div><div class='messages-content'>"
                     ."<div class='username'>".$username.", ".$userdesig."</div>"                            
                     .$userdetax.                            
                    "<div class='mess-sub'>".l(t($query1[$i]->mess_subject),"user-view/messages/".$query1[$i]->mid."/draft")."</div>".
                    "<div class='mess-body'>".l(views_trim_text($alter,t($query1[$i]->messages)),"user-view/messages/".$query1[$i]->mid."/draft")."</div>"
                   ."</div>"."</div>",
                 '#suffix' => '</div>',   
                );
               $userdata = '';
               $alter = '';
            }
    }else{
         $form['my_field'] = array(
            '#markup' => '<div class="default">'. t('You have no messages here') ."</div>",
            );
    }
   
   
  $form['#submit'][] = 'private_msg_draftmessages_form_submit';
    $form['pager'] = array('#markup' => theme('pager'));
    return $form;
}
  
function private_msg_draftmessages_form_submit($form, &$form_state){
    
  global $user;
  
  foreach($_POST as $key =>$value){
     $value1 = substr(strrchr($key, "copy_"), 0);
     if($value1 == $key){
         $deletedata[] = $value;
     }

  }
  if(isset($deletedata)){
    db_update('private_msg_inbox_data') // Table name no longer needs {}
                        ->fields(array('is_delete' => 1))
                        ->condition('mid', $deletedata, 'IN')
                        ->condition('sent_uid', $user->uid, '=')
                        ->condition('mess_type', 2, '=')
                        ->execute();
    
   // drupal_flush_all_caches();
        drupal_set_message(t("your message has been successfully Deleted.")); 
  }
  drupal_goto("user-draft/messages");
}

//======================= Write new Messages page code ===============================//

function private_msg_writenew(){
       $output['form_two'] = drupal_get_form('private_msg_inboxpages_form_sent');
       
       return $output;
}

function private_msg_inboxpages_form_sent($form, &$form_state){
     
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
        '#submit' => array('custom_submit_send_button_sent'),
        );
  
   $form['actions']['draft'] = array(
        '#type' => 'submit', 
        '#value' => t('Save as Draft'),
        '#submit' => array('custom_submit_draft_button_sent'),
        );
   
   return $form;
    
}

//==================== Send Submiit button code ===================================//

function custom_submit_send_button_sent($form, &$form_state){
    
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
        
         $last_id = db_query('SELECT MAX(mid) FROM {private_msg_inbox_data}')->fetchField();
      for($i=0;$i<count($username1);$i++)
      {
        if(@$userdata[$i]->name){
           
            //========== Store inbox messages information for "{private_msg_inbox_userdata}" =============//
            
            db_insert('private_msg_inbox_userdata')
                ->fields(array(
                    'mid' => $last_id,
                    'recv_uid' => $userdata[$i]->uid, 
                    'is_delete' => 0, 
                    'mess_read' => 0,)    
                     )
                ->execute();
             _custom_rss_contact_mail_send($userdata[$i]->uid,$subject,$messages);
             }
          }
       drupal_set_message(t("your message has been successfully sent."));
       }
       else{
           drupal_set_message(t("choose valid user for sent messages"),"error");
      }
      
}

function custom_submit_draft_button_sent($form, &$form_state){
    
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

//=====================================  Send Draft Messages ===================================//

function private_msg_senddraft($args){
    global $user;
    global $base_url;
         
    $query = db_query("SELECT *   FROM {private_msg_inbox_data} Where mid=:mid and sent_uid=:sent_uid AND is_delete=:is_delete AND draft_users <> :draft_users",array(":mid"=>$args,":sent_uid"=>$user->uid,":is_delete"=>0,":draft_users"=>0));
              $query1 = $query->fetchAll();
     $output = '';
    if(count($query1)){
       
      @$output1 = explode(",",$query1[0]->draft_users);
        
       for($i=0 ; $i<count($output1);$i++){
          //echo $output1[$i]."<br>";          
           $output2 = (int) $output1[$i];
          if($output2){
            db_insert('private_msg_inbox_userdata')
                ->fields(array('mid' => $query1[0]->mid, 'recv_uid' => $output2, 'is_delete' => 0, 'mess_read' => 0)) 
                ->execute();
           }
        }
       db_update('private_msg_inbox_data') // Table name no longer needs {}
                        ->fields(array('mess_type' => 1,'draft_users'=>0,'sent_date'=>date("Y-m-d h:m:s")))
                        ->condition('mid', $query1[0]->mid, '=')
                        ->condition('sent_uid', $user->uid, '=')
                        ->execute();
    
      //  drupal_flush_all_caches();
        drupal_set_message(t("your message has been successfully Sent.")); 
        drupal_goto("user-draft/messages");
        
    }else{
        //drupal_set_message(t("your message has been successfully Sent."));    
        drupal_goto("user-draft/messages");
    }
} 


//==============  Delete button on Draft send and inbox single messages form ===================================//

function private_msg_deletedraft($args, $args1){
    
    global $user;
    if($args1 == "inbox"){
        db_update('private_msg_inbox_userdata') // Table name no longer needs {}
                ->fields(array('is_delete' => 1))
                ->condition('mid', $args, '=')
                ->condition('recv_uid', $user->uid, '=')
                ->execute();
        
     //  drupal_flush_all_caches();    
    drupal_set_message(t("your message has been successfully Deleted.")); 
drupal_goto("user-inbox/messages");
    }
    
    if($args1 == "sent"){
           db_update('private_msg_inbox_data') // Table name no longer needs {}
                        ->fields(array('is_delete' => 1))
                        ->condition('mid', $args, '=')
                        ->condition('sent_uid', $user->uid, '=')
                        ->execute();
        
     //  drupal_flush_all_caches();    
    drupal_set_message(t("your message has been successfully Deleted.")); 
    drupal_goto("user-sent/messages");
    }
    if($args1 == "draft"){
            db_update('private_msg_inbox_data') // Table name no longer needs {}
                    ->fields(array('is_delete' => 1))
                    ->condition('mid', $args, '=')
                    ->condition('sent_uid', $user->uid, '=')
                    ->execute();
    
     //  drupal_flush_all_caches();    
    drupal_set_message(t("your message has been successfully Deleted.")); 
    drupal_goto("user-draft/messages");
    }
    else{
     drupal_set_message(t("kindly open a valid page."),"error"); 
    drupal_goto("user-draft/messages");
    }

}

