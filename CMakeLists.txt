cmake_minimum_required(VERSION 3.12)
project(qmap)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

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
	text.h
	clock.h

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
	surface.c
	clock.c
	iso_font.c)

add_executable(qmap ${SOURCES})
target_link_libraries(qmap ${SDL2_LIBRARIES} m)
