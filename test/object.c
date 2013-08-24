/**
 * @file object.c
 * @brief Testing source file of objects handling in the QxJsonValue class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include <qx.json.value.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *object, *key, *value;

	object = QxJsonValue_objectNew();
	expect_not_null(object);
	expect_ok(QX_JSON_IS_OBJECT(object));

	key = QxJsonValue_stringNew(L"Test", 4);
	value = QxJsonValue_stringNew(L"qx", 2);
	expect_zero(QxJsonValue_objectSet(object, key, value));
	expect_int_equal(QxJsonValue_size(object), 1);
	expect_zero(QxJsonValue_objectUnset(object, key));
	expect_int_equal(QxJsonValue_size(object), 0);

	QxJsonValue_decref(key);
	QxJsonValue_decref(value);
	QxJsonValue_decref(object);
	return EXIT_SUCCESS;
}
