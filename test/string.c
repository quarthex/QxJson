/**
 * @file qx.test.string.c
 * @brief Testing source file of the QxJsonString class.
 * @author Romain DEOUX
 */

#include <string.h>

#include <qx.json.string.h>

#include "assert.h"

int main(void)
{
	QxJsonString *string;
	QxJsonValue *value;

	value = qxJsonStringNew(NULL, 5);
	QX_ASSERT(value == NULL);

	value = qxJsonStringNew(L"Hello", 5);
	QX_ASSERT(QX_JSON_IS_STRING(value));
	string = QX_JSON_STRING(value);
	QX_ASSERT(string != NULL);
	QX_ASSERT(qxJsonStringSize(string) == 5);
	QX_ASSERT(memcmp(qxJsonStringData(string), L"Hello", 5 * sizeof(wchar_t)) == 0);

	qxJsonValueUnref(value);
	return EXIT_SUCCESS;
}

