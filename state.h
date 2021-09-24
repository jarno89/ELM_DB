//
// Created by jarno on 17/06/2020.
//

#include <stdbool.h>
#include "data_types.h"

#ifndef ELM_STATE_H
#define ELM_STATE_H

state_t* init_state();

void print_state(const state_t* state);

state_t* copy_state(const state_t* state);

cluster_t* nearest_cluster_to_point(const state_t* state, const double* data_point);
cluster_t* nearest_cluster_to_cluster(const state_t* state, const cluster_t* cluster);

cluster_t* nearest_weighted_cluster(const state_t* state, const double* data_point);

void add_cluster(state_t* state, cluster_t* cluster);

bool is_equal_state(const state_t* state_a, const state_t* state_b);

void update_state_score(state_t* state);

void remove_cluster_from_state(state_t* state, cluster_t* cluster);

void free_state(state_t* state);

state_linked_list_t* init_state_linked_list();

void print_state_linked_list(state_linked_list_t* list);

void add_state_to_list(state_linked_list_t* list, state_t* state);

void remove_duplicate_states(state_linked_list_t *state_list);

void sort_state_list(state_linked_list_t* state_list);

void free_state_node(state_node_t* state_node);

void free_state_linked_list(state_linked_list_t* state_list);

#endif //ELM_STATE_H
