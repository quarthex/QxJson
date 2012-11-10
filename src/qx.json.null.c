/**
 * @file qx.json.null.c
 * @brief Source file of the QxJsonNull class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "../include/qx.json.null.h"
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
	QxJsonValueTypeNull
};

QxJsonValue *qxJsonNullNew(void)
{
	QxJsonValue *const instance = ALLOC(QxJsonValue);

	if (instance)
	{
		instance->klass = &klass;
		instance->ref = 0;
	}

	return instance;
}

