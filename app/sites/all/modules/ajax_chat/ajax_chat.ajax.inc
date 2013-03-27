<?php
/**
 * @file
 * This file includes implementation of all ajax page callbacks
 */


/**
 * Implements page callback for ajax_chat/ajax/list page.
 *
 * This simple renderes current state of chat entries and prints it out.
 */
function ajax_chat_list_callback() {
  // This is quite important. We *print*, not *return* rendered
  // output. This means, that this page callback prints only
  // part of HTML code, not whole layout. This is quite usable
  // for ajax applications.
  print theme('chat_entries_list');
}
