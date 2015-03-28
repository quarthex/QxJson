/**
 * @file parser.c
 * @brief Source file of the QxJsonParser class.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "../include/qx.json.parser.h"

/* Private structure */

typedef struct TokenStep
{
	int (*const feedChar)(QxJsonParser *self, wchar_t character);
	int (*const endOfStream)(QxJsonParser *self);
} TokenStep;

typedef enum QxJsonTokenType
{
	QxJsonTokenString,
	QxJsonTokenNumber,
	QxJsonTokenFalse,
	QxJsonTokenTrue,
	QxJsonTokenNull,
	QxJsonTokenBeginArray,
	QxJsonTokenValuesSeparator,
	QxJsonTokenEndArray,
	QxJsonTokenBeginObject,
	QxJsonTokenNameValueSeparator,
	QxJsonTokenEndObject
} QxJsonTokenType;

typedef struct SyntaxStep
{
	int(*feed)(QxJsonParser *self);
	int(*canFeed)(QxJsonTokenType type);
} SyntaxStep;

typedef struct StackValue
{
	QxJsonValue *value;
	struct StackValue *next;
} StackValue;

#define StackValue_alloc() ((StackValue *)malloc(sizeof(StackValue)))

/* Private functions */

#define IN_RANGE(value, min, max) (((value) >= (min)) && ((value) <= (max)))
#define WITHIN_0_9(value)  IN_RANGE((value), L'0', L'9')
#define WITHIN_1_9(value)  IN_RANGE((value), L'1', L'9')
#define WITHIN_a_f(value)  IN_RANGE((value), L'a', L'f')
#define WITHIN_A_F(value)  IN_RANGE((value), L'A', L'F')

static int feedDefault(QxJsonParser *self, wchar_t character);
static int feedString(QxJsonParser *self, wchar_t character);
static int feedStringEscape(QxJsonParser *self, wchar_t character);
static int feedStringUnicode(QxJsonParser *self, wchar_t character);
static int feedStringUnicode0(QxJsonParser *self, wchar_t character);
static int feedStringUnicode1(QxJsonParser *self, wchar_t character);
static int feedStringUnicode2(QxJsonParser *self, wchar_t character);
static int feedF(QxJsonParser *self, wchar_t character);
static int feedFa(QxJsonParser *self, wchar_t character);
static int feedFal(QxJsonParser *self, wchar_t character);
static int feedFals(QxJsonParser *self, wchar_t character);
static int feedN(QxJsonParser *self, wchar_t character);
static int feedNu(QxJsonParser *self, wchar_t character);
static int feedNul(QxJsonParser *self, wchar_t character);
static int feedT(QxJsonParser *self, wchar_t character);
static int feedTr(QxJsonParser *self, wchar_t character);
static int feedTru(QxJsonParser *self, wchar_t character);
static int feedNumberMinus(QxJsonParser *self, wchar_t character);
static int feedNumberZero(QxJsonParser *self, wchar_t character);
static int feedNumberInteger(QxJsonParser *self, wchar_t character);
static int feedNumberDot(QxJsonParser *self, wchar_t character);
static int feedNumberFrac(QxJsonParser *self, wchar_t character);
static int feedNumberExp(QxJsonParser *self, wchar_t character);
static int feedNumberExpSign(QxJsonParser *self, wchar_t character);
static int feedNumberExpInteger(QxJsonParser *self, wchar_t character);

static int endDefault(QxJsonParser *self);
static int endUnexpected(QxJsonParser *self);
static int endNumber(QxJsonParser *self);

static int wcharToBuffer(QxJsonParser *self, wchar_t character);
static int raiseToken(QxJsonParser *self, QxJsonTokenType type);

