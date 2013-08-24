/**
 * @file string.c
 * @brief Testing source file of strings handling in the QxJsonValue class.
 * @author Romain DEOUX
 */

#include <stdlib.h>
#include <string.h>

#include <qx.json.value.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *string;

	string = QxJsonValue_stringNew(NULL, 5);
	expect_null(string);

	string = QxJsonValue_stringNew(L"Hello", 5);
	expect_not_null(string);
	expect_ok(QX_JSON_IS_STRING(string));
	expect_int_equal(QxJsonValue_size(string), 5);
	expect_zero(memcmp(QxJsonValue_stringValue(string), L"Hello", 5 * sizeof(wchar_t)));

	QxJsonValue_decref(string);
	return EXIT_SUCCESS;
}
