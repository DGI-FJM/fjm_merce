<?php

/**
 * SWFAudio Player
 */
class SWFAudioPlayer {

  private $pid = NULL;

  /**
   * Constructor
   * @param type $pid 
   */
  function __construct($pid) {
    $this->pid = $pid;
  }

  /**
   * Show MP3
   * @global type $base_url
   * @return type 
   */
  function showMP3() {
    global $base_url;
    $swfURL = $base_url . '/fedora/repository/' . $this->pid . '/PROXY_MP3/MP3.mp3';
    $swfImageURL = $base_url . '/fedora/repository/' . $this->pid . '/TN/tn.jpg';

    //PROXY_MP3 is the datastream id
    $path = drupal_get_path('module', 'Fedora_Repository');

    $content = "";
    $pathTojs = "/sites/all/libraries/mediaplayer4/jwplayer.js";
    drupal_add_js("$pathTojs");

    //swfobject.js is being loaded from swftools module but we are adding the path to jwplayer.js as it is needed as well
    // if swftools are not enabled uncomment this to here drupal_add_js("sites/all/libraries/swfobject/swfobject.js");
    // drupal_add_js("sites/all/libraries/swfobject/swfobject.js");

// player bits

    $js_string = <<<ENDJS

var swfVersionStr = "10.0.0";
var flashvars = { file:escape("$swfURL"),image:escape("$swfImageURL"),autostart:'FALSE' };
var params = { allowfullscreen:'FALSE', allowscriptaccess:'FALSE' };
var attributes = { id:'AudioViewer', name:'AudioViewer' };
swfobject.embedSWF('/sites/all/libraries/mediaplayer4/player.swf','flashContent','400','200','9.0.115','FALSE',flashvars, params, attributes);
swfobject.createCSS("#flashContent", "display:block;text-align:left;");
ENDJS;
    drupal_add_js($js_string, 'inline', 'header');

    $mods_data = $this->renderMods();
    return theme('fjm_merce_audio', $mods_data);
  }

  function renderMods() {
    module_load_include('inc', 'fedora_repository', 'ObjectHelper');
    $object_helper = new ObjectHelper();
    $mods_xml_file = $object_helper->getStream($this->pid, 'MODS');
    $mods_xml = simplexml_load_string($mods_xml_file);
    $mods_xml->registerXPathNamespace('mods', 'http://www.loc.gov/mods/v3');

    $mods_fields = array();
    $mods_fields[0]['title'] = 'test';
    $mods_fields[0]['value'] = 'test value';
    $mods_fields[1]['title'] = 'test 2';
    $mods_fields[1]['value'] = 'test value 2';

    return theme('fjm_merce_audio_mods', $mods_fields);
  }
}