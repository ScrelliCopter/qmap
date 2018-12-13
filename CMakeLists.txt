cmake_minimum_required(VERSION 3.12)
project(qmap)

if (NOT EMSCRIPTEN)
	find_package(SDL2 REQUIRED)
	add_definitions(${SDL2_CFLAGS})
	include_directories(${SDL2_INCLUDE_DIRS})
	set(LIBS ${SDL2_LIBRARIES} m)
else()
	set(FILES
		"--preload-file palette.lmp"
		"--preload-file colormap.lmp"
		"--preload-file e1m1.bsp")

	add_definitions(-O2 -s USE_SDL=2)
	set(LIBS "-s USE_SDL=2" ${FILES} m)
endif()

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
	cam.h

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
	cam.c
	iso_font.c)

add_executable(qmap ${SOURCES})
target_link_libraries(qmap ${LIBS})
