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

static int feedDefault(QxJsonTokenizer *self, wchar_t character);
static int feedString(QxJsonTokenizer *self, wchar_t character);
static int feedStringEscape(QxJsonTokenizer *self, wchar_t character);
static int feedStringUnicode(QxJsonTokenizer *self, wchar_t character);
static int feedStringUnicode0(QxJsonTokenizer *self, wchar_t character);
static int feedStringUnicode1(QxJsonTokenizer *self, wchar_t character);
static int feedStringUnicode2(QxJsonTokenizer *self, wchar_t character);
static int feedF(QxJsonTokenizer *self, wchar_t character);
static int feedFa(QxJsonTokenizer *self, wchar_t character);
static int feedFal(QxJsonTokenizer *self, wchar_t character);
static int feedFals(QxJsonTokenizer *self, wchar_t character);
static int feedN(QxJsonTokenizer *self, wchar_t character);
static int feedNu(QxJsonTokenizer *self, wchar_t character);
static int feedNul(QxJsonTokenizer *self, wchar_t character);
static int feedT(QxJsonTokenizer *self, wchar_t character);
static int feedTr(QxJsonTokenizer *self, wchar_t character);
static int feedTru(QxJsonTokenizer *self, wchar_t character);
static int feedNumberMinus(QxJsonTokenizer *self, wchar_t character);
static int feedNumberZero(QxJsonTokenizer *self, wchar_t character);
static int feedNumberInteger(QxJsonTokenizer *self, wchar_t character);
static int feedNumberDot(QxJsonTokenizer *self, wchar_t character);
static int feedNumberFrac(QxJsonTokenizer *self, wchar_t character);
static int feedNumberExp(QxJsonTokenizer *self, wchar_t character);
static int feedNumberExpSign(QxJsonTokenizer *self, wchar_t character);
static int feedNumberExpInteger(QxJsonTokenizer *self, wchar_t character);

static int endDefault(QxJsonTokenizer *self);
static int endUnexpected(QxJsonTokenizer *self);
static int endNumber(QxJsonTokenizer *self);

typedef struct Step
{
	int (*const feed)(QxJsonTokenizer *self, wchar_t character);
	int (*const end)(QxJsonTokenizer *self);
} Step;

static Step const stepDefault = { &feedDefault, &endDefault };
static Step const stepString = { &feedString, &endUnexpected };
static Step const stepStringEscape = { &feedStringEscape, endUnexpected };
static Step const stepStringUnicode  = { &feedStringUnicode  , endUnexpected  };
static Step const stepStringUnicode0 = { &feedStringUnicode0 , &endUnexpected };
static Step const stepStringUnicode1 = { &feedStringUnicode1 , &endUnexpected };
static Step const stepStringUnicode2 = { &feedStringUnicode2 , &endUnexpected };
static Step const stepF    = { &feedF    , &endUnexpected };
static Step const stepFa   = { &feedFa   , &endUnexpected };
static Step const stepFal  = { &feedFal  , &endUnexpected };
static Step const stepFals = { &feedFals , &endUnexpected };
static Step const stepN   = { &feedN   , &endUnexpected };
static Step const stepNu  = { &feedNu  , &endUnexpected };
static Step const stepNul = { &feedNul , &endUnexpected };
static Step const stepT   = { &feedT   , &endUnexpected };
static Step const stepTr  = { &feedTr  , &endUnexpected };
static Step const stepTru = { &feedTru , &endUnexpected };
static Step const stepNumberMinus = { &feedNumberMinus , &endUnexpected };
static Step const stepNumberZero = { &feedNumberZero , endNumber };
static Step const stepNumberInteger = { &feedNumberInteger , &endNumber };
static Step const stepNumberDot = { &feedNumberDot , &endUnexpected };
static Step const stepNumberFrac = { &feedNumberFrac , &endNumber };
static Step const stepNumberExp = { &feedNumberExp , &endUnexpected };
static Step const stepNumberExpSign = { &feedNumberExpSign , &endUnexpected };
static Step const stepNumberExpInteger = { &feedNumberExpInteger, &endNumber };