static int feedAfterVoid(QxJsonParser *self);
static int feedAfterValue(QxJsonParser *self);
static int feedAfterArrayBegin(QxJsonParser *self);
static int feedAfterArrayValue(QxJsonParser *self);
static int feedAfterArrayComma(QxJsonParser *self);
static int feedAfterObjectBegin(QxJsonParser *self);
static int feedAfterObjectKey(QxJsonParser *self);
static int feedAfterObjectColon(QxJsonParser *self);
static int feedAfterObjectValue(QxJsonParser *self);
static int feedAfterObjectComma(QxJsonParser *self);
static int canFeedTokenAfterVoid(QxJsonTokenType type);
static int canFeedTokenAfterValue(QxJsonTokenType type);
static int canFeedTokenAfterArrayBegin(QxJsonTokenType type);
static int canFeedTokenAfterArrayValue(QxJsonTokenType type);
static int canFeedTokenAfterArrayComma(QxJsonTokenType type);
static int canFeedTokenAfterObjectBegin(QxJsonTokenType type);
static int canFeedTokenAfterObjectKey(QxJsonTokenType type);
static int canFeedTokenAfterObjectColon(QxJsonTokenType type);
static int canFeedTokenAfterObjectValue(QxJsonTokenType type);
static int canFeedTokenAfterObjectComma(QxJsonTokenType type);
static void popStackItem(QxJsonParser *self);
static QxJsonValue *createValueFromToken(QxJsonParser *self);

/* Private constants */

#define Yes  1
#define No   0

static TokenStep const stepDefault = { &feedDefault, &endDefault };
static TokenStep const stepString = { &feedString, &endUnexpected };
static TokenStep const stepStringEscape = { &feedStringEscape, endUnexpected };
static TokenStep const stepStringUnicode  = { &feedStringUnicode  , endUnexpected  };
static TokenStep const stepStringUnicode0 = { &feedStringUnicode0 , &endUnexpected };
static TokenStep const stepStringUnicode1 = { &feedStringUnicode1 , &endUnexpected };
static TokenStep const stepStringUnicode2 = { &feedStringUnicode2 , &endUnexpected };
static TokenStep const stepF    = { &feedF    , &endUnexpected };
static TokenStep const stepFa   = { &feedFa   , &endUnexpected };
static TokenStep const stepFal  = { &feedFal  , &endUnexpected };
static TokenStep const stepFals = { &feedFals , &endUnexpected };
static TokenStep const stepN   = { &feedN   , &endUnexpected };
static TokenStep const stepNu  = { &feedNu  , &endUnexpected };
static TokenStep const stepNul = { &feedNul , &endUnexpected };
static TokenStep const stepT   = { &feedT   , &endUnexpected };
static TokenStep const stepTr  = { &feedTr  , &endUnexpected };
static TokenStep const stepTru = { &feedTru , &endUnexpected };
static TokenStep const stepNumberMinus = { &feedNumberMinus , &endUnexpected };
static TokenStep const stepNumberZero = { &feedNumberZero , endNumber };
static TokenStep const stepNumberInteger = { &feedNumberInteger , &endNumber };
static TokenStep const stepNumberDot = { &feedNumberDot , &endUnexpected };
static TokenStep const stepNumberFrac = { &feedNumberFrac , &endNumber };
static TokenStep const stepNumberExp = { &feedNumberExp , &endUnexpected };
static TokenStep const stepNumberExpSign = { &feedNumberExpSign , &endUnexpected };
static TokenStep const stepNumberExpInteger = { &feedNumberExpInteger, &endNumber };

static SyntaxStep const stepVoid = { &feedAfterVoid, &canFeedTokenAfterVoid };
static SyntaxStep const stepValue = { &feedAfterValue, &canFeedTokenAfterValue };
static SyntaxStep const stepArrayBegin = { &feedAfterArrayBegin, &canFeedTokenAfterArrayBegin };
static SyntaxStep const stepArrayValue = { &feedAfterArrayValue, &canFeedTokenAfterArrayValue };
static SyntaxStep const stepArrayComma = { &feedAfterArrayComma, &canFeedTokenAfterArrayComma };
static SyntaxStep const stepObjectBegin = { &feedAfterObjectBegin, &canFeedTokenAfterObjectBegin };
static SyntaxStep const stepObjectKey = { &feedAfterObjectKey, &canFeedTokenAfterObjectKey };
static SyntaxStep const stepObjectColon = { &feedAfterObjectColon, &canFeedTokenAfterObjectColon };
static SyntaxStep const stepObjectValue = { &feedAfterObjectValue, &canFeedTokenAfterObjectValue };
static SyntaxStep const stepObjectComma = { &feedAfterObjectComma, &canFeedTokenAfterObjectComma };

