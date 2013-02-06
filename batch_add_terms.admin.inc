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
  $terms_names = trim($form_state['values']['terms']);
  $terms_names_array = explode("\n", $terms_names);
  $parent_terms = array();
  
  foreach ($terms_names_array as $term_name) {
    $term_depth = 0;
    if (preg_match('#^(-+)(.+)#', $term_name, $matches)) {
      $term_depth = drupal_strlen($matches[1]);
      $term_name = $matches[2];
    }
    
    $term = (object)array(
      'vid' => $vocabulary->vid,
      'name' => trim($term_name),
      'parent' => $term_depth ? $parent_terms[$term_depth - 1] : 0,
    );
    taxonomy_term_save($term);
    
    $parent_terms[$term_depth] = $term->tid;
  }
  
  drupal_set_message(t('Added @count new terms', array('@count' => count($terms_names_array))));
  $form_state['redirect'] = 'admin/structure/taxonomy/' . $vocabulary->machine_name;
}