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
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['fjm_merce_purge_ingested'] = array(
    'callback' => 'fjm_merce_purge_ingested',
    'description' => 'Purge objects.',
    'examples' => array(
      'drush -l http://example.com fjm_merce_purge_ingested'
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['fjm_merce_fix_book_order'] = array(
    'callback' => 'fjm_merce_fix_book_order',
    'description' => 'Purge objects.',
    'examples' => array(
      'drush -l http://example.com fjm_merce_fix_book_order'
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['fjm_merce_reindex_ocr'] = array(
    'callback' => 'fjm_merce_reindex_ocr',
    'description' => 'Reindex OCR.',
    'examples' => array(
      'drush -l http://example.com fjm_merce_reindex_ocr'
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['fjm_merce_reindex_all'] = array(
    'callback' => 'fjm_merce_reindex',
    'description' => 'Reindex',
    'examples' => array(
      'drush -l http://example.com fjm_merce_reindex_all'
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

function fjm_merce_ingest() {
  module_load_include('inc', 'fedora_repository', 'ObjectHelper');
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $drupal_root = getcwd();

  // Photo Albums
  $album_directory = file_directory_path() . '/data/photo albums';
  $mask = '.xml';
  $albums = file_scan_directory($album_directory, $mask);

  ksort($albums);
  foreach ($albums AS $album) {
    $album_xml = simplexml_load_file($drupal_root . '/' . $album->filename);
    $namespaces = $album_xml->getDocNamespaces();
    $album_xml->registerXPathNamespace('mets', $namespaces['']);
    $album_xml->registerXPathNamespace('xlink', $namespaces['xlink']);

    foreach ($album_xml->dmdSec AS $section) {
      $element_id = (string) $section['ID'];
      $mods = $section->mdWrap->xmlData->children('http://www.loc.gov/mods/v3');
      $mods->registerXPathNamespace('mods', $namespaces['http://www.loc.gov/mods/v3']);
      $title = $mods->xpath("mods:titleInfo/mods:title");
      $title = html_entity_decode(trim((string) $title[0]), ENT_COMPAT, 'UTF-8');
      $title = substr($title, 0, 244);
      // $div_node = $album_xml->xpath("//mets:div[@DMDID='" . $element_id . "']");
      $file_id = $album_xml->xpath("//mets:div[@DMDID='" . $element_id . "']/mets:fptr[1]");
      $file = $album_xml->xpath("//mets:file[@ID='" . (string) $file_id[0]['FILEID'] . "']/mets:FLocat/@xlink:href");
      $file = str_replace('\\', '/', $file[0]['href']);
      $parent_node = $album_xml->xpath("//mets:div[@DMDID='" . $element_id . "'][not(ancestor::mets:div)]");

      if (empty($parent_node) && !empty($file_id[0]['FILEID'])) {
        // $type = 'page';
        $page_pid = fjm_merce_ingest_page($title, $page_num, $book_pid, $mods, $file, 'photo albums', FALSE);
        // fjm_merce_ingest_page($label, $page_num, $book_pid, $mods, $file, $path, $ocr = FALSE)
        $page_num++; //Get from order
      }
      elseif (empty($parent_node)) {
        // $type = 'photo';
        fjm_merce_ingest_photo($title, $page_pid, $mods);
      }
      else {
        //album
        $book_pid = fjm_merce_ingest_book($title, 'cam:collectionAlbums', $mods);
        $page_num = 1;
      }

      // echo 'Type is: ' . $type . ' | Element id is: ' . $element_id . ' | Order is: ' . (string) $div_node[0]['ORDER'] . ' | File_id is: ' . (string) $file_id[0]['FILEID'];
    }
  }
  /*
  // Newspaper Clippings
  //$clippings_directory = drupal_get_path('module', 'fjm_merce') . '/data/clippings-albums';
  $clippings_directory = file_directory_path() . '/data/clippings albums/';
  $mask = '.xml';
  $clippings = file_scan_directory($clippings_directory, $mask);

  ksort($clippings);
  foreach ($clippings AS $clipping) {
    $clipping_xml = simplexml_load_file($drupal_root . '/' . $clipping->filename);
    $namespaces = $clipping_xml->getDocNamespaces();
    $clipping_xml->registerXPathNamespace('mets', $namespaces['']);
    $clipping_xml->registerXPathNamespace('xlink', $namespaces['xlink']);

    foreach ($clipping_xml->dmdSec AS $section) {
      $element_id = (string) $section['ID'];
      $mods = $section->mdWrap->xmlData->children('http://www.loc.gov/mods/v3');
      $mods->registerXPathNamespace('mods', $namespaces['http://www.loc.gov/mods/v3']);
      $title = $mods->xpath("mods:titleInfo/mods:title");
      $title = html_entity_decode(trim((string) $title[0]), ENT_COMPAT, 'UTF-8');

      //$div_node = $clipping_xml->xpath("//mets:div[@DMDID='" . $element_id . "']");
      $file_id = $clipping_xml->xpath("//mets:div[@DMDID='" . $element_id . "']/mets:fptr[1]");
      $file = $clipping_xml->xpath("//mets:file[@ID='" . (string) $file_id[0]['FILEID'] . "']/mets:FLocat/@xlink:href");
      $file = str_replace('\\', '/', $file[0]['href']);
      $parent_node = $clipping_xml->xpath("//mets:div[@DMDID='" . $element_id . "'][not(ancestor::mets:div)]");

      if (empty($parent_node) && !empty($file_id[0]['FILEID'])) {
       // $type = 'page';
        fjm_merce_ingest_page($title, $page_num, $book_pid, $mods, $file, 'clippings albums', TRUE);
        $page_num++; //Get from order
      }
      else {
        $book_pid = fjm_merce_ingest_book($title, 'cam:collectionClippings', $mods);
        $page_num = 1;
      }

      //echo ' | Element id is: ' . $element_id . ' | File_id is: ' . (string) $file_id[0]['FILEID'];
    }
  }

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
*/
  drush_print_timers();
}

function fjm_merce_ingest_book($label, $collection, $mods) {
  $new_pid = Fedora_Item::get_next_PID_in_namespace('cam');
  $object = Fedora_Item::ingest_new_item($new_pid, 'A', $label);

  $object->add_relationship('isMemberOfCollection', $collection);
  $object->add_relationship('hasModel', 'islandora:bookCModel', FEDORA_MODEL_URI);
  $object->add_datastream_from_string($mods->asXML(), 'MODS', 'MODS', 'text/xml', 'X', 'added MODS Stream');
  $object->add_datastream_from_file(drupal_get_path('module', 'islandora_book') . '/images/book_module_TN.png', 'TN', 'Thumbnail', 'image/png', 'M');

  return $new_pid;
}

function fjm_merce_ingest_photo($label, $page_pid, $mods) {
  $new_pid = Fedora_Item::get_next_PID_in_namespace('cam');
  $object = Fedora_Item::ingest_new_item($new_pid, 'A', $label);

  $object->add_relationship('isMemberOf', $page_pid);
  $object->add_relationship('hasModel', 'cam:photoCModel', FEDORA_MODEL_URI);
  $object->add_datastream_from_string($mods->asXML(), 'MODS', 'MODS', 'text/xml', 'X', 'added MODS Stream');
  $object->add_datastream_from_file(drupal_get_path('module', 'islandora_book') . '/images/book_module_TN.png', 'TN', 'Thumbnail', 'image/png', 'M');
}

function fjm_merce_ingest_page($label, $page_num, $book_pid, $mods, $file, $path, $ocr = FALSE) {
  $page_file = '/var/www/html/drupal/sites/digital.march.es.merce/files' . '/data/' . $path . '/' . $file;
  $new_pid = "$book_pid-" . sprintf("%03d", $page_num);
  drush_print_r('Pid is: ' . $new_pid . ' File is: ' . $page_file);
  $page_item = Fedora_Item::ingest_new_item($new_pid, 'A', $label);

  $page_item->add_relationship('isMemberOf', $book_pid);
  $page_item->add_relationship('hasModel', 'islandora:pageCModel', FEDORA_MODEL_URI);
  $page_item->add_datastream_from_string($mods->asXML(), 'MODS', 'MODS', 'text/xml', 'X', 'added MODS Stream');
  // add original tiff

  $file_info = pathinfo($page_file);
  $file_name = $file_info['basename'];

  $page_item->add_datastream_from_file($page_file, 'OBJ', 'Archival JPEG', 'image/jpeg', 'M');
  //  $page_file = preg_replace('/\/\//', '/', $page_file);
  drush_print_r('Page file is now: ' . $page_file);
  //add compresed jp2 for viewer

  if (variable_get('islandora_book_create_images', TRUE)) {
    //    exec('kdu_compress -i "' . $page_file . '" -o "' . $file_name . '.jp2' . '" -rate 0.5 Clayers=1 Clevels=7 "Cprecincts={256,256},{256,256},{256,256},{128,128},{128,128},{64,64},{64,64},{32,32},{16,16}" "Corder=RPCL" "ORGgen_plt=yes" "ORGtparts=R" "Cblk={32,32}" Cuse_sop=yes', $output, $returnValue);
    if ($returnValue == '0') {
      // $page_item->add_datastream_from_file("$file_name.jp2", 'JP2', "Compressed JP2", 'image/jp2', 'M');
    }
    //    unlink($file_name . '.jp2');
    //add thumbnail image

    exec("convert " . escapeshellarg($page_file) . " -colorspace RGB -thumbnail 120x " . escapeshellarg($file_name . '-tn.jpg'), $output, $returnValue);
    if ($returnValue == '0') {
      $page_item->add_datastream_from_file("$file_name-tn.jpg", 'TN', 'Thumbnail', 'image/jpeg', 'M');
      unlink($file_name . '-tn.jpg');
      /*
      if ($page_num == 1) {
        $book_item = new Fedora_Item($book_pid);
        $DSIDS = $book_item->get_datastreams_list_as_array();
        if (array_key_exists("TN", $DSIDS)) {
          $book_item->purge_datastream('TN');
        }
        $book_item->add_datastream_from_file("$page_file-tn.jpg", 'TN', 'Thumbnail', 'image/jpeg', 'M');
      }
      */
    }

    //add jpeg
    exec("convert  -resize 800 " . escapeshellarg($page_file) . " " . escapeshellarg($file_name . '-med.jpg'), $output, $returnValue);
    if ($returnValue == '0') {
      $page_item->add_datastream_from_file("$file_name-med.jpg", 'JPEG', "JPEG image", 'image/jpeg', 'M');
      unlink($file_name . '-med.jpg');
    }
  }
  //OCR

  if ($ocr) {
    $pdf_file_name = str_ireplace('.tif', '.PDF', $file);
    $pdf_file_name = str_ireplace('.tiff', '.PDF', $pdf_file_name);
    $pdf_file = file_directory_path() . '/data/' . $path . '/pdfs' . $pdf_file_name;
    drush_print_r("trying ocr for: " . $pdf_file);
    if (strpos($pdf_file, '.PDF') !== FALSE) {
      exec("pdftotext \"{$pdf_file}\" ocr.txt", $output, $returnValue);
      $page_item->add_datastream_from_file("ocr.txt", 'OCR', "Scanned Text", 'text/plain', 'M');
      unlink('ocr.txt');
    }
    else {
      drush_print_r('No pdf for pid: ' . $new_pid);
    }
  }

  return $new_pid;
}

function fjm_merce_ingest_recording($label, $mods, $image, $mp3) {
  if (!empty($mp3[0]['href'])) {
    //OBJ
    $mp3[0]['href'] = str_replace('\\', '/', $mp3[0]['href']);
    $corrected_file = pathinfo($mp3[0]['href']);
    $file_path = file_directory_path() . '/data/recordings/mp3/' . $corrected_file['basename'];
    $out_file = file_directory_path() . '/ingest-work/' . $corrected_file['basename'];
    //OBJ_EXIFTOOL
    // exec('exiftool -X ' . escapeshellarg($file_path) . '', $output);
    // file_put_contents($file_path . '_OBJ_EXIFTOOL.xml', implode("\n", $output));

    //PROXY_MP3
    $command_string = "lame -V5 --vbr-new '${file_path}' '${out_file}-vbr.mp3'";
    exec($command_string, $output, $returnValue);
  }

  if (!empty($image[0]['href'])) {
    $image[0]['href'] = str_replace('\\', '/', $image[0]['href']);
    $corrected_image_file = pathinfo($image[0]['href']);
    $image_file_path = file_directory_path() . '/data/recordings/img/' . $corrected_image_file['basename'];
    $out_image_path = file_directory_path() . '/ingest-work/' . $corrected_image_file['basename'];
    exec("convert " . escapeshellarg($image_file_path) . " -colorspace RGB -thumbnail 120x " . escapeshellarg($out_image_path) . "-tn.jpg", $output, $returnValue);

    exec("convert " . escapeshellarg($image_file_path) . " -colorspace RGB -thumbnail 500x " . escapeshellarg($out_image_path) . "-img.jpg", $output, $returnValue);
  }

  $new_pid = Fedora_Item::get_next_PID_in_namespace('cam');
  $object = Fedora_Item::ingest_new_item($new_pid, 'A', $label);

  $object->add_relationship('isMemberOfCollection', 'cam:collectionRecordings');
  $object->add_relationship('hasModel', 'islandora:sp-audioCModel', FEDORA_MODEL_URI);
  $object->add_datastream_from_string($mods->asXML(), 'MODS', 'MODS', 'text/xml', 'X', 'added MODS Stream');
  $object->add_datastream_from_file($file_path, 'OBJ', trim($corrected_file['basename']), 'audio/mp3', 'M', 'added OBJ stream');
  // $object->add_datastream_from_file($file_path . '_OBJ_EXIFTOOL.xml', 'OBJ_EXIFTOOL', trim($corrected_file['basename']) . '_OBJ_EXIFTOOL.xml', 'text/xml', 'M', 'added OBJ_EXIFTOOL stream');
  // unlink($file_path . '_OBJ_EXIFTOOL.xml');
  $object->add_datastream_from_file($out_file . '-vbr.mp3', 'PROXY_MP3', trim($corrected_file['basename']) . '-vbr.mp3', 'audio/mpeg', 'M', 'added PROXY_MP3 stream');
  unlink($out_file . '-vbr.mp3');
  $object->add_datastream_from_file("{$out_image_path}-tn.jpg", 'TN', 'Thumbnail', 'image/jpeg', 'M');
  unlink($out_image_path . '-tn.jpg');
  $object->add_datastream_from_file("{$out_image_path}-img.jpg", 'IMG', 'Image', 'image/jpeg', 'M');
  unlink($out_image_path . '-img.jpg');
  sleep(6);

}

function fjm_merce_purge_ingested() {
  module_load_include('inc', 'fedora_repository', 'api/fedora_collection');
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');

  //$objects = get_related_items_as_array('cam:collectionRecordings', 'isMemberOfCollection');
  $objects = get_related_items_as_array('cam:301"', 'isMemberOf');
  $count = 0;

  foreach ($objects AS $object) {
    $item = new Fedora_Item($object);
    $item->purge('purged using islandorapp drush script');
    $count++;
  }
  drush_print('Purged ' . $count . ' items');
}

function fjm_merce_fix_book_order() {
  $book_query = "select \$object from <#ri>
    where \$object <fedora-model:hasModel> <info:fedora/islandora:bookCModel>";
  $book_pids = _process_book_query($book_query);

  module_load_include('inc', 'islandora_book', 'book_pack_utils');
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  foreach ($book_pids AS $book_pid) {
    $page_pids = get_page_pids($book_pid);

    foreach ($page_pids as $pid) {
      $item = new Fedora_Item($pid);
      if ($item->exists()) {    //added  this to pass by badly broken objects
        $real_page_num = explode('-', $pid);
        if (is_numeric($real_page_num[1])) {
          $rels_ext = $item->get_datastream_dissemination('RELS-EXT');
          $relsextxml = new DomDocument();
          $relsextxml->loadXML($rels_ext);

          $rels = $relsextxml->getElementsByTagNameNS('info:islandora/islandora-system:def/pageinfo#', 'isPageNumber');

          foreach ($rels AS $rel) {
            $newRel = $relsextxml->createElementNS('info:islandora/islandora-system:def/pageinfo#', 'isPageNumber');
            $newRel->setAttribute('rdf:resource', 'info:fedora/' . $real_page_num[1]);
            $rel->parentNode->replaceChild($newRel, $rel);
          }
          $item->modify_datastream_by_value($relsextxml->saveXML(), 'RELS-EXT', 'Fedora Object-to-Object Relationship Metadata', 'text/xml');
          drush_print_r('Update page number to: ' . $real_page_num[1] . ' for PID: ' . $pid);
        }
      }
    }
  }
}

function fjm_merce_reindex_ocr() {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');

  $query = "select \$object from <#ri>
    where \$object <fedora-model:hasModel> <info:fedora/islandora:pageCModel>";
  $pids = _process_book_query($query);

  foreach ($pids AS $pid) {
    $item = new Fedora_Item($pid);
    $ocr = $item->get_datastream_dissemination('OCR');
    if (!empty($ocr)) {
      $ocr = preg_replace('/[\x{000C}]/u', '', $ocr);

      $temp_file_dir = file_directory_path();
      $temp_file_path = file_create_filename("ocr.txt", $temp_file_dir);
      file_save_data($ocr, $temp_file_path);
      $temp_file_url = file_create_url($temp_file_path);
      $item->modify_datastream_by_reference($temp_file_url, 'OCR', 'Scanned Text', 'text/plain');
      file_delete($temp_file_path);

      drush_print_r('OCR updated for PID: ' . $pid);
    }
    else {
      drush_print_r('OCR DS empty for PID: ' . $pid);
    }
  }
}

// Fake a change to get solr to reindex
function fjm_merce_reindex() {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');

  $query = "select \$object from <#ri>
    where \$object <fedora-model:hasModel> <info:fedora/islandora:sp-audioCModel>";
  $pids = _process_book_query($query);

  foreach ($pids AS $pid) {
    $name_space = explode(':', $pid);
    if ($name_space[0] == 'cam') {
      $item = new Fedora_Item($pid);

      $item->set_datastream_state('MODS', 'I', 'Modified by Islandora API');
      $item->set_datastream_state('MODS', 'A', 'Modified by Islandora API');

      drush_print_r('MODS updated for PID: ' . $pid);
    }
  }
}

function _process_book_query($query) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
  $query = htmlentities(urlencode($query));
  $content = '';

  $url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
  $url .= "?type=tuples&flush=TRUE&format=csv&limit=$limit&offset=$offset&lang=itql&stream=on&query=" . $query;
  $content .= do_curl($url);
  $results = explode("\n", $content);
  return array_filter(preg_replace('/^info:fedora\/|"object"/', '', $results));
}
