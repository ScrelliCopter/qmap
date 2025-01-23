cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
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
find_package(SDL3 REQUIRED CONFIG)
target_link_libraries(qmap SDL3::SDL3)
target_link_libraries(qmap $<$<C_COMPILER_ID:Clang,GNU>:m>)
target_compile_options(qmap PRIVATE
	$<$<C_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra>)

if (CMAKE_C_PLATFORM_ID STREQUAL "Darwin")
	get_property(SDL3_IMPORTED_LOCATION TARGET SDL3::SDL3 PROPERTY IMPORTED_LOCATION)
	if (SDL3_IMPORTED_LOCATION MATCHES "^/Library/Frameworks/")
		set_property(TARGET qmap PROPERTY BUILD_RPATH "/Library/Frameworks")
	endif()
endif()

if (EMSCRIPTEN)
	set(FILES
		"--embed-file=palette.lmp"
		"--embed-file=colormap.lmp"
		"--embed-file=e1m1.bsp")
	target_link_options(qmap PRIVATE ${FILES})
endif()
