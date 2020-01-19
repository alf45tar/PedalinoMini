#include <TickerTimer.h>
#include <Ticker.h>

Ticker timer[NUM_TIMERS];
volatile int interruptCounter[NUM_TIMERS] = { 0 } ;

void onTimer(int timer_no)
{
    assert(0 <= timer_no < NUM_TIMERS);
    interruptCounter[timer_no]++;
}

void TickerTimerAttach(int timer_no, uint32_t ms)
{
    assert(0 <= timer_no < NUM_TIMERS);
    timer[timer_no].attach_ms<int>(ms, onTimer, timer_no);
}

void TickerTimerClear(int timer_no)
{
    assert(0 <= timer_no < NUM_TIMERS);
    interruptCounter[timer_no] = 0;
}

bool TickerTimerIsSet(int timer_no)
{
    assert(0 <= timer_no < NUM_TIMERS);
    return interruptCounter[timer_no] > 0;
}
