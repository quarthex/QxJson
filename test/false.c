/**
 * @file qx.test.false.c
 * @brief Testing source file of the QxJsonFalse class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include <qx.json.false.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *const value = qxJsonFalseNew();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_FALSE(value));
	qxJsonValueUnref(value);
	return EXIT_SUCCESS;
}
