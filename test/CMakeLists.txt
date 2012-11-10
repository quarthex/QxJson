cmake_minimum_required(VERSION 2.6)

include_directories(../include)

foreach(x null true false number string array)
	add_executable(qx.${x} qx.test.${x}.c)
	target_link_libraries(qx.${x} QxJson)
	add_test(${x} qx.${x})
endforeach()

