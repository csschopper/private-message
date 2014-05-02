<?php
function private_msg_inboxsearch_pages(){
    
    global $user;
    
    if(@$_GET['search']){
         $output['form_one'] = drupal_get_form('private_msg_inboxsearchmessages_form');
           return $output;
   }
   else{
         drupal_goto("user-inbox/messages");
   }
}
function private_msg_inboxsearchmessages_form($form, &$form_state){
    
    $searchdata = $_GET['search'];          
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
                          jQuery(".inbox-link a").addClass( "active" );  
                        });
                        
jQuery("#edit-submit").click(function() {
  var count = jQuery("input[class*=\'case\']:checked").length;
  if(count==0) {
    alert("Please Select the Messages for delete .");
    return false;
  }
  else
   return true;
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
    
    $form['my_field1'] = array(
            '#markup' => '<div class="default1">'. t('Inbox Messages') ."</div>",
            );
    
    $query = db_query("SELECT DISTINCT(pmid.mid), pmid.*  FROM {private_msg_inbox_data} pmid JOIN {private_msg_inbox_userdata} as pmiu on pmid.mid = pmiu.mid  Where (pmid.mess_subject like LOWER(:mess_subject) or pmid.messages like LOWER(:messages)) and (pmiu.is_delete=:is_delete1 and pmiu.recv_uid=:recv_uid) ORDER BY pmid.mid DESC",array(":mess_subject"=>'%'.db_like(strtolower($searchdata)).'%',":messages"=>'%'.db_like(strtolower($searchdata)).'%',":is_delete1"=>0,":recv_uid"=>$user->uid));
           
          $query1 = $query->fetchAll();
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
                    "<div class='mess-sub'>".l(t($query1[$i]->mess_subject),"user-view/messages/".$query1[$i]->mid."/inbox")."</div>".
                    "<div class='mess-body'>".l(views_trim_text($alter,t($query1[$i]->messages)),"user-view/messages/".$query1[$i]->mid."/inbox")."</div>"
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
   
   
  $form['#submit'][] = 'private_msg_inboxsearchmessages_form_submit';
  
    return $form;
}
function private_msg_inboxsearchmessages_form_submit($form, &$form_state){
    
  global $user;
  
  foreach($_POST as $key =>$value){
     $value1 = substr(strrchr($key, "copy_"), 0);
     if($value1 == $key){
         $deletedata[] = $value;
     }

  }
  if(isset($deletedata)){
    db_update('private_msg_inbox_userdata') // Table name no longer needs {}
                        ->fields(array('is_delete' => 1))
                        ->condition('mid', $deletedata, 'IN')
                        ->condition('recv_uid', $user->uid, '=')
                        ->execute();
    
    //drupal_flush_all_caches();
  }
  drupal_set_message(t("your message has been successfully Deleted.")); 
  drupal_goto("user-inbox/messages");
}

//========================  Sent messages search form page ===========================================//

function private_msg_sentsearch_pages(){
        
    if(@$_GET['search']){
         $output['form_one'] = drupal_get_form('private_msg_sentsearchmessages_form');
           return $output;
   }
   else{
       drupal_goto("user-sent/messages");
   }
}

function private_msg_sentsearchmessages_form($form, &$form_state){
 
    $searchdata = $_GET['search'];          
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
                            jQuery(".sent-link a").addClass( "active" );
                        });
                        
jQuery("#edit-submit").click(function() {
  var count = jQuery("input[class*=\'case\']:checked").length;
  if(count==0) {
    alert("Please Select the Messages for delete .");
    return false;
  }
  else
   return true;
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
    
        $form['my_field1'] = array(
            '#markup' => '<div class="default1">'. t('Sent Messages') ."</div>",
            );
        
    $query = db_query("SELECT DISTINCT(pmid.mid), pmid.*  FROM {private_msg_inbox_data} pmid Where (pmid.mess_subject like LOWER(:mess_subject) or pmid.messages like LOWER(:messages)) and (pmid.is_delete=:is_delete1 and pmid.sent_uid=:sent_uid and pmid.mess_type =:mess_type) ORDER BY pmid.mid DESC",array(":mess_subject"=>'%'.db_like(strtolower($searchdata)).'%',":messages"=>'%'.db_like(strtolower($searchdata)).'%',":is_delete1"=>0,":sent_uid"=>$user->uid,":mess_type"=>1));
    
    $query1 = $query->fetchAll();
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
                    "<div class='mess-sub'>".l(t($query1[$i]->mess_subject),"user-view/messages/".$query1[$i]->mid."/sent")."</div>".
                    "<div class='mess-body'>".l(views_trim_text($alter,t($query1[$i]->messages)),"user-view/messages/".$query1[$i]->mid."/sent")."</div>"
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
   
   
  $form['#submit'][] = 'private_msg_sentsearchmessages_form_submit';
  
    return $form;
}
  
function private_msg_sentsearchmessages_form_submit($form, &$form_state){
    
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
                        ->execute();
    
  //  drupal_flush_all_caches();
    drupal_set_message(t("your message has been successfully Deleted.")); 
   }
  drupal_goto("user-sent/messages");
}

