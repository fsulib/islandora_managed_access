<?php

/**
 * @file
 * Admin form for the islandora_managed_access.module
 */

/**
 * MA Profile form
 */
function islandora_managed_access_profile_form($form, &$form_state, $profile = NULL) {
  $form['hname'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => isset($profile->hname) ? $profile->hname : '',
    '#description' => t('Human-readable name'),
    '#required' => TRUE,
  );
  $form['mname'] = array(
    '#title' => t('Machine Name'),
    '#type' => 'textfield',
    '#default_value' => isset($profile->mname) ? $profile->mname : '',
    '#description' => t('Machine-readable name to be used as a semantic ID.'),
    '#required' => TRUE,
    '#disabled' => isset($profile->id) ? TRUE : FALSE,
  );
  $form['role_name'] = array(
    '#title' => t('Associated Role'),
    '#type' => 'textfield',
    '#default_value' => isset($profile->rid) ? user_role_load($profile->rid)->name : '',
    '#description' => t('Drupal User Role associated with this profile.'),
    '#required' => TRUE,
    '#disabled' => isset($profile->id) ? TRUE : FALSE,
  );
  $form['lifetime'] = array(
    '#title' => t('Access Lifetime'),
    '#type' => 'textfield',
    '#default_value' => isset($profile->lifetime) ? $profile->lifetime : '',
    '#description' => t('The number of days before a user loses access. Set to 0 for no expiration.'),
    '#required' => TRUE,
  );
  $form['ip_whitelist'] = array(
    '#title' => t('IP Whitelist'),
    '#type' => 'textfield',
    '#default_value' => isset($profile->ip_whitelist) ? $profile->ip_whitelist : '',
    '#description' => t('A comma-separated list of IPs or IP ranges that should have access to the object.'),
    '#required' => FALSE,
  );
  $form['user_active_by_default'] = array(
    '#title' => t('User active by default?'),
    '#type' => 'checkbox',
    '#default_value' => isset($profile->user_active_by_default) ? $profile->user_active_by_default : FALSE,
    '#description' => t('Check if you want users who register for objects managed by this profile to have user accounts created automatically.'),
    '#required' => FALSE,
  );
  $form['justification_info'] = array(
    '#title' => t('Justification Information'),
    '#type' => 'textarea',
    '#default_value' => isset($profile->justification_info) ? $profile->justification_info : '',
    '#description' => t('Information displayed to the user about why the object has managed access.'),
    '#required' => TRUE,
  );
  $form['usage_info'] = array(
    '#title' => t('Usage Information'),
    '#type' => 'textarea',
    '#default_value' => isset($profile->usage_info) ? $profile->usage_info : '',
    '#description' => t('Information displayed to the user about the terms of use for the object.'),
    '#required' => TRUE,
  );
  $form['contact_info'] = array(
    '#title' => t('Contact Information'),
    '#type' => 'textarea',
    '#default_value' => isset($profile->contact_info) ? $profile->contact_info : '',
    '#description' => t('Information displayed to the user about who to contact if they have questions.'),
    '#required' => TRUE,
  );
  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => isset($profile->id) ? t('Update profile') : t('Save profile'),
    ),
  );
  return $form;
}

function islandora_managed_access_profile_form_validate($form, &$form_state) {
  if (isset($form_state['values']['lifetime']) && !is_numeric($form_state['values']['lifetime'])) {
    form_set_error('lifetime', t('Lifetime field must be an integer value.'));
  }
  if (isset($form_state['values']['ip_whitelist']) && $form_state['values']['ip_whitelist'] != "") {
    $ips = explode(',', str_replace(' ', '', $form_state['values']['ip_whitelist']));
    foreach ($ips as $ip) {
      if (strpos($ip, "/")) {
        list($net, $mask) = explode("/", $ip); 
        if (!filter_var($net, FILTER_VALIDATE_IP)) {
          form_set_error('ip_whitelist', "\"{$net}\" in \"{$ip}\" is not a valid IP address.");
        }
        if (!filter_var($mask, FILTER_VALIDATE_INT)) {
          form_set_error('ip_whitelist', "\"{$mask}\" in \"{$ip}\" is not a valid range.");
        }
      }
      else {
        if (!filter_var($ip, FILTER_VALIDATE_IP)) {
          form_set_error('ip_whitelist', "\"{$ip}\" is not a valid IP address.");
        }
      }
    }
  }
}

