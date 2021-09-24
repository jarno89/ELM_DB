//
// Created by jarno on 17/06/2020.
//
#define MAX_DIMENSION 128

#ifndef ELM_DATA_TYPES_H
#define ELM_DATA_TYPES_H

typedef struct {
    double center[MAX_DIMENSION];
    double lin_sum[MAX_DIMENSION];
    double sqr_sum[MAX_DIMENSION];
    unsigned int size;
    double avg_norm;
} cluster_t;

typedef struct cluster_node_t {
    cluster_t* cluster;
    struct cluster_node_t* next;
} cluster_node_t;

typedef struct {
    cluster_node_t* cluster_head;
    unsigned int size;
} cluster_linked_list_t;

typedef struct {
    cluster_linked_list_t clusters;
    double score;
} state_t;

typedef struct state_node_t {
    state_t* state;
    struct state_node_t* next;
} state_node_t;

typedef struct {
    state_node_t* state_head;
    unsigned int size;
} state_linked_list_t;

#endif //ELM_DATA_TYPES_H
