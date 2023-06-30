cmake_minimum_required(VERSION 3.18.0)
project(CacheSim VERSION 0.1.0)



set(dirs											# dirs to compile
	src
)

set(COMPILE_SOURCES)					# files from dirs put into COMPILE_SOURCES

foreach(dir ${dirs})
	file(GLOB_RECURSE sources  "${dir}/*.cpp" "${dir}/*.c")
	list(APPEND COMPILE_SOURCES ${sources})
endforeach()

add_executable(${PROJECT_NAME}
	${COMPILE_SOURCES}
)

