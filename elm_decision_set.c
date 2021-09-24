//
// Created by jarno on 18/06/2020.
//

#include <math.h>
#include <stdio.h>

#include "cluster.h"
#include "elm_decision_set.h"
#include "state.h"
#include "util.h"

extern double radius;
extern unsigned int max_states;

void process_sample(state_linked_list_t* state_list, state_t* state, const double* data_point) {
    //find nearest cluster
    cluster_t* cluster_a = nearest_cluster_to_point(state, data_point);

    if (cluster_a == NULL) {
        //no cluster found, create first cluster
        add_cluster(state, init_cluster(data_point));
        return;
    }
    double distance_ca_point = distance_vectors(cluster_a->center, data_point);
    if (distance_ca_point >= (fmax(cluster_a->avg_norm, radius) + radius)) {
        //no cluster in range of data point: create new cluster
        add_cluster(state, init_cluster(data_point));
        return;
    }

#if DECISION_BRANCHING==1
    //cluster in range of data point
    if (distance_ca_point > 2.0 * radius) {
        //copy state and add cluster to new state
        state_t* new_state = copy_state(state);
        add_state_to_list(state_list, new_state);
        add_cluster(new_state, init_cluster(data_point));
    }
#endif

    add_point_to_cluster(cluster_a, data_point);
    cluster_t* cluster_b = nearest_cluster_to_cluster(state, cluster_a);

    if (cluster_b == NULL || cluster_b == cluster_a) {
        //no other cluster found
        return;
    }

    double distance_ca_cb = distance_vectors(cluster_a->center, cluster_b->center);
    if (distance_ca_cb >= (fmax(cluster_a->avg_norm, radius) + fmax(cluster_b->avg_norm, radius))) {
        //no cluster in range of cluster_a
        return;
    }

#if DECISION_BRANCHING==1
    // cluster in range of cluster_a
    if (distance_ca_cb > 2.0 * radius) {
        //copy state, continue with original state
        state_t* new_state = copy_state(state);
        add_state_to_list(state_list, new_state);
    }
#endif

    add_cluster(state, merge_clusters(cluster_a, cluster_b));
    remove_cluster_from_state(state, cluster_a);
    remove_cluster_from_state(state, cluster_b);
}

void fit_sample(state_linked_list_t* state_list, const double* data_point) {
    if (state_list->size == 0) {
        add_state_to_list(state_list, init_state());
    }
    state_node_t* curr_node = state_list->state_head;
    while (curr_node != NULL) {
        //newly created states are prepended to list
        process_sample(state_list, curr_node->state, data_point);
        curr_node = curr_node->next;
    }

#if DECISION_BRANCHING==1
    //remove duplicate states
    remove_duplicate_states(state_list);
#endif

    //update scores
    state_node_t* node = state_list->state_head;
    while (node != NULL) {
        update_state_score(node->state);
        node = node->next;
    }

#if DECISION_BRANCHING==1
    unsigned int n = max_states;
    if (state_list->size > n) {
        //sort on score
        sort_state_list(state_list);

        state_list->size = n;

        n--;
        node = state_list->state_head;
        for (; n > 0; n--) {
            if (node == NULL) {
                break;
            }
            node = node->next;
        }
        //keep only first n states with highest score
        state_node_t* remove_node = node->next;
        node->next = NULL;
        if (remove_node != NULL) {
            free_state_node(remove_node);
        }
    }
#endif
}

int predict_sample(state_linked_list_t* state_list, const double* data_point) {
    if (state_list->size == 0) {
        printf("No state found\n");
        return -1;
    }
    //find nearest cluster
    state_t* state = state_list->state_head->state;
    cluster_t* nearest = nearest_weighted_cluster(state, data_point);

    //determine index of nearest cluster
    int i = 0;
    cluster_node_t* curr = state->clusters.cluster_head;
    while (curr != NULL) {
        if (nearest == curr->cluster) {
            //cluster found, returning index
            return i;
        }
        curr = curr->next;
        i++;
    }

    printf("No cluster found\n");
    return -1;
}
