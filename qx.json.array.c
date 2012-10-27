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

typedef struct Node Node;

struct Node
{
	Node *next;
	Node *previous;
	QxJsonValue *value;
};

struct QxJsonArray
{
	QxJsonValue parent;
	Node *head;
	Node *tail;
};

static void finalize(QxJsonValue *value)
{
	QxJsonArray *array = (QxJsonArray *)value;
	Node *node;
	assert(value != NULL);
	assert(QX_JSON_IS_ARRAY(value));

	while (array->head)
	{
		qxJsonValueDecRef(array->head->value);
		node = array->head;
		array->head = node->next;
		free(node);
	}

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
		instance->head = NULL;
		instance->tail = NULL;
		return &instance->parent;
	}

	return NULL;
}

size_t qxJsonArraySize(QxJsonArray const *array)
{
	size_t size = 0;
	Node *node;

	if (array)
	{
		node = array->head;

		for (; node; node = node->next)
		{
			++size;
		}
	}

	return size;
}

int qxJsonArrayAppend(QxJsonArray *array, QxJsonValue *value)
{
	if (qxJsonArrayAppendNew(array, value))
	{
		return -1;
	}

	++value->ref;
	return 0;
}

int qxJsonArrayAppendNew(QxJsonArray *array, QxJsonValue *value)
{
	if (array && value)
	{
		if (array->tail)
		{
			assert(array->head != NULL);
			array->tail->next = ALLOC(Node);

			if (array->tail->next != NULL)
			{
				array->tail->next->previous = array->tail;
				array->tail = array->tail->next;
			}
		}
		else
		{
			assert(array->head == NULL);
			array->tail = ALLOC(Node);

			if (array->tail)
			{
				array->head = array->tail;
				array->head->previous = NULL;
			}
		}

		if (array->tail)
		{
			array->tail->next = NULL;
			array->tail->value = value;
			return 0;
		}
	}

	return -1;
}

