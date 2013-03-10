/**
 * @file qx.test.array.c
 * @brief Testing source file of the QxJsonArray class.
 * @author Romain DEOUX
 */

#include <stddef.h>
#include <stdlib.h>

#include <qx.json.array.h>
#include <qx.json.false.h>
#include <qx.json.null.h>
#include <qx.json.true.h>

#include "expect.h"

int main(void)
{
	QxJsonArray *array;
	QxJsonValue *value;

	value = qxJsonArrayNew();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_ARRAY(value));
	array = QX_JSON_ARRAY(value);
	expect_not_null(array);
	expect_int_equal(qxJsonArraySize(array), 0);

	expect_int_not_equal(qxJsonArrayAppend(NULL, value), 0);
	expect_int_not_equal(qxJsonArrayAppend(array, NULL), 0);
	expect_int_equal(qxJsonArraySize(array), 0);

	expect_zero(qxJsonArrayAppendNew(array, qxJsonNullNew()));
	expect_int_equal(qxJsonArraySize(array), 1);
	expect_ok(QX_JSON_IS_NULL(qxJsonArrayGet(array, 0)));
	expect_null(qxJsonArrayGet(array, 1));
	/* N */

	expect_zero(qxJsonArrayPrependNew(array, qxJsonTrueNew()));
	expect_int_equal(qxJsonArraySize(array), 2);
	expect_ok(QX_JSON_IS_TRUE(qxJsonArrayGet(array, 0)));
	expect_ok(QX_JSON_IS_NULL(qxJsonArrayGet(array, 1)));
	expect_null(qxJsonArrayGet(array, 2));
	/* T N */

	expect_zero(qxJsonArrayInsertNew(array, 1, qxJsonFalseNew()));
	expect_int_equal(qxJsonArraySize(array), 3);
	expect_ok(QX_JSON_IS_FALSE(qxJsonArrayGet(array, 1)));
	/* T F N */

	expect_zero(qxJsonArrayInsertNew(array, 3, qxJsonTrueNew()));
	expect_int_equal(qxJsonArraySize(array), 4);
	expect_ok(QX_JSON_IS_TRUE(qxJsonArrayGet(array, 3)));
	/* T F N T */

	qxJsonValueUnref(value);
	return EXIT_SUCCESS;
}
