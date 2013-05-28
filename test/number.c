/**
 * @file number.c
 * @brief Testing source file of the QxJsonNumber class.
 * @author Romain DEOUX
 */

#include <stdlib.h>
#include <string.h>

#include <qx.json.number.h>

#include "expect.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static int compareNumbers(qx_json_number_t first, qx_json_number_t last)
{
	return memcmp(&first, &last, sizeof(qx_json_number_t));
}

int main(void)
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
		expect_not_null(value);
		expect_ok(QX_JSON_IS_NUMBER(value));
		number = QX_JSON_NUMBER(value);
		expect_not_null(number);
		expect_zero(compareNumbers(qxJsonNumberValue(number), numbers[index]));
		qxJsonValueUnref(value);
	}

	value = qxJsonNumberNew(-0.);
	expect_not_zero(compareNumbers(qxJsonNumberValue(QX_JSON_NUMBER(value)), 0.));
	qxJsonValueUnref(value);

	return EXIT_SUCCESS;
}
