include(CTest)

if(BUILD_TESTING)
	include_directories(../include)

	foreach(x array false null number object parser string true wikipedia)
		add_executable(test-${x}
			../test/${x}.c ../test/expect.c ../test/expect.h)
		target_link_libraries(test-${x} QxJson)
		add_test(
			NAME ${x}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMAND test-${x}
		)
	endforeach()
endif()
