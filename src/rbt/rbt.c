/*
 * Copyright(c) 2024 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <target-sys/bf_sal/bf_sys_intf.h>
#include <target-utils/rbt/rbt.h>

bf_rbt_node_t *bf_create_rbt_node(uint32_t key, bf_rbt_node_t *root) {
  bf_rbt_node_t *new_node = (bf_rbt_node_t *)bf_sys_calloc(1, sizeof(bf_rbt_node_t));
  if (new_node == NULL)
    return NULL;
  new_node->key = key;
  new_node->color = RED;
  new_node->left = new_node->right = NULL;
  new_node->parent = root;
  return new_node;
}

bf_rbt_node_t *bf_right_rotate_rbt_node(bf_rbt_node_t *node, bf_rbt_node_t **rbt_head) {
  bf_rbt_node_t *g_parent = node->parent;
  bf_rbt_node_t *l_child = node->left;
  bf_rbt_node_t *g_child = l_child->right;
  bool c_color = l_child->color;
  bool p_color = node->color;

  l_child->right = node;
  l_child->parent = g_parent;
  node->left = g_child;
  node->parent = l_child;
  l_child->color = p_color;
  node->color = c_color;

  if (g_child != NULL)
    g_child->parent = node;

  if (g_parent != NULL) {
    if (g_parent->key < l_child->key)
      g_parent->right = l_child;
    else
      g_parent->left = l_child;
  }

  if (node == *rbt_head)
    *rbt_head = l_child;

  return l_child;
}

bf_rbt_node_t *bf_left_rotate_rbt_node(bf_rbt_node_t *node, bf_rbt_node_t **rbt_head) {
  bf_rbt_node_t *g_parent = node->parent;
  bf_rbt_node_t *r_child = node->right;
  bf_rbt_node_t *g_child = r_child->left;
  bool p_color = node->color;
  bool c_color = r_child->color;

  r_child->left = node;
  r_child->parent = g_parent;
  node->right = g_child;
  node->parent = r_child;
  r_child->color = p_color;
  node->color = c_color;

  if (g_child != NULL)
    g_child->parent = node;

  if (g_parent != NULL) {
    if (g_parent->key < r_child->key)
      g_parent->right = r_child;
    else
      g_parent->left = r_child;
  }

  if (node == *rbt_head)
    *rbt_head = r_child;

  return r_child;
}

bf_rbt_node_t *bf_perform_rotation(bf_rbt_node_t *parent, bf_rbt_node_t **rbt_head, uint32_t key) {
  bf_rbt_node_t *g_parent = parent->parent;
  /* If g_parent->key < parent->key,
   * then parent is right child to grand parent
   */
  if (g_parent->key < parent->key) {
    /* If parent->key < key, then new node is right child to parent
     * Perform left rotation
     */
    if (parent->key < key) {
      parent = bf_left_rotate_rbt_node(g_parent, rbt_head);
    }
    /* If new node is left child to parent. It needs 2 rotations.
     * Right rotate parent, left rotate grand parent node
     */
    else {
      parent = bf_right_rotate_rbt_node(parent, rbt_head);
      parent = bf_left_rotate_rbt_node(g_parent, rbt_head);
    }
  }
  /* If g_parent->key > parent->key,
   * then parent is left child to grand parent
   */
  else {
    /* If parent->key < key, then new node is right child to parent
     * Left rotate parent, right rotate grand parent node
     */
    if (parent->key < key) {
      parent = bf_left_rotate_rbt_node(parent, rbt_head);
      parent = bf_right_rotate_rbt_node(g_parent, rbt_head);
    }
    /* If new node is left child to parent
     * Perform right rotation
     */
    else {
      parent = bf_right_rotate_rbt_node(g_parent, rbt_head);
    }
  }
  return parent;
}

bf_rbt_node_t *bf_get_successor_rbt_node(bf_rbt_node_t *root) {
  bf_rbt_node_t *successor = root->right;
  if (successor == NULL)
    return NULL;
  while (successor->left != NULL)
    successor = successor->left;
  return successor;
}

bf_rbt_node_t *bf_get_predecessor_rbt_node(bf_rbt_node_t *root) {
  bf_rbt_node_t *predecessor = root->left;
  if (predecessor == NULL)
    return NULL;
  while (predecessor->right != NULL)
    predecessor = predecessor->right;
  return predecessor;
}

bf_rbt_node_t *bf_get_lower_bound(uint32_t key, bf_rbt_node_t *rbt_head) {
  bf_rbt_node_t *lower_bound = NULL;
  bf_rbt_node_t *root = rbt_head;

  while (root != NULL) {
    if (root->key == key) {
      return root;  // Exact match found
    } else if (root->key > key) {
      root = root->left;
    } else {
      lower_bound = root;  // Update result and move to the right subtree
      root = root->right;
    }
  }

  return lower_bound;
}