static int Tokenizer_wcharToBuffer(QxJsonTokenizer *self, wchar_t character);

static int Tokenizer_raiseToken(QxJsonTokenizer *self, QxJsonTokenType type);

/* Public implementations */

struct QxJsonTokenizer
{
	QxJsonTokenizerHandler handler;
	void *userData;

	/* Working variable */
	Step const *step;

	/* Buffer */
	wchar_t *bufferData;
	size_t bufferSize;
	size_t bufferAlloc;
};

QxJsonTokenizer *QxJsonTokenizer_new(QxJsonTokenizerHandler handler, void *userData)
{
	QxJsonTokenizer *instance;
	assert(handler != NULL);

	instance = (QxJsonTokenizer *)malloc(sizeof(QxJsonTokenizer));

	if (instance)
	{
		memset(instance, 0, sizeof(QxJsonTokenizer));
		instance->handler = handler;
		instance->userData = userData;
		instance->step = &stepDefault;
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
		error = self->step->feed(self, *data);

	return error;
}

int QxJsonTokenizer_end(QxJsonTokenizer *self)
{
	return self->step->end(self);
}

int qxJsonTokenize(wchar_t const *data, size_t size,
	QxJsonTokenizerHandler handler, void *userData)
{
	QxJsonTokenizer *instance;
	int error = -1;

	instance = QxJsonTokenizer_new(handler, userData);

	if (instance)
	{
		error = QxJsonTokenizer_feed(instance, data, size);

		if (!error)
			error = QxJsonTokenizer_end(instance);

		QxJsonTokenizer_delete(instance);
	}

	return error;
}

/* Private implementations */

static int feedDefault(QxJsonTokenizer *self, wchar_t character)
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
		self->step = &stepString;
		self->bufferSize = 0;
		return 0;

	case L',':
		return Tokenizer_raiseToken(self, QxJsonTokenValuesSeparator);

	case L'-':
		self->step = &stepNumberMinus;
		self->bufferSize = 0;
		return Tokenizer_wcharToBuffer(self, L'-');

	case L'0':
		self->step = &stepNumberZero;
		self->bufferSize = 0;
		return Tokenizer_wcharToBuffer(self, L'0');

	case L':':
		return Tokenizer_raiseToken(self, QxJsonTokenNameValueSeparator);

	case L'[':
		return Tokenizer_raiseToken(self, QxJsonTokenBeginArray);

	case L']':
		return Tokenizer_raiseToken(self, QxJsonTokenEndArray);

	case L'f':
		self->step = &stepF;
		return 0;

	case L'n':
		self->step = &stepN;
		return 0;

	case L't':
		self->step = &stepT;
		return 0;

	case L'{':
		return Tokenizer_raiseToken(self, QxJsonTokenBeginObject);

	case L'}':
		return Tokenizer_raiseToken(self, QxJsonTokenEndObject);

	default:

		if (WITHIN_1_9(character))
		{
			self->step = &stepNumberInteger;
			return Tokenizer_wcharToBuffer(self, character);
		}
	}

	/* Unexpected character */
	return -1;
}

static int feedString(QxJsonTokenizer *self, wchar_t character)
{
	switch (character)
	{
	case L'"': /* End of the string */
		return Tokenizer_raiseToken(self, QxJsonTokenString);

	case L'\\': /* Escaped sequence */
		self->step = &stepStringEscape;
		break;

	default:
		Tokenizer_wcharToBuffer(self, character);
	}

	return 0;
}

static int feedStringEscape(QxJsonTokenizer *self, wchar_t character)
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
		self->step = &stepStringUnicode;
		return 0;
	}

	translationOffset = translation;

	while (*translationOffset && (character != *translationOffset))
	{
		translationOffset += 2;
	}

	if (*translationOffset)
	{
		self->step = &stepString;
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

static int feedStringUnicode(QxJsonTokenizer *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepStringUnicode0;
	return Tokenizer_wcharToBuffer(self, value << 24);
}

static int feedStringUnicode0(QxJsonTokenizer *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepStringUnicode1;
	self->bufferData[self->bufferSize] |= value << 16;
	return 0;
}

static int feedStringUnicode1(QxJsonTokenizer *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepStringUnicode2;
	self->bufferData[self->bufferSize] |= value << 8;
	return 0;
}

static int feedStringUnicode2(QxJsonTokenizer *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepString;
	self->bufferData[self->bufferSize] |= value;
	return 0;
}

static int feedF(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'a')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepFa;
	return 0;
}

