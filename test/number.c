/**
 * @file number.c
 * @brief Testing source file of numbers handling of the QxJsonValue class.
 * @author Romain DEOUX
 */

#include <stdlib.h>
#include <string.h>

#include <qx.json.value.h>

#include "expect.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static int compareNumbers(double first, double last)
{
	return memcmp(&first, &last, sizeof(double));
}

int main(void)
{
	QxJsonValue *number;
	size_t index = 0;

	double const numbers[] = {
		1234.5678,
		-0.,
		12345678,
	};

	for (; index != ARRAY_SIZE(numbers); ++index)
	{
		number = QxJsonValue_numberNew(numbers[index]);
		expect_not_null(number);
		expect_ok(QX_JSON_IS_NUMBER(number));
		expect_zero(compareNumbers(QxJsonValue_numberValue(number), numbers[index]));
		QxJsonValue_decref(number);
	}

	number = QxJsonValue_numberNew(-0.);
	expect_not_zero(compareNumbers(QxJsonValue_numberValue(number), 0.));
	QxJsonValue_decref(number);

	return EXIT_SUCCESS;
}