bf_rbt_node_t *bf_get_upper_bound(uint32_t key, bf_rbt_node_t *rbt_head) {
  bf_rbt_node_t *upper_bound = NULL;
  bf_rbt_node_t *root = rbt_head;

  while (root != NULL) {
    if (root->key == key) {
      return root;  // Exact match found
    } else if (root->key < key) {
      root = root->right;
    } else {
      upper_bound = root;  // Update result and move to the left subtree
      root = root->left;
    }
  }

  return upper_bound;
}

bf_rbt_node_t *bf_get_highest_key_node(bf_rbt_node_t *rbt_head) {
    bf_rbt_node_t *current = rbt_head;
    if (current == NULL)
      return NULL;

    // Traverse to the rightmost node
    while (current->right != NULL) {
        current = current->right;
    }

    return current;
}

bf_rbt_node_t *bf_get_lowest_key_node(bf_rbt_node_t *rbt_head) {
    bf_rbt_node_t *current = rbt_head;
    if (current == NULL)
      return NULL;

    // Traverse to the leftmost node
    while (current->left != NULL) {
        current = current->left;
    }

    return current;
}

bf_rbt_node_direction_t bf_get_rbt_node_direction(bf_rbt_node_t *root) {
  if (root->parent == NULL)
    return BF_RBT_ROOT_NODE;
  if (root->parent->key < root->key ||
      (root->parent->right != NULL && root->parent->right->key == root->key))
    return BF_RBT_RIGHT_NODE;
  return BF_RBT_LEFT_NODE;
}

bool bf_get_rbt_neigh_color(bf_rbt_node_t *root) {
  bf_rbt_node_t *parent = root->parent;
  if (parent == NULL)
    return BLACK;
  if ((parent->right != NULL && parent->right->key == root->key) ||
      parent->key < root->key) {
    if (parent->left != NULL)
      return parent->left->color;
  } else if ((parent->left != NULL && parent->left->key == root->key) ||
      parent->key > root->key) {
    if (parent->right != NULL)
      return parent->right->color;
  }
  return BLACK;
}

bf_rbt_node_t *bf_get_neighbor_rbt_node(bf_rbt_node_t *root) {
  bf_rbt_node_t *parent = root->parent;
  if (parent == NULL)
    return NULL;
  if ((parent->right != NULL && parent->right->key == root->key) ||
      parent->key < root->key) {
    return parent->left;
  } else if ((parent->left != NULL && parent->left->key == root->key) ||
       parent->key > root->key) {
    return parent->right;
  }
  return NULL;
}

void bf_color_fix_rbt_nodes(bf_rbt_node_t *root, bf_rbt_node_t **rbt_head) {
  if (root->color == BLACK)
    return;
  bf_rbt_node_t *parent = root->parent;
  bf_rbt_node_t *sibling;
  if(parent->key < root->key) {
    sibling = parent->left;
  } else {
    sibling = parent->right;
  }
  root->color = sibling->color = BLACK;
  if (parent->parent == NULL) {
    parent->color = BLACK;
    return;
  } else {
    parent->color = RED;
    //Color fixing should be done from current node to top until reach root node
    bf_balance_rbt_post_insertion(parent->parent, rbt_head, root->key);
  }
}

void bf_balance_rbt_post_insertion(bf_rbt_node_t *root, bf_rbt_node_t **rbt_head, uint32_t key) {
  bool neigh_color;
  if (root->color == RED) {
    neigh_color = bf_get_rbt_neigh_color(root);
    /* If neighbor node color is RED, perform
     * recoloring upper node to balance RB Tree
     */
    if (neigh_color == RED)
      bf_color_fix_rbt_nodes(root, rbt_head);
    /* If neighbor node color is BLACK, perform
     * rotation to balance the RB Tree
     */
    else
      bf_perform_rotation(root, rbt_head, key);
  }
}

bf_rbt_node_t *bf_insert_rbt_entry(bf_rbt_node_t *root, uint32_t key, bf_rbt_node_t **rbt_head) {
  bf_rbt_node_t *prev = root;
  bf_rbt_node_t *res_node;
  /* If tree is empty, create new node and update it's color to black
   * root node should be always black
   */
  if (root == NULL) {
    bf_rbt_node_t *new_node = bf_create_rbt_node(key, root);
    if (new_node == NULL)
      return NULL;
    new_node->color = BLACK;
    *rbt_head = new_node;
    return new_node;
  }
  while (root != NULL) {
    prev = root;
    if (root->key < key)
      root = root->right;
    else if (root->key > key)
      root = root->left;
    else
      break;
  }
  if (root == NULL)
    root = prev;
  if (root->key == key) {
    return root;
  } else if (root->key < key) {
    root->right = bf_create_rbt_node(key, root);
    if (root->right == NULL)
      return NULL;
    res_node = root->right;
    bf_balance_rbt_post_insertion(root, rbt_head, key);
  } else {
    root->left = bf_create_rbt_node(key, root);
    if (root->left == NULL)
      return NULL;
    res_node = root->left;
    bf_balance_rbt_post_insertion(root, rbt_head, key);
  }
  return res_node;
}

