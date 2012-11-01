/**
 * @file qx.assert.h
 * @brief Header file used for the tests
 * @author Romain DEOUX
 */
#ifndef _H_QX_ASSERT
#define _H_QX_ASSERT

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Exit the program with a failure exit code if the assertion failed.
 */
#define QX_ASSERT(condition) do { \
	if (!(condition)) { \
		printf("Assertion failed: `" #condition "`\n"); \
		exit(EXIT_FAILURE); \
	} \
} while(0)

#endif /* _H_QX_ASSERT */
