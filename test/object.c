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

	value = qxJsonObjectNew();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_OBJECT(value));
	object = QX_JSON_OBJECT(value);
	expect_not_null(object);

	value = qxJsonStringNew(L"Test", 4);
	key = QX_JSON_STRING(value);
	value = qxJsonStringNew(L"qx", 2);
	expect_zero(qxJsonObjectSet(object, key, value));
	expect_int_equal(qxJsonObjectSize(object), 1);
	expect_zero(qxJsonObjectUnset(object, key));
	expect_int_equal(qxJsonObjectSize(object), 0);

	qxJsonValueUnref((QxJsonValue *)(key));
	qxJsonValueUnref(value);
	qxJsonValueUnref((QxJsonValue *)(object));
	return EXIT_SUCCESS;
}
