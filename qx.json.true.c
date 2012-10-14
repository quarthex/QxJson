/**
 * @file qx.json.true.c
 * @brief Source file of the QxJsonTrue class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "qx.json.value.h"
#include "qx.json.value.private.h"
#include "qx.json.true.h"

#define ALLOC(type) ((type *)malloc(sizeof(type)))

static void finalize(QxJsonValue *value)
{
	QX_UNUSED(value);
	return;
}

static QxJsonValueClass const klass =
{
	finalize,
	QxJsonValueTypeTrue
};

QxJsonValue *qxJsonTrueNew(void)
{
	QxJsonValue *const instance = ALLOC(QxJsonValue);

	if (instance)
	{
		instance->klass = &klass;
		instance->ref = 0;
	}

	return instance;
}

