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
static int Tokenizer_addToBuffer(Tokenizer *self, wchar_t character);
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
static Callbacks const Tokenizer_callbacksNumberMinus;
static Callbacks const Tokenizer_callbacksNumberZero;
static Callbacks const Tokenizer_callbacksNumberInterger;
static Callbacks const Tokenizer_callbacksNumberDot;
static Callbacks const Tokenizer_callbacksNumberFrac;
static Callbacks const Tokenizer_callbacksNumberExponent;
static Callbacks const Tokenizer_callbacksNumberExponentSign;
static Callbacks const Tokenizer_callbacksNumberExponentSignInteger;

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

	if (self->bufferData)
	{
		assert(self->bufferAlloc > 0);
		free(self->bufferData);
	}

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

static int Tokenizer_addToBuffer(Tokenizer *self, wchar_t character)
{
	wchar_t *dataTmp;

	if (self->bufferAlloc == self->bufferSize)
	{
		self->bufferAlloc += 512;
		dataTmp = (wchar_t *)realloc(self->bufferData,
			self->bufferAlloc * sizeof(wchar_t));

		if (!dataTmp)
		{
			/* Memory allocation failed */
			self->bufferAlloc -= 512;
			return -1;
		}

		self->bufferData = dataTmp;
	}

	self->bufferData[self->bufferSize] = character;
	++self->bufferSize;
	return 0;
}

static int Tokenizer_raiseToken(Tokenizer *self, TokenType type)
{
	Token token;

	token.type = type;
	token.size = self->bufferSize;
	token.data = (token.size > 0) ? self->bufferData : NULL;

	self->callbacks = &Tokenizer_callbacksDefault;
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

	if (character >= L'1' && character <= L'9')
	{
		Tokenizer_addToBuffer(self, character);
		self->callbacks = &Tokenizer_callbacksNumberInterger;
		return 0;
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

	return Tokenizer_addToBuffer(self, character);
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

static int Tokenizer_flushNumber(Tokenizer *self)
{
	return Tokenizer_raiseToken(self, QxJsonTokenNumber);
}

static int Tokenizer_writeNumberMinus(Tokenizer *self, wchar_t character)
{
	switch (character)
	{
	case L'0':
		self->callbacks = &Tokenizer_callbacksNumberZero;
		break;

	default:
		if (character >= L'1' && character <= L'9')
		{
			self->callbacks = &Tokenizer_callbacksNumberInterger;
		}
		else
		{
			/* Unexpected character */
			return -1;
		}
	}

	return Tokenizer_addToBuffer(self, L'O');
}

static Callbacks const Tokenizer_callbacksNumberMinus = {
	Tokenizer_writeNumberMinus,
	Tokenizer_flushFail
};

static int Tokenizer_writeNumberZero(Tokenizer *self, wchar_t character)
{
	switch (character)
	{
	case L'.':
		self->callbacks = &Tokenizer_callbacksNumberDot;
		break;

	case L'e':
	case L'E':
		self->callbacks = &Tokenizer_callbacksNumberExponent;
		break;

	default:
		/* Unexpected character */
		return -1;
	}

	return Tokenizer_addToBuffer(self, character);
}

static Callbacks const Tokenizer_callbacksNumberZero = {
	Tokenizer_writeNumberZero,
	Tokenizer_flushNumber
};

static int Tokenizer_writeNumberInteger(Tokenizer *self, wchar_t character)
{
	int error;

	switch (character)
	{
	case L'.':
		self->callbacks = &Tokenizer_callbacksNumberDot;
		break;

	default:
		if (character < L'0' || character > L'9')
		{
			/* End of the number */
			error = Tokenizer_flushNumber(self);

			if (error)
			{
				return error;
			}

			assert(self->callbacks->write == &Tokenizer_writeDefault);
			return Tokenizer_writeDefault(self, character);
		}
	}

	return Tokenizer_addToBuffer(self, L'.');
}

static Callbacks const Tokenizer_callbacksNumberInterger = {
	Tokenizer_writeNumberInteger,
	Tokenizer_flushNumber
};

static int Tokenizer_writeNumberDot(Tokenizer *self, wchar_t character)
{
	if (character >= L'0' && character <= L'9')
	{
		self->callbacks = &Tokenizer_callbacksNumberFrac;
		return Tokenizer_addToBuffer(self, character);
	}

	/* Unexpected character */
	return -1;
}

static Callbacks const Tokenizer_callbacksNumberDot = {
	Tokenizer_writeNumberDot,
	Tokenizer_flushFail
};

static int Tokenizer_writeNumberFrac(Tokenizer *self, wchar_t character)
{
	int error;

	switch (character)
	{
	case L'e':
	case L'E':
		self->callbacks = &Tokenizer_callbacksNumberExponent;
		break;

	default:
		if (character < L'0' || character > L'9')
		{
			/* End of number */
			error = Tokenizer_raiseToken(self, QxJsonTokenNumber);

			if (error)
			{
				return error;
			}

			assert(self->callbacks == &Tokenizer_callbacksDefault);
			return Tokenizer_writeDefault(self, character);
		}
	}

	return Tokenizer_addToBuffer(self, character);
}

static Callbacks const Tokenizer_callbacksNumberFrac = {
	Tokenizer_writeNumberFrac,
	Tokenizer_flushNumber
};

static int Tokenizer_writeNumberExponent(Tokenizer *self, wchar_t character)
{
	int error;

	switch (character)
	{
	case L'-':
	case L'+':
		self->callbacks = &Tokenizer_callbacksNumberExponentSign;
		break;

	default:
		if (character < L'0' || character > L'9')
		{
			/* End of the number */

			error = Tokenizer_raiseToken(self, QxJsonTokenNumber);

			if (error)
			{
				return error;
			}

			assert(self->callbacks = &Tokenizer_callbacksDefault);
			return Tokenizer_writeDefault(self, character);
		}
	}

	return Tokenizer_addToBuffer(self, character);
}

static Callbacks const Tokenizer_callbacksNumberExponent = {
	Tokenizer_writeNumberExponent,
	Tokenizer_flushNumber
};

static int Tokenizer_writeNumberExponentSign(Tokenizer *self, wchar_t character)
{
	if (character >= L'0' && character <= L'9')
	{
		self->callbacks = &Tokenizer_callbacksNumberExponentSignInteger;
		return Tokenizer_addToBuffer(self, character);
	}

	/* Unexpected character */
	return -1;
}

static Callbacks const Tokenizer_callbacksNumberExponentSign = {
	Tokenizer_writeNumberExponentSign,
	Tokenizer_flushNumber
};

static int Tokenizer_writeNumberExponentSignInteger(Tokenizer *self, wchar_t character)
{
	int error;

	if (character >= L'0' && character <= L'9')
	{
		return Tokenizer_addToBuffer(self, character);
	}

	/* End of the number */

	error = Tokenizer_raiseToken(self, QxJsonTokenNumber);

	if (error)
	{
		return error;
	}

	return Tokenizer_writeDefault(self, character);
}

static Callbacks const Tokenizer_callbacksNumberExponentSignInteger = {
	Tokenizer_writeNumberExponentSignInteger,
	Tokenizer_flushNumber
};
