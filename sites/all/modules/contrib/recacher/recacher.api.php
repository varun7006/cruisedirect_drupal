<?php

/**
 * @file
 * This file contains no working PHP code; it exists to provide additional
 * documentation for doxygen as well as to document hooks in the standard
 * Drupal manner.
 */

/**
 * @defgroup recacher_hooks cache warming's hooks
 * @{
 * Hooks that can be implemented by other modules in order to extend Recacher.
 */

/**
 * Alter URLs coming from hook_expire_cache() before they are saved or scraped.
 *
 * @param array $urls
 *   List of internal paths and/or absolute URLs that should be flushed.
 *   Example of array (when base url include option is enabled):
 *     array(
 *       'node/1' => 'http://example.com/node/1',
 *       'news' => 'http://example.com/news',
 *     );
 *   Example of array (when base url include option is disabled):
 *     array(
 *       'node/1' => 'node/1',
 *       'news' => 'news',
 *     );
 * @param array $wildcards
 *   Array with wildcards implementations for each internal path.
 *   Indicates whether should be executed wildcard cache flush.
 *   Example:
 *     array(
 *       'node/1' => FALSE,
 *       'news' => TRUE,
 *     );
 * @param string $object_type
 *  Name of object type ('node', 'comment', 'user', etc).
 * @param object $object
 *   Object (node, comment, user, etc) for which expiration is executes.
 */
function hook_recacher_urls_alter(&$urls, $wildcards, $object_type, $object) {
  // Keep a copy of the top arguments appended to the page being targetted.
  $top_arguments = variable_get('my_module_top_args', array('blue', 'purple', 'green'));

  // Loop over each of the wildcards, if any are enabled then replace that URL
  // with versions that have each of the top arguments appended.
  foreach ($wildcards as $path => $wildcard_enabled) {
    // Only deal with paths that have the wildcard option enabled.
    if ($wildcard_enabled) {
      // Get the (possibly) verbose path.
      $full_path = $urls[$path];
      // Delete the existing record.
      unset($urls[$path]);
      // Create replacement paths with the mostly commonly used
      foreach ($top_arguments as $argument) {
        $urls[$path . '/' . $argument] = $full_path . '/' . $argument;
      }
    }
  }
}

/**
 * Alter HTTPRL request options.
 *
 * @param array $recacher_role_options
 *   A keyed array of HTTPRL request options array containing:
 *    - headers  (array)
 *    - timeout  (int)
 *    - blocking (bool)
 *   Each new key will trigger an additional httprl call.
 *
 * @see httprl_request()
 */
function hook_recacher_role_options_alter(&$recacher_role_options) {
  // Add another layer of cache for authenticated users.
  $recacher_role_options['authenticated'] = $recacher_role_options['anonymous'];
  $recacher_role_options['authenticated']['headers']['Cookie'] = "SESS1e76b28=jTXaYtV";
}

/**
 * Alter HTTPRL request options.
 *
 * Deprecated, use hook_recacher_role_options_alter() instead.
 */
function hook_recacher_role_options($recacher_role_options) {
  // Add another layer of cache for authenticated users.
  $recacher_role_options['authenticated'] = $recacher_role_options['anonymous'];
  $recacher_role_options['authenticated']['headers']['Cookie'] = "SESS1e76b28=jTXaYtV";
  
  return $recacher_role_options;
}

/**
 * @}
 */
