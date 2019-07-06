#include <Ticker.h>

Ticker        timer1, timer2, timer3;

volatile int  interruptCounter1 = 0;
volatile int  interruptCounter2 = 0;
volatile int  interruptCounter3 = 0;

void onTimer1()
{
  interruptCounter1++;
}

void onTimer2()
{
  interruptCounter2++;
}

void onTimer3()
{
  interruptCounter3++;
}

void Timer1Attach(uint32_t ms)
{
 timer1.attach_ms(ms, onTimer1);
}

void Timer2Attach(uint32_t ms)
{
 timer2.attach_ms(ms, onTimer2);
}

void Timer3Attach(uint32_t ms)
{
 timer3.attach_ms(ms, onTimer3);
}
