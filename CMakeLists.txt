cmake_minimum_required(VERSION 2.6)

project(QxJson)

include_directories(../include)

add_library(QxJson SHARED
	src/qx.json.value.c
	src/qx.json.null.c
	src/qx.json.true.c
	src/qx.json.false.c
	src/qx.json.number.c
	src/qx.json.string.c
	src/qx.json.array.c)
set_target_properties(QxJson
	PROPERTIES COMPILE_DEFINITIONS QX_JSON_BUILD)

enable_testing()
add_subdirectory(test)

