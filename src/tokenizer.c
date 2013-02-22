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
static int Tokenizer_raiseToken(Tokenizer *self, TokenType type);
static int Tokenizer_flushFail(Tokenizer *self);

typedef struct Callbacks Callbacks;
struct Callbacks
{
	int (*const write)(Tokenizer *self, wchar_t character);
	int (*const flush)(Tokenizer *self);
};

static Callbacks const Tokenizer_callbacksDefault;
static Callbacks const Tokenizer_callbacksFalse;
static Callbacks const Tokenizer_callbacksNull;
static Callbacks const Tokenizer_callbacksTrue;
static Callbacks const Tokenizer_callbacksString;
static Callbacks const Tokenizer_callbacksStringEscape;

/* Public implementations */

struct QxJsonTokenizer
{
	/* State design pattern */
	Callbacks const *callbacks;

	/* Buffer */
	wchar_t *bufferData;
	size_t bufferSize;
	size_t bufferAlloc;

	/* Handler */
	int(*handlerCallback)(Token const *, void *);
	void *handlerUserData;
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

		instance->bufferAlloc = 512;
		instance->callbacks = &Tokenizer_callbacksDefault;
	}

	return instance;
}

void QxJsonTokenizer_setHandler(Tokenizer *self,
	int (*callback)(Token const *, void *), void *userData)
{
	assert(self != NULL);
	assert(callback != NULL);
	self->handlerCallback = callback;
	self->handlerUserData = userData;
	return;
}

void QxJsonTokenizer_delete(Tokenizer *self)
{
	assert(self != NULL);
	assert(self->bufferAlloc > 0);
	assert(self->bufferData != NULL);
	free(self->bufferData);
	free(self);
	return;
}

int QxJsonTokenizer_write(Tokenizer *self, wchar_t const *data, size_t size)
{
	int error = 0;
	wchar_t const *const end = data + size;

	assert(self != NULL);
	assert(data != NULL);

	for (; !error && (data != end); ++data)
	{
		assert(self->callbacks != NULL);
		assert(self->callbacks->write != NULL);
		error = self->callbacks->write(self, *data);
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

static int Tokenizer_grow(Tokenizer *self)
{
	wchar_t *dataTmp;
	self->bufferAlloc += 512;
	dataTmp =	(wchar_t *)realloc(self->bufferData,
		self->bufferAlloc * sizeof(wchar_t));

	if (!dataTmp)
	{
		/* Memory allocation failed */
		self->bufferAlloc -= 512;
		return -1;
	}

	self->bufferData = dataTmp;
	return 0;
}

static int Tokenizer_raiseToken(Tokenizer *self, TokenType type)
{
	Token token;

	token.type = type;
	token.size = self->bufferSize;
	token.data = (token.size > 0) ? self->bufferData : NULL;

	return self->handlerCallback(&token, self->handlerUserData);
}

static int Tokenizer_flushFail(Tokenizer *self)
{
	/* Unflushabled */
	QX_UNUSED(self);
	return -1;
}

static int Tokenizer_writeDefault(Tokenizer *self, wchar_t character)
{
	assert(self != NULL);

	self->bufferSize = 0;

	switch (character)
	{
	case L'\t':
	case L'\n':
	case L'\r':
	case L' ':
		/* Ignored white space */
		return 0;

	case L'"':
		self->callbacks = &Tokenizer_callbacksString;
		return 0;

	case L',':
		return Tokenizer_raiseToken(self, QxJsonTokenValuesSeparator);

	case L':':
		return Tokenizer_raiseToken(self, QxJsonTokenNameValueSeparator);

	case L'[':
		return Tokenizer_raiseToken(self, QxJsonTokenBeginArray);

	case L']':
		return Tokenizer_raiseToken(self, QxJsonTokenEndArray);

	case L'f':
		self->callbacks = &Tokenizer_callbacksFalse;
		return 0;

	case L'n':
		self->callbacks = &Tokenizer_callbacksNull;
		return 0;

	case L't':
		self->callbacks = &Tokenizer_callbacksTrue;
		return 0;

	case L'{':
		return Tokenizer_raiseToken(self, QxJsonTokenBeginObject);

	case L'}':
		return Tokenizer_raiseToken(self, QxJsonTokenEndObject);
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

typedef struct Atom Atom;
struct Atom
{
	wchar_t const *const atom;
	size_t atomSize;
	TokenType type;
};

static int Tokenizer_writeAtom(Tokenizer *self, wchar_t character, Atom const *atom)
{
	if (character != atom->atom[self->bufferSize])
	{
		return -1;
	}

	++self->bufferSize;

	if (self->bufferSize == atom->atomSize)
	{
		self->callbacks = &Tokenizer_callbacksDefault;
		self->bufferSize = 0;
		return Tokenizer_raiseToken(self, atom->type);
	}

	/* Parsing not yet finished */
	return 0;
}

static int Tokenizer_writeFalse(Tokenizer *self, wchar_t character)
{
	static Atom const atom = { L"alse", 4, QxJsonTokenFalse };
	return Tokenizer_writeAtom(self, character, &atom);
}

static Callbacks const Tokenizer_callbacksFalse = {
	Tokenizer_writeFalse,
	Tokenizer_flushFail
};

static int Tokenizer_writeNull(Tokenizer *self, wchar_t character)
{
	static Atom const atom = { L"ull", 3, QxJsonTokenNull };
	return Tokenizer_writeAtom(self, character, &atom);
}

static Callbacks const Tokenizer_callbacksNull = {
	Tokenizer_writeNull,
	Tokenizer_flushFail
};

static int Tokenizer_writeTrue(Tokenizer *self, wchar_t character)
{
	static Atom const atom = { L"rue", 3, QxJsonTokenTrue };
	return Tokenizer_writeAtom(self, character, &atom);
}

static Callbacks const Tokenizer_callbacksTrue = {
	Tokenizer_writeTrue,
	Tokenizer_flushFail
};

static int Tokenizer_writeString(Tokenizer *self, wchar_t character)
{
	assert(self != NULL);

	switch (character)
	{
	case L'"': /* End of the string */
		self->callbacks = &Tokenizer_callbacksDefault;
		return Tokenizer_raiseToken(self, QxJsonTokenString);

	case L'\\': /* Escaped sequence */
		self->callbacks = &Tokenizer_callbacksStringEscape;
		return 0;
	}

	if (self->bufferSize == self->bufferAlloc)
	{
		if (Tokenizer_grow(self) != 0)
		{
			/* Failed to allocate more memory */
			return -1;
		}
	}

	self->bufferData[self->bufferSize] = character;
	++self->bufferSize;
	return 0;
}

static Callbacks const Tokenizer_callbacksString = {
	Tokenizer_writeString,
	Tokenizer_flushFail
};

static int Tokenizer_writeStringEscape(Tokenizer *self, wchar_t character)
{
	wchar_t const *const translation =
		L"\"" L"\""
		L"/"  L"/"
		L"\\" L"\\"
		L"b"  L"\b"
		L"f"  L"\f"
		L"r"  L"\r"
		L"n"  L"\n"
		L"t"  L"\t";
	wchar_t const *offset = translation;

	assert(self != NULL);

	for (; *offset; offset += 2)
	{
		if (character == *offset)
		{
			self->bufferData[self->bufferSize] = *(offset + 1);
			++self->bufferSize;
			self->callbacks = &Tokenizer_callbacksString;
			return 0;
		}
	}

	/* Unexpected sequence */
	return -1;
}

static Callbacks const Tokenizer_callbacksStringEscape = {
	Tokenizer_writeStringEscape,
	Tokenizer_flushFail
};