//========================  Draft messages search form page ===========================================//

function private_msg_draftsearch_pages(){
   
   if(@$_GET['search']){
         $output['form_one'] = drupal_get_form('private_msg_draftsearchmessages_form');
           return $output;
   }
   else{
     drupal_goto("user-draft/messages");
   }
}

function private_msg_draftsearchmessages_form($form, &$form_state){
 
    $searchdata = $_GET['search'];
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
                            jQuery(".draft-link a").addClass( "active" );
                        });
                        
jQuery("#edit-submit").click(function() {
  var count = jQuery("input[class*=\'case\']:checked").length;
  if(count==0) {
    alert("Please Select the Messages for delete .");
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
 
        $form['my_field1'] = array(
            '#markup' => '<div class="default1">'. t('Draft Messages') ."</div>",
            );
        
  $query = db_query("SELECT DISTINCT(pmid.mid), pmid.*  FROM {private_msg_inbox_data} pmid Where (pmid.mess_subject like LOWER(:mess_subject) or pmid.messages like LOWER(:messages)) and (pmid.is_delete=:is_delete1 and pmid.sent_uid=:sent_uid and pmid.mess_type =:mess_type) ORDER BY pmid.mid DESC",array(":mess_subject"=>'%'.db_like(strtolower($searchdata)).'%',":messages"=>'%'.db_like(strtolower($searchdata)).'%',":is_delete1"=>0,":sent_uid"=>$user->uid,":mess_type"=>2));
   
      $query1 = $query->fetchAll();
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
   
   
  $form['#submit'][] = 'private_msg_draftsearchmessages_form_submit';
  
    return $form;
}
  
function private_msg_draftsearchmessages_form_submit($form, &$form_state){
    
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

function private_msg_favoritesearch_pages(){
    
     if(@$_GET['search']){
         $output['form_one'] = drupal_get_form('private_msg_favoritesearchmessages_form');
           return $output;
   }
   else{
     drupal_goto("user-draft/messages");
   }
}

function private_msg_favoritesearchmessages_form($form, &$form_state){
 
    $searchdata = $_GET['search'];
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
                            jQuery(".favorite-link a").addClass( "active" );
                        });
                        
jQuery("#edit-submit").click(function() {
  var count = jQuery("input[class*=\'case\']:checked").length;
  if(count==0) {
    alert("Please Select the Messages for delete .");
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
 
        $form['my_field1'] = array(
            '#markup' => '<div class="default1">'. t('Favorite Messages') ."</div>",
            );
        
  $query = db_query("SELECT DISTINCT(pmid.mid), pmid.*  FROM {private_msg_inbox_data} pmid JOIN {private_msg_inbox_userdata} as pmiu on pmid.mid = pmiu.mid  Where (pmid.mess_subject like LOWER(:mess_subject) or pmid.messages like LOWER(:messages)) and (pmiu.is_delete=:is_delete1 and pmiu.recv_uid=:recv_uid and pmiu.mess_read=:mess_read) ORDER BY pmid.mid DESC",array(":mess_subject"=>'%'.db_like(strtolower($searchdata)).'%',":messages"=>'%'.db_like(strtolower($searchdata)).'%',":is_delete1"=>0,":recv_uid"=>$user->uid, ":mess_read"=> 1 ));
   
      $query1 = $query->fetchAll();
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
   
   
  $form['#submit'][] = 'private_msg_favioritesearchmessages_form_submit';
  
    return $form;
}

function private_msg_favioritesearchmessages_form_submit($form, &$form_state){
             global $user;
  
  foreach($_POST as $key =>$value){
     $value1 = substr(strrchr($key, "copy_"), 0);
     if($value1 == $key){
         $deletedata[] = $value;
     }

  }
  if(isset($deletedata)){
   db_update('private_msg_inbox_userdata') // Table name no longer needs {}
                        ->fields(array('is_delete' => 1))
                        ->condition('mid', $deletedata, 'IN')
                        ->condition('recv_uid', $user->uid, '=')
                        ->condition('mess_read', 1, '=')
                        ->execute();
    
   // drupal_flush_all_caches();
        drupal_set_message(t("your message has been successfully Deleted.")); 
  }
  drupal_goto("user-favorite/messages");
}