/**
 * @file qx.test.number.c
 * @brief Testing source file of the QxJsonNumber class.
 * @author Romain DEOUX
 */

#include <qx.json.number.h>

#include "qx.assert.h"

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
		QX_ASSERT(value != NULL);
		QX_ASSERT(QX_JSON_IS_NUMBER(value));
		number = QX_JSON_NUMBER(value);
		QX_ASSERT(number != NULL);
		QX_ASSERT(compareNumbers(qxJsonNumberValue(number), numbers[index]) == 0);
		qxJsonValueUnref(value);
	}

	value = qxJsonNumberNew(-0.);
	QX_ASSERT(compareNumbers(qxJsonNumberValue(QX_JSON_NUMBER(value)), 0.) != 0);
	qxJsonValueUnref(value);

	return EXIT_SUCCESS;
}

