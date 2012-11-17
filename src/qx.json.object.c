/**
 * @file qx.json.object.c
 * @brief Source file of the QxJsonObject class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "../include/qx.json.object.h"
#include "qx.json.value.private.h"

#define ALLOC(type) ((type *)malloc(sizeof(type)))

static void finalize(QxJsonValue *value)
{
	QX_UNUSED(value);
	return;
}

static QxJsonValueClass const klass =
{
	finalize,
	QxJsonValueTypeObject
};

QxJsonValue *qxJsonObjectNew(void)
{
	QxJsonValue *const instance = ALLOC(QxJsonValue);

	if (instance)
	{
		instance->klass = &klass;
		instance->ref = 0;
	}

	return instance;
}

