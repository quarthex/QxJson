/**
 * @file parser.c
 * @brief Source file of the QxJsonParser class.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../include/qx.json.parser.h"

static int feedDefault(QxJsonParser *self, QxJsonToken const *token);
static int feedArrayBegin(QxJsonParser *self, QxJsonToken const *token);
static int feedArrayValue(QxJsonParser *self, QxJsonToken const *token);
static int feedArraySeparator(QxJsonParser *self, QxJsonToken const *token);
static int feedObjectBegin(QxJsonParser *self, QxJsonToken const *token);
static int feedObjectKey(QxJsonParser *self, QxJsonToken const *token);
static int feedObjectNVSeparator(QxJsonParser *self, QxJsonToken const *token);
static int feedObjectValue(QxJsonParser *self, QxJsonToken const *token);
static int feedObjectVSeparator(QxJsonParser *self, QxJsonToken const *token);

static void *Parser_createValue(QxJsonParser *self, QxJsonValueType type);
static void *Parser_createValueFromToken(QxJsonParser *self, QxJsonToken const *token);
static int Parser_push(QxJsonParser *self, int isObject);
static int Parser_pop(QxJsonParser *self);

/* Public implementations */

typedef struct Array
{
	void **values;
	size_t alloc;
	size_t size;
} Array;

static void Array_init(Array *self);
static void Array_destroy(Array *self);
static int Array_push(Array *self, void *value);

typedef struct Object
{
	void **keys;
	void **values;
	size_t alloc;
	size_t size;
} Object;

static void Object_init(Object *self);
static void Object_destroy(Object *self);
static int Object_push(Object *self, void *key);

typedef struct StackItem
{
	int isObject;
	union
	{
		Array array;
		Object object;
	} value;
} StackItem;

struct QxJsonParser
{
	QxJsonValueFactory factory;
	void *userPtr;
	QxJsonValueSpec spec;


	int (*feed)(QxJsonParser *self, QxJsonToken const *token);

	StackItem *stack;
	size_t alloc;
	size_t size;
};

QxJsonParser *QxJsonParser_new(QxJsonValueFactory factory, void *userPtr)
{
	QxJsonParser *instance;

	instance = (QxJsonParser *)malloc(sizeof(QxJsonParser));

	if (instance)
	{
		memset(instance, 0, sizeof(QxJsonParser));
		instance->factory = factory;
		instance->userPtr = userPtr;
		instance->feed = &feedDefault;
	}

	return instance;
}

void QxJsonParser_delete(QxJsonParser *self)
{
	assert(self != NULL);

	while (self->size)
	{
		--self->size;

		if (self->stack[self->size].isObject)
			Object_destroy(&self->stack[self->size].value.object);
		else
			Array_destroy(&self->stack[self->size].value.array);
	}

	if (self->alloc)
	{
		assert(self->stack != NULL);
		free(self->stack);
	}

	free(self);
}

int QxJsonParser_feed(QxJsonParser *self, QxJsonToken const *token)
{
	return self->feed(self, token);
}

/* Private implementations */

static int feedDefault(QxJsonParser *self, QxJsonToken const *token)
{
	if (Parser_createValueFromToken(self, token) != NULL)
		return 0;

	switch (token->type)
	{
	case QxJsonTokenBeginArray:
		self->feed = &feedArrayBegin;
		return Parser_push(self, 0);

	case QxJsonTokenBeginObject:
		self->feed = &feedObjectBegin;
		return Parser_push(self, 1);

	default: break;
	}

	/* Unexpected token */
	return -1;
}

static int feedArrayBegin(QxJsonParser *self, QxJsonToken const *token)
{
	void *const value = Parser_createValueFromToken(self, token);

	if (value)
	{
		self->feed = &feedArrayValue;
		return Array_push(&self->stack[self->size - 1].value.array, value);
	}

	switch (token->type)
	{
	case QxJsonTokenBeginArray:
		self->feed = &feedArrayBegin;
		return Parser_push(self, 0);

	case QxJsonTokenBeginObject:
		self->feed = &feedObjectBegin;
		return Parser_push(self, 1);

	case QxJsonTokenEndArray:
		return Parser_pop(self);

	default: break;
	}

	/* Unexpected token or value not created */
	return -1;
}

