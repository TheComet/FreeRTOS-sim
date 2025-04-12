#include "Simulation/Tick.h"
#include <time.h>

static struct timespec ns_to_timespec(uint64_t ns) {
  struct timespec ts;
  ts.tv_sec = ns / 1000000000ULL;
  ts.tv_nsec = ns - ts.tv_sec * 1000000000ULL;
  return ts;
}

static uint64_t timespec_to_ns(struct timespec *ts) {
  return ts->tv_sec * 1000000000ULL + ts->tv_nsec;
}

void Tick_Config(struct Tick *t, int tps) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  t->last = timespec_to_ns(&ts);
  t->interval = 1000000000ULL / tps;
}

int Tick_Advance(struct Tick *t) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  uint64_t now = timespec_to_ns(&ts);
  uint64_t wait_until = t->last + t->interval;

  if (now > wait_until) {
    int ticks_behind = (now - wait_until) / t->interval;
    if (ticks_behind > 0)
      t->last = wait_until;
    return ticks_behind;
  }

  return 0;
}

int Tick_Wait(struct Tick *t) { return Tick_WaitFor(t, 1); }

int Tick_WaitFor(struct Tick *t, int ticks) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  uint64_t now = timespec_to_ns(&ts);
  uint64_t wait_until = t->last + t->interval * ticks;

  if (now > wait_until) {
    t->last = wait_until;
    return (now - wait_until) / t->interval;
  }

  ts = ns_to_timespec(wait_until);
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
  t->last = wait_until;
  return 0;
}

void Tick_Skip(struct Tick *t) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  t->last = timespec_to_ns(&ts);
}
