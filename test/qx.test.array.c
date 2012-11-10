/**
 * @file qx.test.array.c
 * @brief Testing source file of the QxJsonArray class.
 * @author Romain DEOUX
 */

#include <stddef.h>
#include <qx.json.array.h>
#include <qx.json.false.h>
#include <qx.json.null.h>
#include <qx.json.true.h>

#include "qx.assert.h"

int main(void)
{
	QxJsonArray *array;
	QxJsonValue *value;

	value = qxJsonArrayNew();
	QX_ASSERT(value != NULL);
	QX_ASSERT(QX_JSON_IS_ARRAY(value));
	array = QX_JSON_ARRAY(value);
	QX_ASSERT(array != NULL);
	QX_ASSERT(qxJsonArraySize(array) == 0);

	QX_ASSERT(qxJsonArrayAppend(NULL, value) != 0);
	QX_ASSERT(qxJsonArrayAppend(array, NULL) != 0);
	QX_ASSERT(qxJsonArraySize(array) == 0);

	QX_ASSERT(qxJsonArrayAppendNew(array, qxJsonNullNew()) == 0);
	QX_ASSERT(qxJsonArraySize(array) == 1);
	QX_ASSERT(QX_JSON_IS_NULL(qxJsonArrayGet(array, 0)));
	QX_ASSERT(qxJsonArrayGet(array, 1) == NULL);

	QX_ASSERT(qxJsonArrayPrependNew(array, qxJsonTrueNew()) == 0);
	QX_ASSERT(qxJsonArraySize(array) == 2);
	QX_ASSERT(QX_JSON_IS_TRUE(qxJsonArrayGet(array, 0)));
	QX_ASSERT(QX_JSON_IS_NULL(qxJsonArrayGet(array, 1)));
	QX_ASSERT(qxJsonArrayGet(array, 2) == NULL);

	QX_ASSERT(qxJsonArrayInsertNew(array, 1, qxJsonFalseNew()) == 0);
	QX_ASSERT(qxJsonArraySize(array) == 3);
	QX_ASSERT(QX_JSON_IS_FALSE(qxJsonArrayGet(array, 1)));

	qxJsonValueUnref(value);
	return EXIT_SUCCESS;
}

