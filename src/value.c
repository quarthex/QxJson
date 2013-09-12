/**
 * @file value.c
 * @brief Source file of the QxJsonValue class
 * @author Romain DEOUX
 */

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "../include/qx.json.value.h"

typedef struct ArrayNode ArrayNode;
struct ArrayNode
{
	ArrayNode *next;
	ArrayNode *previous;
	QxJsonValue *value;
};

#define ArrayNode_alloc() ((ArrayNode *)malloc(sizeof(ArrayNode)));
#define ArrayNode_delete(node) do {   \
	QxJsonValue_decref((node)->value); \
	free((node));                      \
} while (0)

typedef struct ObjectNode ObjectNode;
struct ObjectNode
{
	ObjectNode *next;
	ObjectNode *previous;
	QxJsonValue *key;
	QxJsonValue *value;
};

#define ObjectNode_alloc() ((ObjectNode *)malloc(sizeof(ObjectNode)));
#define ObjectNode_delete(node) do {  \
	QxJsonValue_release((node)->key);   \
	QxJsonValue_release((node)->value); \
	free((node));                      \
} while (0)

struct QxJsonValue
{
	QxJsonValueType type;
	unsigned long int ref;
	size_t size;
	union
	{
		ArrayNode array;
		double number;
		ObjectNode object;
		wchar_t *string;
	} data;
};

#define QxJsonValue_alloc() ((QxJsonValue *)malloc(sizeof(QxJsonValue)))
#define QxJsonValue_init(self, t) do { \
	(self)->type = (t); (self)->ref = 0; (self)->size = 0; \
} while (0)

void QxJsonValue_retains(QxJsonValue *self)
{
	assert(self != NULL);
	++self->ref;
	return;
}

void QxJsonValue_release(QxJsonValue *self)
{
	void *node, *end;
	assert(self != NULL);

	if (self->ref)
	{
		--self->ref;
	}
	else
	{
		switch (self->type)
		{
		case QxJsonValueTypeString:
			assert(self->data.string);
			free(self->data.string);
			break;

		case QxJsonValueTypeArray:
			node = self->data.array.next;
			end = &self->data.array;

			while (node != end)
			{
				assert(((ArrayNode *)node)->value != NULL);
				QxJsonValue_release(((ArrayNode *)node)->value);
				node = ((ArrayNode *)node)->next;
				free(((ArrayNode *)node)->previous);
			}

			break;

		case QxJsonValueTypeObject:
			node = self->data.object.next;
			end = &self->data.object;

			while (node != end)
			{
				assert(((ObjectNode *)node)->key != NULL);
				QxJsonValue_release(((ObjectNode *)node)->key);
				assert(((ObjectNode *)node)->value != NULL);
				QxJsonValue_release(((ObjectNode *)node)->value);
				node = ((ObjectNode *)node)->next;
				free(((ObjectNode *)node)->previous);
			}

			break;

		default:
			break;
		}

		free(self);
	}

	return;
}

QxJsonValueType QxJsonValue_type(QxJsonValue const *self)
{
	assert(self != NULL);
	return self->type;
}

size_t QxJsonValue_size(QxJsonValue const *self)
{
	assert(self != NULL);
	return self->size;
}

/* Array */

QxJsonValue *QxJsonValue_arrayNew(void)
{
	ArrayNode *head;
	QxJsonValue *const instance = QxJsonValue_alloc();

	if (instance)
	{
		QxJsonValue_init(instance, QxJsonValueTypeArray);
		head = &instance->data.array;
		instance->data.array.next = head;
		instance->data.array.previous = head;
	}

	return instance;
}

int QxJsonValue_arrayAppend(QxJsonValue *self, QxJsonValue *value)
{
	int const r = QxJsonValue_arrayAppendNew(self, value);

	if (r == 0)
		++value->ref;

	return r;
}

int QxJsonValue_arrayAppendNew(QxJsonValue *self, QxJsonValue *value)
{
	ArrayNode *node;

	if (!self || self->type != QxJsonValueTypeArray || !value)
		/* Invalid argument */
		return -1;

	node = ArrayNode_alloc();

	if (!node)
		/* Out of memory */
		return -1;

	node->value = value;
	node->next = &self->data.array;
	node->previous = self->data.array.previous;
	node->next->previous = node;
	node->previous->next = node;
	++self->size;
	return 0;
}

