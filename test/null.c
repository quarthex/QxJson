/**
 * @file null.c
 * @brief Testing source file of null value handling in the QxJsonValue class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include <qx.json.value.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *const value = QxJsonValue_nullNew();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_NULL(value));
	QxJsonValue_decref(value);
	return EXIT_SUCCESS;
}
