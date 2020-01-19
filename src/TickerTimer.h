#ifndef TICKERTIMER_H
#define TICKERTIMER_H

#include <stdint.h>
#include <Ticker.h>

const int NUM_TIMERS = 3;

const int TICKERTIMER1 = 0;
const int TICKERTIMER2 = 1;
const int TICKERTIMER3 = 2;

void TickerTimerAttach(int timer_no, uint32_t ms);
void TickerTimerClear(int timer_no);
bool TickerTimerIsSet(int timer_no);

#endif /* TICKERTIMER_H */