/* Public implementations */

struct QxJsonParser
{
	/* Token level */
	TokenStep const *tokenStep;
	QxJsonTokenType tokenType;
	wchar_t *bufferData;
	size_t bufferSize;
	size_t bufferAlloc;

	/* Syntax level */
	QxJsonValue *key;
	SyntaxStep const *syntaxStep;
	StackValue head;
};

QxJsonParser *QxJsonParser_new(void)
{
	QxJsonParser *instance;

	instance = (QxJsonParser *)malloc(sizeof(QxJsonParser));

	if (instance)
	{
		memset(instance, 0, sizeof(QxJsonParser));
		instance->tokenStep = &stepDefault;
		instance->syntaxStep = &stepVoid;
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

		if (self->bufferData)
		{
			assert(self->bufferAlloc > 0);
			free(self->bufferData);
		}

		free(self);
	}
}

int QxJsonParser_feed(QxJsonParser *self, wchar_t const *data, size_t size)
{
	int error = 0;

	if (!self || !data)
		/* Invalid argument */
		return -1;

	for (; size && !error; ++data, --size)
		error = self->tokenStep->feedChar(self, *data);

	return error;
}

int QxJsonParser_end(QxJsonParser *self, QxJsonValue **value)
{
	int error;

	if (!self || !value)
		/* Invalid arguments */
		return -1;

	error = self->tokenStep->endOfStream(self);

	if (error != 0)
	{
		/* Tokenizing error */
		return error;
	}

	if (!self->head.value || self->head.next)
	{
		/* Value is not ready */
		return -1;
	}

	*value = self->head.value;
	self->head.value = NULL;
	self->syntaxStep = &stepVoid;
	return 0;
}

/* Private implementations */

static int feedAfterVoid(QxJsonParser *self)
{
	self->head.value = createValueFromToken(self);

	if (!self->head.value)
		/* Unexpected token */
		return -1;

	if (self->syntaxStep == &stepVoid)
		self->syntaxStep = &stepValue;

	return 0;
}

static int feedAfterValue(QxJsonParser *self)
{
	(void)self;
	/* A root value is ready.
	 * Get it using QxJsonParser_end().
	 */
	return -1;
}

static int feedAfterArrayBegin(QxJsonParser *self)
{
	QxJsonValue *array;
	QxJsonValue *value;

	switch (self->tokenType)
	{
	case QxJsonTokenEndArray:
		popStackItem(self);
		break;

	default:
		array = self->head.next->value;
		assert(QX_JSON_IS_ARRAY(array));
		value = createValueFromToken(self);

		if (!value)
			/* Unexpected token */
			return -1;

		if (QxJsonValue_arrayAppendNew(array, value) != 0)
			/* Failed to append a value to the array */
			return -1;

		switch (QxJsonValue_type(value))
		{
		case QxJsonValueTypeArray:
		case QxJsonValueTypeObject:
			break;

		default:
			self->syntaxStep = &stepArrayValue;
		}

		break;
	}

	return 0;
}

static int feedAfterArrayValue(QxJsonParser *self)
{
	switch (self->tokenType)
	{
	case QxJsonTokenEndArray:
		popStackItem(self);
		break;

	case QxJsonTokenValuesSeparator:
		self->syntaxStep = &stepArrayComma;
		break;

	default:
		/* Unexpected token */
		return -1;
	}

	return 0;
}

