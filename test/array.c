/**
 * @file array.c
 * @brief Testing source file of arrays handling in the QxJsonValue class.
 * @author Romain DEOUX
 */

#include <stddef.h>
#include <stdlib.h>

#include <qx.json.value.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *array;

	array = QxJsonValue_arrayNew();
	expect_not_null(array);
	expect_ok(QX_JSON_IS_ARRAY(array));
	expect_int_equal(QxJsonValue_size(array), 0);

	expect_int_not_equal(QxJsonValue_arrayAppend(NULL, array), 0);
	expect_int_not_equal(QxJsonValue_arrayAppend(array, NULL), 0);
	expect_int_equal(QxJsonValue_size(array), 0);

	expect_zero(QxJsonValue_arrayAppendNew(array, QxJsonValue_nullNew()));
	expect_int_equal(QxJsonValue_size(array), 1);
	expect_ok(QX_JSON_IS_NULL(QxJsonValue_arrayGet(array, 0)));
	expect_null(QxJsonValue_arrayGet(array, 1));
	/* N */

	expect_zero(QxJsonValue_arrayPrependNew(array, QxJsonValue_trueNew()));
	expect_int_equal(QxJsonValue_size(array), 2);
	expect_ok(QX_JSON_IS_TRUE(QxJsonValue_arrayGet(array, 0)));
	expect_ok(QX_JSON_IS_NULL(QxJsonValue_arrayGet(array, 1)));
	expect_null(QxJsonValue_arrayGet(array, 2));
	/* T N */

	expect_zero(QxJsonValue_arrayInsertNew(array, 1, QxJsonValue_falseNew()));
	expect_int_equal(QxJsonValue_size(array), 3);
	expect_ok(QX_JSON_IS_FALSE(QxJsonValue_arrayGet(array, 1)));
	/* T F N */

	expect_zero(QxJsonValue_arrayInsertNew(array, 3, QxJsonValue_trueNew()));
	expect_int_equal(QxJsonValue_size(array), 4);
	expect_ok(QX_JSON_IS_TRUE(QxJsonValue_arrayGet(array, 3)));
	/* T F N T */

	QxJsonValue_decref(array);
	return EXIT_SUCCESS;
}
