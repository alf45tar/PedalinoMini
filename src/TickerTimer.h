/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2024 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
 */

#include <Ticker.h>

Ticker        timer1, timer2, timer3;

volatile int  interruptCounter1 = 0;
volatile int  interruptCounter2 = 0;
volatile int  interruptCounter3 = 0;

static void onTimer1()
{
  interruptCounter1++;
}

static void onTimer2()
{
  interruptCounter2++;
}

static void onTimer3()
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
