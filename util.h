//
// Created by jarno on 16/06/2020.
//

#include <stdbool.h>
#include "data_types.h"

#ifndef ELM_UTIL_H
#define ELM_UTIL_H

double* init_vector();

void free_vector(double* v);

void copy_vector(double* dest, const double* src);

void print_vector(const double* v);

void sqr_vector(double* v_res, const double* v);

void mul_double_vector(double* v_res, double x, const double* v);

void mul_vector_vector(double* v_res, const double* x, const double* v);

void div_vector_double(double* v_res, const double* v, double x);

void add_vectors(double* v_res, const double* v1, const double* v2);

void sub_vectors(double* v_res, const double* v1, const double* v2);

double norm_vector(const double* v);

double distance_vectors(const double* v1, const double* v2);

bool is_close(double a, double b);

bool vector_is_close(const double* vector_a, const double* vector_b);

#endif //ELM_UTIL_H
