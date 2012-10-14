/**
 * @file qx.js.true.c
 * @brief Source file of the QxJsTrue class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "qx.js.value.h"
#include "qx.js.value.private.h"
#include "qx.js.true.h"

#define unused(value) ((void)value)

static void finalize(QxJsValue *value)
{
	unused(value);
	return;
}

static QxJsValueClass const klass =
{
	finalize,
	QxJsValueTypeTrue
};

QxJsValue *qxJsTrueNew(void)
{
	QxJsValue *instance = (QxJsValue *)malloc(sizeof(QxJsValue));

	if (instance)
	{
		instance->klass = &klass;
		instance->ref = 0;
	}

	return instance;
}

