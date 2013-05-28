/**
 * @file true.c
 * @brief Testing source file of the QxJsonTrue class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include <qx.json.true.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *const value = qxJsonTrueNew();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_TRUE(value));
	qxJsonValueUnref(value);
	return EXIT_SUCCESS;
}
