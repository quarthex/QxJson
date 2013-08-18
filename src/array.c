/**
 * @file array.c
 * @brief Source file of the QxJsonArray class.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>

#include "../include/qx.json.array.h"
#include "value.private.h"

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
	Node head;
};

static void finalize(QxJsonValue *value)
{
	QxJsonArray *array = (QxJsonArray *)value;
	Node *node;

	assert(value != NULL);
	assert(QX_JSON_IS_ARRAY(value));

	for (node = array->head.next; node != &array->head;)
	{
		QxJsonValue_decref(node->value);
		node = node->next;
		free(node->previous);
	}

	return;
}

static QxJsonValueClass const klass =
{
	finalize,
	QxJsonValueTypeArray
};

QxJsonValue *QxJsonArray_new(void)
{
	QxJsonArray *const instance = ALLOC(QxJsonArray);

	if (instance)
	{
		instance->parent.klass = &klass;
		instance->parent.ref = 0;
		instance->head.next = &instance->head;
		instance->head.previous = &instance->head;
		return &instance->parent;
	}

	return NULL;
}

size_t QxJsonArray_size(QxJsonArray const *array)
{
	size_t size = 0;
	Node *node = array->head.next;

	while (node != &array->head)
	{
		node = node->next;
		++size;
	}

	return size;
}

int QxJsonArray_append(QxJsonArray *array, QxJsonValue *value)
{
	if (QxJsonArray_appendNew(array, value))
	{
		return -1;
	}

	++value->ref;
	return 0;
}

int QxJsonArray_appendNew(QxJsonArray *array, QxJsonValue *value)
{
	Node *node;

	if (array && value && (&array->parent != value))
	{
		node = ALLOC(Node);

		if (!node)
			return -1;

		node->value = value;
		node->next = &array->head;
		node->previous = array->head.previous;
		node->next->previous = node;
		node->previous->next = node;
		return 0;
	}

	return -1;
}

int QxJsonArray_prepend(QxJsonArray *array, QxJsonValue *value)
{
	return QxJsonArray_insert(array, 0, value);
}

int QxJsonArray_prependNew(QxJsonArray *array, QxJsonValue *value)
{
	Node *node;

	if (array && value && (&array->parent != value))
	{
		node = ALLOC(Node);

		if (!node)
			return -1;

		node->value = value;
		node->next = array->head.next;
		node->previous = &array->head;
		node->next->previous = node;
		node->previous->next = node;
		return 0;
	}

	return -1;
}

int QxJsonArray_insert(QxJsonArray *array, size_t index,  QxJsonValue *value)
{
	if (QxJsonArray_insertNew(array, index, value))
	{
		return -1;
	}

	++value->ref;
	return 0;
}

int QxJsonArray_insertNew(QxJsonArray *array, size_t index, QxJsonValue *value)
{
	Node *next, *node;

	if (!index)
		return QxJsonArray_prependNew(array, value);

	if (array && value && (&array->parent != value))
	{
		next = array->head.next;

		while (index && next != &array->head)
		{
			--index;
			next = next->next;
		}

		if (!index)
		{
			node = ALLOC(Node);

			if (node)
			{
				node->value = value;
				node->next = next;
				node->previous = next->previous;
				node->next->previous = node;
				node->previous->next = node;
				return 0;
			}
		}
	}

	return -1;
}

QxJsonValue const *QxJsonArray_get(QxJsonArray *array, size_t index)
{
	Node *node;

	if (array && array->head.next != &array->head)
	{
		node = array->head.next;

		while (index && node != &array->head)
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
