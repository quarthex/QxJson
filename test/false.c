/**
 * @file false.c
 * @brief Testing source file of false value handling in the QxJsonValue class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include <qx.json.value.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *const value = QxJsonValue_falseNew();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_FALSE(value));
	QxJsonValue_decref(value);
	return EXIT_SUCCESS;
}
