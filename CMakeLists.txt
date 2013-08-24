cmake_minimum_required(VERSION 2.6)

project(QxJson)

include_directories(../include)
if(CMAKE_COMPILER_IS_GNUCC)
	add_definitions(-W -Wall -pedantic)
endif()

add_library(QxJson SHARED
	include/qx.json.macro.h
	include/qx.json.parser.h
	include/qx.json.tokenizer.h
	include/qx.json.value.h
	src/parser.c
	src/tokenizer.c
	src/value.c
)
set_target_properties(QxJson
	PROPERTIES COMPILE_DEFINITIONS QX_JSON_BUILD)

enable_testing()
add_subdirectory(test)
