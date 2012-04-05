<?php

/**
 * @file
 * Drush script to ingest data.
 * 
 */

/**
 * Implementation of hook_drush_command().
 */
function fjm_merce_drush_command() {
  $items = array();
  $items['fjm_merce_ingest'] = array(
    'callback' => 'fjm_merce_ingest',
    'description' => 'Ingest objects.',
    'examples' => array(
      'drush -l http://example.com fjm_merce_ingest'
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );
  
  return $items;
}

function fjm_merce_ingest() {
  $start = microtime();
  $start = explode(" ",$start);
  $start = $start[1] + $start[0];
  $drupal_root = getcwd();
  
  module_load_include('inc', 'fedora_repository', 'ObjectHelper');
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  /*
  // Photo Albums
  $album_directory = drupal_get_path('module', 'fjm_merce') . '/data/photo albums';
  $mask = '.xml';
  $albums = file_scan_directory($album_directory, $mask);
  
  foreach ($albums AS $album) {
    $album_xml = simplexml_load_file($drupal_root . '/' . $album->filename);
    $namespaces = $album_xml->getDocNamespaces();
    $album_xml->registerXPathNamespace('mets', $namespaces['']); 
    
    foreach ($album_xml->dmdSec AS $section) {
      $element_id = (string) $section['ID'];
      $mods = $section->mdWrap->xmlData->children('http://www.loc.gov/mods/v3');
      $div_node = $album_xml->xpath("//mets:div[@DMDID='" . $element_id . "']");
      $file_id = $album_xml->xpath("//mets:div[@DMDID='" . $element_id . "']/mets:fptr[1]");
      $parent_node = $album_xml->xpath("//mets:div[@DMDID='" . $element_id . "'][not(ancestor::mets:div)]");
      
      if (empty($parent_node) && !empty($file_id[0]['FILEID'])) {
        $type = 'page';
      }
      elseif (empty($parent_node)) {
        $type = 'photo';
      }
      else {
        $type = 'album';
      }
      
     // echo 'Type is: ' . $type . ' | Element id is: ' . $element_id . ' | Order is: ' . (string) $div_node[0]['ORDER'] . ' | File_id is: ' . (string) $file_id[0]['FILEID'];
    }
  }
  
  // Newspaper Clippings
  $clippings_directory = drupal_get_path('module', 'fjm_merce') . '/data/clippings albums';
  $mask = '.xml';
  $clippings = file_scan_directory($clippings_directory, $mask);
  
  foreach ($clippings AS $clipping) {
    $clipping_xml = simplexml_load_file($drupal_root . '/' . $clipping->filename);
    $namespaces = $album_xml->getDocNamespaces();
    $clipping_xml->registerXPathNamespace('mets', $namespaces['']); 
    
    foreach ($clipping_xml->dmdSec AS $section) {
      $element_id = (string) $section['ID'];
      $mods = $section->mdWrap->xmlData->children('http://www.loc.gov/mods/v3');
      $div_node = $clipping_xml->xpath("//mets:div[@DMDID='" . $element_id . "']");
      $file_id = $clipping_xml->xpath("//mets:div[@DMDID='" . $element_id . "']/mets:fptr[1]");
      $parent_node = $clipping_xml->xpath("//mets:div[@DMDID='" . $element_id . "'][not(ancestor::mets:div)]");
      
      if (empty($parent_node) && !empty($file_id[0]['FILEID'])) {
        $type = 'page';
      }
      else {
        $type = 'album';
      }
      
      //echo 'Type is: ' . $type . ' | Element id is: ' . $element_id . ' | Order is: ' . (string) $div_node[0]['ORDER'] . ' | File_id is: ' . (string) $file_id[0]['FILEID'];
    }
  }
  */
  // Recordings
  $recordings_directory = drupal_get_path('module', 'fjm_merce') . '/data/recordings';
  $mask = '.xml';
  $recordings = file_scan_directory($recordings_directory, $mask);
  
  foreach ($recordings AS $recording) {
    $recording_xml = simplexml_load_file($drupal_root . '/' . $recording->filename);
    $namespaces = $recording_xml->getDocNamespaces();
    $recording_xml->registerXPathNamespace('mets', $namespaces['']);
    $recording_xml->registerXPathNamespace('xlink', $namespaces['xlink']);
    
    foreach ($recording_xml->dmdSec AS $section) {
      $element_id = (string) $section['ID'];
      $mods = $section->mdWrap->xmlData->children('http://www.loc.gov/mods/v3');

      $image = $recording_xml->xpath("//mets:file[@USE='Image-Master']/mets:FLocat/@xlink:href");
      $mp3 = $recording_xml->xpath("//mets:file[@USE='Audio-Streaming']/mets:FLocat/@xlink:href");

      $new_pid = Fedora_Item::get_next_PID_in_namespace('islandora');
      $label = 'audio file';
      $object = Fedora_Item::ingest_new_item($new_pid, 'A', $label);
    
    
      $object->add_relationship('isMemberOfCollection', 'islandora:audio_collection');
      $object->add_relationship('hasModel', 'islandora:sp-audioCModel', FEDORA_MODEL_URI);

      if (!empty($mp3[0]['href'])) {
        $mp3[0]['href'] = str_replace('\\', '/', $mp3[0]['href']);
        $corrected_file = pathinfo($mp3[0]['href']);
        $file_path = drupal_get_path('module', 'fjm_merce')  . '/data/recordings/mp3/' . $corrected_file['basename'];
        watchdog('mitch-test', 'File path is: ' . $file_path);
        $object->add_datastream_from_file($file_path, 'OBJ', trim($corrected_file['basename']), 'audio/mp3', 'M', 'added OBJ stream');
        // $object_helper = new ObjectHelper();
        // $object_helper->get_and_do_datastream_rules($new_pid, 'OBJ', $file_path);
      }
      $object->add_datastream_from_string($mods->asXML(), 'MODS', 'MODS', 'text/xml', 'X', 'added MODS Stream');
    }
    break;
  }
  
  $end = microtime();
  $end = explode(" ",$end);
  $end = $end[1] + $end[0];
  $duration = $end - $start;
  echo ($duration) . ' execution time';
}
