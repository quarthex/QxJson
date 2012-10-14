/**
 * @file qx.js.false.c
 * @brief Source file of the QxJsNull class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "qx.js.value.h"
#include "qx.js.value.private.h"
#include "qx.js.false.h"

static void finalize(QxJsValue *value)
{
	QX_UNUSED(value);
	return;
}

static QxJsValueClass const klass =
{
	finalize,
	QxJsValueTypeFalse
};

QxJsValue *qxJsFalseNew(void)
{
	QxJsValue *instance = (QxJsValue *)malloc(sizeof(QxJsValue));

	if (instance)
	{
		instance->klass = &klass;
		instance->ref = 0;
	}

	return instance;
}

