/**
 * @file true.c
 * @brief Testing source file of true value handling in the QxJsonValue class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include <qx.json.value.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *const value = QxJsonValue_trueNew();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_TRUE(value));
	QxJsonValue_release(value);
	return EXIT_SUCCESS;
}
