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
#include <stdint.h>
#include <stdbool.h>

#define BLACK 0
#define RED   1

typedef enum bf_rbt_sts {
  BF_RBT_OK,
  BF_RBT_ERR,
  BF_RBT_NO_KEY,
  BF_RBT_KEY_EXISTS
} bf_rbt_sts_t;

typedef enum bf_rbt_node_direction {
  BF_RBT_LEFT_NODE,
  BF_RBT_RIGHT_NODE,
  BF_RBT_ROOT_NODE
} bf_rbt_node_direction_t;

typedef struct bf_rbt_node_t {
  uint32_t key;
  bool color;
  struct bf_rbt_node_t *left, *right, *parent;
  void *data;
} bf_rbt_node_t;

/*!
 * Create a tree node for the key
 *
 * @param key to be created
 * @param root parent node of new node to be created
 * @return pointer to newly created node
 */
bf_rbt_node_t *bf_create_rbt_node(uint32_t key, bf_rbt_node_t *root);

/*!
 * Perform a right rotation of node
 *
 * @param node around which rotation happens
 * @param rbt_head head ptr of rb-tree
 * @return pointer to node after rotation
 */
bf_rbt_node_t *bf_right_rotate_rbt_node(bf_rbt_node_t *node, bf_rbt_node_t **rbt_head);

/*!
 * Perform a left rotation of node
 *
 * @param node around which rotation happens
 * @param rbt_head head ptr of rb-tree
 * @return pointer to node after rotation
 */
bf_rbt_node_t *bf_left_rotate_rbt_node(bf_rbt_node_t *node, bf_rbt_node_t **rbt_head);

/*!
 * Call left/right rotate APIs based on parent-child node direction
 *
 * @param parent node based on which direction of rotation is evaluated
 * @param rbt_head head ptr of rb-tree
 * @param key of child node
 * @return pointer to node after rotation
 */
bf_rbt_node_t *bf_perform_rotation(bf_rbt_node_t *parent, bf_rbt_node_t **rbt_head, uint32_t key);

/*!
 * Fix RBT node colors as part of balancing tree
 *
 * @param parent node based on which direction of rotation is evaluated
 * @param rbt_head head ptr of rb-tree
 * @return void
 */
void bf_color_fix_rbt_nodes(bf_rbt_node_t *parent, bf_rbt_node_t **rbt_head);

/*!
 * Retrieve color of neighbor node
 *
 * @param root node for which it is necessary to determine its neighboring node color.
 * @return color of neighbor node RED/BLACK
 */
bool bf_get_rbt_neigh_color(bf_rbt_node_t *root);

/*!
 * Retrieve neighbor node for given node
 *
 * @param root node for which it is necessary to determine its neighboring node.
 * @return pointer to neighbor node
 */
bf_rbt_node_t *bf_get_neighbor_rbt_node(bf_rbt_node_t *root);

/*!
 * Retrieve direction of given node
 *
 * @param root child node for which it is necessary to determine its direction.
 * @return direction left/right child
 */
bf_rbt_node_direction_t bf_get_rbt_node_direction(bf_rbt_node_t *root);

/*!
 * Retrieve inorder successor node for given node
 *
 * @param root node for which it is necessary to determine its successor node.
 * @return pointer to successor node
 */
bf_rbt_node_t *bf_get_successor_rbt_node(bf_rbt_node_t *root);

/*!
 * Retrieve inorder predecessor node for given node
 *
 * @param root node for which it is necessary to determine its predecessor node.
 * @return pointer to predecessor node
 */
bf_rbt_node_t *bf_get_predecessor_rbt_node(bf_rbt_node_t *root);

/*!
 * Insert a tree node into RBT with given key
 *
 * @param key to be inserted into RBT
 * @param root parent node of new node to be created
 * @param rbt_head head ptr of rb-tree
 * @return pointer to newly created node
 */
bf_rbt_node_t *bf_insert_rbt_entry(bf_rbt_node_t *root, uint32_t key, bf_rbt_node_t **rbt_head);

/*!
 * Rtrieve node which is just lesser than or equal to given key
 *
 * @param key for which lower bound has to be found
 * @param rbt_head head ptr of rb-tree
 * @return pointer to lower bound node
 */
bf_rbt_node_t *bf_get_lower_bound(uint32_t key, bf_rbt_node_t *rbt_head);

/*!
 * Retrieve node which is just greater than or equal to given key
 *
 * @param key for which upper bound has to be found
 * @param rbt_head head ptr of rb-tree
 * @return pointer to upper bound node
 */
bf_rbt_node_t *bf_get_upper_bound(uint32_t key, bf_rbt_node_t *rbt_head);

/*!
 * Retrieve node with highest key in rb-tree
 *
 * @param rbt_head head ptr of rb-tree
 * @return pointer to highest key node
 */
bf_rbt_node_t *bf_get_highest_key_node(bf_rbt_node_t *rbt_head);

/*!
 * Retrieve node with lowest key in rb-tree
 *
 * @param rbt_head head ptr of rb-tree
 * @return pointer to lowest key node
 */
bf_rbt_node_t *bf_get_lowest_key_node(bf_rbt_node_t *rbt_head);

/*!
 * Perform BST node deletion for given key and update color information
 *
 * @param key to be deleted from RBT
 * @param rbt_head head ptr of rb-tree
 * @param color address where color needs to be updated
 * @return node which has to be deleted
 */
bf_rbt_node_t *bf_bst_node_deletion(uint32_t key, bf_rbt_node_t *rbt_head, int *color);

/*!
 * Remove a tree node from RBT with given key
 *
 * @param key of node to be deleted from RBT
 * @param rbt_head head ptr of rb-tree
 * @return status of API call
 */
int bf_remove_rbt_entry(uint32_t key, bf_rbt_node_t **rbt_head);

/*!
 * Balance RBT after insertion of new node
 *
 * @param root parent of the node which got inserted
 * @param rbt_head head ptr of rb-tree
 * @param key of inserted node
 * @return void
 */
void bf_balance_rbt_post_insertion(bf_rbt_node_t *root, bf_rbt_node_t **rbt_head, uint32_t key);

/*!
 * Balance RBT after deletion of a node
 *
 * @param node node to be deleted from RBT
 * @param rbt_head head ptr of rb-tree
 * @return void
 */
void bf_balance_rbt_post_deletion(bf_rbt_node_t *node, bf_rbt_node_t **rbt_head);
