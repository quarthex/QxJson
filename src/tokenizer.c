/**
 * @file tokenizer.c
 * @brief Source file of the JSON tokenizer.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../include/qx.json.tokenizer.h"

/* Private functions */
#define IN_RANGE(value, min, max) (((value) >= (min)) && ((value) <= (max)))
#define DIGIT(value)              IN_RANGE((value), L'0', L'9')
#define DIGIT_1_9(value)          IN_RANGE((value), L'1', L'9')

static int Tokenizer_streamAtEnd(QxJsonTokenizer *self);
static int Tokenizer_wcharToBuffer(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_cursorToBuffer(QxJsonTokenizer *self);

static int Tokenizer_nextAtom(QxJsonTokenizer *self,
	QxJsonToken *token, QxJsonTokenType type);
static int Tokenizer_nextIdentifier(QxJsonTokenizer *self,
	QxJsonToken *token, char const *identifier, QxJsonTokenType type);
static int Tokenizer_nextString(QxJsonTokenizer *self, QxJsonToken *token);
static int Tokenizer_nextNumber(QxJsonTokenizer *self, QxJsonToken *token);

/* Public implementations */

struct QxJsonTokenizer
{
	/* Input stream */
	wchar_t const *streamCursor;
	wchar_t const *streamEnd;

	/* Buffer */
	wchar_t *bufferData;
	size_t bufferSize;
	size_t bufferAlloc;
};

QxJsonTokenizer *QxJsonTokenizer_new(void)
{
	QxJsonTokenizer *const instance = (QxJsonTokenizer *)malloc(sizeof(QxJsonTokenizer));

	if (instance)
	{
		memset(instance, 0, sizeof(QxJsonTokenizer));
	}

	return instance;
}

void QxJsonTokenizer_delete(QxJsonTokenizer *self)
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

int QxJsonTokenizer_resetStream(QxJsonTokenizer *self,
	wchar_t const *data, size_t size)
{
	assert(self != NULL);
	assert(data != NULL);

	self->streamCursor = data;
	self->streamEnd = data + size;

	return 0;
}

int QxJsonTokenizer_nextToken(QxJsonTokenizer *self, QxJsonToken *token)
{
	assert(self != NULL);
	assert(token != NULL);

	while (!Tokenizer_streamAtEnd(self))
	{
		switch (*self->streamCursor)
		{
		case L'\t':
		case L'\n':
		case L'\r':
		case L' ':
			/* Ignored white space */
			++self->streamCursor;
			break;

		case L'"':
			return Tokenizer_nextString(self, token);

		case L',':
			return Tokenizer_nextAtom(self, token, QxJsonTokenValuesSeparator);

		case L':':
			return Tokenizer_nextAtom(self,
				token, QxJsonTokenNameValueSeparator);

		case L'[':
			return Tokenizer_nextAtom(self, token, QxJsonTokenBeginArray);

		case L']':
			return Tokenizer_nextAtom(self, token, QxJsonTokenEndArray);

		case L'f':
			return Tokenizer_nextIdentifier(self,
				token, "alse", QxJsonTokenFalse);

		case L'n':
			return Tokenizer_nextIdentifier(self,
				token, "ull", QxJsonTokenNull);

		case L't':
			return Tokenizer_nextIdentifier(self,
				token, "rue", QxJsonTokenTrue);

		case L'{':
			return Tokenizer_nextAtom(self, token, QxJsonTokenBeginObject);

		case L'}':
			return Tokenizer_nextAtom(self, token, QxJsonTokenEndObject);

		default:

			if (DIGIT(*self->streamCursor) || (*self->streamCursor == L'-'))
			{
				return Tokenizer_nextNumber(self, token);
			}

			/* Unexpected character */
			return -1;
		}
	}

	return 0;
}

/* Private implementations */

static int Tokenizer_streamAtEnd(QxJsonTokenizer *self)
{
	return self->streamCursor == self->streamEnd;
}

static int Tokenizer_wcharToBuffer(QxJsonTokenizer *self, wchar_t character)
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

static int Tokenizer_cursorToBuffer(QxJsonTokenizer *self)
{
	int const error = Tokenizer_wcharToBuffer(self, *self->streamCursor);
	++self->streamCursor;
	return error;
}

static int Tokenizer_nextAtom(QxJsonTokenizer *self,
	QxJsonToken *token, QxJsonTokenType type)
{
	++self->streamCursor;
	token->type = type;
	token->size = 0;
	token->data = 0;
	return 1;
}

static int Tokenizer_nextIdentifier(QxJsonTokenizer *self,
	QxJsonToken *token, char const *identifier, QxJsonTokenType type)
{
	for (++self->streamCursor; !Tokenizer_streamAtEnd(self) && *identifier;
		++self->streamCursor, ++identifier)
	{
		if (*self->streamCursor != *identifier)
		{
			return -1;
		}
	}

	if (*identifier)
	{
		return -1;
	}

	token->type = type;
	token->size = 0;
	token->data = NULL;
	return 1;
}

static int Tokenizer_nextString(QxJsonTokenizer *self, QxJsonToken *token)
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
	wchar_t const *translationOffset;

	assert(self != NULL);
	assert(token != NULL);

	assert(*self->streamCursor == L'"');
	++self->streamCursor;
	self->bufferSize = 0;

	while (!Tokenizer_streamAtEnd(self))
	{
		switch (*self->streamCursor)
		{
		case L'"': /* End of the string */
			++self->streamCursor;
			token->type = QxJsonTokenString;
			token->size = self->bufferSize;
			token->data = self->bufferData;
			Tokenizer_wcharToBuffer(self, L'\0');
			return 1;

		case L'\\': /* Escaped sequence */
			++self->streamCursor;
			translationOffset = translation;

			while (*translationOffset && (*self->streamCursor != *translationOffset))
			{
				translationOffset += 2;
			}

			if (!*translationOffset)
			{
				/* Unsupported escapped sequence */
				return -1;
			}

			Tokenizer_wcharToBuffer(self, *(translationOffset + 1));
			++self->streamCursor;
			break;

		default:
			Tokenizer_cursorToBuffer(self);
			break;
		}
	}

	token->type = QxJsonTokenString;
	token->size = self->bufferSize;
	token->data = self->bufferData;
	return 1;
}

