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
	QxJsonValue *const value = QxJsonNull_new();
	expect_not_null(value);
	expect_ok(QX_JSON_IS_NULL(value));
	QxJsonValue_decref(value);
	return EXIT_SUCCESS;
}
