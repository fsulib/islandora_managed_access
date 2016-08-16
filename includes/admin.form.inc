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
  );
  $form['role'] = array(
    '#title' => t('Associated Role'),
    '#type' => 'textfield',
    '#default_value' => isset($profile->role) ? $profile->role : '',
    '#description' => t('Drupal User Role associated with this profile.'),
    '#required' => TRUE,
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
}

function islandora_managed_access_profile_form_submit($form, &$form_state) {
  $profile = entity_ui_form_submit_build_entity($form, $form_state);
  if (!isset($profile->id)) { 
    $profile->created_at = time();
  }
  $profile->updated_at = time();
  $profile->save();
  drupal_set_message(t('The "@hname" profile has been saved.', array('@hname' => $profile->hname)));
  $form_state['redirect'] = 'admin/islandora/tools/managed_access/profiles';
}

/**
 * MA Object form
 */
function islandora_managed_access_object_form($form, &$form_state, $object = NULL) {
  $form['pid'] = array(
    '#title' => t('PID'),
    '#type' => 'textfield',
    '#default_value' => isset($object->pid) ? $object->pid : '',
    '#description' => t('PID of the Islandora object to be managed.'),
    '#required' => TRUE,
  );
  $form['profile'] = array(
    '#title' => t('Associated Profile'),
    // This should really be a select list, do this later
    '#type' => 'textfield',
    '#default_value' => isset($object->profile) ? $object->profile : '',
    '#description' => t('Machine Name of the Managed Access Profile associated with this object.'),
    '#required' => TRUE,
  );
  $form['pid_whitelist'] = array(
    '#title' => t('PID Whitelist'),
    '#type' => 'textfield',
    '#default_value' => isset($object->pid_whitelist) ? $object->pid_whitelist : '',
    '#description' => t('If object is hierarchichal, specify PIDs that should NOT be managed in a comma-separated list.'),
    '#required' => FALSE,
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

function islandora_managed_access_object_form_validate($form, &$form_state) {
  if (isset($form_state['values']['pid']) && !strpos($form_state['values']['pid'], ":")) {
    form_set_error('PID', t('PID has incorrect syntax.'));
  }
}

function islandora_managed_access_object_form_submit($form, &$form_state) {
  $object = entity_ui_form_submit_build_entity($form, $form_state);
  if (!isset($object->id)) { 
    $object->created_at = time();
  }
  $object->updated_at = time();
  $object->save();
  drupal_set_message(t('@pid has been saved.', array('@pid' => $object->pid)));
  $form_state['redirect'] = 'admin/islandora/tools/managed_access/objects';
}

/**
 * MA User form
 */
function islandora_managed_access_user_form($form, &$form_state, $user = NULL) {
  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => isset($user->name) ? $user->name : '',
    //'#description' => t('Full name.'),
    '#required' => TRUE,
  );
  $form['institution'] = array(
    '#title' => t('Affiliated Institution'),
    // This should really be a select list, do this later
    '#type' => 'textfield',
    '#default_value' => isset($user->institution) ? $user->institution : '',
    //'#description' => t('Machine Name of the Managed Access Profile associated with this object.'),
    '#required' => TRUE,
  );
  $form['request_info'] = array(
    '#title' => t('Request Information'),
    '#type' => 'textarea',
    '#default_value' => isset($user->request_info) ? $user->request_info : '',
    //'#description' => t('If object is hierarchichal, specify PIDs that should NOT be managed in a comma-separated list.'),
    '#required' => TRUE,
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

function islandora_managed_access_user_form_validate($form, &$form_state) {
  /*
  if (isset($form_state['values']['pid']) && !strpos($form_state['values']['pid'], ":")) {
    form_set_error('PID', t('PID has incorrect syntax.'));
  }
  */
}

function islandora_managed_access_user_form_submit($form, &$form_state) {
  $user = entity_ui_form_submit_build_entity($form, $form_state);
  if (!isset($user->id)) { 
    $user->created_at = time();

    // obvs a test, fix this later with the returned ID when creating a drupal user as a subprocess
    $user->uid = "TEST";
    $user->refreshed_at = "777";
  }
  $user->updated_at = time();
  $user->save();
  drupal_set_message(t('@name has been saved.', array('@name' => $user->name)));
  $form_state['redirect'] = 'admin/islandora/tools/managed_access/users';
}

