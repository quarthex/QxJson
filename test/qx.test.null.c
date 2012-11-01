/**
 * @file qx.test.null.c
 * @brief Testing source file of the QxJsonNull class.
 * @author Romain DEOUX
 */

#include <qx.json.null.h>

#include "qx.assert.h"

int main(void)
{
	QxJsonValue *const value = qxJsonNullNew();
	QX_ASSERT(value != NULL);
	QX_ASSERT(QX_JSON_IS_NULL(value));
	qxJsonValueDecRef(value);
	return EXIT_SUCCESS;
}
