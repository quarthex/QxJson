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
#include "qx.json.string.h"
#include "qx.json.array.h"

#define ASSERT(condition) do { if (!(condition)) return -1; } while(0)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static int testJsonNull(void);
static int testJsonTrue(void);
static int testJsonFalse(void);
static int testJsonNumber(void);
static int testJsonString(void);
static int testJsonArray(void);

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
		{ "javascript number value", testJsonNumber },
		{ "javascript string value", testJsonString },
		{ "javascript array value", testJsonArray }
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
	ASSERT(QX_JSON_IS_NULL(value));
	qxJsonValueDecRef(value);
	return 0;
}

static int testJsonTrue(void)
{
	QxJsonValue *value = qxJsonTrueNew();
	ASSERT(value != NULL);
	ASSERT(QX_JSON_IS_TRUE(value));
	qxJsonValueDecRef(value);
	return 0;
}

static int testJsonFalse(void)
{
	QxJsonValue *value = qxJsonFalseNew();
	ASSERT(value != NULL);
	ASSERT(QX_JSON_IS_FALSE(value));
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
		ASSERT(QX_JSON_IS_NUMBER(value));
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

static int testJsonString(void)
{
	QxJsonString *string;
	QxJsonValue *value;

	value = qxJsonStringNew(NULL, 5);
	ASSERT(value == NULL);

	value = qxJsonStringNew(L"Hello", 5);
	ASSERT(QX_JSON_IS_STRING(value));
	string = QX_JSON_STRING(value);
	ASSERT(string != NULL);
	ASSERT(qxJsonStringSize(string) == 5);
	ASSERT(memcmp(qxJsonStringData(string), L"Hello", 5 * sizeof(wchar_t)) == 0);
	qxJsonValueDecRef(value);

	return 0;
}

static int testJsonArray(void)
{
	QxJsonArray *array;
	QxJsonValue *value;

	value = qxJsonArrayNew();
	ASSERT(value != NULL);
	ASSERT(QX_JSON_IS_ARRAY(value));
	array = QX_JSON_ARRAY(value);
	ASSERT(array != NULL);
	ASSERT(qxJsonArraySize(array) == 0);

	qxJsonValueDecRef(value);
	return 0;
}
