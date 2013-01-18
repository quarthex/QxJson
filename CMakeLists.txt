cmake_minimum_required(VERSION 2.6)

project(QxJson)

include_directories(../include)

add_library(QxJson SHARED
	src/value.c
	src/null.c
	src/true.c
	src/false.c
	src/number.c
	src/string.c
	src/array.c
	src/object.c
	src/tockenizer.c
)
set_target_properties(QxJson
	PROPERTIES COMPILE_DEFINITIONS QX_JSON_BUILD)

enable_testing()
add_subdirectory(test)

