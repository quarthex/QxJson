cmake_minimum_required(VERSION 2.6)

project(QxJson)

include_directories(../include)
if(CMAKE_COMPILER_IS_GNUCC)
	add_definitions(-W -Wall -pedantic)
endif()

add_library(QxJson SHARED
	include/qx.json.array.h
	include/qx.json.false.h
	include/qx.json.macro.h
	include/qx.json.null.h
	include/qx.json.number.h
	include/qx.json.object.h
	include/qx.json.string.h
	include/qx.json.tockenizer.h
	include/qx.json.true.h
	include/qx.json.value.h
	src/array.c
	src/false.c
	src/null.c
	src/number.c
	src/object.c
	src/string.c
	src/tockenizer.c
	src/true.c
	src/value.c
	src/value.private.h
	src/tools/queue.c
	src/tools/queue.h
)
set_target_properties(QxJson
	PROPERTIES COMPILE_DEFINITIONS QX_JSON_BUILD)

enable_testing()
add_subdirectory(test)
