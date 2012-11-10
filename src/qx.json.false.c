/**
 * @file qx.json.false.c
 * @brief Source file of the QxJsonFalse class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "../include/qx.json.false.h"
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
	QxJsonValueTypeFalse
};

QxJsonValue *qxJsonFalseNew(void)
{
	QxJsonValue *const instance = ALLOC(QxJsonValue);

	if (instance)
	{
		instance->klass = &klass;
		instance->ref = 0;
	}

	return instance;
}

