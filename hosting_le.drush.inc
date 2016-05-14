<?php

/**
 * Implements hook_hosting_site_context_options().
 */
function hosting_le_hosting_site_context_options(&$task) {
  if (isset($task->ref) && !empty($task->ref)) {
    // $task->context_options['site_data'] = $task->ref->site_data;
    // ddl($task->ref);
  }
}

/**
 * Implements hook_drush_context_import().
 */
function hosting_le_drush_context_import($context, &$node) {
  drush_log('[hosting_le] Context import');
  // if ($context->type == 'site') {
  //   if (isset($context->site_data) && !empty($context->site_data)) {
  //     $node->site_data = $context->site_data;
  //   }
  // }
}
