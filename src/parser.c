/**
 * @file parser.c
 * @brief Source file of the QxJsonParser class.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "../include/qx.json.parser.h"

static int feedAfterVoid(QxJsonParser *self, QxJsonToken const *token);
static int feedAfterValue(QxJsonParser *self, QxJsonToken const *token);
static int feedAfterArrayBegin(QxJsonParser *self, QxJsonToken const *token);
static int feedAfterArrayValue(QxJsonParser *self, QxJsonToken const *token);
static int feedAfterArrayComma(QxJsonParser *self, QxJsonToken const *token);
static int feedAfterObjectBegin(QxJsonParser *self, QxJsonToken const *token);
static int feedAfterObjectKey(QxJsonParser *self, QxJsonToken const *token);
static int feedAfterObjectColon(QxJsonParser *self, QxJsonToken const *token);
static int feedAfterObjectValue(QxJsonParser *self, QxJsonToken const *token);
static int feedAfterObjectComma(QxJsonParser *self, QxJsonToken const *token);
static void popStackItem(QxJsonParser *self);
static QxJsonValue *createValueFromToken(QxJsonParser *self, QxJsonToken const *token);

/* Public implementations */

typedef struct StackValue
{
	QxJsonValue *value;
	struct StackValue *next;
} StackValue;

#define StackValue_alloc() ((StackValue *)malloc(sizeof(StackValue)))

struct QxJsonParser
{
	void *key;
	int(*feed)(QxJsonParser *self, QxJsonToken const *token);
	StackValue head;
};

QxJsonParser *QxJsonParser_new(void)
{
	QxJsonParser *instance;

	instance = (QxJsonParser *)malloc(sizeof(QxJsonParser));

	if (instance)
	{
		memset(instance, 0, sizeof(QxJsonParser));
		instance->feed = &feedAfterVoid;
	}

	return instance;
}

void QxJsonParser_release(QxJsonParser *self)
{
	StackValue *item;

	if (self)
	{
		if (self->head.value)
		{
			QxJsonValue_release(self->head.next->value);

			if (self->key)
				QxJsonValue_release(self->key);

			while (self->head.next)
			{
				item = self->head.next;
				self->head.next = item->next;
				free(item);
			}
		}

		free(self);
	}
}

int QxJsonParser_feed(QxJsonParser *self, QxJsonToken const *token)
{
	if (!self || !token)
		/* Invalid argument */
		return -1;

	return (*self->feed)(self, token);
}

int QxJsonParser_end(QxJsonParser *self, QxJsonValue **value)
{
	if (!self || !value)
		/* Invalid arguments */
		return -1;

	if (self->head.value && !self->head.next)
	{
		*value = self->head.value;
		self->head.value = NULL;
		self->feed = &feedAfterVoid;
		return 0;
	}

	/* Value is not ready */
	return -1;
}

/* Private implementations */

static int feedAfterVoid(QxJsonParser *self, QxJsonToken const *token)
{
	self->head.value = createValueFromToken(self, token);

	if (!self->head.value)
		return -1;

	if (self->feed == &feedAfterVoid)
		self->feed = &feedAfterValue;

	return 0;
}

static int feedAfterValue(QxJsonParser *self, const QxJsonToken *token)
{
	(void)self;
	(void)token;
	return -1;
}

static int feedAfterArrayBegin(QxJsonParser *self, QxJsonToken const *token)
{
	QxJsonValue *value;

	switch (token->type)
	{
	case QxJsonTokenEndArray:
		popStackItem(self);
		break;

	default:
		value = createValueFromToken(self, token);

		if (!value)
			/* Unexpected token */
			return -1;

		if (QxJsonValue_arrayAppendNew(self->head.next->value, value) != 0)
			/* Failed to append a value to the array */
			return -1;

		self->feed = &feedAfterArrayValue;
		break;
	}

	return 0;
}

static int feedAfterArrayValue(QxJsonParser *self, QxJsonToken const *token)
{
	switch (token->type)
	{
	case QxJsonTokenEndArray:
		popStackItem(self);
		break;

	case QxJsonTokenValuesSeparator:
		self->feed = &feedAfterArrayComma;
		break;

	default:
		/* Unexpected token */
		return -1;
	}

	return 0;
}

