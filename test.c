/**
 * @file test.c
 * @brief Testing source file.
 * @author Romain DEOUX
 */

#include <stdio.h>
#include <stdlib.h>

#include "qx.js.value.h"
#include "qx.js.null.h"
#include "qx.js.true.h"
#include "qx.js.false.h"

#define ASSERT(condition) do { if (!(condition)) return -1; } while(0)

static int testJsNull(void);
static int testJsTrue(void);
static int testJsFalse(void);

typedef struct UnitTest
{
	char const *name;
	int (*function)(void);
} UnitTest;

int main(void)
{
	UnitTest const tests[] = {
		{ "javascript null value", testJsNull },
		{ "javascript true value", testJsTrue },
		{ "javascript false value", testJsFalse }
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

static int testJsTrue(void)
{
	QxJsValue *value = qxJsTrueNew();
	ASSERT(value != NULL);
	ASSERT(qxJsValueType(value) == QxJsValueTypeTrue);
	qxJsValueDecRef(value);
}

static int testJsFalse(void)
{
	QxJsValue *value = qxJsFalseNew();
	ASSERT(value != NULL);
	ASSERT(qxJsValueType(value) == QxJsValueTypeFalse);
	qxJsValueDecRef(value);
}

