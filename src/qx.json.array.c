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
	Node *node;
};

static void finalize(QxJsonValue *value)
{
	QxJsonArray *array = (QxJsonArray *)value;
	Node *node;

	assert(value != NULL);
	assert(QX_JSON_IS_ARRAY(value));

	if (array->node)
	{
		while (array->node->previous)
		{
			node = array->node->previous;
			array->node->previous = node->previous;
			qxJsonValueUnref(node->value);
			free(node);
		}

		while (array->node->next)
		{
			node = array->node->next;
			array->node->next = node->next;
			qxJsonValueUnref(node->value);
			free(node);
		}

		qxJsonValueUnref(array->node->value);
		free(array->node);
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
		instance->node = NULL;
		return &instance->parent;
	}

	return NULL;
}

size_t qxJsonArraySize(QxJsonArray const *array)
{
	size_t size = 0;
	Node *node;

	if (array && array->node)
	{
		for (node = array->node; node->previous; node = node->previous)
		{
			++size;
		}

		for (node = array->node; node->next; node = node->next)
		{
			++size;
		}

		++size;
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
		if (array->node)
		{
			while (array->node->next)
			{
				array->node = array->node->next;
			}

			array->node->next = ALLOC(Node);

			if (array->node->next)
			{
				array->node->next->previous = array->node;
				array->node = array->node->next;
			}
		}
		else
		{
			array->node = ALLOC(Node);

			if (array->node)
			{
				array->node->previous = NULL;
			}
		}

		if (array->node)
		{
			array->node->next = NULL;
			array->node->value = value;
			return 0;
		}
	}

	return -1;
}

int qxJsonArrayPrepend(QxJsonArray *array, QxJsonValue *value)
{
	return qxJsonArrayInsert(array, 0, value);
}

int qxJsonArrayPrependNew(QxJsonArray *array, QxJsonValue *value)
{
	return qxJsonArrayInsertNew(array, 0, value);
}

int qxJsonArrayInsert(QxJsonArray *array, size_t index,  QxJsonValue *value)
{
	if (qxJsonArrayInsertNew(array, index, value))
	{
		return -1;
	}

	++value->ref;
	return 0;
}

int qxJsonArrayInsertNew(QxJsonArray *array, size_t index, QxJsonValue *value)
{
	Node *node;

	if (array && value && (&array->parent != value))
	{
		if (array->node)
		{
			while (array->node->previous)
			{
				array->node = array->node->previous;
			}

			for (; index > 0 && array->node->next; --index)
			{
				array->node = array->node->next;
			}

			node = ALLOC(Node);

			if (node)
			{
				if (index)
				{
					node->previous = array->node;
					node->next = NULL;
				}
				else
				{
					node->previous = array->node->previous;
					node->next = array->node;
				}

				if (node->previous)
				{
					node->previous->next = node;
				}

				if (node->next)
				{
					node->next->previous = node;
				}

				array->node = node;
			}
		}
		else
		{
			array->node = ALLOC(Node);

			if (array->node)
			{
				array->node->previous = NULL;
				array->node->next = NULL;
			}
		}

		if (array->node)
		{
			array->node->value = value;
			return 0;
		}
	}

	return -1;
}

QxJsonValue const *qxJsonArrayGet(QxJsonArray *array, size_t index)
{
	if (array && array->node)
	{
		while (array->node->previous)
		{
			array->node = array->node->previous;
		}

		while (index > 0 && array->node->next)
		{
			--index;
			array->node = array->node->next;
		}

		if (!index)
		{
			return array->node->value;
		}
	}

	return NULL;
}