function islandora_managed_access_profile_form_submit($form, &$form_state) {
  $profile = entity_ui_form_submit_build_entity($form, $form_state);
  $old_role = isset($profile->rid) ? user_role_load($profile->rid) : '';
  if (empty($old_role)) {
    $new_role = new stdClass();
    $new_role->name = $form['role_name']['#value'];
    user_role_save($new_role);
    $profile->rid = user_role_load_by_name($form['role_name']['#value'])->rid;
  } 
  $profile->save();
  drupal_set_message(t('The "@hname" profile has been saved.', array('@hname' => $profile->hname)));
  $form_state['redirect'] = 'admin/islandora/tools/managed_access/profiles';
}

/**
 * MA Object form
 */
function islandora_managed_access_object_form($form, &$form_state, $object = NULL) {
  $profiles = get_islandora_managed_access_profiles();
  if (count($profiles) == 0) {
    drupal_set_message("No managed access profiles detected. At least one managed access profile is required to create a managed access object.", "error");
    return array();
  }
  else {
    $form['pid'] = array(
      '#title' => t('PID'),
      '#type' => 'textfield',
      '#default_value' => isset($object->pid) ? $object->pid : '',
      '#description' => t('PID of the Islandora object to be managed.'),
      '#required' => TRUE,
      '#disabled' => isset($object->id) ? TRUE : FALSE,
    );
    $form['profile'] = array(
      '#title' => t('Associated Profile'),
      '#type' => 'select',
      '#options' => $profiles, 
      '#default_value' => isset($object->profile) ? $object->profile : '',
      '#description' => t('Name of the Managed Access Profile associated with this object.'),
      '#required' => TRUE,
      '#disabled' => isset($object->id) ? TRUE : FALSE,
    );
    $form['pid_whitelist'] = array(
      '#title' => t('PID Whitelist'),
      '#type' => 'textfield',
      '#default_value' => isset($object->pid_whitelist) ? $object->pid_whitelist : '',
      '#description' => t('If object is hierarchichal, specify PIDs that should NOT be managed in a comma-separated list.'),
      '#required' => FALSE,
      '#disabled' => isset($object->id) ? TRUE : FALSE,
    );
    $form['actions'] = array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => isset($object->id) ? t('Update object') : t('Save object'),
      ),
    );
    return $form;
  }
}

function islandora_managed_access_object_form_validate($form, &$form_state) {
  if (isset($form_state['values']['pid']) && !islandora_object_load($form_state['values']['pid'])) {
    form_set_error('pid', "\"{$form_state['values']['pid']}\" is not a valid PID.");
  }
  if (isset($form_state['values']['pid_whitelist']) && $form_state['values']['pid_whitelist'] != "") {
    $pids = explode(',', str_replace(' ', '', $form_state['values']['pid_whitelist']));
    foreach ($pids as $pid) {
      if (!islandora_object_load($pid)) {
        form_set_error('pid_whitelist', "\"{$pid}\" is not a valid PID.");
      }
    }
  }
}

function islandora_managed_access_object_form_submit($form, &$form_state) {
  $object = entity_ui_form_submit_build_entity($form, $form_state);
  $object->save();
  drupal_set_message(t('@pid has been saved.', array('@pid' => $object->pid)));
  $form_state['redirect'] = 'admin/islandora/tools/managed_access/objects';
}

/**
 * MA User form
 */
