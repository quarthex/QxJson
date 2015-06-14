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

find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		@ONLY
	)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generatin API documentation with Doxygen" VERBATIM
	)
endif()

include(test.cmake)
