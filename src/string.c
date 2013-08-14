/**
 * @file string.c
 * @brief Source file of the QxJsonString class.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../include/qx.json.string.h"
#include "value.private.h"

#define ALLOC(type) ((type *)malloc(sizeof(type)))

struct QxJsonString
{
	QxJsonValue parent;

	/**
	 * @brief Native data of the string.
	 */
	wchar_t *data;

	/**
	 * @brief Size of the string.
	 */
	size_t size;
};

static void finalize(QxJsonValue *value)
{
	assert(value != NULL);
	assert(QX_JSON_IS_STRING(value));
	free(((QxJsonString *)value)->data);
	return;
}

static QxJsonValueClass const klass =
{
	finalize,
	QxJsonValueTypeString
};

QxJsonValue *qxJsonStringNew(qx_json_string_t data, size_t size)
{
	QxJsonString *instance;

	if (data)
	{
		instance = ALLOC(QxJsonString);

		if (instance)
		{
			instance->parent.klass = &klass;
			instance->parent.ref = 0;
			instance->data = (wchar_t *)malloc((size + 1) * sizeof(wchar_t));
			memcpy(instance->data, data, size * sizeof(wchar_t));
			instance->data[size] = L'\0';
			instance->size = size;
			return &instance->parent;
		}
	}

	return NULL;
}

qx_json_string_t qxJsonStringData(QxJsonString const *string)
{
	return string ? string->data : NULL;
}

size_t qxJsonStringSize(QxJsonString const *string)
{
	return string ? string->size : 0;
}