static int feedAfterArrayComma(QxJsonParser *self, QxJsonToken const *token)
{
	QxJsonValue *value;
	StackValue *head;

	head = self->head.next;
	value = createValueFromToken(self, token);

	if (!value)
		/* Unexpected token */
		return -1;

	if (QxJsonValue_arrayAppendNew(head->value, value) != 0)
		/* Failed to append a value to the array */
		return -1;

	self->feed = &feedAfterArrayValue;
	return 0;
}

static int feedAfterObjectBegin(QxJsonParser *self, QxJsonToken const *token)
{
	switch (token->type)
	{
	case QxJsonTokenString:
		self->key = QxJsonValue_stringNew(token->data, token->size);

		if (!self->key)
			/* Failed to create string value */
			return -1;

		self->feed = &feedAfterObjectKey;
		break;

	case QxJsonTokenEndObject:
		popStackItem(self);
		break;

	default:
		/* Unexpected token */
		return -1;
	}

	return 0;
}

static int feedAfterObjectKey(QxJsonParser *self, QxJsonToken const *token)
{
	if (token->type != QxJsonTokenNameValueSeparator)
	{
		/* Unexpected token */
		return -1;
	}

	self->feed = &feedAfterObjectColon;
	return 0;
}

static int feedAfterObjectColon(QxJsonParser *self, QxJsonToken const *token)
{
	QxJsonValue *value;
	value = createValueFromToken(self, token);

	if (!value)
		/* Unexpected token */
		return -1;

	assert(self->key != NULL);

	if (QxJsonValue_objectSet(self->head.next->value, self->key, value) != 0)
		/* Failed to add the new key/value pair */
		return -1;

	QxJsonValue_release(self->key);
	self->key = NULL;
	QxJsonValue_release(value);

	self->feed = &feedAfterObjectValue;
	return 0;
}

static int feedAfterObjectValue(QxJsonParser *self, QxJsonToken const *token)
{
	switch (token->type)
	{
	case QxJsonTokenValuesSeparator:
		self->feed = &feedAfterObjectComma;
		break;

	case QxJsonTokenEndObject:
		popStackItem(self);
		break;

	default:
		/* Unexpected token */
		return -1;
	}

	return 0;
}

static int feedAfterObjectComma(QxJsonParser *self, QxJsonToken const *token)
{
	if (token->type != QxJsonTokenString)
		/* Unexpected token */
		return -1;

	assert(self->key == NULL);
	self->key = QxJsonValue_stringNew(token->data, token->size);

	if (!self->key)
		/* Failed to create the string */
		return -1;

	self->feed = &feedAfterObjectKey;
	return 0;
}

static void popStackItem(QxJsonParser *self)
{
	StackValue *item;
	item = self->head.next;
	self->head.next = item->next;
	free(item);

	if (self->head.next)
	{
		if (QX_JSON_IS_ARRAY(self->head.next->value))
		{
			self->feed = &feedAfterArrayValue;
		}
		else
		{
			assert(QX_JSON_IS_OBJECT(self->head.next->value));
			self->feed = &feedAfterObjectValue;
		}
	}
	else
	{
		self->feed = &feedAfterValue;
	}
}

static QxJsonValue *createValueFromToken(QxJsonParser *self, QxJsonToken const *token)
{
	StackValue *item = NULL;
	double number;
	wchar_t *endptr;

	switch (token->type)
	{
	case QxJsonTokenString:
		return QxJsonValue_stringNew(token->data, token->size);

	case QxJsonTokenNumber:
		endptr = NULL;
		number = wcstod(token->data, &endptr);

		if (endptr == token->data + token->size)
			return QxJsonValue_numberNew(number);

		break;

	case QxJsonTokenFalse:
		return QxJsonValue_falseNew();

	case QxJsonTokenTrue:
		return QxJsonValue_trueNew();

	case QxJsonTokenNull:
		return QxJsonValue_nullNew();

	case QxJsonTokenBeginArray:

		item = StackValue_alloc();

		if (item)
		{
			item->value = QxJsonValue_arrayNew();

			if (item->value)
				self->feed = &feedAfterArrayBegin;
		}

		/* Allocation error */
		break;

	case QxJsonTokenBeginObject:

		item = StackValue_alloc();

		if (item)
		{
			item->value = QxJsonValue_objectNew();

			if (item->value)
				self->feed = &feedAfterObjectBegin;
		}

		/* Allocation error */
		break;

	default:
		break;
	}

	if (item)
	{
		if (item->value)
		{
			item->next = self->head.next;
			self->head.next = item;
			return item->value;
		}

		/* Allocation error */
		free(item);
	}

	return NULL;
}
