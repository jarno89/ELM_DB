#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "cluster.h"
#include "data_types.h"
#include "elm_decision_branching.h"
#include "state.h"
#include "util.h"

int dimension = 0;
double radius = 0;
unsigned int max_states = 1;

//calculate runtime
void calculate_time(char time_[16], struct timeval start, struct timeval end)
{
    int *hours = (int*) malloc(sizeof(int));
    int *minutes = (int*) malloc(sizeof(int));
    double *seconds  = (double*) malloc(sizeof(double));
    *hours = 0, *minutes = 0;
    double microsecs = (double)(end.tv_sec - start.tv_sec)*1000000 +
                  ((double)(end.tv_usec - start.tv_usec));
    *seconds = floor(microsecs)/1000000.0;
    //variable to carry internal calculation
    double totalSeconds = *seconds;
    if(totalSeconds>=3600.0)
    {
        //divide to obtain the amount of hours
        *hours = floor(totalSeconds/3600);
        //substract the total amount of hours in seconds from the amount of seconds
        totalSeconds = totalSeconds - *hours*3600;
    }
    if(totalSeconds>=60)
    {
        //divide to obtain the amount of minutes
        *minutes = floor(totalSeconds/60);
        //substract the total amount of minutes in seconds from the amount of seconds
        totalSeconds = totalSeconds - *minutes*60;
    }
    //whatever is left after calculations, return
    *seconds = totalSeconds;
    sprintf(time_, "%02d:%02d:%09.6fs", *hours, *minutes, *seconds);

    free(hours);
    free(minutes);
    free(seconds);
}

int main(int argc, char *argv[]) {
    char time_[16];

#if DECISION_BRANCHING==1
    printf("ELM Decision Branching\n");
#else
    printf("ELM\n");
#endif

    //parsing arguments
    int opt;
    char* input_filename = NULL;
    char* output_filename = NULL;
    bool has_header = false;
    while((opt = getopt(argc, argv, "i:o:r:s:h")) != -1) {
        switch (opt) {
            case 'i':
                input_filename = optarg;
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'r':
                radius = strtod(optarg, NULL);
                break;
            case 's':
                max_states = strtol(optarg, NULL, 10);
                break;
            case 'h':
                has_header = true;
                break;
            default:
                printf("unknown parameter '%c'\n", opt);
                return -1;
        }
    }
    if (input_filename == NULL) {
        printf("No input file specified, aborting...\n");
        exit(-1);
    }
    if (output_filename == NULL) {
        printf("No output file specified, aborting...\n");
        exit(-1);
    }
    if (radius <= 0) {
        printf("Invalid radius specified, aborting...\n");
        exit(-1);
    }

    printf("clustering dataset with radius: %.2f and max states: %d\n", radius, max_states);

    // opening input file
    errno = 0;
    FILE *input_fp = fopen(input_filename, "r");
    if (input_fp == NULL) {
        printf("Can't open input file: '%s'\n", input_filename);
        printf("Error %d \n", errno);
        return -1;
    }

    //opening output file
    FILE *output_fp = fopen(output_filename, "w");
    if (output_fp == NULL) {
        printf("Can't open output file: '%s'\n", output_filename);
        printf("Error %d \n", errno);
        return -1;
    }

    int buffer_size = 20480;
    char buf[buffer_size];
    int row_nr = 0;
    int col_nr;
    double data[MAX_DIMENSION];
    state_linked_list_t* state_list = init_state_linked_list();

    // print current time
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("%s", asctime(timeinfo));

    // start timers
    struct timeval start, end;
    struct timeval start_0, end_0;
    printf("Processing dataset...\n");
    gettimeofday(&start, NULL);
    gettimeofday(&start_0, NULL);

    //skip header
    if (has_header) {
        fgets(buf, buffer_size, input_fp);
    }

    //read input csv line and fit sample
    while(fgets(buf, buffer_size, input_fp)) {
        col_nr = 0;
        row_nr++;

        char* field = strtok(buf, ",");

        //read csv fields
        while (field) {
            char* end_ptr;
            data[col_nr] = strtod(field, &end_ptr);

            field = strtok(NULL,",");
            col_nr++;
        }

        //check number of fields
        if (dimension == 0) {
            if (dimension > MAX_DIMENSION) {
                printf("Max dimension exceeded");
                exit(-1);
            }
            dimension = col_nr;
        } else if (dimension != col_nr) {
            printf("Expected %d columns on row %d, got %d", dimension, row_nr, col_nr);
            exit(-1);
        }

        //process data sample
        fit_sample(state_list, data);

        //update progress info
        if (row_nr % 100 == 0) {
            printf("\r%d rows processed", row_nr);

#if DECISION_BRANCHING==1
            state_node_t* node = state_list->state_head;
            unsigned int size = node->state->clusters.size;
            unsigned int total_clusters = size;
            unsigned int min_clusters = size;
            unsigned int max_clusters = size;
            node = node->next;
            while (node != NULL) {
                size = node->state->clusters.size;
                total_clusters += size;
                if (size < min_clusters) min_clusters = size;
                if (size > max_clusters) max_clusters = size;
                node = node->next;
            }
            printf(" (clusters: %d-%d, avg %.2f)", min_clusters, max_clusters, ((double)total_clusters)/state_list->size);
#else
            printf(" (clusters: %d)", state_list->state_head->state->clusters.size);
#endif

            fflush(stdout);
        }

    }

    //stop and print timer
    gettimeofday(&end_0, NULL);
    calculate_time(time_, start_0, end_0);
    printf("\r%d rows processed in %s\n\n", row_nr, time_);

    //reset input file pointer
    rewind(input_fp);
    row_nr = 0;

    //start timer
    struct timeval start_1, end_1;
    printf("Labelling dataset...\n");
    gettimeofday(&start_1, NULL);

    //skip header
    if (has_header) {
        fgets(buf, buffer_size, input_fp);
    }

    //read csv line and predict sample
    while(fgets(buf, buffer_size, input_fp)) {
        col_nr = 0;
        row_nr++;

        char* field = strtok(buf, ",");

        //read csv fields
        while (field) {
            char* end_ptr;
            data[col_nr] = strtod(field, &end_ptr);

            field = strtok(NULL,",");
            col_nr++;
        }

        //check number of fields
        if (dimension != col_nr) {
            printf("Expected %d columns on row %d, got %d", dimension, row_nr, col_nr);
            return -1;
        }

        //process data sample
        int label = predict_sample(state_list, data);
        if (label < 0) {
            printf("invalid label returned\n");
            fflush(stdout);
            return -1;
        }
        fprintf(output_fp, "%d\n", label);

        //update progress info
        if (row_nr % 1000 == 0) {
            printf("\r%d rows labeled", row_nr);
            fflush(stdout);
        }
    }

    //stop timers
    gettimeofday(&end_1, NULL);
    calculate_time(time_, start_1, end_1);
    printf("\r%d rows labeled in %s\n\n", row_nr, time_);
    gettimeofday(&end, NULL);
    calculate_time(time_, start, end);
    printf("Total processing time: %s\n\n", time_);

    //close files
    fclose(input_fp);
    fclose(output_fp);

    //print clustering result
    printf("Results:\n");
    print_state_linked_list(state_list);
    free_state_linked_list(state_list);

    return 0;
}
