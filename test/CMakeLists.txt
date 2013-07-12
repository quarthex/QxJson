cmake_minimum_required(VERSION 2.6)

include_directories(../include)

foreach(x array false null number object string tokenizer true wikipedia)
	add_executable(${x}.out ${x}.c expect.c expect.h)
	target_link_libraries(${x}.out QxJson)
	add_test(
		NAME ${x}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMAND ${x}.out
	)
endforeach()
