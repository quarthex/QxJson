cmake_minimum_required(VERSION 2.6)

include_directories(../include)

foreach(x null true false number string array object tockenizer)
	add_executable(${x}.out ${x}.c)
	target_link_libraries(${x}.out QxJson)
	add_test(${x} ${x}.out)
endforeach()

