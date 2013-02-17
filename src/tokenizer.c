/**
 * @file tokenizer.c
 * @brief Source file of the JSON tokenizer.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../include/qx.json.tokenizer.h"

/* Some alias */
typedef QxJsonTokenizer Tokenizer;
typedef QxJsonToken     Token;
typedef QxJsonTokenType TokenType;

/* Private functions */
static int Tokenizer_grow(Tokenizer *self);
static int Tokenizer_raiseAtom(Tokenizer *self, TokenType type);
static int Tokenizer_flushFail(Tokenizer *self);

typedef struct Callbacks Callbacks;
struct Callbacks
{
	int (*const write)(Tokenizer *self);
	int (*const flush)(Tokenizer *self);
};

static Callbacks const Tokenizer_callbacksDefault;
static Callbacks const Tokenizer_callbacksAtom;
static Callbacks const Tokenizer_callbacksString;

/* Public implementations */

struct QxJsonTokenizer
{
	/* Raised token */
	Token token;
	/* State design pattern */
	Callbacks const *callbacks;
	/* Allocated space in token.data */
	size_t alloc;
	/* Data to be processed */
	wchar_t const *data;
	wchar_t const *dataEnd;
	/* Handler */
	int(*callback)(Token const *, void *);
	void *userData;
};

Tokenizer *QxJsonTokenizer_new(void)
{
	Tokenizer *const instance = (Tokenizer *)malloc(sizeof(Tokenizer));

	if (instance)
	{
		memset(instance, 0, sizeof(Tokenizer));

		if (Tokenizer_grow(instance) != 0)
		{
			/* memory allocation failed */
			free(instance);
			return NULL;
		}

		instance->alloc = 512;
		instance->callbacks = &Tokenizer_callbacksDefault;
	}

	return instance;
}

void QxJsonTokenizer_setHandler(QxJsonTokenizer *self,
	int (*callback)(Token const *, void *), void *userData)
{
	assert(self != NULL);
	assert(callback != NULL);
	self->callback = callback;
	self->userData = userData;
	return;
}

void QxJsonTokenizer_delete(Tokenizer *self)
{
	assert(self != NULL);

	if (self->alloc)
	{
		free(self->token.data);
	}

	free(self);
	return;
}

int QxJsonTokenizer_write(Tokenizer *self, wchar_t const *data, size_t size)
{
	int error = 0;

	assert(self != NULL);
	assert(data != NULL);

	self->data = data;
	self->dataEnd = data + size;

	while (!error && (self->data != self->dataEnd))
	{
		assert(self->callbacks != NULL);
		assert(self->callbacks->write != NULL);
		error = self->callbacks->write(self);
	}

	return error;
}

int QxJsonTokenizer_flush(Tokenizer *self)
{
	assert(self != NULL);
	assert(self->callbacks);
	assert(self->callbacks->flush);
	return self->callbacks->flush(self);
}

/* Private implementations */

static int Tokenizer_grow(QxJsonTokenizer *self)
{
	wchar_t *dataTmp;
	self->alloc += 512;
	dataTmp =	(wchar_t *)realloc(self->token.data, self->alloc * sizeof(wchar_t));

	if (!dataTmp)
	{
		/* Memory allocation failed */
		self->alloc -= 512;
		return -1;
	}

	self->token.data = dataTmp;
	return 0;
}

static int Tokenizer_raiseAtom(Tokenizer *self, TokenType type)
{
	assert(self->token.size == 0);

	self->token.type = type;
	self->token.data[0] = L'\0';

	return self->callback(&self->token, self->userData);
}

static int Tokenizer_flushFail(Tokenizer *self)
{
	/* Unflushabled */
	QX_UNUSED(self);
	return -1;
}

static int Tokenizer_writeDefault(Tokenizer *self)
{
	assert(self != NULL);
	assert(self->data != self->dataEnd);

	self->token.size = 0;

	switch (*self->data)
	{
	case L'\t':
	case L'\n':
	case L'\r':
	case L' ':
		/* Ignored white space */
		++self->data;
		return 0;

	case L'"':
		++self->data;
		self->callbacks = &Tokenizer_callbacksDefault;
		return 0;

	case L',':
		++self->data;
		return Tokenizer_raiseAtom(self, QxJsonTokenValuesSeparator);

	case L':':
		++self->data;
		return Tokenizer_raiseAtom(self, QxJsonTokenNameValueSeparator);

	case L'[':
		++self->data;
		return Tokenizer_raiseAtom(self, QxJsonTokenBeginArray);

	case L']':
		++self->data;
		return Tokenizer_raiseAtom(self, QxJsonTokenEndArray);

	case L'f':
		++self->data;
		self->token.type = QxJsonTokenFalse;
		memcpy(self->token.data, L"esla", 4 * sizeof(wchar_t));
		self->token.size = 4;
		self->callbacks = &Tokenizer_callbacksAtom;
		return 0;

	case L'n':
		++self->data;
		self->token.type = QxJsonTokenNull;
		memcpy(self->token.data, L"llu", 3 * sizeof(wchar_t));
		self->token.size = 3;
		self->callbacks = &Tokenizer_callbacksAtom;
		return 0;

	case L't':
		++self->data;
		self->token.type = QxJsonTokenTrue;
		memcpy(self->token.data, L"eur", 3 * sizeof(wchar_t));
		self->token.size = 3;
		self->callbacks = &Tokenizer_callbacksAtom;
		return 0;

	case L'{':
		++self->data;
		return Tokenizer_raiseAtom(self, QxJsonTokenBeginObject);

	case L'}':
		++self->data;
		return Tokenizer_raiseAtom(self, QxJsonTokenEndObject);
	}

	/* Unexpected character */
	return -1;
}

static int Tokenizer_flushDefault(Tokenizer *self)
{
	/* Idle state, nothing in the buffer */
	QX_UNUSED(self);
	return 0;
}

static Callbacks const Tokenizer_callbacksDefault = {
	Tokenizer_writeDefault,
	Tokenizer_flushDefault
};

static int Tokenizer_writeAtom(Tokenizer *self)
{
	assert(self != NULL);

	while (self->token.size && (self->data != self->dataEnd))
	{
		--self->token.size;

		if (*self->data != self->token.data[self->token.size])
		{
			return -1;
		}

		++self->data;
	}

	if (self->token.size)
	{
		/* Atom parsing not yet finished */
		return 0;
	}

	self->callbacks = &Tokenizer_callbacksDefault;
	assert(self->callback);
	return self->callback(&self->token, self->userData);
}

static Callbacks const Tokenizer_callbacksAtom = {
	Tokenizer_writeAtom,
	Tokenizer_flushFail
};

static int Tokenizer_writeString(Tokenizer *self)
{
	assert(self != NULL);

	for (; self->data != self->dataEnd; ++self->data, ++self->token.size)
	{
		if (*self->data == L'"') /* End of the string */
		{
			self->callbacks = &Tokenizer_callbacksDefault;
			return self->callback(&self->token, self->userData);
		}

		if (self->token.size == self->alloc)
		{
			if (Tokenizer_grow(self) != 0)
			{
				/* Failed to allocate more memory */
				return -1;
			}
		}

		self->token.data[self->token.size] = *self->data;
	}

	/* String parsing not yet finished */
	return 0;
}

static Callbacks const Tokenizer_callbacksString = {
	Tokenizer_writeString,
	Tokenizer_flushFail
};
