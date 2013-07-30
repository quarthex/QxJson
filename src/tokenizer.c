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
#define WITHIN_0_9(value)  IN_RANGE((value), L'0', L'9')
#define WITHIN_1_9(value)  IN_RANGE((value), L'1', L'9')
#define WITHIN_a_f(value)  IN_RANGE((value), L'a', L'f')
#define WITHIN_A_F(value)  IN_RANGE((value), L'A', L'F')

static int Tokenizer_feedDefault(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedString(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedStringEscape(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedStringUnicode(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedStringUnicode0(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedStringUnicode1(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedStringUnicode2(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedF(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedFa(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedFal(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedFals(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedN(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedNu(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedNul(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedT(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedTr(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedTru(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedNumberMinus(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedNumberZero(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedNumberInteger(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedNumberDot(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedNumberFrac(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedNumberExponent(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedNumberExponentSign(QxJsonTokenizer *self, wchar_t character);
static int Tokenizer_feedNumberExponentInteger(QxJsonTokenizer *self, wchar_t character);

static int Tokenizer_endDefault(QxJsonTokenizer *self);
static int Tokenizer_endUnexpected(QxJsonTokenizer *self);
static int Tokenizer_endNumber(QxJsonTokenizer *self);

static int Tokenizer_wcharToBuffer(QxJsonTokenizer *self, wchar_t character);

static int Tokenizer_raiseToken(QxJsonTokenizer *self, QxJsonTokenType type);

/* Public implementations */

struct QxJsonTokenizer
{
	QxJsonTokenizerHandler *handler;

	/* Working variable */
	int (*feed)(QxJsonTokenizer *self, wchar_t character);
	int (*end)(QxJsonTokenizer *self);

	/* Buffer */
	wchar_t *bufferData;
	size_t bufferSize;
	size_t bufferAlloc;
};

QxJsonTokenizer *QxJsonTokenizer_new(QxJsonTokenizerHandler *handler)
{
	QxJsonTokenizer *instance;
	assert(handler != NULL);

	instance = (QxJsonTokenizer *)malloc(sizeof(QxJsonTokenizer));

	if (instance)
	{
		memset(instance, 0, sizeof(QxJsonTokenizer));
		instance->handler = handler;
		instance->feed = &Tokenizer_feedDefault;
		instance->end = &Tokenizer_endDefault;
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

int QxJsonTokenizer_feed(QxJsonTokenizer *self,
	wchar_t const *data, size_t size)
{
	int error = 0;

	for (; size && !error; ++data, --size)
		error = self->feed(self, *data);

	return error;
}

int QxJsonTokenizer_end(QxJsonTokenizer *self)
{
	return self->end(self);
}

/* Private implementations */

static int Tokenizer_feedDefault(QxJsonTokenizer *self, wchar_t character)
{
	assert(self != NULL);

	switch (character)
	{
	case L'\t':
	case L'\n':
	case L'\r':
	case L' ':
		/* Ignored white space */
		return 0;

	case L'"':
		self->feed = &Tokenizer_feedString;
		self->end = &Tokenizer_endUnexpected;
		self->bufferSize = 0;
		return 0;

	case L',':
		return Tokenizer_raiseToken(self, QxJsonTokenValuesSeparator);

	case L'-':
		self->feed = &Tokenizer_feedNumberMinus;
		self->end = &Tokenizer_endUnexpected;
		self->bufferSize = 0;
		return Tokenizer_wcharToBuffer(self, L'-');

	case L'0':
		self->feed = &Tokenizer_feedNumberZero;
		self->end = &Tokenizer_endUnexpected;
		self->bufferSize = 0;
		return Tokenizer_wcharToBuffer(self, L'0');

	case L':':
		return Tokenizer_raiseToken(self, QxJsonTokenNameValueSeparator);

	case L'[':
		return Tokenizer_raiseToken(self, QxJsonTokenBeginArray);

	case L']':
		return Tokenizer_raiseToken(self, QxJsonTokenEndArray);

	case L'f':
		self->feed = &Tokenizer_feedF;
		self->end = &Tokenizer_endUnexpected;
		return 0;

	case L'n':
		self->feed = &Tokenizer_feedN;
		self->end = &Tokenizer_endUnexpected;
		return 0;

	case L't':
		self->feed = &Tokenizer_feedT;
		self->end = &Tokenizer_endUnexpected;
		return 0;

	case L'{':
		return Tokenizer_raiseToken(self, QxJsonTokenBeginObject);

	case L'}':
		return Tokenizer_raiseToken(self, QxJsonTokenEndObject);

	default:

		if (WITHIN_1_9(character))
		{
			self->feed = &Tokenizer_feedNumberInteger;
			self->end = &Tokenizer_endNumber;
			return Tokenizer_wcharToBuffer(self, character);
		}
	}

	/* Unexpected character */
	return -1;
}

static int Tokenizer_feedString(QxJsonTokenizer *self, wchar_t character)
{
	switch (character)
	{
	case L'"': /* End of the string */
		return Tokenizer_raiseToken(self, QxJsonTokenString);

	case L'\\': /* Escaped sequence */
		self->feed = &Tokenizer_feedStringEscape;
		self->end = &Tokenizer_endUnexpected;
		break;

	default:
		Tokenizer_wcharToBuffer(self, character);
	}

	return 0;
}

static int Tokenizer_feedStringEscape(QxJsonTokenizer *self, wchar_t character)
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
	wchar_t const *translationOffset = translation;

	if (character == L'u')
	{
		self->feed = &Tokenizer_feedStringUnicode;
		assert(self->end == &Tokenizer_endUnexpected);
		return 0;
	}

	translationOffset = translation;

	while (*translationOffset && (character != *translationOffset))
	{
		translationOffset += 2;
	}

	if (*translationOffset)
	{
		self->feed = &Tokenizer_feedString;
		assert(self->end == &Tokenizer_endUnexpected);
		return Tokenizer_wcharToBuffer(self, *(translationOffset + 1));
	}

	/* Unsupported escapped sequence */
	return -1;
}

static int hexaDigitToValue(wchar_t digit)
{
	if (WITHIN_0_9(digit))
	{
		return digit - L'0';
	}

	if (WITHIN_a_f(digit))
	{
		return digit - L'a' + 10;
	}

	if (WITHIN_A_F(digit))
	{
		return digit - L'A' + 10;
	}

	return -1;
}

static int Tokenizer_feedStringUnicode(QxJsonTokenizer *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedStringUnicode0;
	assert(self->end == &Tokenizer_endUnexpected);
	return Tokenizer_wcharToBuffer(self, value << 24);
}

static int Tokenizer_feedStringUnicode0(QxJsonTokenizer *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedStringUnicode1;
	assert(self->end == &Tokenizer_endUnexpected);
	self->bufferData[self->bufferSize] |= value << 16;
	return 0;
}

static int Tokenizer_feedStringUnicode1(QxJsonTokenizer *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedStringUnicode2;
	assert(self->end == &Tokenizer_endUnexpected);
	self->bufferData[self->bufferSize] |= value << 8;
	return 0;
}

static int Tokenizer_feedStringUnicode2(QxJsonTokenizer *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedString;
	assert(self->end == &Tokenizer_endUnexpected);
	self->bufferData[self->bufferSize] |= value;
	return 0;
}

static int Tokenizer_feedF(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'a')
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedFa;
	assert(self->end == &Tokenizer_endUnexpected);
	return 0;
}

static int Tokenizer_feedFa(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedFal;
	assert(self->end == &Tokenizer_endUnexpected);
	return 0;
}

static int Tokenizer_feedFal(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L's')
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedFals;
	assert(self->end == &Tokenizer_endUnexpected);
	return 0;
}

static int Tokenizer_feedFals(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'e')
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedDefault;
	self->end = &Tokenizer_endDefault;
	return Tokenizer_raiseToken(self, QxJsonTokenFalse);
}

static int Tokenizer_feedN(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'u')
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedNu;
	assert(self->end == &Tokenizer_endUnexpected);
	return 0;
}

static int Tokenizer_feedNu(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedNul;
	assert(self->end == &Tokenizer_endUnexpected);
	return 0;
}

static int Tokenizer_feedNul(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedDefault;
	self->end = &Tokenizer_endDefault;
	return Tokenizer_raiseToken(self, QxJsonTokenNull);
}

static int Tokenizer_feedT(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'r')
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedTr;
	assert(self->end == &Tokenizer_endUnexpected);
	return 0;
}

static int Tokenizer_feedTr(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'u')
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedTru;
	assert(self->end == &Tokenizer_endUnexpected);
	return 0;
}

static int Tokenizer_feedTru(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'e')
	{
		/* Unexpected character */
		return -1;
	}

	self->feed = &Tokenizer_feedDefault;
	self->end = &Tokenizer_endDefault;
	return Tokenizer_raiseToken(self, QxJsonTokenTrue);
}

static int Tokenizer_feedNumberMinus(QxJsonTokenizer *self, wchar_t character)
{
	if (character == L'0')
	{
		Tokenizer_wcharToBuffer(self, L'0');
		self->feed = &Tokenizer_feedNumberZero;
		self->end = &Tokenizer_endNumber;
		return 0;
	}

	if (WITHIN_1_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->feed = &Tokenizer_feedNumberInteger;
		self->end = &Tokenizer_endNumber;
		return 0;
	}

	/* Unexpected character */
	return -1;
}

static int Tokenizer_feedNumberZero(QxJsonTokenizer *self, wchar_t character)
{
	int error;

	if (character == L'.')
	{
		Tokenizer_wcharToBuffer(self, L'.');
		self->feed = &Tokenizer_feedNumberDot;
		self->end = &Tokenizer_endUnexpected;
		return 0;
	}

	if ((character == L'e') || (character == L'E'))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->feed = &Tokenizer_feedNumberExponent;
		self->end = &Tokenizer_endUnexpected;
		return 0;
	}

	/* Maybe a new token */
	error = Tokenizer_endNumber(self);
	assert(self->feed == &Tokenizer_feedDefault);
	assert(self->end == &Tokenizer_endDefault);
	return error ? error : Tokenizer_feedDefault(self, character);
}

static int Tokenizer_feedNumberInteger(QxJsonTokenizer *self, wchar_t character)
{
	if (WITHIN_0_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		return 0;
	}

	/* Same expectations that after 0 */
	return Tokenizer_feedNumberZero(self, character);
}

static int Tokenizer_feedNumberDot(QxJsonTokenizer *self, wchar_t character)
{
	if (WITHIN_0_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->feed = &Tokenizer_feedNumberFrac;
		self->end = &Tokenizer_endNumber;
		return 0;
	}

	/* Unexpected character */
	return -1;
}

static int Tokenizer_feedNumberFrac(QxJsonTokenizer *self, wchar_t character)
{
	int error;

	if (WITHIN_0_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		return 0;
	}

	if ((character == L'e') || (character == L'E'))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->feed = &Tokenizer_feedNumberExponent;
		self->end = &Tokenizer_endUnexpected;
		return 0;
	}

	/* Maybe a new token */
	error = Tokenizer_endNumber(self);
	assert(self->feed == &Tokenizer_feedDefault);
	assert(self->end == &Tokenizer_endDefault);
	return error ? error : Tokenizer_feedDefault(self, character);
}

static int Tokenizer_feedNumberExponent(QxJsonTokenizer *self, wchar_t character)
{
	if ((character == L'-') || (character == L'+'))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->feed = &Tokenizer_feedNumberExponentSign;
		self->end = &Tokenizer_endUnexpected;
		return 0;
	}

	/* Same expectation as exponent sign */
	return Tokenizer_feedNumberExponentSign(self, character);
}

static int Tokenizer_feedNumberExponentSign(QxJsonTokenizer *self, wchar_t character)
{
	if (WITHIN_0_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->feed = &Tokenizer_feedNumberExponentInteger;
		self->end = &Tokenizer_endNumber;
		return 0;
	}

	/* Unexpected character */
	return -1;
}

static int Tokenizer_feedNumberExponentInteger(QxJsonTokenizer *self, wchar_t character)
{
	int error;

	if (WITHIN_0_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->feed = &Tokenizer_feedNumberExponentInteger;
		self->end = &Tokenizer_endNumber;
		return 0;
	}

	/* Maybe a new token */
	error = Tokenizer_endNumber(self);
	assert(self->feed == &Tokenizer_feedDefault);
	assert(self->end == &Tokenizer_endDefault);
	return error ? error : Tokenizer_feedDefault(self, character);
}

static int Tokenizer_endDefault(QxJsonTokenizer *self)
{
	QX_UNUSED(self);
	return 0;
}

static int Tokenizer_endUnexpected(QxJsonTokenizer *self)
{
	QX_UNUSED(self);
	return -1;
}

static int Tokenizer_endNumber(QxJsonTokenizer *self)
{
	return Tokenizer_raiseToken(self, QxJsonTokenNumber);
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

#ifndef NDEBUG
		/* Be kind with Valgrind */
		memset(dataTmp + (self->bufferAlloc - 512) * sizeof(wchar_t), 0, 512);
#endif
		self->bufferData = dataTmp;
	}

	self->bufferData[self->bufferSize] = character;
	++self->bufferSize;
	return 0;
}

static int Tokenizer_raiseToken(QxJsonTokenizer *self, QxJsonTokenType type)
{
	QxJsonToken token;
	self->feed = &Tokenizer_feedDefault;
	self->end = &Tokenizer_endDefault;
	int error;

	if (self->handler)
	{
		token.type = type;

		if (self->bufferSize)
		{
			/* Add a trailing nul character */
			error = Tokenizer_wcharToBuffer(self, L'\0');

			if (error)
			{
				return error;
			}

			/* minus the trailing nul character */
			token.size = self->bufferSize - 1;
			token.data = self->bufferData;

			self->bufferSize = 0;
		}
		else
		{
			token.size = 0;
			token.data = NULL;
		}

		return self->handler->feed(self->handler, &token);
	}

	self->bufferSize = 0;
	return 0;
}
