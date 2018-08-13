<?php

/**
 * @file
 * Classes for the islandora_managed_access_object entity.
 */

/**
 * Entity class for MA Users
 */
class IslandoraManagedAccessUserEntity extends Entity {
}

/**
 * Entity Controller class for MA Users
 */
class IslandoraManagedAccessUserEntityController extends EntityAPIController {
  public static function save_new_ma_user($ma_user, $ma_profile, $email) {
    $role = user_role_load($ma_profile->rid);
    //$username = $ma_profile->mname . "_user_" . time() . "_" . rand(1000, 9999); 
    $username = str_replace(" ", "", $ma_user->name); 
    $username = str_replace(".", "", $username); 
    $username = $username . time(); 
    $password = uniqid() . rand(100000000000, 999999999999);
    $fields = array(
      'name' => $username,
      'mail' => $email,
      'pass' => $password,
      'status' => 0,
      'roles' => array(
        DRUPAL_AUTHENTICATED_RID => 'authenticated user',
        $role->rid => $role->name,
      ),
    );
    $account = user_save(NULL, $fields);
    $ma_user->uid = $account->uid;
    $ma_user->save();

    $params = array(
      'ma_user_id' => $ma_user->id,
    );

    watchdog("Islandora Managed Access", "Managed Access User '{$ma_user->name}' created.");
 
    $from = $ma_profile->contact_email;
    drupal_mail('islandora_managed_access', 'notify_new_user_admin', $ma_profile->contact_email, language_default(), $params, $from, TRUE);

    if ($ma_profile->user_active_by_default == 1) {
      IslandoraManagedAccessUserEntityController::activate_ma_user($ma_user);
    }
    else {
      drupal_mail('islandora_managed_access', 'notify_inactive_new_user', $email, language_default(), $params, $from, TRUE);
    }

    return array('ma_user' => $ma_user, 'drupal_user' => $account, 'password' => $password);
  }
  
  public static function activate_ma_user($ma_user) {
    watchdog("Islandora Managed Access", "Managed Access User '{$ma_user->name}' activated.");
    $drupal_user = user_load($ma_user->uid);
    user_save($drupal_user, array('status' => 1));
    $ma_user->active = 1;
    $params = array('ma_user_id' => $ma_user->id);
    $ma_profile = IslandoraManagedAccessUserEntityController::get_profile_of_ma_user($ma_user);
    drupal_mail('islandora_managed_access', 'notify_user_activated', $drupal_user->mail, language_default(), $params, $ma_profile->contact_email, TRUE);
  }

  public static function deactivate_ma_user($ma_user) {
    watchdog("Islandora Managed Access", "Managed Access User '{$ma_user->name}' deactivated.");
    $drupal_user = user_load($ma_user->uid);
    user_save($drupal_user, array('status' => 0));
    $ma_user->active = 0;
    $params = array('ma_user_id' => $ma_user->id);
    $ma_profile = IslandoraManagedAccessUserEntityController::get_profile_of_ma_user($ma_user);
    drupal_mail('islandora_managed_access', 'notify_user_deactivated', $drupal_user->mail, language_default(), $params, $ma_profile->contact_email, TRUE);
  }

  public static function get_profile_of_ma_user($ma_user) {
    $rids = array_keys(user_load($ma_user->uid)->roles);
    $rid = array_pop($rids);
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'islandora_managed_access_profile')->propertyCondition('rid', $rid);
    $results = $query->execute();
    $profile_id = array_keys($results['islandora_managed_access_profile']);
    $profile_obj = entity_load('islandora_managed_access_profile', array((int)$profile_id));
    $profile = $profile_obj[(int)$profile_id];
    return $profile;
  }

  public function save($entity, DatabaseTransaction $transaction = NULL) {
    if (!isset($entity->id)) { 
      $entity->created_at = time();
      $entity->refreshed_at = time();
    }
    $entity->updated_at = time();
    return parent::save($entity, $transaction);
  }
 
  public function delete($ids) {
    $users = entity_load('islandora_managed_access_user', $ids);
    foreach ($users as $user) {
      $ma_profile = islandora_managed_access_get_users_profile($user);
      $drupal_user = user_load($user->uid);
      $params = array(
        'real_name' => $user->name,
        'profile_name' => $ma_profile->hname,
        'contact' => $ma_profile->contact_info,
      );
      user_delete(intval($user->uid)); 
      db_delete('islandora_managed_access_user')->condition('id', $user->id)->execute();
    }
  }
}

/**
 * Entity UI Controller class for MA Users
 */
class IslandoraManagedAccessUserEntityUIController extends EntityDefaultUIController {

  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    $items[$this->path]['weight'] = 0;
    return $items;
  }

  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $header = $additional_header;
    array_unshift($header, t('UID'));
    if (!empty($this->entityInfo['exportable'])) {
      $header[] = t('Status');
    }
    $header[] = array('data' => t('Operations'), 'colspan' => $this->operationCount());
    return $header;
  }

  public function overviewTable($conditions = array()) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $this->entityType);
    foreach ($conditions as $key => $value) {
      $query->propertyCondition($key, $value);
    }
    if ($this->overviewPagerLimit) {
      $query->pager($this->overviewPagerLimit);
    }
    $results = $query->execute();
    $ids = isset($results[$this->entityType]) ? array_keys($results[$this->entityType]) : array();
    $entities = $ids ? entity_load($this->entityType, $ids) : array();
    ksort($entities);
    $rows = array();
    foreach ($entities as $entity) {

      $rids = array_keys(user_load($entity->uid)->roles);
      $rid = array_pop($rids);
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'islandora_managed_access_profile')->propertyCondition('rid', $rid);
      $results = $query->execute();
      $profile_id = array_keys($results['islandora_managed_access_profile']);
      $profile_obj = entity_load('islandora_managed_access_profile', array((int)$profile_id));
      $profile = $profile_obj[(int)$profile_id]->hname;
      $activity = ($entity->active == 1 ? "Yes" : "No");
      
      $additional_cols = array($profile, $activity, $entity->name, user_load($entity->uid)->mail, $entity->institution, date('M d, Y', $entity->created_at));
      $rows[] = $this->overviewTableRow($conditions, entity_id($this->entityType, $entity), $entity, $additional_cols);
    }

    $additional_headers = array('Profile', 'Active', 'Name', 'Email', 'Institution', 'Created');
    $render = array(
      '#theme' => 'table',
      '#header' => $this->overviewTableHeaders($conditions, $rows, $additional_headers),
      '#rows' => $rows,
      '#empty' => t('None.'),
    );
    return $render;
  }
}


