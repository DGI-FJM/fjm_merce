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
  $items['fjm_merce_purge_ingested'] = array(
    'callback' => 'fjm_merce_purge_ingested',
    'description' => 'Purge objects.',
    'examples' => array(
      'drush -l http://example.com fjm_merce_purge_ingested'
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  
  return $items;
}

function fjm_merce_ingest() {
  module_load_include('inc', 'fedora_repository', 'ObjectHelper');
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $drupal_root = getcwd();
  /*
  // Photo Albums
  $album_directory = drupal_get_path('module', 'fjm_merce') . '/data/photo-albums';
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
  //$clippings_directory = drupal_get_path('module', 'fjm_merce') . '/data/clippings-albums';
  $clippings_directory = file_directory_path() . '/data/clippings-albums/';
  $mask = '.xml';
  $clippings = file_scan_directory($clippings_directory, $mask);

  ksort($clippings);
  foreach ($clippings AS $clipping) {
    $clipping_xml = simplexml_load_file($drupal_root . '/' . $clipping->filename);
    $namespaces = $album_xml->getDocNamespaces();
    $clipping_xml->registerXPathNamespace('mets', $namespaces['']); 
    
    foreach ($clipping_xml->dmdSec AS $section) {
      $element_id = (string) $section['ID'];
      $mods = $section->mdWrap->xmlData->children('http://www.loc.gov/mods/v3');
      $mods->registerXPathNamespace('mods', $namespaces['http://www.loc.gov/mods/v3']);
      $title = $mods->xpath("mods:titleInfo/mods:title");
      $title = trim((string) $title[0]);

      $div_node = $clipping_xml->xpath("//mets:div[@DMDID='" . $element_id . "']");
      $file_id = $clipping_xml->xpath("//mets:div[@DMDID='" . $element_id . "']/mets:fptr[1]");
      $parent_node = $clipping_xml->xpath("//mets:div[@DMDID='" . $element_id . "'][not(ancestor::mets:div)]");
      
      if (empty($parent_node) && !empty($file_id[0]['FILEID'])) {
        $type = 'page';
      }
      else {
        fjm_merce_ingest_book($collection, )
      }
      
      //echo 'Type is: ' . $type . ' | Element id is: ' . $element_id . ' | Order is: ' . (string) $div_node[0]['ORDER'] . ' | File_id is: ' . (string) $file_id[0]['FILEID'];
    }
  }
*/
  // Recordings
  $recordings_directory = file_directory_path() . '/data/recordings/';
  $mask = '.xml';
  $recordings = file_scan_directory($recordings_directory, $mask);

  ksort($recordings);
  foreach ($recordings AS $recording) {
    $recording_xml = simplexml_load_file($drupal_root . '/' . $recording->filename);
    $namespaces = $recording_xml->getDocNamespaces();
    $recording_xml->registerXPathNamespace('mets', $namespaces['']);
    $recording_xml->registerXPathNamespace('xlink', $namespaces['xlink']);
    
    foreach ($recording_xml->dmdSec AS $section) {
      $element_id = (string) $section['ID'];
      $mods = $section->mdWrap->xmlData->children('http://www.loc.gov/mods/v3');
      $mods->registerXPathNamespace('mods', $namespaces['http://www.loc.gov/mods/v3']);
      $title = $mods->xpath("mods:titleInfo/mods:title");
      $title = html_entity_decode(trim((string) $title[0]), ENT_COMPAT, 'UTF-8');
      $image = $recording_xml->xpath("//mets:file[@USE='Image-Master']/mets:FLocat/@xlink:href");
      $mp3 = $recording_xml->xpath("//mets:file[@USE='Audio-Streaming']/mets:FLocat/@xlink:href");
      fjm_merce_ingest_recording($title, $mods, $image, $mp3);

    }
  }

  drush_print_timers();
}

/*
function fjm_merce_ingest_book($collection, ) {

}
*/

function fjm_merce_ingest_page() {
  require_once './' . drupal_get_path('module', 'islandora_book') . '/add_pages.inc';
  $page_file = file_directory_path() . '/data/clippings-albums/TIFF/01-000_1.tif';
  ibook_add_single_page_object('islandora:40', $page_file, 1, 'test page');
}

function fjm_merce_ingest_recording($label, $mods, $image, $mp3) {
  $new_pid = Fedora_Item::get_next_PID_in_namespace('cam');
  $object = Fedora_Item::ingest_new_item($new_pid, 'A', $label);

  $object->add_relationship('isMemberOfCollection', 'cam:collectionRecordings');
  $object->add_relationship('hasModel', 'islandora:sp-audioCModel', FEDORA_MODEL_URI);

  if (!empty($mp3[0]['href'])) {
    //OBJ
    $mp3[0]['href'] = str_replace('\\', '/', $mp3[0]['href']);
    $corrected_file = pathinfo($mp3[0]['href']);
    $file_path = file_directory_path() . '/data/recordings/mp3/' . $corrected_file['basename'];
    $object->add_datastream_from_file($file_path, 'OBJ', trim($corrected_file['basename']), 'audio/mp3', 'M', 'added OBJ stream');

    //OBJ_EXIFTOOL
    exec('exiftool -X ' . escapeshellarg($file_path) . '', $output);
    file_put_contents($file_path . '_OBJ_EXIFTOOL.xml', implode("\n", $output));
    $object->add_datastream_from_file($file_path . '_OBJ_EXIFTOOL.xml', 'OBJ_EXIFTOOL', trim($corrected_file['basename']) . '_OBJ_EXIFTOOL.xml', 'text/xml', 'M', 'added OBJ_EXIFTOOL stream');
    unlink($file_path . '_OBJ_EXIFTOOL.xml');

    //PROXY_MP3
    $command_string = "lame -V5 --vbr-new '${file_path}' '${file_path}-vbr.mp3'";
    exec($command_string, $output, $returnValue);
    $object->add_datastream_from_file($file_path . '-vbr.mp3', 'PROXY_MP3', trim($corrected_file['basename']) . '-vbr.mp3', 'audio/mpeg', 'M', 'added PROXY_MP3 stream');
    unlink($file_path . '-vbr.mp3');
  }

  if (!empty($image[0]['href'])) {
    $image[0]['href'] = str_replace('\\', '/', $image[0]['href']);
    $corrected_image_file = pathinfo($image[0]['href']);
    $image_file_path = file_directory_path() . '/data/recordings/img/' . $corrected_image_file['basename'];

    exec("convert " . escapeshellarg($image_file_path) . " -colorspace RGB -thumbnail 120x " . escapeshellarg($image_file_path) . "-tn.jpg", $output, $returnValue);
    $object->add_datastream_from_file("{$image_file_path}-tn.jpg", 'TN', 'Thumbnail', 'image/jpeg', 'M');
    unlink($image_file_path . '-tn.jpg');

    exec("convert " . escapeshellarg($image_file_path) . " -colorspace RGB -thumbnail 500x " . escapeshellarg($image_file_path) . "-img.jpg", $output, $returnValue);
    $object->add_datastream_from_file("{$image_file_path}-img.jpg", 'IMG', 'Image', 'image/jpeg', 'M');
    unlink($image_file_path . '-img.jpg');
  }
  $object->add_datastream_from_string($mods->asXML(), 'MODS', 'MODS', 'text/xml', 'X', 'added MODS Stream');
}

function fjm_merce_purge_ingested() {
  module_load_include('inc', 'fedora_repository', 'api/fedora_collection');
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');

  $objects = get_related_items_as_array('cam:collectionRecordings', 'isMemberOfCollection');
  $count = 0;

  foreach ($objects AS $object) {
    $item = new Fedora_Item($object);
    $item->purge('purged using islandorapp drush script');
    $count++;
  }
  drush_print('Purged ' . $count . ' items');
}