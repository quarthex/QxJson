/**
 * @file qx.js.null.c
 * @brief Source file of the QxJsNull class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "qx.js.value.h"
#include "qx.js.value.private.h"
#include "qx.js.null.h"

static void finalize(QxJsValue *value)
{
	QX_UNUSED(value);
	return;
}

static QxJsValueClass const klass =
{
	finalize,
	QxJsValueTypeNull
};

QxJsValue *qxJsNullNew(void)
{
	QxJsValue *instance = (QxJsValue *)malloc(sizeof(QxJsValue));

	if (instance)
	{
		instance->klass = &klass;
		instance->ref = 0;
	}

	return instance;
}

