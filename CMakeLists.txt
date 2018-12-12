cmake_minimum_required(VERSION 3.12)
project(qmap)

set(SOURCES

	3d.h
	bspfile.h
	bsp.h
	cmdlib.h
	fix.h
	mode.h
	poly.h
	render.h
	scr.h
	s.h
	surface.h
	tmap3d.h
	tm.h

	main.c
	clippoly.c
	render.c
	3d.c
	mode.c
	bsp.c
	readbsp.c
	tmap3d.c
	poly.c
	tm_c.c
	surface.c)

add_executable(qmap ${SOURCES})
