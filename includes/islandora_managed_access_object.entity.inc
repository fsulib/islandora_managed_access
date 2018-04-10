<?php

/**
 * @file
 * Classes for the islandora_managed_access_object entity.
 */

/**
 * Entity class for MA Objects
 */
class IslandoraManagedAccessObjectEntity extends Entity {
}

/**
 * Entity Controller class for MA Objects
 */
class IslandoraManagedAccessObjectEntityController extends EntityAPIController {

  // Applies MA Profile policy to single PID
  public static function manageIslandoraObject($pid, $profile_mname) {
    $object = islandora_object_load($pid);
    $object->relationships->add(ISLANDORA_RELS_EXT_URI, 'hasManagedAccessProfile', $profile_mname, RELS_TYPE_PLAIN_LITERAL);
  }

  // Applies MA Profile policy to single PID
  public static function unmanageIslandoraObject($pid) {
    $object = islandora_object_load($pid);
    $object->relationships->remove(ISLANDORA_RELS_EXT_URI, 'hasManagedAccessProfile');
  }

  // Gets all child PIDs of object
  public static function getChildPids($root_pid) {
    $root_cmodel = islandora_object_load($root_pid)->models[0];
    switch ($root_cmodel) {
      case "islandora:collectionCModel":
        $predicate = "<info:fedora/fedora-system:def/relations-external#isMemberOfCollection>";
        break;
      case "islandora:compoundCModel":
        $predicate = "<info:fedora/fedora-system:def/relations-external#isConstituentOf>";
        break;
      case "islandora:newspaperCModel":
        $predicate = "<info:fedora/fedora-system:def/relations-external#isMemberOf>";
        break;
      case "islandora:geneticEditionCModel":
        $predicate = "<info:fedora/fedora-system:def/relations-external#isMemberOf>";
        break;
      default:
        $predicate = FALSE;
    }
    $pids = array();
    $pids[] = $root_pid;
    if ($predicate) {
      $query = <<<EOQ
SELECT ?child FROM <#ri> WHERE {
  ?child $predicate <info:fedora/$root_pid>
}
EOQ;
      $connection = islandora_get_tuque_connection();
      $results = $connection->repository->ri->sparqlQuery($query);
      foreach ($results as $result) {
        $result = $result['child']['value'];
        $pids[] = $result;
      
        // Go one level deeper for 3 tiered cmodels like newspapers & genetic editions
        $subchildren = self::getChildPids($result);
        foreach ($subchildren as $subchild) {
          $pids[] = $subchild;
        }
      }
    }
    return array_unique($pids);
  }
  
  // Call self::getChildPids but filter out whitelisted objects
  public static function getFilteredChildPids($entity) {
    $children = self::getChildPids($entity->pid);
    $pid_whitelist = explode(',', str_replace(' ', '', $entity->pid_whitelist));
    foreach ($pid_whitelist as $pid) {
      if (($key = array_search($pid, $children)) !== FALSE) {
        unset($children[$key]);
      }
    }
    return $children;
  }

  // Applies MA Profile policy on Islandora object
  public static function manageMAObject($entity) {
    $children = self::getFilteredChildPids($entity);
    $profile = entity_load('islandora_managed_access_profile', array($entity->profile));
    $profile_mname = $profile[$entity->profile]->mname;
    foreach ($children as $child) {
      self::manageIslandoraObject($child, $profile_mname); 
    }
  }
  
  // Removes MA Profile policy from Islandora object
  public static function unmanageMAObject($entity) {
    $children = self::getChildPids($entity->pid);
    foreach ($children as $child) {
      self::unmanageIslandoraObject($child); 
    }
  }

  // Removes & reapplies MA Profile policy on Islandora object
  public static function remanageMAObject($entity) {
    self::unmanageMAObject($entity);
    self::manageMAObject($entity);
  }

  public function save($entity, DatabaseTransaction $transaction = NULL) {
    if (!isset($entity->id)) { 
      $entity->created_at = time();
    }
    $entity->updated_at = time();
    self::remanageMAObject($entity);
    return parent::save($entity, $transaction);
  }

  public function delete($ids) {
    $objects = entity_load('islandora_managed_access_object', $ids);
    foreach ($objects as $object) {
      self::unmanageMAObject($object);
      db_delete('islandora_managed_access_object')->condition('id', $object->id)->execute();
    }
  }

}

/**
 * Entity UI Controller class for MA Objects
 */
class IslandoraManagedAccessObjectEntityUIController extends EntityDefaultUIController {

  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    $items[$this->path]['weight'] = -1;
    return $items;
  }

  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $header = $additional_header;
    array_unshift($header, t('PID'));
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

      $profile_id = $entity->profile;
      $profile_obj = entity_load('islandora_managed_access_profile', array((int)$profile_id));
      $profile = $profile_obj[(int)$profile_id]->hname;

      $label = islandora_object_load($entity->pid)->label;    

      $additional_cols = array($label, $profile);
      $rows[] = $this->overviewTableRow($conditions, entity_id($this->entityType, $entity), $entity, $additional_cols);
    }

    $additional_headers = array('Label', 'Associated Profile');
    $render = array(
      '#theme' => 'table',
      '#header' => $this->overviewTableHeaders($conditions, $rows, $additional_headers),
      '#rows' => $rows,
      '#empty' => t('None.'),
    );
    return $render;
  }
}