static int feedArrayValue(QxJsonParser *self, QxJsonToken const *token)
{
	switch (token->type)
	{
	case QxJsonTokenValuesSeparator:
		self->feed = &feedArraySeparator;
		return 0;

	case QxJsonTokenEndArray:
		return Parser_pop(self);

	default: break;
	}

	/* Unexpected token */
	return -1;
}

static int feedArraySeparator(QxJsonParser *self, QxJsonToken const *token)
{
	void *const value = Parser_createValueFromToken(self, token);

	if (value)
	{
		self->feed = &feedArrayValue;
		return Array_push(&self->stack[self->size - 1].value.array, value);
	}

	switch (token->type)
	{
	case QxJsonTokenBeginArray:
		self->feed = &feedArrayBegin;
		return Parser_push(self, 0);

	case QxJsonTokenBeginObject:
		self->feed = &feedObjectBegin;
		return Parser_push(self, 1);

	default: break;
	}

	/* Unexpected token or value not created */
	return -1;
}

static int feedObjectBegin(QxJsonParser *self, QxJsonToken const *token)
{
	void *key;

	switch (token->type)
	{
	case QxJsonTokenString:
		self->spec.data.string.data = token->data;
		self->spec.data.string.size = token->size;
		key = Parser_createValue(self, QxJsonValueTypeString);

		if (!key)
			return -1;

		self->feed = &feedObjectKey;
		return Object_push(&self->stack[self->size - 1].value.object, key);

	case QxJsonTokenEndObject:
		return Parser_pop(self);

	default: break;
	}

	/* Unexpected token */
	return -1;
}

static int feedObjectKey(QxJsonParser *self, QxJsonToken const *token)
{
	if (token->type == QxJsonTokenNameValueSeparator)
	{
		self->feed = &feedObjectNVSeparator;
		return 0;
	}

	/* Unexpected token */
	return -1;
}

static int feedObjectNVSeparator(QxJsonParser *self, QxJsonToken const *token)
{
	void *const value = Parser_createValueFromToken(self, token);
	Object *object;

	if (value)
	{
		object = &self->stack[self->size - 1].value.object;
		object->values[object->size - 1] = value;
		self->feed = &feedObjectValue;
		return 0;
	}

	switch (token->type)
	{
	case QxJsonTokenBeginArray:
		self->feed = &feedArrayBegin;
		return Parser_push(self, 0);

	case QxJsonTokenBeginObject:
		self->feed = &feedObjectBegin;
		return Parser_push(self, 1);

	default: break;
	}

	/* Unexpected token or value not created */
	return -1;
}

static int feedObjectValue(QxJsonParser *self, QxJsonToken const *token)
{
	switch (token->type)
	{
	case QxJsonTokenValuesSeparator:
		self->feed = &feedObjectVSeparator;
		return 0;

	case QxJsonTokenEndObject:
		return Parser_pop(self);

	default: break;
	}

	/* Unexpected token */
	return -1;
}

static int feedObjectVSeparator(QxJsonParser *self, QxJsonToken const *token)
{
	void *key;

	if (token->type == QxJsonTokenString)
	{
		self->spec.data.string.data = token->data;
		self->spec.data.string.size = token->size;
		key = Parser_createValue(self, QxJsonValueTypeString);

		if (!key)
			return -1;

		return Object_push(&self->stack[self->size - 1].value.object, key);
	}

	/* Unexpected token */
	return -1;
}

static void *Parser_createValue(QxJsonParser *self, QxJsonValueType type)
{
	self->spec.type = type;
	return (*self->factory)(&self->spec, self->userPtr);
}

