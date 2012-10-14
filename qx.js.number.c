/**
 * @file qx.js.number.c
 * @brief Source file of the QxJsNumber class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "qx.js.value.h" 
#include "qx.js.value.private.h" 
#include "qx.js.number.h"

static void finalize(QxJsValue *value)
{
	QX_UNUSED(value);
	return;
}

static QxJsValueClass const klass =
{
	finalize,
	QxJsValueTypeNumber
};

struct QxJsNumber
{
	QxJsValue parent;

	/**
	 * @brief Native value of the JavaScript number.
	 */
	qx_js_number_t number;
};

QxJsValue *qxJsNumberNew(qx_js_number_t number)
{
	QxJsNumber *const instance = (QxJsNumber *)malloc(sizeof(QxJsNumber));

	if (instance)
	{
		instance->parent.klass = &klass;
		instance->parent.ref = 0;
		instance->number = number;
	}

	return &instance->parent;
}

qx_js_number_t qxJsNumberValue(QxJsNumber const *number)
{
	if (number)
	{
		return number->number;
	}

	return 0.;
}

