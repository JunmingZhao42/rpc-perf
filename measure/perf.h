#include <linux/perf_event.h>
#include <stdint.h>

// Interface for tracking various hardware and software performance counters, to gain deeper
// insight into SPIRIT besides timing. Uses the Linux kernel's perf events interface.

typedef struct perf_counter_t {
  int fd; // file descriptor returned by perf_event_open
} perf_counter_t;

typedef struct counter_raw_data_t {
  uint64_t value;
  uint64_t time_enabled;
  uint64_t time_running;
} counter_raw_data_t;

typedef struct counter_value_and_scale_t {
  uint64_t value;
  double scale;
} counter_value_and_scale_t;

typedef struct perf_event_attr perf_event_attr_t;

// for use if populating the perf event configuration struct manually
void apply_default_options(perf_event_attr_t *config);
void apply_required_options(perf_event_attr_t *config);

// automatically populates the perf event configuration struct using libpfm4
// for information on how to format the event string, see:
// https://perfmon2.sourceforge.net/docs_v4.html
// https://perfmon2.sourceforge.net/manv4/libpfm.html
// https://perfmon2.sourceforge.net/manv4/libpfm_intel_x86_arch.html
void get_counter_config(const char *event, perf_event_attr_t *config);

void init_perf_counter(perf_counter_t *counter, perf_event_attr_t *config);

void start_perf_counter(perf_counter_t *counter);

void stop_perf_counter(perf_counter_t *counter);

uint64_t get_perf_counter(perf_counter_t *counter);

counter_value_and_scale_t get_perf_counter_w_scale(perf_counter_t *counter);

void clean_up_perf_counter(perf_counter_t *counter);
