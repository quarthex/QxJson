/**
 * @file expect.c
 * @brief Source file used for the tests
 * @author Romain DEOUX
 */
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expect.h"

void __expect(char const *file, int line, int condition, char const *message, ...)
{
	va_list args;

	if (!condition)
	{
		fprintf(stderr, "Assertion failed in %s(%d): ", file, line);
		va_start(args, message);
		vfprintf(stderr, message, args);
		va_end(args);
		fprintf(stderr, "\n");
		raise(SIGTRAP);
		exit(EXIT_FAILURE);
	}
}

void __expect_int_equal(char const *file, int line,
	char const *actualName, int expected, int actual)
{
	__expect(file, line, expected == actual,
		"%s expected to be equal to %d bus is actually equal to %d",
		actualName, expected, actual);
}

void __expect_int_not_equal(char const *file, int line,
	char const *actualName, int expected, int actual)
{
	__expect(file, line, expected != actual,
			 "%s expected not to be equal to %d", actualName, expected);
}

void __expect_double_equal(char const *file, int line,
	char const *actualName, double expected, double actual)
{
	__expect(file, line, expected == actual,
		"%s expected to be equal to %g bus is actually equal to %g",
		actualName, expected, actual);
}

void __expect_str_equal(char const *file, int line,
	char const *actualName, char const *expected, char const *actual)
{
	int ok;

	if (expected && actual)
		ok = strcmp(expected, actual) == 0; /* Both are not nul */
	else if (expected || actual)
		ok = 0; /* One of them is nul */
	else
		ok = 1; /* Both are nuls */

	__expect(file, line, ok,
		"%s expected to be equal to \"%s\" but is actually equal to \"%s\"",
		actualName, expected, actual);
}

void __expect_wstr_equal(char const *file, int line,
	char const *actualName, wchar_t const *expected, wchar_t const *actual)
{
	int ok;

	if (expected && actual)
		ok = wcscmp(expected, actual) == 0; /* Both are not nul */
	else if (expected || actual)
		ok = 0; /* One of them is nul */
	else
		ok = 1; /* Both are nuls */

	__expect(file, line, ok,
		"%s expected to be equal to \"%ls\" but is actually equal to \"%ls\"",
		actualName, expected, actual);
}
