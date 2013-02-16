cmake_minimum_required(VERSION 2.6)

include_directories(../include)

foreach(x array false null number object string tokenizer true)
	add_executable(${x}.out ${x}.c assert.h)
	target_link_libraries(${x}.out QxJson)
	add_test(${x} ${x}.out)
endforeach()