bf_rbt_node_t *bf_bst_node_deletion(uint32_t key, bf_rbt_node_t *rbt_head, int *color) {
  bf_rbt_node_t *root_node = rbt_head;
  bf_rbt_node_t *replacement;
  while (root_node != NULL && root_node->key != key) {
    if (root_node->key < key)
      root_node = root_node->right;
    else if (root_node->key > key)
      root_node = root_node->left;
  }
  while (root_node != NULL) {
    /* If the node be deleted is a leaf node, update
     * color and just return that node
     */
    if (root_node->right == NULL && root_node->left == NULL) {
      *color = root_node->color;
      return root_node;
    }
    /* If the node be deleted is an internal node, replace the value
     * in the node with either in-order successor/predecessor and return
     * in-order successor/predecessor. Repeat the same until reach leaf node
     */
    else {
      replacement = bf_get_successor_rbt_node(root_node);
      if (replacement == NULL)
        replacement = bf_get_predecessor_rbt_node(root_node);
      root_node->key = replacement->key;
      root_node->data = replacement->data;
      *color = replacement->color;
      root_node = replacement;
    }
  }
  return root_node;
}

void bf_balance_rbt_post_deletion(bf_rbt_node_t *node, bf_rbt_node_t **rbt_head) {
  bf_rbt_node_t *neigh_node;
  bool imbalance_tree = true;
  bf_rbt_node_direction_t node_dir;
  while (imbalance_tree != false && node != NULL) {
    if (node->parent == NULL) {
      imbalance_tree = false;
      break;
    }
    neigh_node = bf_get_neighbor_rbt_node(node);
    node_dir = bf_get_rbt_node_direction(node);
    if (neigh_node == NULL || neigh_node->color == BLACK) {
      /* Check if neigh_node is either NULL or
       * doesn't have a RED child
       */
      if (neigh_node == NULL || ((neigh_node->right == NULL || neigh_node->right->color == BLACK) &&
          (neigh_node->left == NULL || neigh_node->left->color == BLACK))) {
        if (neigh_node != NULL)
          neigh_node->color = RED;
        if (node->parent->color == RED) {
          node->parent->color = BLACK;
          imbalance_tree = false;
        }
        else {
          node = node->parent;
        }
      }
      /* Check if far child from current node is RED which means
       * right child of neighbor is RED if current node is in the left
       */
      else if (node_dir == BF_RBT_LEFT_NODE && neigh_node->right != NULL && neigh_node->right->color == RED) {
        bf_left_rotate_rbt_node(node->parent, rbt_head);
        neigh_node->right->color = BLACK;
        imbalance_tree = false;
      }
      /* Mirror copy of above case
       * left child of neighbor is RED if current node is in the right
       */
      else if (node_dir == BF_RBT_RIGHT_NODE && neigh_node->left != NULL && neigh_node->left->color == RED) {
        bf_right_rotate_rbt_node(node->parent, rbt_head);
        neigh_node->left->color = BLACK;
        imbalance_tree = false;
      }
      /* Check if far child from current node is BLACK and near child is RED which means
       * right child of neighbor is BLACK, left child of neighbor is RED if current node is in the left
       */
      else if (node_dir == BF_RBT_LEFT_NODE && (neigh_node->right == NULL || neigh_node->right->color == BLACK) &&
         (neigh_node->left != NULL && neigh_node->left->color == RED)) {
        bf_right_rotate_rbt_node(neigh_node, rbt_head);
      }
      /* Mirror copy of above case
       * right child of neighbor is RED, left child of neighbor is BLACK if current node is in the right
       */
      else if (node_dir == BF_RBT_RIGHT_NODE && (neigh_node->left == NULL || neigh_node->left->color == BLACK) &&
         (neigh_node->right != NULL && neigh_node->right->color == RED)) {
        bf_left_rotate_rbt_node(neigh_node, rbt_head);
      }
    } else {
      if (node_dir == BF_RBT_RIGHT_NODE)
        bf_right_rotate_rbt_node(node->parent, rbt_head);
      else
        bf_left_rotate_rbt_node(node->parent, rbt_head);
    }
  }
  if ((*rbt_head)->color == RED)
    (*rbt_head)->color = BLACK;
}

int bf_remove_rbt_entry(uint32_t key, bf_rbt_node_t **rbt_head) {
  bf_rbt_node_t *res_node, *parent;
  bf_rbt_node_direction_t child_dir;
  int color;
  //Apply traditional BST deletion and get the node to be deleted
  res_node = bf_bst_node_deletion(key, *rbt_head, &color);
  if (res_node == NULL)
    return BF_RBT_NO_KEY;
  /* If the retrieved node is black,
   * then RB Tree should be re-balanced
   */
  if (color == BLACK)
    bf_balance_rbt_post_deletion(res_node, rbt_head);
  parent = res_node->parent;
  if (parent == NULL) {
    *rbt_head = NULL;
  } else {
    // Update the parent of node to be deleted
    child_dir = bf_get_rbt_node_direction(res_node);
    if (child_dir == BF_RBT_LEFT_NODE) {
      parent->left = NULL;
    } else {
      parent->right = NULL;
    }
  }
  bf_sys_free(res_node);
  res_node = NULL;
  return BF_RBT_OK;
}
