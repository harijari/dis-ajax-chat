<?php
/**
 * @file TODO
 */

/**
 * Implementation of ajax_chat_ui_admin_form.
 *
 * This function is an callback for administration form used by ajax_chat_ui
 * module. This function returns FORM API array structure.
 *
 * This stucture defines form (all elements, submit handlers etc).
 * You can find more information about Form API here:
 * http://api.drupal.org/api/drupal/developer!topics!forms_api_reference.html/7
 */
function ajax_chat_ui_admin_form() {
  $form = array();

  // This will generate list of moderated entries in administration form
  // @see _ajax_chat_ui_admin_form() function for more details.
  $form['chat_entries'] = _ajax_chat_ui_admin_form();

  // This will add Delete button and link to new chat entry creation form.
  $form['buttons'] = array(
    'delete' => array(
      '#title' => t('Delete selected values'),
      '#value' => t('Delete selected values.'),
      '#submit' => array('ajax_chat_ui_admin_form_delete'),
      '#type' => 'submit',
    ),
    'add' => array(
      '#markup' => l(t('Add new element'), 'ajax_chat_ui/chat_entry_form'),
    ),
  );

  return $form;
}


/**
 * Helper function. Generates list of chat entries in form api structure.
 *
 * This function makes proper query to database, and retrieves all
 * chat entries from it. Results of query are transformed to
 * structure valid in form api language. This will help us to make
 * a simple administration panel.
 *
 * This referers to real life example.
 * @see node_admin_nodes()
 */
function _ajax_chat_ui_admin_form() {
  // We want to create table stucture.
  // 1. We need to define headers. 'Field value points to database field. This
  // is required for sorting. sort attribute defines default order.
  $header = array(
    'id'     => array('data' => t('Id'), 'field' => 'ce.chid'),
    'author' => array('data' => t('Author'), 'field' => 'u.name', 'sort' => 'desc'),
    'mail'   => array('data' => t('Mail'), 'field' => 'u.mail'),
    'time'   => array('data' => t('Date'), 'field' => 'ce.timestamp', 'sort' => 'desc'),
    'body'   => array('data' => t('Body'), 'field' => 'ce.body'),
  );

  // Initialize empty arrays for entries from DB and form API structure.
  $form_elements = array();
  $entries = array();

  // Let's create our query, with DBTNG API.
  // @see http://drupal.org/developing/api/database for more details.
  $query = db_select('chat_entry', 'ce');

  // This could be writen as INNER JOIN users u USING(uid)
  $query->innerJoin('users', 'u', 'u.uid = ce.uid');

  // @codingStandardsIgnoreStart
  // This is an example of chaining functionality in DBTNG API
  // @see http://drupal.org/node/1060924 for more details
  // This is quite similar to idea from jquery library.
  $results = $query->fields('ce', array('chid', 'body', 'timestamp'))
    ->fields('u', array('name', 'mail', 'uid'))
    ->extend('PagerDefault') // Add support for pager (page= GET arguments)
    ->extend('TableSort') // Add support for sort GET argument
    ->limit(25) // Display 25 entries per pager page
    ->orderByHeader($header) // Order by settings from header
    ->execute(); // Boom - send query to database
  // @codingStandardsIgnoreEnd

  // Thank to SPL, we can itereate through results like all others structures.
  foreach ($results as $single_result) {

    // Make a basic transformation of data retrieved from database.
    $entries[$single_result->chid] = array(
      'id' => l(
        $single_result->chid,
        'ajax_chat_ui/chat_entry_form/' . $single_result->chid
      ),
      'author' => theme('username', array('account' => user_load($single_result->uid))),
      'mail' => $single_result->mail,
      'time' => format_date($single_result->timestamp, 'short'),
      'body' => filter_xss($single_result->body),
    );
  }

  // We got header, and all entries from database. This is enough to
  // glue them together in one structure - tableselect.
  // This structure renders quite usable table with sort support,
  // and massive operations.
  $form_elements['entries'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $entries,
    '#empty' => t('No content available'),
  );

  // Render pager, next to list of entries.
  $form_elements['pager'] = array(
    '#markup' => theme('pager'),
  );

  return $form_elements;
}


/**
 * Implementation of form: ajax_chat_ui_chat_entry
 *
 * This form allows edition and creation of chat entries
 *
 * @param array $form
 * @param array $form_state
 * @param int $chat_entry_form
 *   This is optional parameter. This is a id of chat entry, that wil
 */
function ajax_chat_ui_chat_entry_form($form, &$form_state, $chat_entry_id = NULL) {
  global $user;

  if (is_numeric($chat_entry_id) && user_access('administer chat')) {
    $chat_entry_list = ajax_chat_entries_retrieve(array($chat_entry_id));
    $chat_entry = count($chat_entry_list) == 1 ? (object) $chat_entry_list[0] : NULL;
  }
  else {
    $chat_entry = NULL;
  }

  return array(
    '#access' => user_access('add chat entry'),
    'chid' => array(
      '#type' => 'value',
      '#value' => is_null($chat_entry) ? NULL : $chat_entry->chid,
    ),

    'timestamp' => array(
      '#type' => 'value',
      '#value' => is_null($chat_entry) ? REQUEST_TIME : $chat_entry->timestamp,
    ),

    'uid' => array(
      '#type' => 'value',
      '#value' => is_null($chat_entry) ? $user->uid : $chat_entry->uid,
    ),
    'body' => array(
      '#type' => 'textarea',
      '#title' => t('Message'),
      '#default_value' => is_null($chat_entry) ? NULL : $chat_entry->body,
      '#description' => t('Please put here your message for others. Please behave yourself!'),
    ),
    'save' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
  );
}

function ajax_chat_ui_chat_entry_form_submit($form, &$form_state) {
  global $user;

  $entry = $form_state['values'];

  $entry['timestamp'] = is_int($entry['timestamp']) ? $entry['timestamp'] : REQUEST_TIME;

  $entry['uid'] = is_int($entry['uid']) ? $entry['uid'] : $user->uid;

  if (is_numeric($entry['chid'])) {
    drupal_write_record('chat_entry', $entry, array('chid'));
    drupal_set_message('Entry was updated.');
  }
  else {
    drupal_write_record('chat_entry', $entry);
    drupal_set_message('New entry was added.');
  }

}


function ajax_chat_ui_admin_form_delete($form, &$form_state)  {
  $selected_values = array_filter($form_state['values']['entries']);
  ajax_chat_entries_remove($selected_values);

  drupal_set_message(
    t(
      '%count entries wcere deleted.',
      array('%count' => count($selected_values))
    )
  );
}



function ajax_chat_ui_message_form($form, &$form_state) {
  $form = ajax_chat_ui_chat_entry_form($form, $form_state);

  $form['save']['#ajax'] = array(
    'wrapper' => 'ajax-chat-wrapper',
    'callback' => 'ajax_chat_ajax_update_callback',
    'effect' => 'fade',
  );

  return $form;
}

