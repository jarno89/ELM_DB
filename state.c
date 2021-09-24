//
// Created by jarno on 17/06/2020.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cluster.h"
#include "state.h"
#include "util.h"

#define MAX_CLUSTERS_PRINTED 100
#define MAX_STATES_PRINTED 200

state_t* init_state() {
    //create empty state
    state_t* state = malloc(sizeof(state_t));
    state->score = -1;
    state->clusters.size = 0;
    state->clusters.cluster_head = NULL;
    return state;
}

void print_state(const state_t* state) {
    //print state properties
    printf("State (score: %.2f):\n", state->score);

    int cluster_cnt = 0;
    cluster_node_t* cluster_node = state->clusters.cluster_head;
    while (cluster_node != NULL) {
        cluster_cnt++;
        if (cluster_cnt > MAX_CLUSTERS_PRINTED) {
            printf("  ... (%d more clusters)\n", state->clusters.size - MAX_CLUSTERS_PRINTED);
            break;
        }

        //print properties of clusters in state
        print_cluster(cluster_node->cluster);
        cluster_node = cluster_node->next;
    }

}

state_t* copy_state(const state_t* state) {
    //create new state
    state_t* new_state = malloc(sizeof(state_t));
    new_state->score = state->score;
    new_state->clusters.size = state->clusters.size;

    //check if cluster_head is empty
    if (state->clusters.cluster_head == NULL) {
        new_state->clusters.cluster_head = NULL;
        return new_state;
    }
    new_state->clusters.cluster_head = malloc(sizeof(cluster_node_t));

    //copy first cluster node of linked list
    cluster_node_t* node = state->clusters.cluster_head;
    cluster_node_t* new_node = new_state->clusters.cluster_head;

    new_node->cluster = copy_cluster(node->cluster);

    //copy next cluster nodes in linked list
    while (node->next != NULL) {
        new_node->next = malloc(sizeof(cluster_node_t));

        node = node->next;
        new_node = new_node->next;

        new_node->cluster = copy_cluster(node->cluster);
    }

    new_node->next = NULL;
    return new_state;
}

cluster_t* nearest_cluster_to_point(const state_t* state, const double* data_point) {
    cluster_node_t* curr = state->clusters.cluster_head;

    cluster_t* nearest = NULL;
    double min_dist = INFINITY;

    while (curr != NULL) {
        double dist = distance_vectors(curr->cluster->center, data_point);
        if (dist < min_dist) {
            nearest = curr->cluster;
            min_dist = dist;
        }

        curr = curr->next;
    }
    return nearest;
}

cluster_t* nearest_cluster_to_cluster(const state_t* state, const cluster_t* cluster) {
    cluster_node_t* curr = state->clusters.cluster_head;

    cluster_t* nearest = NULL;
    double min_dist = INFINITY;

    while (curr != NULL) {
        if (curr->cluster != cluster) {
            double dist = distance_vectors(curr->cluster->center, cluster->center);
            if (dist < min_dist) {
                nearest = curr->cluster;
                min_dist = dist;
            }
        }

        curr = curr->next;
    }
    return nearest;
}

cluster_t* nearest_weighted_cluster(const state_t* state, const double* data_point) {
    cluster_node_t* curr = state->clusters.cluster_head;

    cluster_t* nearest = NULL;
    double min_dist = INFINITY;

    while (curr != NULL) {
        double dist = distance_vectors(curr->cluster->center, data_point) - curr->cluster->avg_norm;
        if (dist < min_dist) {
            nearest = curr->cluster;
            min_dist = dist;
        }

        curr = curr->next;
    }
    return nearest;
}

void add_cluster(state_t* state, cluster_t* cluster) {
    cluster_node_t* node = malloc(sizeof(cluster_node_t));
    node->cluster = cluster;
    node->next = state->clusters.cluster_head;
    state->clusters.cluster_head = node;
    state->clusters.size++;
}

bool cluster_in_state(const cluster_t* cluster, const state_t* state) {
    cluster_node_t* node = state->clusters.cluster_head;
    while (node != NULL) {
        if (is_equal_cluster(node->cluster, cluster)) {
            return true;
        }
        node = node->next;
    }
    return false;
}

bool is_equal_state(const state_t* state_a, const state_t* state_b) {
    //same pointer -> must be the same state
    if (state_a == state_b) {
        return true;
    }

    //compare number of clusters
    if (state_a->clusters.size != state_b->clusters.size) {
        return false;
    }

    //check that each cluster from state_a is in state_b
    cluster_node_t* node_a = state_a->clusters.cluster_head;
    while (node_a != NULL) {
        if (!cluster_in_state(node_a->cluster, state_b)) {
            return false;
        }
        node_a = node_a->next;
    }

    //check that each cluster from state_b is in state_a
    cluster_node_t* node_b = state_b->clusters.cluster_head;
    while (node_b != NULL) {
        if (!cluster_in_state(node_b->cluster, state_a)) {
            return false;
        }
        node_b = node_b->next;
    }

    //states are equal
    return true;
}

