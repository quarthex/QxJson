cmake_minimum_required(VERSION 2.6)

project(QxJson)

add_library(QxJson SHARED
	qx.json.value.c
	qx.json.null.c
	qx.json.true.c
	qx.json.false.c
	qx.json.number.c
	qx.json.string.c
	qx.json.array.c)
set_target_properties(QxJson
	PROPERTIES COMPILE_DEFINITIONS QX_JSON_BUILD)

enable_testing()
add_subdirectory(test)

