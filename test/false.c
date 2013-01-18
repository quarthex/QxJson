/**
 * @file qx.test.false.c
 * @brief Testing source file of the QxJsonFalse class.
 * @author Romain DEOUX
 */

#include <qx.json.false.h>

#include "assert.h"

int main(void)
{
	QxJsonValue *const value = qxJsonFalseNew();
	QX_ASSERT(value != NULL);
	QX_ASSERT(QX_JSON_IS_FALSE(value));
	qxJsonValueUnref(value);
	return EXIT_SUCCESS;
}

