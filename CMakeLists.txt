cmake_minimum_required(VERSION 4.4.4)
project(rlWidgets LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(demo1src
	examples/main.c
)

add_executable(demo1 ${demo1src})

target_include_directories(demo1 PRIVATE
    ${CMAKE_SOURCE_DIR}/.
)

target_compile_options(demo1 PRIVATE
	-g
)

target_link_directories(demo1 PRIVATE
	${CMAKE_SOURCE_DIR}/vendor/raylib/lib
)
target_compile_definitions(demo1 PRIVATE SDL_STATIC_LIB)
target_include_directories(demo1 PRIVATE
	${CMAKE_SOURCE_DIR}/vendor/raylib/include/
)
target_link_libraries(demo1 PRIVATE
	libraylib.a
	m
	pthread
	dl
	X11
)
