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
	QxJsonValue *object, *value;
	QxJsonValue *key0, *value0;
	QxJsonValue *key1, *value1;

	object = QxJsonValue_objectNew();
	expect_not_null(object);
	expect_ok(QX_JSON_IS_OBJECT(object));

	/* set key/value 0 */
	key0 = QxJsonValue_stringNew(L"key0", 4);
	value0 = QxJsonValue_stringNew(L"value0", 6);
	expect_zero(QxJsonValue_objectSet(object, key0, value0));
	expect_int_equal(QxJsonValue_size(object), 1);

	/* set key/value 1 */
	key1 = QxJsonValue_stringNew(L"key1", 4);
	value1 = QxJsonValue_stringNew(L"value1", 6);
	expect_zero(QxJsonValue_objectSet(object, key1, value1));
	expect_int_equal(QxJsonValue_size(object), 2);

	/* get value 0 */
	value = NULL;
	expect_zero(QxJsonValue_objectGet(object, key0, &value));
	expect_not_null(value);
	expect_ok(QX_JSON_IS_STRING(value));
	expect_wstr_equal(QxJsonValue_stringValue(value), L"value0");

	/* get value 1 */
	value = NULL;
	expect_zero(QxJsonValue_objectGet(object, key1, &value));
	expect_not_null(value);
	expect_ok(QX_JSON_IS_STRING(value));
	expect_wstr_equal(QxJsonValue_stringValue(value), L"value1");

	/* unset key 0 */
	expect_zero(QxJsonValue_objectUnset(object, key0));
	expect_int_equal(QxJsonValue_size(object), 1);
	expect_not_zero(QxJsonValue_objectGet(object, key0, &value));
	expect_null(value);

	/* unset key 1 */
	expect_zero(QxJsonValue_objectUnset(object, key1));
	expect_int_equal(QxJsonValue_size(object), 0);
	expect_not_zero(QxJsonValue_objectGet(object, key1, &value));
	expect_null(value);

	QxJsonValue_release(key0);
	QxJsonValue_release(value0);
	QxJsonValue_release(key1);
	QxJsonValue_release(value1);
	QxJsonValue_release(object);
	return EXIT_SUCCESS;
}
