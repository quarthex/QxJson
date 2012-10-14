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
#include "qx.js.number.h"

#define ASSERT(condition) do { if (!(condition)) return -1; } while(0)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static int testJsNull(void);
static int testJsTrue(void);
static int testJsFalse(void);
static int testJsNumber(void);

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
		{ "javascript false value", testJsFalse },
		{ "javascript number value", testJsNumber }
	};
	size_t failed = 0;
	size_t index = 0;

	for (; index != ARRAY_SIZE(tests); ++index)
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
	return 0;
}

static int testJsFalse(void)
{
	QxJsValue *value = qxJsFalseNew();
	ASSERT(value != NULL);
	ASSERT(qxJsValueType(value) == QxJsValueTypeFalse);
	qxJsValueDecRef(value);
	return 0;
}

static int testJsNumber(void)
{
	QxJsNumber *number;
	QxJsValue *value;
	size_t index = 0;

	qx_js_number_t const numbers[] = {
		1234.5678,
		-0.0,
		12345678,
	};

	for (; index != ARRAY_SIZE(numbers); ++index)
	{
		value = qxJsNumberNew(numbers[index]);
		ASSERT(value != NULL);
		ASSERT(qxJsValueIsNumber(value));
		number = QX_JS_NUMBER(value);
		ASSERT(number != NULL);
		ASSERT(qxJsNumberValue(number) == numbers[index]);
		qxJsValueDecRef(value);
	}

	return 0;
}

