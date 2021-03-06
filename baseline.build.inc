<?php

/**
 * @file
 * Baseline build functions.
 */

/**
 * Creates aliases
 *
 * @param array $alias_info
 *  Array containing alias info.
 */
function baseline_build_aliases($alias_info) {
  foreach ($alias_info as $source => $alias_language_info) {
    foreach ($alias_language_info as $langcode => $alias) {
      baseline_api_create_path_alias($source, $alias, $langcode);
    }
  }
}

/**
 * Creates blocks.
 *
 * @param array $blocks_info
 *   Blocks info.
 */
function baseline_build_blocks($blocks_info) {
  foreach ($blocks_info as $block_info) {
    $is_custom_block = (isset($block_info['custom']) && $block_info['custom'] === TRUE) ? TRUE : FALSE;
    if ($is_custom_block) {
      baseline_api_create_custom_block($block_info);
    }
    else {
      baseline_api_create_block($block_info);
    }
  }
}

/**
 * Creates nodes.
 *
 * @param array $nodes
 *   Nodes info.
 */
function baseline_build_nodes($nodes) {
  foreach ($nodes as $node) {
    baseline_api_create_node($node);
  }
}

/**
 * Creates menus.
 *
 * @param array
 *   Menus info.
 */
function baseline_build_menus($menus) {
  foreach ($menus as $menu_info) {
    $query = db_merge('menu_custom')
                ->key(array('menu_name' => $menu_info['menu_name']))
                ->fields(array(
                    'menu_name'   => $menu_info['menu_name'],
                    'title'       => $menu_info['title'],
                    'description' => $menu_info['description'],
                    'language'    => $menu_info['language'],
                    'i18n_mode'   => $menu_info['i18n_mode'],
                 ))
                ->execute();
  }
}

/**
 * Builds date formats.
 *
 * @param array $formats
 *   List of date formats.
 */
function baseline_build_date_formats($formats) {
  foreach ((array) $formats as $format) {
    baseline_api_create_date_format($format);
  }
}

/**
 * Builds date types.
 *
 * @param array $types
 *   List of date types.
 */
function baseline_build_date_types($types) {
  foreach ((array) $types as $type) {
    baseline_api_create_date_type($type);
  }
}

/**
 * Builds menu links.
 *
 * @param array $menu_links_info
 *   Menu links info.
 */
function baseline_build_menu_links($menu_links_info) {
  foreach ($menu_links_info as $menu_link_info) {
    baseline_api_create_menu_link($menu_link_info);
  }
}

/**
 * Builds general config.
 *
 * @param array $variables
 *   Variables. info.
 */
function baseline_build_variables($variables) {
  foreach ((array) $variables as $name => $value) {
    variable_set($name, $value);
  }
}

/**
 * Creates user roles.
 *
 * This will create a variable 'baseline_user_role_ids' containing an array
 * mapping the keys of the roles passed to this funtion to the generated role
 * ids.
 *
 * @param array $roles
 *   An array of user roles to create.
 *
 * @see hook_baseline_info_user_roles().
 */
function baseline_build_user_roles($roles) {
  // Save the role ids so we can save them to a variable.
  $role_id_mapping = array();
  foreach ($roles as $role_name => $role) {
    $role = (object) $role;
    // Save the role to the database.
    user_role_save($role);
    $role_id_mapping[$role_name] = $role->rid;
  }
  // Set the variable for being able to access the role ids generated for the
  // roles later in the build process.
  variable_set('baseline_user_role_ids', $role_id_mapping);
}

/**
 * Assigns user permissions to roles.
 *
 * @param array $roles
 *   For each role an array of permissions to assign to that role. The array
 *   is keyed by role id or machine name.
 *
 * @see hook_baseline_info_user_permissions()
 */
function baseline_build_user_permissions($roles) {
  // Fetch the role ids set up by hook_baseline_info_user_permissions().
  $role_ids = variable_get('baseline_user_role_ids', array());
  foreach ($roles as $rid => $permissions) {
    // Find out the role id if a machine name was used.
    if (!is_numeric($rid) && isset($role_ids[$rid])) {
      $rid = $role_ids[$rid];
    }
    // Save the permissions for this role..
    user_role_grant_permissions($rid, $permissions);
  }
}

/**
 * Create nodequeues.
 *
 * @param array $nodeqeueus
 *   An array of nodeqeues, keyed by machine name.
 */
function baseline_build_nodequeues($nodequeues) {
  foreach ($nodequeues as $nodequeue) {
    $nodequeue_object = (object) $nodequeue;
    $qid = nodequeue_save($nodequeue_object);
  }
}
