/**
 * @file qx.js.value.c
 * @brief Source file of the QxJsValue class
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>

#include "qx.js.value.h"
#include "qx.js.value.private.h"

void qxJsValueIncRef(QxJsValue *value)
{
	assert(value != NULL);
	++value->ref;
	return;
}

void qxJsValueDecRef(QxJsValue *value)
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

QxJsValueType qxJsValueType(QxJsValue const *value)
{
	assert(value != NULL);
	assert(value->klass != NULL);
	return value->klass->type;
}

