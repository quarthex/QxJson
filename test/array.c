/**
 * @file array.c
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

	value = QxJsonArray_new();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_ARRAY(value));
	array = QX_JSON_ARRAY(value);
	expect_not_null(array);
	expect_int_equal(QxJsonArray_size(array), 0);

	expect_int_not_equal(QxJsonArray_append(NULL, value), 0);
	expect_int_not_equal(QxJsonArray_append(array, NULL), 0);
	expect_int_equal(QxJsonArray_size(array), 0);

	expect_zero(QxJsonArray_appendNew(array, QxJsonNull_new()));
	expect_int_equal(QxJsonArray_size(array), 1);
	expect_ok(QX_JSON_IS_NULL(QxJsonArray_get(array, 0)));
	expect_null(QxJsonArray_get(array, 1));
	/* N */

	expect_zero(QxJsonArray_prependNew(array, QxJsonTrue_new()));
	expect_int_equal(QxJsonArray_size(array), 2);
	expect_ok(QX_JSON_IS_TRUE(QxJsonArray_get(array, 0)));
	expect_ok(QX_JSON_IS_NULL(QxJsonArray_get(array, 1)));
	expect_null(QxJsonArray_get(array, 2));
	/* T N */

	expect_zero(QxJsonArray_insertNew(array, 1, QxJsonFalse_new()));
	expect_int_equal(QxJsonArray_size(array), 3);
	expect_ok(QX_JSON_IS_FALSE(QxJsonArray_get(array, 1)));
	/* T F N */

	expect_zero(QxJsonArray_insertNew(array, 3, QxJsonTrue_new()));
	expect_int_equal(QxJsonArray_size(array), 4);
	expect_ok(QX_JSON_IS_TRUE(QxJsonArray_get(array, 3)));
	/* T F N T */

	QxJsonValue_decref(value);
	return EXIT_SUCCESS;
}