int QxJsonValue_arrayPrepend(QxJsonValue *self, QxJsonValue *value)
{
	return QxJsonValue_arrayInsert(self, 0, value);
}

int QxJsonValue_arrayPrependNew(QxJsonValue *self, QxJsonValue *value)
{
	ArrayNode *node;

	if (!self || !value || self == value || self->type != QxJsonValueTypeArray)
		/* Invalid argument */
		return -1;

	node = ArrayNode_alloc();

	if (!node)
		/* Out of memory */
		return -1;

	node->value = value;
	node->next = self->data.array.next;
	node->previous = &self->data.array;
	node->next->previous = node;
	node->previous->next = node;
	++self->size;
	return 0;
}

int QxJsonValue_arrayInsert(QxJsonValue *self, size_t index,  QxJsonValue *value)
{
	int const r = QxJsonValue_arrayInsertNew(self, index, value);

	if (r == 0)
		++value->ref;

	return r;
}

int QxJsonValue_arrayInsertNew(QxJsonValue *self, size_t index, QxJsonValue *value)
{
	ArrayNode *node, *next;

	if (index == 0)
		return QxJsonValue_arrayPrependNew(self, value);

	if (!self || !value || self == value || self->type != QxJsonValueTypeArray || index > self->size)
		/* Invalid argument / out of bound */
		return -1;

	next = self->data.array.next;

	for (; index; --index)
	{
		assert(next != NULL);
		next = next->next;
	}

	node = ArrayNode_alloc();

	if (!node)
		/* Out of memory */
		return -1;

	node->value = value;
	node->next = next;
	node->previous = next->previous;
	node->next->previous = node;
	node->previous->next = node;
	++self->size;
	return 0;
}

QxJsonValue const *QxJsonValue_arrayGet(QxJsonValue *self, size_t index)
{
	ArrayNode *node;

	if (!self || self->type != QxJsonValueTypeArray || index >= self->size)
		/* Invalid argument / Index out of range */
		return NULL;

	node = self->data.array.next;

	for (; index; --index)
	{
		assert(node != NULL);
		node = node->next;
	}

	assert(node != NULL);
	assert(node->value != NULL);
	return node->value;
}

int QxJsonValue_arrayEach(QxJsonValue *self,
	int (*callback)(size_t, QxJsonValue *, void *), void *ptr)
{
	ArrayNode *end;
	ArrayNode *node;
	size_t index;
	int error;

	if (!self || self->type != QxJsonValueTypeArray)
		/* Invalid argument */
		return -1;

	end = &self->data.array;
	node = end->next;
	index = 0;

	for (; node != end; node = node->next, ++index)
	{
		error = (*callback)(index, node->value, ptr);

		if (error)
			return error;
	}

	return 0;
}

/* False */

QxJsonValue *QxJsonValue_falseNew(void)
{
	QxJsonValue *const instance = QxJsonValue_alloc();

	if (instance)
		QxJsonValue_init(instance, QxJsonValueTypeFalse);

	return instance;
}

/* Null */

QxJsonValue *QxJsonValue_nullNew(void)
{
	QxJsonValue *const instance = QxJsonValue_alloc();

	if (instance)
		QxJsonValue_init(instance, QxJsonValueTypeNull);

	return instance;
}

/* Number */

QxJsonValue *QxJsonValue_numberNew(double value)
{
	QxJsonValue *instance = NULL;

	if (isfinite(value))
	{
		instance = QxJsonValue_alloc();

		if (instance)
		{
			QxJsonValue_init(instance, QxJsonValueTypeNumber);
			instance->data.number = value;
		}
	}

	return instance;
}

double QxJsonValue_numberValue(QxJsonValue const *self)
{
	if (!self || self->type != QxJsonValueTypeNumber)
		/* Invalid argument */
		return NAN;

	return self->data.number;
}

/* Object */

QxJsonValue *QxJsonValue_objectNew(void)
{
	QxJsonValue *const instance = QxJsonValue_alloc();

	if (instance)
	{
		QxJsonValue_init(instance, QxJsonValueTypeObject);
		instance->data.object.next = &instance->data.object;
		instance->data.object.previous = &instance->data.object;
	}

	return instance;
}

