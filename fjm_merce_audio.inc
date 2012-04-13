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
    $swfImageURL = $base_url . '/fedora/repository/' . $this->pid . '/IMG/img.jpg';
    // skin config file
/*     $swf_config = drupal_get_path('module', 'fjm_merce') . '/config/merce.xml'; */
    $swf_config = $base_url . '/' . drupal_get_path('module', 'fjm_merce') . '/config/merce.zip';

    //PROXY_MP3 is the datastream id
    $path = drupal_get_path('module', 'Fedora_Repository');

    $content = "";
    $pathTojs = "/sites/all/libraries/mediaplayer4/jwplayer.js";
    drupal_add_js("$pathTojs");

    //swfobject.js is being loaded from swftools module but we are adding the path to jwplayer.js as it is needed as well
    // if swftools are not enabled uncomment this to here drupal_add_js("sites/all/libraries/swfobject/swfobject.js");
    // drupal_add_js("sites/all/libraries/swfobject/swfobject.js");

// player bits
// include if you want the controlbar to be displayed overlaying the background image.
// , 'controlbar.position':'over'
    $js_string = <<<ENDJS

var swfVersionStr = "10.0.0";
var flashvars = { file:escape("$swfURL"),image:escape("$swfImageURL"),autostart:'FALSE', skin:escape("$swf_config") };
var params = { allowfullscreen:'FALSE', allowscriptaccess:'FALSE' };
var attributes = { id:'AudioViewer', name:'AudioViewer' };
swfobject.embedSWF('/sites/all/libraries/mediaplayer4/player.swf','flashContent','650','500','9.0.115','FALSE',flashvars, params, attributes);
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

    $mods_fields = fjm_merce_render_mods($mods_xml_file);

    return theme('fjm_merce_mods', $mods_fields);
  }
}
