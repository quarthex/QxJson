cmake_minimum_required(VERSION 2.6)
project(QxJson)

add_definitions(-DQX_JSON_BUILD)
if(CMAKE_COMPILER_IS_GNUCC)
	add_definitions(-W -Wall -pedantic)
endif()

add_library(QxJson SHARED
	../include/qx.json.macro.h
	../include/qx.json.parser.h
	../include/qx.json.value.h
	../src/parser.c
	../src/value.c
)

include(test.cmake)
