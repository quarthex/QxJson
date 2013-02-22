/**
 * @file qx.test.tokenizer.c
 * @brief Testing source file of the QxJsonTokenizer class.
 * @author Romain DEOUX
 */

#include <string.h>
#include <wchar.h>
#include <qx.json.tokenizer.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#include "assert.h"

typedef QxJsonTokenizer Tokenizer;
typedef QxJsonToken     Token;
typedef QxJsonTokenType TokenType;

typedef struct GenericTest GenericTest;
struct GenericTest
{
	size_t index;
	Token const *tokens;
	size_t tokensCount;
};

static int testGenericCallback(Token const *token, void *userData)
{
	GenericTest *const gt = (GenericTest *)userData;
	Token const *expected;

	if (gt->index == gt->tokensCount)
	{
		QX_ASSERT(0 /* Unexpected token */);
	}
	else
	{
		expected = &gt->tokens[gt->index];
		QX_ASSERT(token->type == expected->type);
		QX_ASSERT(token->size == expected->size);

		if (token->size)
		{
			QX_ASSERT(memcmp(token->data, expected->data,
				token->size * sizeof(wchar_t)) == 0);
		}

		++gt->index;
	}

	return 0;
}

static void testGeneric(wchar_t const *data,
	Token const *tokens, size_t tokensCount)
{
	Tokenizer *tokenizer;
	GenericTest gt;

	gt.index = 0;
	gt.tokens = tokens;
	gt.tokensCount = tokensCount;

	tokenizer = QxJsonTokenizer_new();
	QX_ASSERT(tokenizer != NULL);
	QxJsonTokenizer_setHandler(tokenizer, testGenericCallback, &gt);
	QX_ASSERT(QxJsonTokenizer_write(tokenizer, data, wcslen(data)) == 0);
	QX_ASSERT(QxJsonTokenizer_flush(tokenizer) == 0);
	QX_ASSERT(gt.index == tokensCount);
	QxJsonTokenizer_delete(tokenizer);
	return;
}

static void testArray(void)
{
	Token const tokens[] = {
		{ QxJsonTokenBeginArray     , NULL, 0 },
		{ QxJsonTokenValuesSeparator, NULL, 0 },
		{ QxJsonTokenEndArray       , NULL, 0 }
	};
	testGeneric(L"[ , ]", tokens, ARRAY_SIZE(tokens));
	return;
}

static void testObject(void)
{
	Token const tokens[] = {
		{ QxJsonTokenBeginObject       , NULL, 0 },
		{ QxJsonTokenNameValueSeparator, NULL, 0 },
		{ QxJsonTokenValuesSeparator   , NULL, 0 },
		{ QxJsonTokenNameValueSeparator, NULL, 0 },
		{ QxJsonTokenEndObject         , NULL, 0 }
	};
	testGeneric(L"{ : , : }", tokens, ARRAY_SIZE(tokens));
	return;
}

static void testNull(void)
{
	Token const token = { QxJsonTokenNull, NULL, 0 };
	testGeneric(L"null", &token, 1);
	return;
}

static void testTrue(void)
{
	Token const token = { QxJsonTokenTrue, NULL, 0 };
	testGeneric(L"true", &token, 1);
	return;
}

static void testFalse(void)
{
	Token const token = { QxJsonTokenFalse, NULL, 0 };
	testGeneric(L"false", &token, 1);
	return;
}

static void testString(void)
{
	Token const tokens[] = {
		{ QxJsonTokenString, L""      , 0 },
		{ QxJsonTokenString, L"QxJson", 6 }
	};
	testGeneric(L"\"\"" L"\"QxJson\"", tokens, ARRAY_SIZE(tokens));
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
