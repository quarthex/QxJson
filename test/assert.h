/**
 * @file qx.assert.h
 * @brief Header file used for the tests
 * @author Romain DEOUX
 */
#ifndef _H_QX_ASSERT
#define _H_QX_ASSERT

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define _QX_ERROR(message) \
	fwrite((message), sizeof(char), sizeof(message), stderr)

/**
 * @brief Exit the program with a failure exit code if the assertion failed.
 */
#define QX_ASSERT(condition) do { \
	if (!(condition)) { \
		_QX_ERROR("Assertion failed: `" #condition "`\n"); \
		exit(EXIT_FAILURE); \
	} \
} while(0)

#endif /* _H_QX_ASSERT */
