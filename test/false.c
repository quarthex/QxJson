/**
 * @file false.c
 * @brief Testing source file of the QxJsonFalse class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include <qx.json.false.h>

#include "expect.h"

int main(void)
{
	QxJsonValue *const value = QxJsonFalse_new();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_FALSE(value));
	QxJsonValue_decref(value);
	return EXIT_SUCCESS;
}