function islandora_managed_access_user_form($form, &$form_state, $user = NULL) {
  $profiles = get_islandora_managed_access_profiles();
  if (count($profiles) == 0) {
    drupal_set_message("No managed access profiles detected. At least one managed access profile is required to create a managed access user.", "error");
    return array();
  }
  else {
    $refreshed_date = isset($user->refreshed_at) ? date('M d, Y', $user->refreshed_at) : FALSE;
    $form['refresh_display'] = array(
      '#markup' => isset($user->refreshed_at) ? "<p>Last refreshed at: {$refreshed_date}</p>" : '',
    );
    $form['name'] = array(
      '#title' => t('Name'),
      '#type' => 'textfield',
      '#default_value' => isset($user->name) ? $user->name : '',
      '#required' => TRUE,
      '#disabled' => isset($user->id) ? TRUE : FALSE,
    );
    $form['email'] = array(
      '#title' => t('Email'),
      '#type' => 'textfield',
      '#default_value' => isset($user->uid) ? user_load($user->uid)->mail : '',
      '#required' => TRUE,
      '#disabled' => isset($user->id) ? TRUE : FALSE,
    );
    $form['profile'] = array(
      '#title' => t('Associated Profile'),
      '#description' => t('Name of the Managed Access Profile associated with this user.'),
      '#type' => 'select',
      '#options' => $profiles, 
      '#default_value' => isset($object->profile) ? $object->profile : '',
      '#required' => TRUE,
      '#disabled' => isset($user->id) ? TRUE : FALSE,
    );
    $form['active'] = array(
      '#title' => t('Active?'),
      '#description' => t('Whether or not the user currently has access to relevant managed access objects.'),
      '#type' => 'checkbox',
      '#default_value' => isset($user->active) ? $user->active : FALSE,
    );
    $form['institution'] = array(
      '#title' => t('Affiliated Institution'),
      '#description' => t('Name of organization that user is from.'),
      '#type' => 'textfield',
      '#default_value' => isset($user->institution) ? $user->institution : '',
      '#required' => TRUE,
    );
    $form['phone'] = array(
      '#title' => t('Phone Number'),
      '#description' => t('Full phone number where you can be contacted in \'XXX-XXX-XXXX\' format.'),
      '#type' => 'textfield',
      '#default_value' => isset($user->phone) ? $user->phone : '',
      '#required' => TRUE,
    );
    $form['address'] = array(
      '#title' => t('Physical Address'),
      '#description' => t('A mailing address (include city and state) where you can be contacted.'),
      '#type' => 'textfield',
      '#default_value' => isset($user->address) ? $user->address : '',
      '#required' => TRUE,
    );
    $form['request_info'] = array(
      '#title' => t('Request Information'),
      '#description' => t('Reason why user wants to access managed access objects.'),
      '#type' => 'textarea',
      '#default_value' => isset($user->request_info) ? $user->request_info : '',
      '#required' => TRUE,
    );
    $form['hidden_id'] = array(
      '#type' => 'hidden',
      '#value' => isset($user->id) ? $user->id : FALSE,
    );
    $form['actions'] = array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => isset($user->id) ? t('Update user') : t('Save user'),
      ),
    );
    return $form;
  }
}

function islandora_managed_access_user_form_validate($form, &$form_state) {
  if (isset($form_state['values']['email']) && !valid_email_address($form_state['values']['email'])) {
    form_set_error('email', "\"{$form_state['values']['email']}\" is not a valid email address.");
  }
  if (isset($form_state['values']['phone']) && !preg_match('/^[0-9]{3}-[0-9]{3}-[0-9]{4}$/', $form_state['values']['phone'])) {
    form_set_error('phone', "\"{$form_state['values']['phone']}\" is not in 'XXX-XXX-XXXX' format.");
  }
}

function islandora_managed_access_user_form_submit($form, &$form_state) {
  if ($form['hidden_id']['#value'] != FALSE) {
    $old_user_entity = entity_load("islandora_managed_access_user", array($form['hidden_id']['#value']));
    $old_activity_value = $old_user_entity["{$form['hidden_id']['#value']}"]->active; 
    $new_activity_value = $form['active']['#value']; 
    if ($old_activity_value != $new_activity_value) {
      if ($old_activity_value == 1 && $new_activity_value == 0) {
        $activity_delta = "deactivated";
      }
      if ($old_activity_value == 0 && $new_activity_value == 1) {
        $activity_delta = "activated";
      }
    }
  }

  $user_entity = entity_ui_form_submit_build_entity($form, $form_state);
  if (!isset($user_entity->id)) { 
    $profile_id = $form['profile']['#value'];
    $profile_entity = entity_load('islandora_managed_access_profile', array($profile_id)); 
    IslandoraManagedAccessUserEntityController::save_new_ma_user($user_entity, $profile_entity[$profile_id], $form['email']['#value']);
  } 
  else {
    $user_entity->refreshed_at = time();
    $user_entity->save();
  }

  if (isset($activity_delta)) {
    if ($activity_delta == 'activated') {
      IslandoraManagedAccessUserEntityController::activate_ma_user($user_entity);
    }
    if ($activity_delta == 'deactivated') {
      IslandoraManagedAccessUserEntityController::deactivate_ma_user($user_entity);
    }
  }

  drupal_set_message(t('@name has been saved.', array('@name' => $user_entity->name)));
  $form_state['redirect'] = 'admin/islandora/tools/managed_access/users';
}
