/*  QMAP: Quake level viewer
 *
 *   cam->c     Copyright 2018 Nicholas Curtis
 *
 *   General setup control, main "sim" loop
 */

#include <SDL.h>

#include "mode.h"
#include "clock.h"
#include "cam.h"

#define ANG_MXVL 256
#define ANG_ACCL 12
#define ANG_FRCT 6
#define VEL_MXVL 2.0f
#define VEL_ACCL 0.1f
#define VEL_FRCT 0.05f

#define MOUSE_SENS 6

extern void cam_init(camera *cam)
{
	cam->loc    = (vector){0.0f, 0.0f, 0.0f};
	cam->vel    = (vector){0.0f, 0.0f, 0.0f};
	cam->ang    = (angvec){0, 0, 0};
	cam->angvel = (angvec){0, 0, 0};
}

extern void cam_update(camera *cam)
{
	int mx, my;
	vector temp;
	
	// CONTROLS

	if (get_key(SDL_SCANCODE_UP))
		cam->angvel.tx += ANG_ACCL;
	if (get_key(SDL_SCANCODE_DOWN))
		cam->angvel.tx -= ANG_ACCL;
	if (get_key(SDL_SCANCODE_Q))
		cam->angvel.ty += ANG_ACCL;
	if (get_key(SDL_SCANCODE_E))
		cam->angvel.ty -= ANG_ACCL;
	if (get_key(SDL_SCANCODE_RIGHT))
		cam->angvel.tz += ANG_ACCL;
	if (get_key(SDL_SCANCODE_LEFT))
		cam->angvel.tz -= ANG_ACCL;

	if (get_key(SDL_SCANCODE_D))
		cam->vel.x += VEL_ACCL;
	if (get_key(SDL_SCANCODE_A))
		cam->vel.x -= VEL_ACCL;
	if (get_key(SDL_SCANCODE_LSHIFT))
		cam->vel.z -= VEL_ACCL;
	if (get_key(SDL_SCANCODE_SPACE))
		cam->vel.z += VEL_ACCL;
	if (get_key(SDL_SCANCODE_S))
		cam->vel.y -= VEL_ACCL;
	if (get_key(SDL_SCANCODE_W))
		cam->vel.y += VEL_ACCL;
	
	// apply mouse movement
	if (get_mmove(&mx, &my)) {
		cam->ang.tz += mx * MOUSE_SENS;
		cam->ang.tx += my * MOUSE_SENS;
	}
	
	
	// "PHYSICS"
	
	// clamp velocity
	cam->vel.x = CLAMP(cam->vel.x, -VEL_MXVL, VEL_MXVL);
	cam->vel.y = CLAMP(cam->vel.y, -VEL_MXVL, VEL_MXVL);
	cam->vel.z = CLAMP(cam->vel.z, -VEL_MXVL, VEL_MXVL);
	
	// clamp rotational velocity
	cam->angvel.tx = (fixang)CLAMP((short)cam->angvel.tx, -ANG_MXVL, ANG_MXVL);
	cam->angvel.ty = (fixang)CLAMP((short)cam->angvel.ty, -ANG_MXVL, ANG_MXVL);
	cam->angvel.tz = (fixang)CLAMP((short)cam->angvel.tz, -ANG_MXVL, ANG_MXVL);

	// apply rotational velocity
	cam->ang.tx += cam->angvel.tx;
	cam->ang.ty += cam->angvel.ty;
	cam->ang.tz += cam->angvel.tz;
	
	//set_view_info(&cam->loc, &cam->ang);

	// apply translational velocity (transformed by rotation)
	temp.x = cam->vel.x;
	temp.y = 0;
	temp.z = 0;
	rotate_vec(&temp);
	cam->loc.x += temp.x;
	cam->loc.y += temp.y;
	cam->loc.z += temp.z;

	temp.x = 0;
	temp.y = cam->vel.y;
	temp.z = 0;
	rotate_vec(&temp);
	cam->loc.x += temp.x;
	cam->loc.y += temp.y;
	cam->loc.z += temp.z;

	temp.x = 0;
	temp.y = 0;
	temp.z = cam->vel.z;
	rotate_vec(&temp);
	cam->loc.x += temp.x;
	cam->loc.y += temp.y;
	cam->loc.z += temp.z;
	
	// apply translational friction
	if (cam->vel.x > 0.0f) {
		cam->vel.x -= VEL_FRCT;
		if (cam->vel.x < 0.0f)
			cam->vel.x = 0.0f;
	} else if (cam->vel.x < 0.0f) {
		cam->vel.x += VEL_FRCT;
		if (cam->vel.x > 0.0f)
			cam->vel.x = 0.0f;
	}
	if (cam->vel.y > 0.0f) {
		cam->vel.y -= VEL_FRCT;
		if (cam->vel.y < 0.0f)
			cam->vel.y = 0.0f;
	} else if (cam->vel.y < 0.0f) {
		cam->vel.y += VEL_FRCT;
		if (cam->vel.y > 0.0f)
			cam->vel.y = 0.0f;
	}
	if (cam->vel.z > 0.0f) {
		cam->vel.z -= VEL_FRCT;
		if (cam->vel.z < 0.0f)
			cam->vel.z = 0.0f;
	} else if (cam->vel.z < 0.0f) {
		cam->vel.z += VEL_FRCT;
		if (cam->vel.z > 0.0f)
			cam->vel.z = 0.0f;
	}
	
	// apply rotational friction
	if ((short)cam->angvel.tx > 0) {
		cam->angvel.tx -= ANG_FRCT;
		if ((short)cam->angvel.tx < 0)
			cam->angvel.tx = 0;
	} else if ((short)cam->angvel.tx < 0) {
		cam->angvel.tx += ANG_FRCT;
		if ((short)cam->angvel.tx > 0)
			cam->angvel.tx = 0;
	}
	if ((short)cam->angvel.ty > 0) {
		cam->angvel.ty -= ANG_FRCT;
		if ((short)cam->angvel.ty < 0)
			cam->angvel.ty = 0;
	} else if ((short)cam->angvel.ty < 0) {
		cam->angvel.ty += ANG_FRCT;
		if ((short)cam->angvel.ty > 0)
			cam->angvel.ty = 0;
	}
	if ((short)cam->angvel.tz > 0) {
		cam->angvel.tz -= ANG_FRCT;
		if ((short)cam->angvel.tz < 0)
			cam->angvel.tz = 0;
	} else if ((short)cam->angvel.tz < 0) {
		cam->angvel.tz += ANG_FRCT;
		if ((short)cam->angvel.tz > 0)
			cam->angvel.tz = 0;
	}
}
