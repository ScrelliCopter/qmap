/*  QMAP: Quake level viewer
 *
 *   clock.c   Copyright 2018 Nicholas Curtis
 *
 *   General setup control, main "sim" loop
 */

#include <SDL_timer.h>
#include <string.h>

#include "clock.h"

float delta, fps;

#define FPS_SMOOTH 128

int fpsbuf[FPS_SMOOTH];
int fpsidx, prevtime;

void clock_init(void)
{
	fpsidx = 0;
   prevtime = SDL_GetTicks();
   memset(fpsbuf, 0, sizeof(fpsbuf));
}

void clock_tick(void)
{
	int i, curtime, diff, total;
   
   curtime = SDL_GetTicks();
   diff = curtime - prevtime;
   fpsidx = (fpsidx + 1) % FPS_SMOOTH;
   prevtime = curtime;
   
   // compute deltatime
   delta = (float)diff / 1000.0f;
   
   // compute average FPS
   fpsbuf[fpsidx] = diff;
   total = 0;
   for (i=0; i < FPS_SMOOTH; ++i)
      total += fpsbuf[i];
   fps = 1000.0f / ((float)total / (float)FPS_SMOOTH);
}
