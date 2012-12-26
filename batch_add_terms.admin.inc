<?php

/**
 * Form for batch add terms.
 */
function batch_add_terms_form($form, $form_state, $vocabulary) {
  $form['terms'] = array(
    '#type' => 'textarea',
    '#title' => t('Terms'),
    '#description' => t('Enter one term name per line.'),
    '#required' => TRUE,
    '#rows' => 15,
  );

  $form['actions'] = array(
    '#type' => '#actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  
  return $form;
}

/**
 * Batch add terms.
 */
function batch_add_terms_form_submit($form, &$form_state) {
  $vocabulary = $form_state['build_info']['args'][0];
  $terms = trim($form_state['values']['terms']);
  $terms_array = explode("\n", $terms);
  foreach ($terms_array as $term_name) {
    $term = (object)array(
      'vid' => $vocabulary->vid,
      'name' => trim($term_name),
    );
    taxonomy_term_save($term);
  }
  drupal_set_message(t('Added @count new terms', array('@count' => count($terms_array))));
  $form_state['redirect'] = 'admin/structure/taxonomy/' . $vocabulary->machine_name;
}