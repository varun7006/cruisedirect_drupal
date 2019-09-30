<?php

/**
 * @file
 * Settings form.
 */

/**
 * Config form shown at admin/config/system/recacher
 */
function recacher_admin($form, &$form_state) {

  // Do not leave on '' (=all) because on systems with many changed pages and
  // infrequent calls it would cause very long requests.
  $default_recacher_max_urls = '10';
  $form['recacher_max_urls'] = array(
    '#type' => 'textfield',
    '#title' => t('How many expired URLs to re-cache'),
    '#default_value' => variable_get('recacher_max_urls', $default_recacher_max_urls),
    '#size' => 3,
    '#maxlength' => 3,
    '#description' => t('Expired URLs are stored in a database table that acts as a queue for re-caching. This setting allows you to set the maximum number of URLs to re-cache at once. Leave empty if you wish to re-cache all expired URLs at once (not recommended as they may pile up if you have many changes and your cron does not run frequently enough). Default: %default_recacher_max_urls', array('%default_recacher_max_urls' => check_plain($default_recacher_max_urls))),
    '#required' => FALSE,
  );

  $times = array();
  $intervals = array('default', 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597);
  $defaultcroninterval = $intervals[0];
  foreach ($intervals as $interval) {
    $times[] = $interval;
  }
  $form['recacher_cron_interval'] = array(
    '#type' => 'select',
    '#title' => t('Minimum time between re-caching'),
    '#default_value' => variable_get('recacher_cron_interval', $defaultcroninterval),
    '#description' => t('Re-caching is always triggered by cron. Typically you would want to have this process run as often as possible so as to quickly warm recently expired caches. High frequency (even a minute, or less) has no downsides because the process will not do anything most of the time. (Of course, your cron has to be triggered at least at the same or higher frequency.) If you use Elysia Cron for more granular settings of your cron (recommended), set this value to "default". Specify the time in <em>minutes</em>. Default: %defaultcroninterval', array('%defaultcroninterval' => check_plain($defaultcroninterval))),
    '#options' => drupal_map_assoc($times),
    '#required' => TRUE,
  );

  $options = array('only unsuccessful', 'all', 'none');
  $defaultwatchdogreporting = $options[0];
  $form['recacher_watchdog'] = array(
    '#type' => 'select',
    '#title' => t('Watchdog reporting'),
    '#default_value' => variable_get('recacher_watchdog', $defaultwatchdogreporting),
    '#description' => t('Success of re-caching can be reported via watchdog. One can choose to only show URLs that failed to recache, or all URLs (useful while debugging), or switch off reporting completely. Default: %defaultwatchdog', array('%defaultwatchdog' => check_plain($defaultwatchdogreporting))),
    '#options' => drupal_map_assoc($options),
    '#required' => TRUE,
  );

  // See this issue: https://www.drupal.org/node/2828873
  $form['recrawler_hook_exit'] = array(
    '#type' => 'checkbox',
    '#title' => t('Try re-caching immediately'),
    '#description' => t('Start the re-crawling process at the end of the current page request (uses "hook_exit()"). In some cases it may be too early, e.g. if external caches such as Fastly or Akamai need to be expired (see https://www.drupal.org/node/2828873). Even if this is checked, re-caching at hook_cron() will run as well as a fallback.'),
    '#default_value' => variable_get('recrawler_hook_exit', FALSE),
  );

  $form['recacher_run_now'] = array(
    '#markup' => '<p>'. l(t('Re-cache immediately'), 'admin/config/system/recacher/crawl') . ': ' . t('click to warm caches of the expired pages immediately.') . '',
  );

  return system_settings_form($form);
}

/**
 * Redirect to the module's permission settings. A callback from hook_menu().
 */
function recacher_perms() {
  drupal_goto('admin/people/permissions', array('fragment' => 'module-recacher'));
}

/**
 * Wrapper for recacher_crawler().
 */
function recacher_crawler_run() {
  $recached = recacher_crawler();
  if (empty($recached)) {
    drupal_set_message(t('No URLs needed to be crawled.'));
  }
  else {
    drupal_set_message(format_plural($recached, '@count URL was crawled.', '@count URLs were crawled.'));
  }
  drupal_goto('admin/config/system/recacher');
}
