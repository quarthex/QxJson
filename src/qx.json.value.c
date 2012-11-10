/**
 * @file qx.json.value.c
 * @brief Source file of the QxJsonValue class
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>

#include "../include/qx.json.value.h"
#include "qx.json.value.private.h"

void qxJsonValueRef(QxJsonValue *value)
{
	assert(value != NULL);
	++value->ref;
	return;
}

void qxJsonValueUnref(QxJsonValue *value)
{
	assert(value != NULL);

	if (value->ref)
	{
		--value->ref;
	}
	else
	{
		assert(value->klass != NULL);
		value->klass->finalize(value);
		free(value);
	}

	return;
}

QxJsonValueType qxJsonValueType(QxJsonValue const *value)
{
	assert(value != NULL);
	assert(value->klass != NULL);
	return value->klass->type;
}

