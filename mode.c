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


int kbhit()
{
   if (SDL_PollEvent(&event) > 0)
      return 1;
   else
      return 0;
}

int getch()
{
   if (event.type == SDL_KEYDOWN)
   {
      if (event.key.keysym.mod & KMOD_SHIFT)
         if (event.key.keysym.sym == SDLK_q)
            return 'Q';
      
      switch (event.key.keysym.sym) {
         case SDLK_ESCAPE: return 27;
   
         case SDLK_v: return 'v';
         case SDLK_r: return 'r';
         case SDLK_q: return 'q';
         case SDLK_e: return 'e';
         case SDLK_d: return 'd';
         case SDLK_a: return 'a';
   
         case SDLK_c: return 'c';
         case SDLK_z: return 'z';
         case SDLK_1: return '1';
         case SDLK_3: return '3';
         case SDLK_x: return 'x';
         case SDLK_w: return 'w';
         
         case SDLK_SPACE: return ' ';
         
         default: return -1;
      }
   }
   else
   if (event.type == SDL_QUIT)
   {
      return 27;
   }
   
   return -1;
}


void fatal_error(char *message, char *file, int line)
{
   close_sdl();
   fprintf(stderr, "Error (%s line %d): %s\n", file, line, message);
   exit(1);
}
