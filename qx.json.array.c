/**
 * @file qx.json.array.c
 * @brief Source file of the QxJsonArray class.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>

#include "qx.macro.h"
#include "qx.json.value.h"
#include "qx.json.value.private.h"
#include "qx.json.array.h"

#define ALLOC(type) ((type *)malloc(sizeof(type)))

struct QxJsonArray
{
	QxJsonValue parent;
};

static void finalize(QxJsonValue *value)
{
	assert(value != NULL);
	assert(QX_JSON_IS_ARRAY(value));
	return;
}

static QxJsonValueClass const klass =
{
	finalize,
	QxJsonValueTypeArray
};

QxJsonValue *qxJsonArrayNew(void)
{
	QxJsonArray *const instance = ALLOC(QxJsonArray);

	if (instance)
	{
		instance->parent.klass = &klass;
		instance->parent.ref = 0;
		return &instance->parent;
	}

	return NULL;
}

