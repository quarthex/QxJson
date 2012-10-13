/**
 * @file test.c
 * @brief Testing source file.
 * @author Romain Déoux <romain.deoux@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>

#include "qx.js.value.h"
#include "qx.js.null.h"

#define ASSERT(condition) do { if (!(condition)) return -1; } while(0)

static int testJsNull(void);

typedef struct UnitTest
{
	char const *name;
	int (*function)(void);
} UnitTest;

int main(void)
{
	UnitTest const tests[] = {
		"javascript null value", testJsNull
	};
	size_t failed = 0;
	size_t index = 0;
	size_t const end = sizeof(tests) / sizeof(UnitTest);

	for (; index != end; ++index)
	{
		printf("%-32s : ", tests[index].name);

		if (tests[index].function() != 0)
		{
			++failed;
			printf("failed");
		}
		else
		{
			printf("succeded");
		}

		putchar('\n');
	}

	switch (failed)
	{
	case 0:
		printf("All tests passed\n");
		return EXIT_SUCCESS;

	case 1:
		printf("1 test");
		break;

	default:
		printf("%d tests");
		break;
	}

	printf(" failed\n");
	return EXIT_FAILURE;
}

static int testJsNull(void)
{
	QxJsValue *value = qxJsNullNew();
	ASSERT(value != NULL);
	ASSERT(qxJsValueType(value) == QxJsValueTypeNull);
	qxJsValueDecRef(value);
	return 0;
}
