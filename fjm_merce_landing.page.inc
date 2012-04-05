<?php

function fjm_merce_landing_page() {
  $menu = _fjm_merce_landing_page_render_menu();
  $search = drupal_get_form('islandora_extended_search_form');
  $bio = _fjm_merce_landing_page_render_bio();
  $browse = _fjm_merce_landing_page_render_browse();
  return theme('fjm_merce_landing', $menu, 'carousel', $bio, $browse, $search);
}

function _fjm_merce_landing_page_render_menu() {
  $menu = menu_navigation_links('menu-merce-landing');
  return theme('fjm_merce_menu', $menu);
}

function _fjm_merce_landing_page_render_carousel() {
  
}

function _fjm_merce_landing_page_render_bio() {
  $text = t('Biography text is here.');
  return theme('fjm_merce_bio', $text);
}

function _fjm_merce_landing_page_render_browse() {
  $photo_albums = menu_navigation_links('menu-photo-albums');
  $recordings = menu_navigation_links('menu-recordings');
  $press_clippings = menu_navigation_links('menu-press-clipping-albums');
  return theme('fjm_merce_browse', $photo_albums, $recordings, $press_clippings);
}