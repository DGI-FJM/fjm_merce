<?php

function fjm_merce_landing_page() {
  $menu = _fjm_merce_landing_page_render_menu();
  $carousel = _fjm_merce_landing_page_render_carousel();
  $disable_search = variable_get('merce_solr_disable_form', '0');
  if (!$disable_search) {
    $search = drupal_get_form('islandora_extended_search_form');
  } 
  else {
    $search = '';
  }
  $bio = _fjm_merce_landing_page_render_bio();
  $browse = _fjm_merce_landing_page_render_browse();
  return theme('fjm_merce_landing', $menu, $carousel, $bio, $browse, $search);
}

function _fjm_merce_landing_page_render_menu() {
  $menu = menu_navigation_links('menu-merce-landing');
  return theme('fjm_merce_menu', $menu);
}

function _fjm_merce_landing_page_render_carousel() {
  $view_name = 'slideshow';
  $display_id = 'block_1';
  return views_embed_view($view_name, $display_id);
}

function _fjm_merce_landing_page_render_bio() {
  $text = '<h2>' . t('Legado Antonia Mercé') . '<br />' . t('“La Argentina”.') . '</h2>';
  $text .= '<p>' . t('“I serve a mute art and I look for complete expression through dance steps. My most assured vocabulary comes from the lines of my body and the expression on my face. I like to make myself understood in silence. I have learnt to live far from words.” (Antonia Mercé, 1890 – 1936).') . '</p>';
    
  return theme('fjm_merce_bio', $text);
}

function _fjm_merce_landing_page_render_browse() {
  $photo_albums = menu_navigation_links('menu-photo-albums');
  $recordings = menu_navigation_links('menu-recordings');
  $press_clippings = menu_navigation_links('menu-press-clipping-albums');
  return theme('fjm_merce_browse', $photo_albums, $recordings, $press_clippings);
}
