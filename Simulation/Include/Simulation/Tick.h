#pragma once

#include <stdint.h>

struct Tick {
  uint64_t interval;
  uint64_t last;
};

/*!
 * \brief Configures the "tick rate" and grabs the current time, so that the
 * next call to Tick_Wait() will wait the proper amount of time. Make sure to
 * call this function before you use Tick_Wait(), or if you wait a long period
 * of time between Tick_Wait() calls.
 * \param[in] t Tick structure
 * \param[in] tps Ticks per second. Configures how long Tick_Wait() will wait
 * for to achieve the number of ticks per second.
 */
void Tick_Config(struct Tick *t, int tps);

/*!
 * \brief If the period of time configured in Tick_Config() has passed since the
 * last call to Tick_Advance(), this function returns non-zero. This is
 * essentially a non-blocking version of Tick_Wait().
 * \return Returns the number of periods (configured in Tick_Config()) that you
 * are behind from where you should be.
 */
int Tick_Advance(struct Tick *t);

/*!
 * \brief Waits (and sleeps if necessary) until the tick rate configured in
 * tick_init() is reached. Then updates the current time, so that the next
 * call to Tick_Wait() will delay for the same amount of time again.
 * \param[in] t Tick structure
 * \return If a long amount of time has passed between calls to Tick_Wait(),
 * then this function will return the number of ticks that you are behind from
 * where you should be. Consider checking this value and setting a threshold,
 * and if you ever go over this threshold it might make sense to call
 * Tick_Config() again or Tick_Skip(). Will return 0 under normal circumstances.
 */
int Tick_Wait(struct Tick *t);

void Tick_Skip(struct Tick *t);
