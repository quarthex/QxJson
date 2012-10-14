/**
 * @file test.c
 * @brief Testing source file.
 * @author Romain DEOUX
 */

#include <stdio.h>
#include <stdlib.h>

#include "qx.json.value.h"
#include "qx.json.null.h"
#include "qx.json.true.h"
#include "qx.json.false.h"
#include "qx.json.number.h"

#define ASSERT(condition) do { if (!(condition)) return -1; } while(0)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static int testJsonNull(void);
static int testJsonTrue(void);
static int testJsonFalse(void);
static int testJsonNumber(void);

typedef struct UnitTest
{
	char const *name;
	int (*function)(void);
} UnitTest;

int main(void)
{
	UnitTest const tests[] = {
		{ "javascript null value", testJsonNull },
		{ "javascript true value", testJsonTrue },
		{ "javascript false value", testJsonFalse },
		{ "javascript number value", testJsonNumber }
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

static int testJsonNull(void)
{
	QxJsonValue *value = qxJsonNullNew();
	ASSERT(value != NULL);
	ASSERT(qxJsonValueType(value) == QxJsonValueTypeNull);
	qxJsonValueDecRef(value);
	return 0;
}

static int testJsonTrue(void)
{
	QxJsonValue *value = qxJsonTrueNew();
	ASSERT(value != NULL);
	ASSERT(qxJsonValueType(value) == QxJsonValueTypeTrue);
	qxJsonValueDecRef(value);
	return 0;
}

static int testJsonFalse(void)
{
	QxJsonValue *value = qxJsonFalseNew();
	ASSERT(value != NULL);
	ASSERT(qxJsonValueType(value) == QxJsonValueTypeFalse);
	qxJsonValueDecRef(value);
	return 0;
}

static int compareNumbers(qx_json_number_t first, qx_json_number_t last)
{
	return memcmp(&first, &last, sizeof(qx_json_number_t));
}

static int testJsonNumber(void)
{
	QxJsonNumber *number;
	QxJsonValue *value;
	size_t index = 0;

	qx_json_number_t const numbers[] = {
		1234.5678,
		-0.,
		12345678,
	};

	for (; index != ARRAY_SIZE(numbers); ++index)
	{
		value = qxJsonNumberNew(numbers[index]);
		ASSERT(value != NULL);
		ASSERT(qxJsonValueIsNumber(value));
		number = QX_JSON_NUMBER(value);
		ASSERT(number != NULL);
		ASSERT(compareNumbers(qxJsonNumberValue(number), numbers[index]) == 0);
		qxJsonValueDecRef(value);
	}

	value = qxJsonNumberNew(-0.);
	ASSERT(compareNumbers(qxJsonNumberValue(QX_JSON_NUMBER(value)), 0.) != 0);
	qxJsonValueDecRef(value);

	return 0;
}