static int feedAfterArrayComma(QxJsonParser *self)
{
	QxJsonValue *array = self->head.next->value;
	QxJsonValue *const value = createValueFromToken(self);

	if (!value)
		/* Unexpected token */
		return -1;

	assert(QX_JSON_IS_ARRAY(array));

	if (QxJsonValue_arrayAppendNew(array, value) != 0)
		/* Failed to append a value to the array */
		return -1;

	switch (QxJsonValue_type(value))
	{
	case QxJsonValueTypeArray:
	case QxJsonValueTypeObject:
		break;

	default:
		self->syntaxStep = &stepArrayValue;
	}

	return 0;
}

static int feedAfterObjectBegin(QxJsonParser *self)
{
	switch (self->tokenType)
	{
	case QxJsonTokenString:
		self->key = QxJsonValue_stringNew(self->bufferData, self->bufferSize);

		if (!self->key)
			/* Failed to create string value */
			return -1;

		self->syntaxStep = &stepObjectKey;
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

static int feedAfterObjectKey(QxJsonParser *self)
{
	if (self->tokenType != QxJsonTokenNameValueSeparator)
	{
		/* Unexpected token */
		return -1;
	}

	self->syntaxStep = &stepObjectColon;
	return 0;
}

static int feedAfterObjectColon(QxJsonParser *self)
{
	QxJsonValue *object, *value;

	object = self->head.next->value;
	value = createValueFromToken(self);

	if (!value)
		/* Unexpected token */
		return -1;

	assert(self->key != NULL);

	if (QxJsonValue_objectSet(object, self->key, value) != 0)
		/* Failed to add the new key/value pair */
		return -1;

	QxJsonValue_release(self->key);
	self->key = NULL;
	QxJsonValue_release(value);

	if (self->syntaxStep == &stepObjectColon)
		self->syntaxStep = &stepObjectValue;

	return 0;
}

static int feedAfterObjectValue(QxJsonParser *self)
{
	switch (self->tokenType)
	{
	case QxJsonTokenValuesSeparator:
		self->syntaxStep = &stepObjectComma;
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

static int feedAfterObjectComma(QxJsonParser *self)
{
	if (self->tokenType != QxJsonTokenString)
		/* Unexpected token */
		return -1;

	assert(self->key == NULL);
	self->key = QxJsonValue_stringNew(self->bufferData, self->bufferSize);

	if (!self->key)
		/* Failed to create the string */
		return -1;

	self->syntaxStep = &stepObjectKey;
	return 0;
}

static int canFeedTokenAfterVoid(QxJsonTokenType type)
{
	switch (type)
	{
	case QxJsonTokenValuesSeparator:
	case QxJsonTokenEndArray:
	case QxJsonTokenNameValueSeparator:
	case QxJsonTokenEndObject:
		return No;

	default:
		return Yes;
	}
}

static int canFeedTokenAfterValue(QxJsonTokenType type)
{
	switch (type)
	{
	case QxJsonTokenValuesSeparator:
	case QxJsonTokenEndArray:
	case QxJsonTokenNameValueSeparator:
	case QxJsonTokenEndObject:
		return Yes;

	default:
		return No;
	}
}

static int canFeedTokenAfterArrayBegin(QxJsonTokenType type)
{
	switch (type)
	{
	case QxJsonTokenValuesSeparator:
	case QxJsonTokenNameValueSeparator:
	case QxJsonTokenEndObject:
		return No;

	default:
		return Yes;
	}
}

static int canFeedTokenAfterArrayValue(QxJsonTokenType type)
{
	switch (type)
	{
	case QxJsonTokenValuesSeparator:
	case QxJsonTokenEndArray:
		return Yes;

	default:
		return No;
	}

}

static int canFeedTokenAfterArrayComma(QxJsonTokenType type)
{
	switch (type)
	{
	case QxJsonTokenValuesSeparator:
	case QxJsonTokenEndArray:
	case QxJsonTokenNameValueSeparator:
	case QxJsonTokenEndObject:
		return No;

	default:
		return Yes;
	}
}

static int canFeedTokenAfterObjectBegin(QxJsonTokenType type)
{
	switch (type)
	{
	case QxJsonTokenString:
	case QxJsonTokenEndObject:
		return Yes;

	default:
		return No;
	}
}

static int canFeedTokenAfterObjectKey(QxJsonTokenType type)
{
	return type == QxJsonTokenNameValueSeparator;
}

static int canFeedTokenAfterObjectColon(QxJsonTokenType type)
{
	switch (type)
	{
	case QxJsonTokenValuesSeparator:
	case QxJsonTokenEndArray:
	case QxJsonTokenNameValueSeparator:
	case QxJsonTokenEndObject:
		return No;

	default:
		return Yes;
	}

}

static int canFeedTokenAfterObjectValue(QxJsonTokenType type)
{
	switch (type)
	{
	case QxJsonTokenValuesSeparator:
	case QxJsonTokenEndObject:
		return Yes;

	default:
		return No;
	}
}

static int canFeedTokenAfterObjectComma(QxJsonTokenType type)
{
	return type == QxJsonTokenString;
}

static void popStackItem(QxJsonParser *self)
{
	StackValue *item = self->head.next;
	self->head.next = item->next;
	free(item);

	if (self->head.next)
	{
		if (QX_JSON_IS_ARRAY(self->head.next->value))
		{
			self->syntaxStep = &stepArrayValue;
		}
		else
		{
			assert(QX_JSON_IS_OBJECT(self->head.next->value));
			self->syntaxStep = &stepObjectValue;
		}
	}
	else
	{
		self->syntaxStep = &stepValue;
	}
}

static QxJsonValue *createValueFromToken(QxJsonParser *self)
{
	StackValue *item = NULL;
	double number;
	wchar_t *endptr;

	switch (self->tokenType)
	{
	case QxJsonTokenString:
		return QxJsonValue_stringNew(self->bufferData, self->bufferSize);

	case QxJsonTokenNumber:
		endptr = NULL;
		number = wcstod(self->bufferData, &endptr);

		if (endptr == self->bufferData + self->bufferSize)
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
				self->syntaxStep = &stepArrayBegin;
		}

		/* Allocation error */
		break;

	case QxJsonTokenBeginObject:

		item = StackValue_alloc();

		if (item)
		{
			item->value = QxJsonValue_objectNew();

			if (item->value)
				self->syntaxStep = &stepObjectBegin;
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

static int feedDefault(QxJsonParser *self, wchar_t character)
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
		if (!self->syntaxStep->canFeed(QxJsonTokenString))
			return -1;

		self->tokenStep = &stepString;
		self->bufferSize = 0;
		return 0;

	case L',':
		if (!self->syntaxStep->canFeed(QxJsonTokenValuesSeparator))
			return -1;

		return raiseToken(self, QxJsonTokenValuesSeparator);

	case L'-':
		if (!self->syntaxStep->canFeed(QxJsonTokenNumber))
			return -1;

		self->tokenStep = &stepNumberMinus;
		self->bufferSize = 0;
		return wcharToBuffer(self, L'-');

	case L'0':
		if (!self->syntaxStep->canFeed(QxJsonTokenNumber))
			return -1;

		self->tokenStep = &stepNumberZero;
		self->bufferSize = 0;
		return wcharToBuffer(self, L'0');

	case L':':
		if (!self->syntaxStep->canFeed(QxJsonTokenNameValueSeparator))
			return -1;

		return raiseToken(self, QxJsonTokenNameValueSeparator);

	case L'[':
		if (!self->syntaxStep->canFeed(QxJsonTokenBeginArray))
			return -1;

		return raiseToken(self, QxJsonTokenBeginArray);

	case L']':
		if (!self->syntaxStep->canFeed(QxJsonTokenEndArray))
			return -1;

		return raiseToken(self, QxJsonTokenEndArray);

	case L'f':
		if (!self->syntaxStep->canFeed(QxJsonTokenFalse))
			return -1;

		self->tokenStep = &stepF;
		return 0;

	case L'n':
		if (!self->syntaxStep->canFeed(QxJsonTokenNull))
			return -1;

		self->tokenStep = &stepN;
		return 0;

	case L't':
		if (!self->syntaxStep->canFeed(QxJsonTokenTrue))
			return -1;

		self->tokenStep = &stepT;
		return 0;

	case L'{':
		if (!self->syntaxStep->canFeed(QxJsonTokenBeginObject))
			return -1;

		return raiseToken(self, QxJsonTokenBeginObject);

	case L'}':
		if (!self->syntaxStep->canFeed(QxJsonTokenEndObject))
			return -1;

		return raiseToken(self, QxJsonTokenEndObject);

	default:

		if (WITHIN_1_9(character))
		{
			if (!self->syntaxStep->canFeed(QxJsonTokenNumber))
				return -1;

			self->tokenStep = &stepNumberInteger;
			self->bufferSize = 0;
			return wcharToBuffer(self, character);
		}
	}

	/* Unexpected character */
	return -1;
}

static int feedString(QxJsonParser *self, wchar_t character)
{
	switch (character)
	{
	case L'"': /* End of the string */
		return raiseToken(self, QxJsonTokenString);

	case L'\\': /* Escaped sequence */
		self->tokenStep = &stepStringEscape;
		break;

	default:
		wcharToBuffer(self, character);
	}

	return 0;
}

static int feedStringEscape(QxJsonParser *self, wchar_t character)
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
		self->tokenStep = &stepStringUnicode;
		return 0;
	}

	translationOffset = translation;

	while (*translationOffset && (character != *translationOffset))
	{
		translationOffset += 2;
	}

	if (*translationOffset)
	{
		self->tokenStep = &stepString;
		return wcharToBuffer(self, *(translationOffset + 1));
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

static int feedStringUnicode(QxJsonParser *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepStringUnicode0;
	return wcharToBuffer(self, value << 24);
}

static int feedStringUnicode0(QxJsonParser *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepStringUnicode1;
	self->bufferData[self->bufferSize] |= value << 16;
	return 0;
}

static int feedStringUnicode1(QxJsonParser *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepStringUnicode2;
	self->bufferData[self->bufferSize] |= value << 8;
	return 0;
}

static int feedStringUnicode2(QxJsonParser *self, wchar_t character)
{
	int const value = hexaDigitToValue(character);

	if (value < 0)
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepString;
	self->bufferData[self->bufferSize] |= value;
	return 0;
}

static int feedF(QxJsonParser *self, wchar_t character)
{
	if (character != L'a')
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepFa;
	return 0;
}

static int feedFa(QxJsonParser *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepFal;
	return 0;
}

static int feedFal(QxJsonParser *self, wchar_t character)
{
	if (character != L's')
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepFals;
	return 0;
}

static int feedFals(QxJsonParser *self, wchar_t character)
{
	if (character != L'e')
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepDefault;
	return raiseToken(self, QxJsonTokenFalse);
}

static int feedN(QxJsonParser *self, wchar_t character)
{
	if (character != L'u')
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepNu;
	return 0;
}

static int feedNu(QxJsonParser *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepNul;
	return 0;
}

static int feedNul(QxJsonParser *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	return raiseToken(self, QxJsonTokenNull);
}

static int feedT(QxJsonParser *self, wchar_t character)
{
	if (character != L'r')
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepTr;
	return 0;
}

static int feedTr(QxJsonParser *self, wchar_t character)
{
	if (character != L'u')
	{
		/* Unexpected character */
		return -1;
	}

	self->tokenStep = &stepTru;
	return 0;
}

static int feedTru(QxJsonParser *self, wchar_t character)
{
	if (character != L'e')
	{
		/* Unexpected character */
		return -1;
	}

	return raiseToken(self, QxJsonTokenTrue);
}

static int feedNumberMinus(QxJsonParser *self, wchar_t character)
{
	/* Unexpected character */
	int error = -1;

	if (WITHIN_0_9(character))
	{
		error = wcharToBuffer(self, character);
	}

	if (error == 0)
	{
		if (character == L'0')
		{
			self->tokenStep = &stepNumberZero;
		}
		else
		{
			self->tokenStep = &stepNumberInteger;
		}
	}

	return error;
}

static int feedNumberZero(QxJsonParser *self, wchar_t character)
{
	int error;

	if (character == L'.')
	{
		wcharToBuffer(self, L'.');
		self->tokenStep = &stepNumberDot;
		return 0;
	}

	if ((character == L'e') || (character == L'E'))
	{
		wcharToBuffer(self, character);
		self->tokenStep = &stepNumberExp;
		return 0;
	}

	/* Maybe a new token */
	error = endNumber(self);
	assert(self->tokenStep == &stepDefault);
	return error ? error : feedDefault(self, character);
}

static int feedNumberInteger(QxJsonParser *self, wchar_t character)
{
	if (WITHIN_0_9(character))
	{
		wcharToBuffer(self, character);
		return 0;
	}

	/* Same expectations that after 0 */
	return feedNumberZero(self, character);
}

static int feedNumberDot(QxJsonParser *self, wchar_t character)
{
	if (WITHIN_0_9(character))
	{
		wcharToBuffer(self, character);
		self->tokenStep = &stepNumberFrac;
		return 0;
	}

	/* Unexpected character */
	return -1;
}

static int feedNumberFrac(QxJsonParser *self, wchar_t character)
{
	int error;

	if (WITHIN_0_9(character))
	{
		wcharToBuffer(self, character);
		return 0;
	}

	if ((character == L'e') || (character == L'E'))
	{
		wcharToBuffer(self, character);
		self->tokenStep = &stepNumberExp;
		return 0;
	}

	/* Maybe a new token */
	error = endNumber(self);
	assert(self->tokenStep == &stepDefault);
	return error ? error : feedDefault(self, character);
}

static int feedNumberExp(QxJsonParser *self, wchar_t character)
{
	if ((character == L'-') || (character == L'+'))
	{
		wcharToBuffer(self, character);
		self->tokenStep = &stepNumberExpSign;
		return 0;
	}

	/* Same expectation as exponent sign */
	return feedNumberExpSign(self, character);
}

static int feedNumberExpSign(QxJsonParser *self, wchar_t character)
{
	if (WITHIN_0_9(character))
	{
		wcharToBuffer(self, character);
		self->tokenStep = &stepNumberExpInteger;
		return 0;
	}

	/* Unexpected character */
	return -1;
}

static int feedNumberExpInteger(QxJsonParser *self, wchar_t character)
{
	int error;

	if (WITHIN_0_9(character))
	{
		wcharToBuffer(self, character);
		self->tokenStep = &stepNumberExpInteger;
		return 0;
	}

	/* Maybe a new token */
	error = endNumber(self);
	assert(self->tokenStep == &stepDefault);
	return error ? error : feedDefault(self, character);
}

static int endDefault(QxJsonParser *self)
{
	(void)self;
	return 0;
}

static int endUnexpected(QxJsonParser *self)
{
	(void)self;
	return -1;
}

static int endNumber(QxJsonParser *self)
{
	return raiseToken(self, QxJsonTokenNumber);
}

static int wcharToBuffer(QxJsonParser *self, wchar_t character)
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

static int raiseToken(QxJsonParser *self, QxJsonTokenType type)
{
	int error;

	self->tokenStep = &stepDefault;
	self->tokenType = type;

	if (self->bufferSize)
	{
		/* Add a trailing nul character */
		error = wcharToBuffer(self, L'\0');

		if (error)
		{
			return error;
		}

		/* minus the trailing nul character */
		--self->bufferSize;
	}

	return (*self->syntaxStep->feed)(self);
}
