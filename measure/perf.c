#include <sys/types.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/syscall.h> 
#include <sys/ioctl.h>
#include <errno.h>

#define gettid() syscall(SYS_gettid)

// libpfm dependencies
#include <perfmon/pfmlib_perf_event.h>
#include <perfmon/pfmlib.h>

#include "perf.h"

void apply_default_options(perf_event_attr_t *config) {
  config->size = sizeof(perf_event_attr_t);
  config->exclude_kernel = 1;
  config->exclude_guest = 1;
  config->disabled = 1;
}

void apply_required_options(perf_event_attr_t *config) {
  // request the time enabled and time running fields, which will be used to scale
  // the counter value if the PMU is unable to capture all events (if it is overcommitted
  // and number of events exceeds number of PMU slots). this is required for get_perf_counter to work.
  config->read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;
}

void get_counter_config(const char *event, perf_event_attr_t *config) {
  int ret;

  // libpfm4 does not clear garbage fields for you; this is to be safe
  memset(config, 0, sizeof(perf_event_attr_t));

  pfm_perf_encode_arg_t arg;
  memset(&arg, 0, sizeof(pfm_perf_encode_arg_t));
  arg.size = sizeof(arg);
  arg.attr = config;

  // note: dfl_plm, the second argument, specifies which privilege level to measure at.
  // right now this is PFM_PLM3, which is lowest user level; may need to change later.
  if ((ret = pfm_get_os_event_encoding(event, PFM_PLM3, PFM_OS_PERF_EVENT, &arg)) != PFM_SUCCESS) {
    fprintf(stderr, "perf counters: cannot get options to pass to perf_event_open: %s", pfm_strerror(ret));
  }
}

void init_perf_counter(perf_counter_t *counter, perf_event_attr_t *config) {
  // specify the calling thread to be monitored
  // - could possibly change and allow specifying pid or cpu manually?
  // - could also add support for creating event groups by specifying groupfd
  if ((counter->fd = syscall(SYS_perf_event_open, config, gettid(), -1, -1, 0)) < 0) {
    fprintf(stderr, "perf counters: cannot create perf event: %s", strerror(errno));
    exit(1);
  }
}

void start_perf_counter(perf_counter_t *counter) {
  if (ioctl(counter->fd, PERF_EVENT_IOC_ENABLE, 0) < 0) {
    fprintf(stderr, "perf counters: could not enable event; %s", strerror(errno));
    exit(1);
  }
}

void stop_perf_counter(perf_counter_t *counter) {
  if (ioctl(counter->fd, PERF_EVENT_IOC_DISABLE, 0) < 0) {
    fprintf(stderr, "perf counters: could not disable event; %s", strerror(errno));
    exit(1);
  }
}

uint64_t get_perf_counter(perf_counter_t *counter) {
  counter_raw_data_t results;

  if (read(counter->fd, &results, sizeof(counter_raw_data_t)) < 0) {
    fprintf(stderr, "perf counters: could not read counter value; %s", strerror(errno));
    exit(1);
  }

  // return the counter value, scaled by the the amount of time it was tracked
  // note: multiply before dividing to avoid roundoff errors
  return (results.value * results.time_enabled) / results.time_running;
}

counter_value_and_scale_t get_perf_counter_w_scale(perf_counter_t *counter) {
  counter_raw_data_t raw_results;
  counter_value_and_scale_t processed_results;

  if (read(counter->fd, &raw_results, sizeof(counter_raw_data_t)) < 0) {
    fprintf(stderr, "perf counters: could not read counter value; %s", strerror(errno));
    exit(1);
  }

  // return the scaled counter value and the scale
  processed_results.scale = (double) raw_results.time_enabled / raw_results.time_running;
  processed_results.value = (raw_results.value * raw_results.time_enabled) / raw_results.time_running;
  return processed_results;
}

void clean_up_perf_counter(perf_counter_t *counter) {
  if (close(counter->fd) < 0) {
    fprintf(stderr, "perf counters: could not close counter; %s", strerror(errno));
    exit(1);
  }
}

// int main() {
//   printf("helloworld");
// }