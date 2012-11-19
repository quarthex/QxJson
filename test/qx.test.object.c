/**
 * @file qx.test.object.c
 * @brief Testing source file of the QxJsonObject class.
 * @author Romain DEOUX
 */

#include <qx.json.object.h>
#include <qx.json.string.h>

#include "qx.assert.h"

int main(void)
{
	QxJsonValue *value;
	QxJsonObject *object;
	QxJsonString *key;

	value = qxJsonObjectNew();
	QX_ASSERT(value != NULL);
	QX_ASSERT(QX_JSON_IS_OBJECT(value));
	object = QX_JSON_OBJECT(value);
	QX_ASSERT(object != NULL);

	value = qxJsonStringNew(L"Test", 4);
	key = QX_JSON_STRING(value);
	value = qxJsonStringNew(L"qx", 2);
	QX_ASSERT(qxJsonObjectSet(object, key, value) == 0);
	QX_ASSERT(qxJsonObjectSize(object) == 1);
	QX_ASSERT(qxJsonObjectUnset(object, key) == 0);
	QX_ASSERT(qxJsonObjectSize(object) == 0);

	qxJsonValueUnref((QxJsonValue *)(key));
	qxJsonValueUnref(value);
	qxJsonValueUnref((QxJsonValue *)(object));
	return EXIT_SUCCESS;
}