static int compareKey(QxJsonValue const *first, QxJsonValue const *last)
{
	size_t keySize;
	wchar_t const *firstData, *lastData;

	assert(first != NULL);
	assert(last != NULL);
	assert(QX_JSON_IS_STRING(first));
	assert(QX_JSON_IS_STRING(last));

	if (first == last)
		/* Same instance */
		return 1;

	keySize = first->size;

	if (keySize != last->size)
		/* Different sizes */
		return 0;

	firstData = first->data.string;
	lastData = last->data.string;

	for (; keySize; --keySize)
	{
		if (*firstData != *lastData)
			/* Different data */
			return 0;
	}

	return 1;
}

int QxJsonValue_objectSet(QxJsonValue *self, QxJsonValue *key, QxJsonValue *value)
{
	ObjectNode *node, *end;

	if (!self || !key || !value
		|| self->type != QxJsonValueTypeObject
		|| key->type != QxJsonValueTypeString)
		/* Invalid argument */
		return -1;

	end = &self->data.object;
	node = end->next;

	for (; node != end; node = node->next)
	{
		if (compareKey(node->key, key))
		{
			/* Existing key */
			QxJsonValue_retains(value);
			QxJsonValue_release(node->value);
			node->value = value;
			return 0;
		}
	}

	/* New key */
	end = ObjectNode_alloc();

	if (!end)
		/* Failed to allocate memory */
		return -1;

	/* Insert the new node */
	end->previous = node;
	end->next = node->next;
	node->next = end;
	end->next->previous = end;

	/* Initialize it */
	end->key = key;
	end->value = value;
	QxJsonValue_retains((QxJsonValue *)key);
	QxJsonValue_retains(value);
	++self->size;

	return 0;
}

int QxJsonValue_objectUnset(QxJsonValue *self, QxJsonValue *key)
{
	ObjectNode *node, *end;

	if (!self || !key
		|| self->type != QxJsonValueTypeObject
		|| key->type != QxJsonValueTypeString)
		/* Invalid argument */
		return -1;

	end = &self->data.object;
	node = end->next;

	for (; node != end; node = node->next)
	{
		if (compareKey(node->key, key))
		{
			/* Key found */
			node->next->previous = node->previous;
			node->previous->next = node->next;
			ObjectNode_delete(node);
			--self->size;
			break;
		}
	}

	return 0;
}

int QxJsonValue_objectGet(QxJsonValue *self, const QxJsonValue *key,
	QxJsonValue **value)
{
	ObjectNode *node, *end;

	if (!self || self->type != QxJsonValueTypeObject
		|| !key || key->type != QxJsonValueTypeString || !value)
		/* Invalid argument */
		return -1;

	end = &self->data.object;
	node = end->next;

	for (; node != end; node = node->next)
		if (compareKey(node->key, key) == 0)
		{
			*value = node->value;
			return 0;
		}

	/* Not found */
	return -1;
}

int QxJsonValue_objectEach(QxJsonValue *self,
	int (*callback)(QxJsonValue const *, QxJsonValue *, void *), void *ptr)
{
	ObjectNode *end;
	ObjectNode *node;
	int error;

	if (!self || self->type != QxJsonValueTypeObject || !callback)
		/* Invalid argument */
		return -1;

	end = &self->data.object;
	node = end->next;

	for (; node != end; node = node->next)
	{
		error = (*callback)(node->key, node->value, ptr);

		if (error)
			return error;
	}

	return 0;
}

/* String */

QxJsonValue *QxJsonValue_stringNew(wchar_t const *data, size_t size)
{
	QxJsonValue *instance = NULL;

	if (data)
	{
		instance = QxJsonValue_alloc();

		if (instance)
		{
			QxJsonValue_init(instance, QxJsonValueTypeString);
			instance->size = size;
			instance->data.string = (wchar_t *)malloc(sizeof(wchar_t) * (size + 1));
			instance->data.string[size] = L'\0';

			while (size)
			{
				--size;
				instance->data.string[size] = data[size];
			}
		}
	}

	return instance;
}

wchar_t const *QxJsonValue_stringValue(QxJsonValue const *self)
{
	assert(self != NULL);

	if (self->type != QxJsonValueTypeString)
		return NULL;

	assert(self->data.string != NULL);
	return self->data.string;
}

/* True */

QxJsonValue *QxJsonValue_trueNew(void)
{
	QxJsonValue *const instance = QxJsonValue_alloc();

	if (instance)
		QxJsonValue_init(instance, QxJsonValueTypeTrue);

	return instance;
}
