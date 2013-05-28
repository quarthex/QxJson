/**
 * @file null.c
 * @brief Testing source file of the QxJsonNull class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include <qx.json.null.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *const value = qxJsonNullNew();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_NULL(value));
	qxJsonValueUnref(value);
	return EXIT_SUCCESS;
}