static int Tokenizer_nextNumber(QxJsonTokenizer *self, QxJsonToken *token)
{
	assert(self != NULL);
	assert(token != NULL);
	assert(!Tokenizer_streamAtEnd(self));

	self->bufferSize = 0;

	if (*self->streamCursor == L'-')
	{
		Tokenizer_cursorToBuffer(self);
	}

	if (Tokenizer_streamAtEnd(self))
	{
		/* Unexpected end of stream */
		return -1;
	}

	if (*self->streamCursor == L'0')
	{
		Tokenizer_cursorToBuffer(self);
	}
	else if (DIGIT_1_9(*self->streamCursor))
	{
		Tokenizer_cursorToBuffer(self);

		while (!Tokenizer_streamAtEnd(self) && DIGIT(*self->streamCursor))
		{
			Tokenizer_cursorToBuffer(self);
		}
	}
	else
	{
		/* Unexpected character */
		return -1;
	}

	if (!Tokenizer_streamAtEnd(self) && (*self->streamCursor == L'.'))
	{
		Tokenizer_cursorToBuffer(self);

		while ((self->streamCursor != self->streamEnd) && DIGIT(*self->streamCursor))
		{
			Tokenizer_cursorToBuffer(self);
		}
	}

	if (!Tokenizer_streamAtEnd(self)
		&& ((*self->streamCursor == L'e') || (*self->streamCursor == L'E')))
	{
		Tokenizer_cursorToBuffer(self);

		if (!Tokenizer_streamAtEnd(self)
			&& ((*self->streamCursor == L'-') || (*self->streamCursor == L'+')))
		{
			Tokenizer_cursorToBuffer(self);
		}

		while (!Tokenizer_streamAtEnd(self) && DIGIT(*self->streamCursor))
		{
			Tokenizer_cursorToBuffer(self);
		}
	}

	token->type = QxJsonTokenNumber;
	token->size = self->bufferSize;
	token->data = self->bufferData;
	Tokenizer_wcharToBuffer(self, L'\0');
	return 1;
}
