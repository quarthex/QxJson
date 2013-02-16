/**
 * @file qx.test.true.c
 * @brief Testing source file of the QxJsonTrue class.
 * @author Romain DEOUX
 */

#include <qx.json.true.h>

#include "assert.h"

int main(void)
{
	QxJsonValue *const value = qxJsonTrueNew();
	QX_ASSERT(value != NULL);
	QX_ASSERT(QX_JSON_IS_TRUE(value));
	qxJsonValueUnref(value);
	return EXIT_SUCCESS;
}
