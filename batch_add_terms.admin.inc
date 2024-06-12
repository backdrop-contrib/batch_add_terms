<?php

/**
 * Form for batch add terms.
 */
function batch_add_terms_form($form, $form_state, $vocabulary) {
  $example = <<<HTML
  <pre>
  animals
  -canine
  --dog
  --wolf
  -feline
  --cat
  </pre>
  HTML;

  $form['terms'] = array(
    '#type' => 'textarea',
    '#title' => t('Terms'),
    '#description' => t('Enter one term name per line. Child terms can be prefixed with a dash "-" (one dash per hierarchy level). Terms that should not become child terms and start with a dash need to be wrapped in double quotes. Example: !example', array('!example' => $example)),
    '#required' => TRUE,
    '#rows' => 15,
  );

  $form['check_duplicates'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check for duplicates'),
    '#description' => t('Enable this option to skip a line if there is an existing term.'),
  );
  if (module_exists('language') && $vocabulary->language) {
    $form['langcode_term'] = array(
      '#title' => t('Add terms in language'),
      '#type' => 'select',
      '#options' => language_list(TRUE, TRUE),
      '#empty_value' => LANGUAGE_NONE,
      '#empty_option' => t('All languages'),
      '#default_value' => LANGUAGE_NONE,
      '#access' => module_exists('language') && $vocabulary->language == TAXONOMY_LANGUAGE_ENABLED,
      '#description' => t('Add terms for a given language. Terms with the "All" language will be shown in every language.'),
    );
  }

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
  $weight = 0;

  foreach ($terms_names_array as $term_name) {
    $term_depth = 0;
    if (preg_match('#^(-+)(.+)#', $term_name, $matches)) {
      $term_depth = backdrop_strlen($matches[1]);
      $term_name = $matches[2];
    }

    $term_name = trim($term_name);
    if ($form_state['values']['check_duplicates'] && taxonomy_term_load_multiple_by_name($term_name, $vocabulary->machine_name)) {
      continue;
    }

    $term_data = array(
      'vocabulary' => $vocabulary->machine_name,
      'name' => $term_name,
      'format' => filter_default_format(),
      'weight' => $weight++,
    );
    if ($term_depth) {
      $term_data['parent'] = array($parent_terms[$term_depth - 1]);
    }
    if (module_exists('language') && $vocabulary->language) {
      $term_data['langcode'] = $form_state['values']['langcode_term'];
    }

    $term = entity_create('taxonomy_term', $term_data);
    taxonomy_term_save($term);

    $parent_terms[$term_depth] = $term->tid;
  }

  backdrop_set_message(t('Added @count new terms', array('@count' => count($terms_names_array))));
  $form_state['redirect'] = 'admin/structure/taxonomy/' . $vocabulary->machine_name;
}

/**
 * Override theme_taxonomy_overview_vocabularies().
 */
function batch_add_terms_taxonomy_overview_vocabularies($variables) {
  $form = $variables['form'];

  $rows = array();

  foreach (element_children($form) as $key) {
    if (isset($form[$key]['name'])) {
      $vocabulary = &$form[$key];

      $row = array();
      $row[] = backdrop_render($vocabulary['name']);
      if (isset($vocabulary['weight'])) {
        $vocabulary['weight']['#attributes']['class'] = array('vocabulary-weight');
        $row[] = backdrop_render($vocabulary['weight']);
      }
      $row[] = backdrop_render($vocabulary['edit']);
      $row[] = backdrop_render($vocabulary['list']);
      $row[] = backdrop_render($vocabulary['add']);
      $row[] = backdrop_render($vocabulary['batch_add']);
      $rows[] = array('data' => $row, 'class' => array('draggable'));
    }
  }

  $header = array(t('Vocabulary name'));
  if (isset($form['actions'])) {
    $header[] = t('Weight');
    backdrop_add_tabledrag('taxonomy', 'order', 'sibling', 'vocabulary-weight');
  }
  $header[] = array('data' => t('Operations'), 'colspan' => '4');
  return theme('table', array('header' => $header, 'rows' => $rows, 'empty' => t('No vocabularies available. <a href="@link">Add vocabulary</a>.', array('@link' => url('admin/structure/taxonomy/add'))), 'attributes' => array('id' => 'taxonomy'))) . backdrop_render_children($form);
}
