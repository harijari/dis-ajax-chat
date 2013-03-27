(function ($) {
  Drupal.behaviors.ajax_chat = {
    attach: function (context, settings) {
      console.log('Test');
      setInterval(
        function() {
          $('.ajax-chat-wrapper').load(
            Drupal.settings.ajax_chat_entries_list_url
          );
        },
        30 * 1000
      );
    }
  }
})(jQuery);
