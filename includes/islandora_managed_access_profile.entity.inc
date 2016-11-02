<?php

/**
 * @file
 * Classes for the islandora_managed_access_profile entity.
 */

/**
 * Entity class for MA Profiles
 */ 
class IslandoraManagedAccessProfileEntity extends Entity {
}

/**
 * Entity Controller class for MA Profiles
 */
class IslandoraManagedAccessProfileEntityController extends EntityAPIController {

  public function save($entity, DatabaseTransaction $transaction = NULL) {
    if (!isset($entity->id)) { 
      $entity->created_at = time();
    }
    $entity->updated_at = time();
    return parent::save($entity, $transaction);
  }

  public function delete($ids) {
    $profiles = entity_load('islandora_managed_access_profile', $ids);
    foreach ($profiles as $profile) {
      user_role_delete(intval($profile->rid)); 
      db_delete('islandora_managed_access_profile')->condition('id', $profile->id)->execute();
    }
  }

}

/**
 * Entity UI Controller class for MA Profiles
 */
class IslandoraManagedAccessProfileEntityUIController extends EntityDefaultUIController {

  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    $items[$this->path]['weight'] = -2;
    return $items;
  }

  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $header = $additional_header;
    array_unshift($header, t('Name'));
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
      $additional_cols = array($entity->mname, user_role_load($entity->rid)->name);
      $rows[] = $this->overviewTableRow($conditions, entity_id($this->entityType, $entity), $entity, $additional_cols);
    }

    $additional_headers = array('Machine Name', 'Associated Role');
    $render = array(
      '#theme' => 'table',
      '#header' => $this->overviewTableHeaders($conditions, $rows, $additional_headers),
      '#rows' => $rows,
      '#empty' => t('None.'),
    );
    return $render;
  }
}

