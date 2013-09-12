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

typedef struct Step
{
	int (*const feed)(QxJsonParser *self, wchar_t character);
	int (*const end)(QxJsonParser *self);
} Step;

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
static void popStackItem(QxJsonParser *self);
static QxJsonValue *createValueFromToken(QxJsonParser *self);

/* Private constants */

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

/* Public implementations */

typedef struct StackValue
{
	QxJsonValue *value;
	struct StackValue *next;
} StackValue;

#define StackValue_alloc() ((StackValue *)malloc(sizeof(StackValue)))

struct QxJsonParser
{
	/* Token level */
	Step const *step;
	QxJsonTokenType type;
	wchar_t *bufferData;
	size_t bufferSize;
	size_t bufferAlloc;

	/* Syntax level */
	void *key;
	int(*feed)(QxJsonParser *self);
	StackValue head;
};

QxJsonParser *QxJsonParser_new(void)
{
	QxJsonParser *instance;

	instance = (QxJsonParser *)malloc(sizeof(QxJsonParser));

	if (instance)
	{
		memset(instance, 0, sizeof(QxJsonParser));
		instance->step = &stepDefault;
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
		error = self->step->feed(self, *data);

	return error;
}

int QxJsonParser_end(QxJsonParser *self, QxJsonValue **value)
{
	int error;

	if (!self || !value)
		/* Invalid arguments */
		return -1;

	error = self->step->end(self);

	if (error != 0)
	{
		/* Tokenizing error */
		return error;
	}

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

static int feedAfterVoid(QxJsonParser *self)
{
	self->head.value = createValueFromToken(self);

	if (!self->head.value)
		return -1;

	if (self->feed == &feedAfterVoid)
		self->feed = &feedAfterValue;

	return 0;
}

static int feedAfterValue(QxJsonParser *self)
{
	(void)self;
	return -1;
}

static int feedAfterArrayBegin(QxJsonParser *self)
{
	QxJsonValue *value;

	switch (self->type)
	{
	case QxJsonTokenEndArray:
		popStackItem(self);
		break;

	default:
		value = createValueFromToken(self);

		if (!value)
			/* Unexpected token */
			return -1;

		switch (QxJsonValue_type(value))
		{
		case QxJsonValueTypeArray:
		case QxJsonValueTypeObject:
			break;

		default:

			if (QxJsonValue_arrayAppendNew(self->head.next->value, value) != 0)
				/* Failed to append a value to the array */
				return -1;
		}

		self->feed = &feedAfterArrayValue;
		break;
	}

	return 0;
}

static int feedAfterArrayValue(QxJsonParser *self)
{
	switch (self->type)
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

static int feedAfterArrayComma(QxJsonParser *self)
{
	QxJsonValue *value;
	StackValue *head;

	head = self->head.next;
	value = createValueFromToken(self);

	if (!value)
		/* Unexpected token */
		return -1;

	if (QxJsonValue_arrayAppendNew(head->value, value) != 0)
		/* Failed to append a value to the array */
		return -1;

	self->feed = &feedAfterArrayValue;
	return 0;
}

static int feedAfterObjectBegin(QxJsonParser *self)
{
	switch (self->type)
	{
	case QxJsonTokenString:
		self->key = QxJsonValue_stringNew(self->bufferData, self->bufferSize);

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

static int feedAfterObjectKey(QxJsonParser *self)
{
	if (self->type != QxJsonTokenNameValueSeparator)
	{
		/* Unexpected token */
		return -1;
	}

	self->feed = &feedAfterObjectColon;
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

	if (self->feed == &feedAfterObjectColon)
		self->feed = &feedAfterObjectValue;

	return 0;
}

static int feedAfterObjectValue(QxJsonParser *self)
{
	switch (self->type)
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

static int feedAfterObjectComma(QxJsonParser *self)
{
	if (self->type != QxJsonTokenString)
		/* Unexpected token */
		return -1;

	assert(self->key == NULL);
	self->key = QxJsonValue_stringNew(self->bufferData, self->bufferSize);

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

static QxJsonValue *createValueFromToken(QxJsonParser *self)
{
	StackValue *item = NULL;
	double number;
	wchar_t *endptr;

	switch (self->type)
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
		self->step = &stepString;
		self->bufferSize = 0;
		return 0;

	case L',':
		return raiseToken(self, QxJsonTokenValuesSeparator);

	case L'-':
		self->step = &stepNumberMinus;
		self->bufferSize = 0;
		return wcharToBuffer(self, L'-');

	case L'0':
		self->step = &stepNumberZero;
		self->bufferSize = 0;
		return wcharToBuffer(self, L'0');

	case L':':
		return raiseToken(self, QxJsonTokenNameValueSeparator);

	case L'[':
		return raiseToken(self, QxJsonTokenBeginArray);

	case L']':
		return raiseToken(self, QxJsonTokenEndArray);

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
		return raiseToken(self, QxJsonTokenBeginObject);

	case L'}':
		return raiseToken(self, QxJsonTokenEndObject);

	default:

		if (WITHIN_1_9(character))
		{
			self->step = &stepNumberInteger;
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
		self->step = &stepStringEscape;
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

	self->step = &stepStringUnicode0;
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

	self->step = &stepStringUnicode1;
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

	self->step = &stepStringUnicode2;
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

	self->step = &stepString;
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

	self->step = &stepFa;
	return 0;
}

static int feedFa(QxJsonParser *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepFal;
	return 0;
}

static int feedFal(QxJsonParser *self, wchar_t character)
{
	if (character != L's')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepFals;
	return 0;
}

static int feedFals(QxJsonParser *self, wchar_t character)
{
	if (character != L'e')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepDefault;
	return raiseToken(self, QxJsonTokenFalse);
}

static int feedN(QxJsonParser *self, wchar_t character)
{
	if (character != L'u')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepNu;
	return 0;
}

static int feedNu(QxJsonParser *self, wchar_t character)
{
	if (character != L'l')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepNul;
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

	self->step = &stepTr;
	return 0;
}

static int feedTr(QxJsonParser *self, wchar_t character)
{
	if (character != L'u')
	{
		/* Unexpected character */
		return -1;
	}

	self->step = &stepTru;
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
	if (character == L'0')
	{
		wcharToBuffer(self, L'0');
		self->step = &stepNumberZero;
		return 0;
	}

	if (WITHIN_1_9(character))
	{
		wcharToBuffer(self, character);
		self->step = &stepNumberInteger;
		return 0;
	}

	/* Unexpected character */
	return -1;
}

static int feedNumberZero(QxJsonParser *self, wchar_t character)
{
	int error;

	if (character == L'.')
	{
		wcharToBuffer(self, L'.');
		self->step = &stepNumberDot;
		return 0;
	}

	if ((character == L'e') || (character == L'E'))
	{
		wcharToBuffer(self, character);
		self->step = &stepNumberExp;
		return 0;
	}

	/* Maybe a new token */
	error = endNumber(self);
	assert(self->step == &stepDefault);
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
		self->step = &stepNumberFrac;
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
		self->step = &stepNumberExp;
		return 0;
	}

	/* Maybe a new token */
	error = endNumber(self);
	assert(self->step == &stepDefault);
	return error ? error : feedDefault(self, character);
}

static int feedNumberExp(QxJsonParser *self, wchar_t character)
{
	if ((character == L'-') || (character == L'+'))
	{
		wcharToBuffer(self, character);
		self->step = &stepNumberExpSign;
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
		self->step = &stepNumberExpInteger;
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
		self->step = &stepNumberExpInteger;
		return 0;
	}

	/* Maybe a new token */
	error = endNumber(self);
	assert(self->step == &stepDefault);
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

	self->step = &stepDefault;
	self->type = type;

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

	return (*self->feed)(self);
}
