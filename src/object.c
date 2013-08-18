/**
 * @file object.c
 * @brief Source file of the QxJsonObject class.
 * @author Romain DEOUX
 */

#include <stdlib.h>
#include <string.h>

#include "../include/qx.json.object.h"
#include "value.private.h"

#define ALLOC(type) ((type *)malloc(sizeof(type)))

typedef struct Node Node;

struct Node
{
	QxJsonString *key;
	QxJsonValue *value;
	Node *previous;
	Node *next;
};

struct QxJsonObject
{
	QxJsonValue parent;
	Node *node;
};

static void freeNode(Node *node)
{
	QxJsonValue_decref((QxJsonValue *)(node->key));
	QxJsonValue_decref(node->value);
	free(node);
	return;
}

static void finalize(QxJsonValue *value)
{
	QxJsonObject *const object = (QxJsonObject *)value;

	if (object->node)
	{
		while (object->node->next)
		{
			object->node->next = object->node->next->next;
			freeNode(object->node->next->previous);
		}

		while (object->node->previous)
		{
			object->node->previous = object->node->previous->previous;
			freeNode(object->node->previous->next);
		}

		freeNode(object->node);
	}

	return;
}

static QxJsonValueClass const klass =
{
	finalize,
	QxJsonValueTypeObject
};

QxJsonValue *QxJsonObject_new(void)
{
	QxJsonObject *const instance = ALLOC(QxJsonObject);

	if (instance)
	{
		instance->parent.klass = &klass;
		instance->parent.ref = 0;
		instance->node = NULL;
	}

	return &instance->parent;
}

static int compareKey(QxJsonString *first, QxJsonString *last)
{
	size_t firstSize, lastSize;

	if (first == last)
	{
		return 0;
	}

	firstSize = QxJsonString_size(first);
	lastSize = QxJsonString_size(last);

	if (firstSize != lastSize)
	{
		return (int)firstSize - (int)lastSize;
	}

	return memcmp(QxJsonString_data(first), QxJsonString_data(last), firstSize);
}

int QxJsonObject_set(QxJsonObject *object, QxJsonString *key,
					QxJsonValue *value)
{
	int cmp;

	if (object && key && value)
	{
		if (object->node)
		{
			cmp = compareKey(object->node->key, key);

			while ((cmp < 0) && (object->node->next))
			{
				object->node = object->node->next;
				cmp = compareKey(object->node->key, key);
			}

			while ((cmp > 0) && (object->node->previous))
			{
				object->node = object->node->previous;
				cmp = compareKey(object->node->key, key);
			}

			/* Key not found */
			if (cmp)
			{
				if (cmp < 0)
				{
					object->node->next = ALLOC(Node);

					if (!object->node->next)
					{
						return -1;
					}

					object->node->next->previous = object->node;
					object->node = object->node->next;
				}
				else
				{
					object->node->previous = ALLOC(Node);

					if (!object->node->previous)
					{
						return -1;
					}

					object->node->previous->next = object->node;
					object->node = object->node->previous;
				}

				object->node->key = NULL;
			}
		}
		else
		{
			object->node = ALLOC(Node);

			if (!object->node)
			{
				return -1;
			}

			object->node->key = NULL;
			object->node->next = NULL;
			object->node->previous = NULL;
		}

		if (object->node->key)
		{
			/* Existing key */
			QxJsonValue_decref(object->node->value);
			object->node->value = value;
			QxJsonValue_incref(value);
		}
		else
		{
			/* New key */
			object->node->key = key;
			QxJsonValue_incref((QxJsonValue *)key);
			object->node->value = value;
			QxJsonValue_incref(value);
		}

		return 0;
	}

	return -1;
}

int QxJsonObject_unset(QxJsonObject *object, QxJsonString *key)
{
	int cmp;
	Node *found;

	if (object && key)
	{
		if (object->node)
		{
			cmp = compareKey(object->node->key, key);

			while ((cmp < 0) && object->node->next)
			{
				object->node = object->node->next;
				cmp = compareKey(object->node->key, key);
			}

			while ((cmp > 0) && object->node->previous)
			{
				object->node = object->node->previous;
				cmp = compareKey(object->node->key, key);
			}

			if (!cmp)
			{
				found = object->node;

				if (object->node->next)
				{
					object->node = object->node->next;
				}
				else
				{
					object->node = object->node->previous;
				}

				if (found->next)
				{
					found->next->previous = found->previous;
				}

				if (found->previous)
				{
					found->previous->next = found->next;
				}

				freeNode(found);
			}
		}

		return 0;
	}

	return -1;
}

size_t QxJsonObject_size(QxJsonObject *object)
{
	size_t size;
	Node *node;

	if (object && object->node)
	{
		size = 1;

		for (node = object->node->next; node; node = node->next)
		{
			++size;
		}

		for (node = object->node->previous; node; node = node->previous)
		{
			++size;
		}

		return size;
	}

	return 0;
}
