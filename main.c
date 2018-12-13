/*  QMAP: Quake level viewer
 *
 *   main.c    Copyright 1997 Sean Barett
 *
 *   General setup control, main "sim" loop
 */

#include <SDL_scancode.h>
#include <SDL_timer.h>
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
#include "text.h"

double chop_temp;

vector cam_loc, cam_vel, new_loc;
angvec cam_ang, cam_angvel;

char *scr_buf;
int   scr_row;

int fps=0, framecount=0, frametimer=0;

#define ANG_MXVL 256
#define ANG_ACCL 12
#define ANG_FRCT 6
#define VEL_MXVL 2.0f
#define VEL_ACCL 0.1f
#define VEL_FRCT 0.05f

#define MOUSE_SENS 6

char colormap[64][256];

void run_sim(void)
{
   bool running = TRUE;
   int mx, my;
   vector temp;
   char text[256];

   scr_buf = malloc(320*200);
   scr_row = 320;
   qmap_set_output(scr_buf, scr_row);

   cam_loc.x = 500;
   cam_loc.y = 240;
   cam_loc.z = 100;

   while (running) {
      
      int curtime = SDL_GetTicks();
      if (SDL_TICKS_PASSED(curtime, frametimer + 1000)) {
         frametimer = curtime;
         fps = framecount;
         framecount = 0;
      }

      // RENDER

      set_view_info(&cam_loc, &cam_ang);
      render_world(&cam_loc);
      blit(scr_buf);

      // UI
      
      snprintf(text, sizeof(text), "FPS: %d", fps);
      draw_text(8, 6, text);
      
      present();
      
      // CONTROLS

      poll_events(&running);
      
      if (get_key(SDL_SCANCODE_ESCAPE))
         running = false;

      if (get_key(SDL_SCANCODE_UP))
         cam_angvel.tx += ANG_ACCL;
      if (get_key(SDL_SCANCODE_DOWN))
         cam_angvel.tx -= ANG_ACCL;
      if (get_key(SDL_SCANCODE_Q))
         cam_angvel.ty += ANG_ACCL;
      if (get_key(SDL_SCANCODE_E))
         cam_angvel.ty -= ANG_ACCL;
      if (get_key(SDL_SCANCODE_RIGHT))
         cam_angvel.tz += ANG_ACCL;
      if (get_key(SDL_SCANCODE_LEFT))
         cam_angvel.tz -= ANG_ACCL;

      if (get_key(SDL_SCANCODE_D))
         cam_vel.x += VEL_ACCL;
      if (get_key(SDL_SCANCODE_A))
         cam_vel.x -= VEL_ACCL;
      if (get_key(SDL_SCANCODE_LSHIFT))
         cam_vel.z -= VEL_ACCL;
      if (get_key(SDL_SCANCODE_SPACE))
         cam_vel.z += VEL_ACCL;
      if (get_key(SDL_SCANCODE_S))
         cam_vel.y -= VEL_ACCL;
      if (get_key(SDL_SCANCODE_W))
         cam_vel.y += VEL_ACCL;

      // "PHYSICS"
      
      cam_vel.x = CLAMP(cam_vel.x, -VEL_MXVL, VEL_MXVL);
      cam_vel.y = CLAMP(cam_vel.y, -VEL_MXVL, VEL_MXVL);
      
      cam_angvel.tx = (fixang)CLAMP((short)cam_angvel.tx, -ANG_MXVL, ANG_MXVL);
      cam_angvel.ty = (fixang)CLAMP((short)cam_angvel.ty, -ANG_MXVL, ANG_MXVL);
      cam_angvel.tz = (fixang)CLAMP((short)cam_angvel.tz, -ANG_MXVL, ANG_MXVL);

      cam_ang.tx += cam_angvel.tx;
      cam_ang.ty += cam_angvel.ty;
      cam_ang.tz += cam_angvel.tz;
      
      if (get_mmove(&mx, &my)) {
         cam_ang.tz += mx * MOUSE_SENS;
         cam_ang.tx += my * MOUSE_SENS;
      }
      
      set_view_info(&cam_loc, &cam_ang);

      temp.x = cam_vel.x; temp.y = 0; temp.z = 0;
      rotate_vec(&temp);
      cam_loc.x  += temp.x; cam_loc.y += temp.y; cam_loc.z += temp.z;

      temp.x = 0; temp.y = cam_vel.y; temp.z = 0;
      rotate_vec(&temp);
      cam_loc.x  += temp.x; cam_loc.y += temp.y; cam_loc.z += temp.z;

      temp.x = 0; temp.y = 0; temp.z = cam_vel.z;
      rotate_vec(&temp);
      cam_loc.x  += temp.x; cam_loc.y += temp.y; cam_loc.z += temp.z;
      
      if (cam_vel.x > 0.0f) {
         cam_vel.x -= VEL_FRCT;
         if (cam_vel.x < 0.0f)
            cam_vel.x = 0.0f;
      } else if (cam_vel.x < 0.0f) {
         cam_vel.x += VEL_FRCT;
         if (cam_vel.x > 0.0f)
            cam_vel.x = 0.0f;
      }
      if (cam_vel.y > 0.0f) {
         cam_vel.y -= VEL_FRCT;
         if (cam_vel.y < 0.0f)
            cam_vel.y = 0.0f;
      } else if (cam_vel.y < 0.0f) {
         cam_vel.y += VEL_FRCT;
         if (cam_vel.y > 0.0f)
            cam_vel.y = 0.0f;
      }
      if (cam_vel.z > 0.0f) {
         cam_vel.z -= VEL_FRCT;
         if (cam_vel.z < 0.0f)
            cam_vel.z = 0.0f;
      } else if (cam_vel.z < 0.0f) {
         cam_vel.z += VEL_FRCT;
         if (cam_vel.z > 0.0f)
            cam_vel.z = 0.0f;
      }
      
      if ((short)cam_angvel.tx > 0) {
         cam_angvel.tx -= ANG_FRCT;
         if ((short)cam_angvel.tx < 0)
            cam_angvel.tx = 0;
      } else if ((short)cam_angvel.tx < 0) {
         cam_angvel.tx += ANG_FRCT;
         if ((short)cam_angvel.tx > 0)
            cam_angvel.tx = 0;
      }
      if ((short)cam_angvel.ty > 0) {
         cam_angvel.ty -= ANG_FRCT;
         if ((short)cam_angvel.ty < 0)
            cam_angvel.ty = 0;
      } else if ((short)cam_angvel.ty < 0) {
         cam_angvel.ty += ANG_FRCT;
         if ((short)cam_angvel.ty > 0)
            cam_angvel.ty = 0;
      }
      if ((short)cam_angvel.tz > 0) {
         cam_angvel.tz -= ANG_FRCT;
         if ((short)cam_angvel.tz < 0)
            cam_angvel.tz = 0;
      } else if ((short)cam_angvel.tz < 0) {
         cam_angvel.tz += ANG_FRCT;
         if ((short)cam_angvel.tz > 0)
            cam_angvel.tz = 0;
      }
      
      ++framecount;
   }
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

      run_sim();
      close_sdl();
   }
   return 0;
}
