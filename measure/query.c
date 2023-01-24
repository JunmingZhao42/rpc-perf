#include <pthread.h>
#include <string.h>
#include <sys/mman.h>
#include <assert.h>
#include "query.h"

// TODO: add query_evaluator to storage thread in rpc
void *
query_evaluator(void *query_stats)
{
  query_stats_t *q_stat = (query_stats_t *) query_stats;

  init_query_stats(q_stat);
  // start_perf_counter(&args->q_stat.n_llc_misses);
  start_perf_counter(&q_stat->stalls_l1d_miss);
  start_perf_counter(&q_stat->stalls_l2_miss);
  start_perf_counter(&q_stat->stalls_l3_miss);
  start_perf_counter(&q_stat->stalls_mem_any);

  // stop_perf_counter(&q_stat.n_cycles);
  // stop_perf_counter(&args->q_stat.n_instrs);
  // stop_perf_counter(&args->q_stat.n_llc_refs);
  // // stop_perf_counter(&args->q_stat.n_llc_misses);
  // stop_perf_counter(&args->q_stat.stalls_l1d_miss);
  // stop_perf_counter(&args->q_stat.stalls_l2_miss);
  // stop_perf_counter(&args->q_stat.stalls_l3_miss);
  // stop_perf_counter(&args->q_stat.stalls_mem_any);

  pthread_exit(NULL);
}

void init_query_stats(query_stats_t *q_stats) {
  // initialise perf counters
  perf_event_attr_t config;
  // clear all fields
  memset(&config, 0, sizeof(config));

  // common options across counters
  apply_default_options(&config);
  apply_required_options(&config);
  config.type = PERF_TYPE_HARDWARE;

  // TODO: PERF_COUNT_HW_CACHE_REFERENCES is a generic hardware event. The documentation
  // says that this usually refers to LLC references, but could depend on your hardware.
  // Notably, the count given by this and the LLC_loads hardware cache event don't perfectly match;
  // need to investigate further what exactly this is measuring.
  // config.config = PERF_COUNT_HW_CACHE_REFERENCES;
  // init_perf_counter(&q_stats->n_llc_refs, &config);

  config.config = PERF_COUNT_HW_CACHE_MISSES;
  init_perf_counter(&q_stats->n_llc_misses, &config);

  get_counter_config("cycle_activity.stalls_l1d_miss", &config);
  init_perf_counter(&q_stats->stalls_l1d_miss, &config);

  get_counter_config("cycle_activity.stalls_l2_miss", &config);
  init_perf_counter(&q_stats->stalls_l2_miss, &config);

  get_counter_config("cycle_activity.stalls_l3_miss", &config);
  init_perf_counter(&q_stats->stalls_l3_miss, &config);

  get_counter_config("cycle_activity.stalls_mem_any", &config);
  init_perf_counter(&q_stats->stalls_mem_any, &config);

  q_stats->qid = 0;
}

// TODO: for every minute, when printing the stats from rpc, also print cache misses stats
void print_query_stats(query_stats_t *t) {
  printf("query_perf_counters: n_llc_misses stalls_l1d_miss stalls_l2_miss stalls_l3_miss stalls_mem_any\n");
  counter_value_and_scale_t n_llc_misses = {0, 0.0};
  counter_value_and_scale_t stalls_l1d_miss = get_perf_counter_w_scale(&t->stalls_l1d_miss);
  counter_value_and_scale_t stalls_l2_miss = get_perf_counter_w_scale(&t->stalls_l2_miss);
  counter_value_and_scale_t stalls_l3_miss = get_perf_counter_w_scale(&t->stalls_l3_miss);
  counter_value_and_scale_t stalls_mem_any = get_perf_counter_w_scale(&t->stalls_mem_any);
  printf("values: %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 " \n", n_llc_misses.value, stalls_l1d_miss.value, stalls_l2_miss.value, stalls_l3_miss.value, stalls_mem_any.value);
  printf("correction factors: %.6f %.6f %.6f %.6f %.6f \n", n_llc_misses.scale, stalls_l1d_miss.scale, stalls_l2_miss.scale, stalls_l3_miss.scale, stalls_mem_any.scale);
}
