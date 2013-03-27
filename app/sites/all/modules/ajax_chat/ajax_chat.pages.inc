<?php
/**
 * @file
 * This file includes implementation of all page callbacks
 */


/**
 * Implementation of page callback ajax_chat page.
 *
 * @return
 *   This function returns rendered page. This means, that output will
 *   be sent to theme system and full page will be rendered.
 */
function ajax_chat_page() {
  return theme('ajax_chat_page', array('count' => 20));
}
