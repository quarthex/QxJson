/**
 * @file qx.test.tokenizer.c
 * @brief Testing source file of the QxJsonTokenizer class.
 * @author Romain DEOUX
 */

#include <wchar.h>
#include <qx.json.tokenizer.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define WCSLEN(str) (ARRAY_SIZE(str) - 1)

#include "assert.h"

typedef QxJsonTokenizer Tokenizer;
typedef QxJsonToken     Token;
typedef QxJsonTokenType TokenType;

typedef struct GenericTest GenericTest;
struct GenericTest
{
	size_t index;
	TokenType const *types;
	size_t typesCount;
	void (*overload)(Token const *token, int index);
};

static int testGenericCallback(Token const *token, void *userData)
{
	GenericTest *const gt = (GenericTest *)userData;

	if (gt->index == gt->typesCount)
	{
		QX_ASSERT(0 /* Unexpected token */);
	}
	else
	{
		QX_ASSERT(token->type == gt->types[gt->index]);

		if (gt->overload)
		{
			gt->overload(token, gt->index);
		}

		++gt->index;
	}

	return 0;
}

static void testGeneric(wchar_t const *data, TokenType const *types,
	size_t typesCount, void (*overload)(Token const *, int))
{
	Tokenizer *tokenizer;
	GenericTest gt;

	gt.index = 0;
	gt.types = types;
	gt.typesCount = typesCount;
	gt.overload = overload;

	tokenizer = QxJsonTokenizer_new();
	QX_ASSERT(tokenizer != NULL);
	QxJsonTokenizer_setHandler(tokenizer, testGenericCallback, &gt);
	QX_ASSERT(QxJsonTokenizer_write(tokenizer, data, wcslen(data)) == 0);
	QX_ASSERT(QxJsonTokenizer_flush(tokenizer) == 0);
	QxJsonTokenizer_delete(tokenizer);
	return;
}

static void testArray(void)
{
	TokenType const types[] = {
		QxJsonTokenBeginArray,
		QxJsonTokenValuesSeparator,
		QxJsonTokenEndArray
	};
	testGeneric(L"[ , ]", types, ARRAY_SIZE(types), NULL);
	return;
}

static void testObject(void)
{
	TokenType const types[] = {
		QxJsonTokenBeginObject,
		QxJsonTokenNameValueSeparator,
		QxJsonTokenValuesSeparator,
		QxJsonTokenNameValueSeparator,
		QxJsonTokenEndObject,
	};
	testGeneric(L"{ : , : }", types, ARRAY_SIZE(types), NULL);
	return;
}

static void testAtom(wchar_t const *text, TokenType expectedType)
{
	testGeneric(text, &expectedType, 1, NULL);
	return;
}

static void testNull(void)
{
	testAtom(L"null", QxJsonTokenNull);
	return;
}

static void testTrue(void)
{
	testAtom(L"true", QxJsonTokenTrue);
	return;
}

static void testFalse(void)
{
	testAtom(L"false", QxJsonTokenFalse);
	return;
}

static int testStringHandler(Token const *token, void *userData)
{
	int *const index = (int *)userData;

	QX_ASSERT(token->type == QxJsonTokenString);

	switch (*index)
	{
	case 0:
		QX_ASSERT(token->size == 0);
		break;

	default:
		QX_ASSERT(0 /* Unexpected token */);
		break;
	}

	return 0;
}

static void testString(void)
{
	Tokenizer *tokenizer = QxJsonTokenizer_new();
	int index = 0;

	QX_ASSERT(tokenizer != NULL);
	QxJsonTokenizer_setHandler(tokenizer, testStringHandler, &index);

	QX_ASSERT(QxJsonTokenizer_write(tokenizer, L"\"\" true", 2) == 0);
	QX_ASSERT(QxJsonTokenizer_flush(tokenizer) == 0);

	QxJsonTokenizer_delete(tokenizer);
	return;
}

int main(void)
{
	testArray();
	testObject();
	testNull();
	testTrue();
	testFalse();
	testString();
	return EXIT_SUCCESS;
}
