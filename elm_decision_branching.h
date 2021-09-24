//
// Created by jarno on 18/06/2020.
//

#include <stdbool.h>
#include "data_types.h"

#ifndef ELM_ELM_DECISION_BRANCHING_H
#define ELM_ELM_DECISION_BRANCHING_H

void process_sample(state_linked_list_t* state_list, state_t* state, const double* data_point);

void fit_sample(state_linked_list_t* state_list, const double* data_point);

int predict_sample(state_linked_list_t* state_list, const double* data_point);

#endif //ELM_ELM_DECISION_BRANCHING_H
