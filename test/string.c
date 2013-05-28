/**
 * @file string.c
 * @brief Testing source file of the QxJsonString class.
 * @author Romain DEOUX
 */

#include <stdlib.h>
#include <string.h>

#include <qx.json.string.h>

#include "expect.h"

int main(void)
{
	QxJsonString *string;
	QxJsonValue *value;

	value = qxJsonStringNew(NULL, 5);
	expect_null(value);

	value = qxJsonStringNew(L"Hello", 5);
	expect_ok(QX_JSON_IS_STRING(value));
	string = QX_JSON_STRING(value);
	expect_not_null(string);
	expect_int_equal(qxJsonStringSize(string), 5);
	expect_zero(memcmp(qxJsonStringData(string), L"Hello", 5 * sizeof(wchar_t)));

	qxJsonValueUnref(value);
	return EXIT_SUCCESS;
}
