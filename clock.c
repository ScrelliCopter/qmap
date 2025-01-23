/* clock.c
 *   Copyright (C) 2018, 2023, 2025 a dinosaur (0BSD)
*/

#include <SDL3/SDL_timer.h>
#include <string.h>

#include "clock.h"

float delta, fps;

#define FPS_SMOOTH 128

unsigned fpsbuf[FPS_SMOOTH];
int fpsidx;
Uint64 prevtime;

#define USE_PERFCOUNT

void clock_init(void)
{
   fpsidx = 0;
#ifdef USE_PERFCOUNT
   prevtime = SDL_GetPerformanceCounter();
#else
   prevtime = SDL_GetTicksNS();
#endif
   memset(fpsbuf, 0, sizeof(fpsbuf));
}

void clock_tick(void)
{
   int i;
   Uint64 curtime, diff, total;

#ifdef USE_PERFCOUNT
   curtime = SDL_GetPerformanceCounter();
   diff = curtime - prevtime;
   delta = (float)((double)diff / (double)SDL_GetPerformanceFrequency());
#else
   curtime = SDL_GetTicksNS();
   diff = curtime - prevtime;
   delta = (float)((double)diff / 1000000000.0); // compute deltatime
#endif
   if (++fpsidx >= FPS_SMOOTH)
      fpsidx = 0;
   prevtime = curtime;

   // compute average FPS
   fpsbuf[fpsidx] = (unsigned)diff;
   total = 0;
   for (i=0; i < FPS_SMOOTH; ++i)
      total += fpsbuf[i];
#ifdef USE_PERFCOUNT
   fps = (float)((double)SDL_GetPerformanceFrequency() / ((double)total / (double)FPS_SMOOTH));
#else
   fps = (float)(1000000000.0 / ((double)total / (double)FPS_SMOOTH));
#endif
}
