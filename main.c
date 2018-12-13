/*  QMAP: Quake level viewer
 *
 *   main.c    Copyright 1997 Sean Barett
 *
 *   General setup control, main "sim" loop
 */

#include <SDL_scancode.h>
#include <stdio.h>
#include "bspfile.h"
#include "mode.h"
#include "3d.h"
#include "fix.h"
#include "scr.h"
#include "tm.h"
#include "render.h"
#include "bsp.h"
#include "surface.h"
#include "poly.h"
#include "clock.h"
#include "text.h"
#include "cam.h"

double chop_temp;

char *scr_buf;
int   scr_row;

char colormap[64][256];

void run_sim(void)
{
   bool running = TRUE;
   char text[256];
   camera cam;

   scr_buf = malloc(320*200);
   scr_row = 320;
   qmap_set_output(scr_buf, scr_row);

   cam_init(&cam);
   cam.loc.x = 500;
   cam.loc.y = 240;
   cam.loc.z = 100;

   while (running) {

      // RENDER

      set_view_info(&cam.loc, &cam.ang);
      render_world(&cam.loc);
      blit(scr_buf);

      // UI
      
      snprintf(text, sizeof(text), "FPS: %.2f", fps);
      draw_text(8, 6, text);
      
      present();
      
      // INPUT

      poll_events(&running);
      if (get_key(SDL_SCANCODE_ESCAPE))
		   running = false;
      
      // LOGIC
      
      clock_tick();
      cam_update(&cam);
   }
   
   free(scr_buf);
}

void load_graphics(void)
{
   char pal[768];
   FILE *f;
   if ((f = fopen("palette.lmp", "rb")) == NULL)
      fatal("Couldn't read palette.lmp\n");
   fread(pal, 1, 768, f);
   fclose(f);
   set_pal((uchar*)pal);
   if ((f = fopen("colormap.lmp", "rb")) == NULL)
      fatal("Couldn't read colormap.lmp\n");
   fread(colormap, 256, 64, f);
   fclose(f);
}

int main(int argc, char **argv)
{
   if (argc != 2) {
      printf("Usage: qmap <bspfile>\n");
   } else {
      LoadBSPFile(argv[1]);
      setup_sdl();
      load_graphics();
      init_cache();
      setup_default_point_list();
      clock_init();
      
      run_sim();
      close_sdl();
   }
   return 0;
}
