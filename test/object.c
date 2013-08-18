/**
 * @file object.c
 * @brief Testing source file of the QxJsonObject class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include <qx.json.object.h>
#include <qx.json.string.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *value;
	QxJsonObject *object;
	QxJsonString *key;

	value = QxJsonObject_new();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_OBJECT(value));
	object = QX_JSON_OBJECT(value);
	expect_not_null(object);

	value = QxJsonString_new(L"Test", 4);
	key = QX_JSON_STRING(value);
	value = QxJsonString_new(L"qx", 2);
	expect_zero(QxJsonObject_set(object, key, value));
	expect_int_equal(QxJsonObject_size(object), 1);
	expect_zero(QxJsonObject_unset(object, key));
	expect_int_equal(QxJsonObject_size(object), 0);

	QxJsonValue_decref((QxJsonValue *)(key));
	QxJsonValue_decref(value);
	QxJsonValue_decref((QxJsonValue *)(object));
	return EXIT_SUCCESS;
}
