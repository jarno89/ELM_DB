//
// Created by jarno on 16/06/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cluster.h"
#include "util.h"


double radius_factor = 1.0;

double avg_norm(const cluster_t* c) {
    double* temp1 = init_vector();
    double* temp2 = init_vector();

    sqr_vector(temp1, c->center);
    mul_double_vector(temp1, c->size, temp1);
    add_vectors(temp1, c->sqr_sum, temp1);
    mul_vector_vector(temp2, c->center, c->lin_sum);
    mul_double_vector(temp2, 2.0, temp2);
    sub_vectors(temp2, temp1, temp2);
    div_vector_double(temp2, temp2, c->size);

    double norm = radius_factor * norm_vector(temp2);
    free_vector(temp1);
    free_vector(temp2);
    return norm;
}

cluster_t* init_cluster(const double* data_point) {
    cluster_t* c = malloc(sizeof(cluster_t));

    copy_vector(c->center, data_point);
    c->size = 1;
    copy_vector(c->lin_sum, data_point);
    sqr_vector(c->sqr_sum, data_point);
    c->avg_norm = 0.0;

    return c;
}

void print_cluster(const cluster_t* cluster) {
    printf("  Cluster (size: %d):\n", cluster->size);
    printf("    center: ");
    print_vector(cluster->center);
    printf("    lin_sum: ");
    print_vector(cluster->lin_sum);
    printf("    sqr_sum: ");
    print_vector(cluster->sqr_sum);
}

cluster_t* copy_cluster(const cluster_t* c) {
    cluster_t* cr = malloc(sizeof(cluster_t));

    copy_vector(cr->center, c->center);
    cr->size = c->size;
    copy_vector(cr->lin_sum, c->lin_sum);
    copy_vector(cr->sqr_sum, c->sqr_sum);
    cr->avg_norm = c->avg_norm;

    return cr;
}

cluster_t * merge_clusters(const cluster_t* c1, const cluster_t* c2) {
    cluster_t* cr = malloc(sizeof(cluster_t));

    cr->size = c1->size + c2->size;
    add_vectors(cr->lin_sum, c1->lin_sum, c2->lin_sum);
    add_vectors(cr->sqr_sum, c1->sqr_sum, c2->sqr_sum);
    div_vector_double(cr->center, cr->lin_sum, cr->size);
    cr->avg_norm = avg_norm(cr);

    return cr;
}

void add_point_to_cluster(cluster_t* c, const double* data_point) {
    double* temp1 = init_vector();

    sqr_vector(temp1, data_point);
    add_vectors(c->lin_sum, c->lin_sum, data_point);
    add_vectors(c->sqr_sum, c->sqr_sum, temp1);

    c->size++;

    mul_double_vector(temp1, c->size - 1, c->center);
    add_vectors(temp1, temp1, data_point);
    div_vector_double(c->center, temp1, c->size);

    c->avg_norm = avg_norm(c);
    free_vector(temp1);
}

bool is_equal_cluster(const cluster_t* cluster_a, const cluster_t* cluster_b) {
    if (cluster_a->size != cluster_b->size) {
        //cluster have different sizes
        return false;
    }
    if (!vector_is_close(cluster_a->lin_sum, cluster_b->lin_sum)) {
        //linear sum is different (small differences are allowed due to floating point calculations)
        return false;
    }
    if (!vector_is_close(cluster_a->sqr_sum, cluster_b->sqr_sum)) {
        //squared sum is different (small differences are allowed due to floating point calculations)
        return false;
    }
    return true;
}

void free_cluster(cluster_t* cluster) {
    free(cluster);
}

void free_cluster_node(cluster_node_t * cluster_node) {
    cluster_node_t* curr_node = cluster_node;
    cluster_node_t* prev_node;

    free_cluster(curr_node->cluster);
    while (curr_node->next != NULL) {
        prev_node = curr_node;
        curr_node = curr_node->next;
        free(prev_node);
        free_cluster(curr_node->cluster);
    }
    prev_node = curr_node;
    free(prev_node);
}