void update_state_score(state_t* state) {
    if (state->clusters.size <= 1) {
        state->score = -1;
        return;
    }

    double s_sum = 0;
    unsigned int total_size = 0;
    cluster_node_t* node_i = state->clusters.cluster_head;
    while (node_i != NULL) {
        double a = node_i->cluster->avg_norm;
        double b_min = INFINITY;

        cluster_node_t* node_j = state->clusters.cluster_head;
        while (node_j != NULL) {
            if (node_i == node_j) {
                node_j = node_j->next;
                continue;
            }
            //determine distance to nearest cluster center
            double b = distance_vectors(node_i->cluster->center, node_j->cluster->center);
            if (b < b_min) {
                b_min = b;
            }

            node_j = node_j->next;
        }
        // calculate silhouette score of each cluster and weighted average silhouette score
        s_sum += ((b_min - a) / fmax(b_min, a)) * node_i->cluster->size;

        total_size += node_i->cluster->size;

        node_i = node_i->next;
    }

    state->score = s_sum / total_size;
}

void remove_cluster_from_state(state_t* state, cluster_t* cluster) {
    if (state->clusters.cluster_head == NULL) {
        return;
    }
    //check first node in list
    if (state->clusters.cluster_head->cluster == cluster) {
        cluster_node_t* remove_node = state->clusters.cluster_head;
        state->clusters.cluster_head = remove_node->next;
        state->clusters.size--;

        remove_node->next = NULL;
        free_cluster_node(remove_node);
        return;
    }

    //check next elements in list
    cluster_node_t* cn = state->clusters.cluster_head;
    while (cn != NULL) {
        if (cn->next->cluster == cluster) {
            cluster_node_t* remove_node = cn->next;
            cn->next = remove_node->next;
            state->clusters.size--;

            remove_node->next = NULL;
            free_cluster_node(remove_node);
            return;
        }
        cn = cn->next;
    }
}

void free_state(state_t* state) {
    if (state->clusters.cluster_head != NULL) {
        free_cluster_node(state->clusters.cluster_head);
    }
    free(state);
}

state_linked_list_t* init_state_linked_list() {
    state_linked_list_t* state_list = malloc(sizeof(state_linked_list_t));
    state_list->state_head = NULL;
    state_list->size = 0;
    return state_list;
}

void print_state_linked_list(state_linked_list_t* list) {
    int state_cnt = 0;
    state_node_t* state_node = list->state_head;
    while (state_node != NULL) {
        state_cnt++;
        if (state_cnt >= MAX_STATES_PRINTED) {
            printf("... (%d more states)\n", list->size - MAX_STATES_PRINTED);
            break;
        }

        print_state(state_node->state);
        state_node = state_node->next;
    }

}

void add_state_to_list(state_linked_list_t* list, state_t* state) {
    state_node_t* new_node = malloc(sizeof(state_node_t));
    new_node->state = state;
    new_node->next = list->state_head;

    list->state_head = new_node;
    list->size++;
}

void remove_duplicate_states(state_linked_list_t *state_list) {
    state_node_t* curr_node_a = state_list->state_head;

    while (curr_node_a != NULL) {
        state_node_t* prev_node_b = curr_node_a;
        state_node_t* curr_node_b = prev_node_b->next;

        while (curr_node_b != NULL) {
            if (is_equal_state(curr_node_a->state, curr_node_b->state)) {
                //remove duplicate state curr_node_b from list
                prev_node_b->next = curr_node_b->next;
                curr_node_b->next = NULL;
                free_state(curr_node_b->state);
                free(curr_node_b);
                state_list->size--;
            } else {
                prev_node_b = prev_node_b->next;
            }
            curr_node_b = prev_node_b->next;
        }
        curr_node_a = curr_node_a->next;
    }
}

void sort_state_list(state_linked_list_t* state_list) {
    //selection sort: find highest score in remaining list and swap with current position
    state_node_t* node_a = state_list->state_head;

    while (node_a != NULL) {

        double score_max = -INFINITY;
        state_node_t* node_max = NULL;

        state_node_t* node_b = node_a->next;
        while (node_b != NULL) {
            if (node_b->state->score > score_max) {
                node_max = node_b;
                score_max = node_max->state->score;
            }

            node_b = node_b->next;
        }

        //switch states when higher score found in list
        if ((score_max > node_a->state->score) && (node_max != NULL)) {
            state_t* state = node_a->state;
            node_a->state = node_max->state;
            node_max->state = state;
        }

        node_a = node_a->next;
    }
}

void free_state_node(state_node_t* state_node) {
    state_node_t* curr_node = state_node;
    state_node_t* prev_node;

    free_state(curr_node->state);
    while (curr_node->next != NULL) {
        prev_node = curr_node;
        curr_node = curr_node->next;
        free(prev_node);
        free_state(curr_node->state);
    }
    prev_node = curr_node;
    free(prev_node);
}

void free_state_linked_list(state_linked_list_t* state_list) {
    if (state_list->state_head != NULL) {
        free_state_node(state_list->state_head);
    }
    free(state_list);
}
