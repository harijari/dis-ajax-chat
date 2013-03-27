<?php
?>

<ul class="chat-entries">
  <?php foreach ($elements as $element): ?>
    <li>
      <span class="name"><?php print check_plain($element['name']); ?></span>
      <span class="message"><?php print check_plain($element['body']); ?></span>
    </li>
  <?php endforeach; ?>
</ul>
