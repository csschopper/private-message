<?php
function private_msg_sentpages(){
  $output = array();
  
   // Messages Menu  block are displaying throught Block and check block list page in admin section. 
  
  $output['form_one'] = drupal_get_form('private_msg_sentmessages_form');
  return $output;
}

//==================== Send Messages form ============================================//
    
function private_msg_sentmessages_form($form, &$form_state){
 
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
 
  //   $query = db_query("SELECT pmid.*   FROM {private_msg_inbox_data} as  pmid Where  pmid.sent_uid=:sent_uid AND pmid.is_delete=:is_delete ORDER BY pmid.mid DESC",array(":sent_uid"=>$user->uid,":is_delete"=>0));
    
    $query = db_select("private_msg_inbox_data", 'pmid');
    $query->fields('pmid', array("mid",'sent_uid',"mess_subject","messages","mess_type","mess_file","is_delete"));
    $query->condition('pmid.sent_uid', $user->uid, '=');
    $query->condition('pmid.is_delete', 0, '=');
     $query->condition('pmid.mess_type', 1, '=');
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
   
   
  $form['#submit'][] = 'private_msg_sentmessages_form_submit';
      $form['pager'] = array('#markup' => theme('pager'));
    return $form;
}
  
function private_msg_sentmessages_form_submit($form, &$form_state){
    
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