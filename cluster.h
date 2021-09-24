//
// Created by jarno on 16/06/2020.
//

#include <stdbool.h>
#include "data_types.h"

#ifndef ELM_CLUSTER_H
#define ELM_CLUSTER_H

cluster_t* init_cluster(const double* data_point);

void print_cluster(const cluster_t* cluster);

cluster_t* copy_cluster(const cluster_t* c);

cluster_t* merge_clusters(const cluster_t* c1, const cluster_t* c2);

void add_point_to_cluster(cluster_t* c, const double* data_point);

bool is_equal_cluster(const cluster_t* cluster_a, const cluster_t* cluster_b);

void free_cluster(cluster_t* cluster);

void free_cluster_node(cluster_node_t * cluster_node);

#endif //ELM_CLUSTER_H
