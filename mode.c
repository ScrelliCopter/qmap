/* QMAP: Quake level viewer
 *
 *   mode.c    Copyright 1997 Sean Barett
 *             Copyright 2018 Nicholas Curtis
 *
 *   General screen functions (set graphics
 *   mode, blit to framebuffer, set palette)
 *
 *   Updated to replace old dos functionality
 *   with modern SDL2 equivalents
 */

#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "text.h"
#include "mode.h"

static bool init=0;
static SDL_Window* window=NULL;
static SDL_Renderer* renderer=NULL;
static SDL_Surface* framebuffer=NULL;
static SDL_Texture* texture=NULL, *fonttex=NULL;
static SDL_Event event;
static SDL_Colour palette[256];
static int mrelx=0, mrely=0;

void setup_sdl(void)
{
   int winpos, i, j, k;
   SDL_Surface *surf;
   Uint32 *pix;
   Uint8 btmp;
   
   if (SDL_Init(SDL_INIT_VIDEO) < 0)
      fatal("Couldn't initialise SDL2");
   init = 1;
   
   winpos = SDL_WINDOWPOS_CENTERED_DISPLAY(0);
   window = SDL_CreateWindow("qmap", winpos, winpos, 320, 200, SDL_WINDOW_RESIZABLE);
   if (!window)
      fatal("Couldn't create window");
   
   renderer = SDL_CreateRenderer(window, -1, 0);
   if (!renderer)
      fatal("Couldn't open graphics interface");
   
   framebuffer = SDL_CreateRGBSurfaceWithFormat(0, 320, 200, 32, SDL_PIXELFORMAT_ABGR8888);
   if (!framebuffer)
      fatal("Couldn't allocate software framebuffer");
   
   texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);
   if (!texture)
      fatal("Couldn't allocate gpu framebuffer");
   
   surf = SDL_CreateRGBSurfaceWithFormat(0, 16 * ISO_CHAR_WIDTH, 16 * ISO_CHAR_HEIGHT, 8, SDL_PIXELFORMAT_RGBA32);
   if (surf) {
   	
   	for (i=ISO_CHAR_MIN; i <= ISO_CHAR_MAX; ++i) {
   		pix = (Uint32*)surf->pixels;
   		pix += (i >> 4) * surf->w * ISO_CHAR_HEIGHT;
   		pix += (i & 0xF) * ISO_CHAR_WIDTH;
   		
   		for (j=0; j < ISO_CHAR_HEIGHT; ++j) {
   			btmp = iso_font[i * ISO_CHAR_HEIGHT + j];
   			
				for (k=0; k < 8; ++k) {
					(*pix++) = (btmp & 0x1) ? 0xFFFFFFFF : 0x00000000;
					btmp >>= 1;
				}
				pix += surf->w - 8;
			}
   	}
   	
   	fonttex = SDL_CreateTextureFromSurface(renderer, surf);
   	if (fonttex) {
   		SDL_SetTextureBlendMode(fonttex, SDL_BLENDMODE_BLEND);
   	}
   	
   	SDL_FreeSurface(surf);
	}
}

void close_sdl(void)
{
	if (fonttex) {
      SDL_DestroyTexture(fonttex);
      fonttex = NULL;
   }
	
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
}

void present()
{
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


void draw_text(int x, int y, const char *text)
{
	SDL_Rect src, dst;
	
	if (fonttex) {
		dst.x = x;
		dst.y = y;
		src.w = ISO_CHAR_WIDTH;
		src.h = ISO_CHAR_HEIGHT;
		dst.w = ISO_CHAR_WIDTH * TEXT_SCALE;
		dst.h = ISO_CHAR_HEIGHT * TEXT_SCALE;
		
		while (*text != '\0') {
			if (*text == '\n') {
				dst.x = x - dst.w;
				dst.y += dst.h;
			} else if (*text != ' ') {
				src.x = (*text & 0xF) * src.w;
				src.y = (*text >> 4) * src.h;
				
				SDL_RenderCopy(renderer, fonttex, &src, &dst);
			}
			
			dst.x += dst.w;
			++text;
		}
	}
}


void poll_events(bool *running)
{
   while (SDL_PollEvent(&event) > 0)
	{
      if (event.type == SDL_QUIT) {
         *running = 0;
      } else if (event.type == SDL_MOUSEMOTION) {
         mrelx += event.motion.xrel;
         mrely += event.motion.yrel;
      } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m) {
         SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());
         mrelx = mrely = 0;
      }
	}
}

bool get_key(int scancode)
{
   const Uint8 *kb = SDL_GetKeyboardState(NULL);
   return (bool)kb[scancode];
}

extern bool get_mmove(int *outx, int *outy)
{
   if (SDL_GetRelativeMouseMode() && (mrelx || mrely)) {
      *outx = mrelx;
      mrelx = 0;
      *outy = mrely;
      mrely = 0;
      return 1;
   } else {
      return 0;
   }
}


void fatal_error(char *message, char *file, int line)
{
   close_sdl();
   fprintf(stderr, "Error (%s line %d): %s\n", file, line, message);
   exit(1);
}
