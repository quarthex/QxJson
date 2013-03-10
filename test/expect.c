#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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

void __expect_wstr_equal(char const *file, int line,
	char const *actualName, wchar_t const *expected, wchar_t const *actual)
{
	__expect(file, line, wcscmp(expected, actual) == 0,
		"%s expected to be equal to \"%ls\" but is actually equal to \"%ls\"",
		actualName, expected, actual);
}
