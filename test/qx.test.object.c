/**
 * @file qx.test.object.c
 * @brief Testing source file of the QxJsonObject class.
 * @author Romain DEOUX
 */

#include <qx.json.object.h>

#include "qx.assert.h"

int main(void)
{
	QxJsonValue *const value = qxJsonObjectNew();
	QX_ASSERT(value != NULL);
	QX_ASSERT(QX_JSON_IS_OBJECT(value));
	qxJsonValueUnref(value);
	return EXIT_SUCCESS;
}

