/**
 * @file number.c
 * @brief Source file of the QxJsonNumber class.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "../include/qx.json.number.h"
#include "value.private.h"

#define ALLOC(type) ((type *)malloc(sizeof(type)))

static void finalize(QxJsonValue *value)
{
	QX_UNUSED(value);
	return;
}

static QxJsonValueClass const klass =
{
	finalize,
	QxJsonValueTypeNumber
};

struct QxJsonNumber
{
	QxJsonValue parent;

	/**
	 * @brief Native value of the JavaScript number.
	 */
	qx_json_number_t number;
};

QxJsonValue *qxJsonNumberNew(qx_json_number_t number)
{
	QxJsonNumber *const instance = ALLOC(QxJsonNumber);

	if (instance)
	{
		instance->parent.klass = &klass;
		instance->parent.ref = 0;
		instance->number = number;
	}

	return &instance->parent;
}

qx_json_number_t qxJsonNumberValue(QxJsonNumber const *number)
{
	if (number)
	{
		return number->number;
	}

	return 0.;
}

