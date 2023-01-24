#ifndef _QUERY_H_
#define _QUERY_H_

#include <stdio.h>
#include <inttypes.h>
#include "perf.h"

typedef struct query_stats_t {
    // perf counters
    perf_counter_t n_cycles;
    perf_counter_t n_instrs;
    perf_counter_t n_llc_refs; 
    perf_counter_t n_llc_misses;
    perf_counter_t stalls_l1d_miss;
    perf_counter_t stalls_l2_miss;
    perf_counter_t stalls_l3_miss;
    perf_counter_t stalls_mem_any;
    int qid;
} query_stats_t;

void *query_evaluator(void *query_args);
void init_query_stats(query_stats_t *q_stats);
void print_query_stats(query_stats_t *t);

#endif
