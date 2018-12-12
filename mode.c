/* QMAP: Quake level viewer
 *
 *   mode.c    Copyright 1997 Sean Barett
 *
 *   General screen functions (set graphics
 *   mode, blit to framebuffer, set palette)
 */

#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mode.h"

static bool init=0;
static SDL_Window* window=NULL;
static SDL_Renderer* renderer=NULL;
static SDL_Surface* framebuffer=NULL;
static SDL_Texture* texture=NULL;
static SDL_Event event;
static SDL_Colour palette[256];

void setup_sdl(void)
{
   int winpos;
   
   if (SDL_Init(SDL_INIT_VIDEO) < 0)
      fatal("Couldn't initialise SDL2");
   init = 1;
   
   winpos = SDL_WINDOWPOS_CENTERED_DISPLAY(0);
   window = SDL_CreateWindow("qmap", winpos, winpos, 320, 200, SDL_WINDOW_RESIZABLE);
   if (!window)
      fatal("Couldn't create window");
   
   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
   if (!renderer)
      fatal("Couldn't open graphics interface");
   
   framebuffer = SDL_CreateRGBSurfaceWithFormat(0, 320, 200, 32, SDL_PIXELFORMAT_ABGR8888);
   if (!framebuffer)
      fatal("Couldn't allocate software framebuffer");
   
   texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);
   if (!texture)
      fatal("Couldn't allocate gpu framebuffer");
}

void close_sdl(void)
{
   if (texture) {
      SDL_DestroyTexture(texture);
      texture = NULL;
   }
   
   if (framebuffer) {
      SDL_FreeSurface(framebuffer);
      framebuffer = NULL;
   }
   
   if (renderer) {
      SDL_DestroyRenderer(renderer);
      renderer = NULL;
   }
   
   if (window) {
      SDL_DestroyWindow(window);
      window = NULL;
   }
   
   if (init) {
      SDL_Quit();
      init = 0;
   }
}

void blit(char *src)
{
   int i, j;
   uchar *linesrc;
   SDL_Colour *dst;
   
   for (i=0; i < 200; ++i) {
      dst = (SDL_Colour*)((char*)framebuffer->pixels + i * framebuffer->pitch);
      linesrc = (uchar*)src + i * 320;
      
      for (j=0; j < 320; ++j) {
         (*dst++) = palette[(*linesrc++)];
      }
   }
   
   SDL_UpdateTexture(texture, NULL, framebuffer->pixels, framebuffer->pitch);
   SDL_RenderCopy(renderer, texture, NULL, NULL);
   SDL_RenderPresent(renderer);
}

void set_pal(uchar *pal)
{
   int i;
   SDL_Colour c;
   
   for (i=0; i < 256; ++i) {
      c.r = (*pal++);
      c.g = (*pal++);
      c.b = (*pal++);
      c.a = 0xFF;
      palette[i] = c;
   }
}


void poll_events(bool* running)
{
   while (SDL_PollEvent(&event) > 0)
	{
      if (event.type == SDL_QUIT)
         *running = 0;
	}
}

bool get_key(int scancode)
{
   const Uint8 *kb = SDL_GetKeyboardState(NULL);
   return (bool)kb[scancode];
}


void fatal_error(char *message, char *file, int line)
{
   close_sdl();
   fprintf(stderr, "Error (%s line %d): %s\n", file, line, message);
   exit(1);
}