static int feedFa(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepFal;
	return 0;
}

static int feedFal(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L's')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepFals;
	return 0;
}

static int feedFals(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'e')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepDefault;
	return Tokenizer_raiseToken(self, QxJsonTokenFalse);
}

static int feedN(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'u')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepNu;
	return 0;
}

static int feedNu(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepNul;
	return 0;
}

static int feedNul(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	return Tokenizer_raiseToken(self, QxJsonTokenNull);
}

static int feedT(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'r')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepTr;
	return 0;
}

static int feedTr(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'u')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepTru;
	return 0;
}

static int feedTru(QxJsonTokenizer *self, wchar_t character)
{
	if (character != L'e')
	{
		/* Unexpected character */
		return -1;
	}

	return Tokenizer_raiseToken(self, QxJsonTokenTrue);
}

static int feedNumberMinus(QxJsonTokenizer *self, wchar_t character)
{
	if (character == L'0')
	{
		Tokenizer_wcharToBuffer(self, L'0');
		self->step = &stepNumberZero;
		return 0;
	}

	if (WITHIN_1_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->step = &stepNumberInteger;
		return 0;
	}

	/* Unexpected character */
	return -1;
}

static int feedNumberZero(QxJsonTokenizer *self, wchar_t character)
{
	int error;

	if (character == L'.')
	{
		Tokenizer_wcharToBuffer(self, L'.');
		self->step = &stepNumberDot;
		return 0;
	}

	if ((character == L'e') || (character == L'E'))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->step = &stepNumberExp;
		return 0;
	}

	/* Maybe a new token */
	error = endNumber(self);
	assert(self->step == &stepDefault);
	return error ? error : feedDefault(self, character);
}

static int feedNumberInteger(QxJsonTokenizer *self, wchar_t character)
{
	if (WITHIN_0_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		return 0;
	}

	/* Same expectations that after 0 */
	return feedNumberZero(self, character);
}

static int feedNumberDot(QxJsonTokenizer *self, wchar_t character)
{
	if (WITHIN_0_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->step = &stepNumberFrac;
		return 0;
	}

	/* Unexpected character */
	return -1;
}

static int feedNumberFrac(QxJsonTokenizer *self, wchar_t character)
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
		self->step = &stepNumberExp;
		return 0;
	}

	/* Maybe a new token */
	error = endNumber(self);
	assert(self->step == &stepDefault);
	return error ? error : feedDefault(self, character);
}

static int feedNumberExp(QxJsonTokenizer *self, wchar_t character)
{
	if ((character == L'-') || (character == L'+'))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->step = &stepNumberExpSign;
		return 0;
	}

	/* Same expectation as exponent sign */
	return feedNumberExpSign(self, character);
}

static int feedNumberExpSign(QxJsonTokenizer *self, wchar_t character)
{
	if (WITHIN_0_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->step = &stepNumberExpInteger;
		return 0;
	}

	/* Unexpected character */
	return -1;
}

static int feedNumberExpInteger(QxJsonTokenizer *self, wchar_t character)
{
	int error;

	if (WITHIN_0_9(character))
	{
		Tokenizer_wcharToBuffer(self, character);
		self->step = &stepNumberExpInteger;
		return 0;
	}

	/* Maybe a new token */
	error = endNumber(self);
	assert(self->step == &stepDefault);
	return error ? error : feedDefault(self, character);
}

static int endDefault(QxJsonTokenizer *self)
{
	QX_UNUSED(self);
	return 0;
}

static int endUnexpected(QxJsonTokenizer *self)
{
	QX_UNUSED(self);
	return -1;
}

static int endNumber(QxJsonTokenizer *self)
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
	int error;

	self->step = &stepDefault;

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

		return self->handler(&token, self->userData);
	}

	self->bufferSize = 0;
	return 0;
}
