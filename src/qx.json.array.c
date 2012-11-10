/**
 * @file qx.json.array.c
 * @brief Source file of the QxJsonArray class.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>

#include "../include/qx.json.array.h"
#include "qx.json.value.private.h"

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
	if (array && value && (&array->parent != value))
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

int qxJsonArrayPrepend(QxJsonArray *array, QxJsonValue *value)
{
	if (qxJsonArrayPrependNew(array, value))
	{
		return -1;
	}

	++value->ref;
	return 0;
}

int qxJsonArrayPrependNew(QxJsonArray *array, QxJsonValue *value)
{
	if (array && value && (&array->parent != value))
	{
		if (array->head)
		{
			assert(array->tail != NULL);
			array->head->previous = ALLOC(Node);

			if (array->head->previous != NULL)
			{
				array->head->previous->next = array->head;
				array->head = array->head->previous;
			}
		}
		else
		{
			assert(array->tail == NULL);
			array->head = ALLOC(Node);

			if (array->head)
			{
				array->tail = array->head;
				array->tail->next = NULL;
			}
		}

		if (array->head)
		{
			array->head->previous = NULL;
			array->head->value = value;
			return 0;
		}
	}

	return -1;
}

QxJsonValue const *qxJsonArrayGet(QxJsonArray const *array, size_t index)
{
	if (array && array->head)
	{
		Node const *node = array->head;

		while (index > 0 && node->next)
		{
			--index;
			node = node->next;
		}

		if (!index)
		{
			return node->value;
		}
	}

	return NULL;
}

