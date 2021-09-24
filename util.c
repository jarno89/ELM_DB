//
// Created by jarno on 16/06/2020.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

extern int dimension;

double* init_vector() {
    return malloc(sizeof(double) * dimension);
}

void free_vector(double* v) {
    free(v);
}

void copy_vector(double* dest, const double* src) {
    memcpy(dest, src, sizeof(double) * dimension);
}

void print_vector(const double* v) {
    printf("[%.2f", v[0]);
    for (int i=1; i<dimension; i++) {
        printf(", %.2f", v[i]);
    }
    printf("]\n");
}

void sqr_vector(double* v_res, const double* v) {
    for (int i = 0; i < dimension; i++) {
        v_res[i] = v[i] * v[i];
    }
}

void mul_double_vector(double* v_res, double x, const double* v) {
    for (int i = 0; i < dimension; i++) {
        v_res[i] = v[i] * x;
    }
}

void mul_vector_vector(double* v_res, const double* v1, const double* v2) {
    for (int i = 0; i < dimension; i++) {
        v_res[i] = v1[i] * v2[i];
    }
}

void div_vector_double(double* v_res, const double* v, double x) {
    for (int i = 0; i < dimension; i++) {
        v_res[i] = v[i] / x;
    }
}

void add_vectors(double* v_res, const double* v1, const double* v2) {
    for (int i = 0; i < dimension; i++) {
        v_res[i] = v1[i] + v2[i];
    }
}

void sub_vectors(double* v_res, const double* v1, const double* v2) {
    for (int i = 0; i < dimension; i++) {
        v_res[i] = v1[i] - v2[i];
    }
}

double norm_vector(const double* v) {
    double sqr_sum = 0.0;
    for (int i = 0; i < dimension; i++) {
        sqr_sum += v[i] * v[i];
    }
    return sqrt(sqr_sum);
}

double distance_vectors(const double* v1, const double* v2) {
    double sqr_sum = 0.0;
    for (int i = 0; i < dimension; i++) {
        double diff = v1[i] - v2[i];
        sqr_sum += diff * diff;
    }
    return sqrt(sqr_sum);
}

#define ABS_ERROR 1e-8

bool is_close(const double a, const double b) {
    return (fabs(a - b) < ABS_ERROR);
}

bool vector_is_close(const double* vector_a, const double* vector_b) {
    for (int n = 0; n<dimension; n++) {
        if (!is_close(vector_a[n], vector_b[n])) {
            return false;
        }
    }
    return true;
}