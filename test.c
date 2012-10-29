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

#define ASSERT(condition) do { if (!(condition)) { return #condition; } } while(0)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static char const *testJsonNull(void);
static char const *testJsonTrue(void);
static char const *testJsonFalse(void);
static char const *testJsonNumber(void);
static char const *testJsonString(void);
static char const *testJsonArray(void);

typedef struct UnitTest
{
	char const *name;
	char const *(*function)(void);
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
	char const *assertion;

	for (; index != ARRAY_SIZE(tests); ++index)
	{
		printf("%-32s : ", tests[index].name);
		assertion = tests[index].function();

		if (assertion != NULL)
		{
			++failed;
			printf("failed\n  > %s\n", assertion);
		}
		else
		{
			printf("succeded\n");
		}
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

static char const *testJsonNull(void)
{
	QxJsonValue *value = qxJsonNullNew();
	ASSERT(value != NULL);
	ASSERT(QX_JSON_IS_NULL(value));
	qxJsonValueDecRef(value);
	return NULL;
}

static char const *testJsonTrue(void)
{
	QxJsonValue *value = qxJsonTrueNew();
	ASSERT(value != NULL);
	ASSERT(QX_JSON_IS_TRUE(value));
	qxJsonValueDecRef(value);
	return 0;
}

static char const *testJsonFalse(void)
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

static char const *testJsonNumber(void)
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

static char const *testJsonString(void)
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

static char const *testJsonArray(void)
{
	QxJsonArray *array;
	QxJsonValue *value;

	value = qxJsonArrayNew();
	ASSERT(value != NULL);
	ASSERT(QX_JSON_IS_ARRAY(value));
	array = QX_JSON_ARRAY(value);
	ASSERT(array != NULL);
	ASSERT(qxJsonArraySize(array) == 0);
	ASSERT(qxJsonArrayAppend(NULL, value) != 0);
	ASSERT(qxJsonArrayAppend(array, NULL) != 0);
	ASSERT(qxJsonArraySize(array) == 0);
	ASSERT(qxJsonArrayAppendNew(array, qxJsonNullNew()) == 0);
	ASSERT(qxJsonArraySize(array) == 1);
	ASSERT(qxJsonArrayPrependNew(array, qxJsonTrueNew()) == 0);
	ASSERT(qxJsonArraySize(array) == 2);
	ASSERT(QX_JSON_IS_TRUE(qxJsonArrayGet(array, 0)));
	ASSERT(QX_JSON_IS_NULL(qxJsonArrayGet(array, 1)));
	ASSERT(qxJsonArrayGet(array, 2) == NULL);

	qxJsonValueDecRef(value);
	return 0;
}