static void *Parser_createValueFromToken(QxJsonParser *self, QxJsonToken const *token)
{
	switch (token->type)
	{
	case QxJsonTokenString:
		self->spec.data.string.data = token->data;
		self->spec.data.string.size = token->size;
		return Parser_createValue(self, QxJsonValueTypeString);

	case QxJsonTokenNumber:
		self->spec.data.string.data = token->data;
		self->spec.data.string.size = token->size;
		return Parser_createValue(self, QxJsonValueTypeNumber);

	case QxJsonTokenFalse:
		return Parser_createValue(self, QxJsonValueTypeFalse);

	case QxJsonTokenTrue:
		return Parser_createValue(self, QxJsonValueTypeTrue);

	case QxJsonTokenNull:
		return Parser_createValue(self, QxJsonValueTypeNull);

	default: break;
	}

	return NULL;
}

static int Parser_push(QxJsonParser *self, int isObject)
{
	StackItem *temp;

	if (self->size == self->alloc)
	{
		temp = (StackItem *)realloc(self->stack, sizeof(StackItem) * (self->alloc << 1 | 1));

		if (!temp)
			return -1;

		self->stack = temp;
		self->alloc = self->alloc << 1 | 1;
	}

	temp = self->stack + self->size;
	temp->isObject = isObject;

	if (isObject)
		Object_init(&temp->value.object);
	else
		Array_init(&temp->value.array);

	++self->size;
	++self->spec.depth;
	return 0;
}

static int Parser_pop(QxJsonParser *self)
{
	void *value;
	StackItem *item;

	assert(self != NULL);
	assert(self->size > 0);
	--self->size;
	--self->spec.depth;
	item = self->stack + self->size;

	/* Close the array/object */
	if (item->isObject)
	{
		self->spec.data.object.keys = item->value.object.keys;
		self->spec.data.object.values = item->value.object.values;
		self->spec.data.object.size = item->value.object.size;
		value = Parser_createValue(self, QxJsonValueTypeObject);
		Object_destroy(&item->value.object);
	}
	else
	{
		self->spec.data.array.values = item->value.array.values;
		self->spec.data.array.size = item->value.array.size;
		value = Parser_createValue(self, QxJsonValueTypeArray);
		Array_destroy(&item->value.array);
	}

	if (!value)
		return -1;

	if (self->size)
	{
		/* Push it at the end of the parent array/object */
		--item;

		if (item->isObject)
		{
			item->value.object.values[item->value.object.size] = value;
			self->feed = &feedObjectValue;
			return 0;
		}

		self->feed = &feedArrayValue;
		return Array_push(&item->value.array, value);
	}

	return 0;
}

static void Array_init(Array *self)
{
	assert(self != NULL);
	memset(self, 0, sizeof(Array));
}

static void Array_destroy(Array *self)
{
	assert(self != NULL);

	if (self->alloc)
	{
		assert(self->values != NULL);
		free(self->values);
	}
}

static int Array_push(Array *self, void *value)
{
	void **temp;

	if (self->size == self->alloc)
	{
		temp = (void **)realloc(self->values, sizeof(void *) * (self->alloc << 1 | 1));

		if (!temp)
			return -1;

		self->values = temp;
		self->alloc = self->alloc << 1 | 1;
	}

	self->values[self->size] = value;
	++self->size;
	return 0;
}

static void Object_init(Object *self)
{
	assert(self != NULL);
	memset(self, 0, sizeof(Object));
}

static void Object_destroy(Object *self)
{
	assert(self != NULL);

	if (self->alloc)
	{
		assert(self->keys != NULL);
		free(self->keys);
		assert(self->values != NULL);
		free(self->values);
	}
}

static int Object_push(Object *self, void *key)
{
	void **tempKeys, **tempValues;

	if (self->size == self->alloc)
	{
		self->alloc = self->alloc << 1 | 1;

		tempKeys = (void **)realloc(self->keys, sizeof(void *) * self->alloc);
		tempValues = (void **)realloc(self->values, sizeof(void *) * self->alloc);

		if (!tempKeys || !tempValues)
		{
			self->alloc >>= 1;
			return -1;
		}

		self->keys = tempKeys;
		self->values = tempValues;
	}

	self->keys[self->size] = key;
	++self->size;
	return 0;
}