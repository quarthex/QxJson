/**
 * @file qx.assert.h
 * @brief Header file used for the tests
 * @author Romain DEOUX
 */
#ifndef _H_QX_EXPECT
#define _H_QX_EXPECT

void __expect(char const *file, int line, int condition, char const *message, ...);

/* Booleans */
#define expect_ok(condition) \
	__expect(__FILE__, __LINE__, !!(condition), #condition " expected to be OK")

/* Integers */
void __expect_int_equal(char const *file, int line,
	char const *actualName, int expected, int actual);
#define expect_int_equal(actual, expected) \
	__expect_int_equal(__FILE__, __LINE__, #actual, (expected), (actual))

void __expect_int_not_equal(char const *file, int line,
	char const *actualName, int expected, int actual);
#define expect_int_not_equal(actual, expected) \
	__expect_int_not_equal(__FILE__, __LINE__, #actual, (expected), (actual))

#define expect_zero(value) expect_int_equal((value), 0)
#define expect_not_zero(value) expect_int_not_equal((value), 0)

/* Pointers */
#define expect_null(pointer) \
	__expect(__FILE__, __LINE__, (pointer) == NULL, \
	"" #pointer " expected to be null but is actually %p", (pointer))
#define expect_not_null(pointer) \
	__expect(__FILE__, __LINE__, (pointer) != NULL, \
	"" #pointer " expected not to be null")

/* Wide strings */
#include <wchar.h>
void __expect_wstr_equal(char const *file, int line,
	char const *actualName, wchar_t const *expected, wchar_t const *actual);
#define expect_wstr_equal(actual, expected) \
	__expect_wstr_equal(__FILE__, __LINE__, #actual, (expected), (actual))

#endif /* _H_QX_EXPECT */
