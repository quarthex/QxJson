/**
 * @file false.c
 * @brief Source file of the QxJsonFalse class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "../include/qx.json.false.h"
#include "value.private.h"

#define ALLOC(type) ((type *)malloc(sizeof(type)))

static void finalize(QxJsonValue *value)
{
	(void)value;
	return;
}

static QxJsonValueClass const klass =
{
	finalize,
	QxJsonValueTypeFalse
};

QxJsonValue *QxJsonFalse_new(void)
{
	QxJsonValue *const instance = ALLOC(QxJsonValue);

	if (instance)
	{
		instance->klass = &klass;
		instance->ref = 0;
	}

	return instance;
}
