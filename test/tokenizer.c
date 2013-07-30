/**
 * @file tokenizer.c
 * @brief Testing source file of the QxJsonTokenizer class.
 * @author Romain DEOUX
 */

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <qx.json.tokenizer.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#include "expect.h"

struct Tester
{
	QxJsonTokenizerHandler tokenizerHandler;
	size_t index;
	size_t count;
	QxJsonToken const *tokens;
};
typedef struct Tester Tester;

int Tester_feed(QxJsonTokenizerHandler *self, QxJsonToken const *token)
{
	Tester *const tester = (Tester *)self;
	QxJsonToken const *pattern;

	expect_not_null(self);
	expect_not_null(token);
	expect_int_not_equal(tester->index, tester->count);

	pattern = &tester->tokens[tester->index];
	expect_int_equal(token->type, pattern->type);
	expect_int_equal(token->size, pattern->size);

	if (token->size)
	{
		expect_not_null(token->data);
		expect_wstr_equal(token->data, pattern->data);
		expect_zero(memcmp(token->data, pattern->data, token->size * sizeof(wchar_t)));
	}
	else
	{
		expect_null(token->data);
	}

	++tester->index;
	return 0;
}

static void testGeneric(wchar_t const *data,
	QxJsonToken const *tokens, size_t tokensCount)
{
	Tester tester;
	QxJsonTokenizer *tokenizer;

	tokenizer = QxJsonTokenizer_new(&tester.tokenizerHandler);
	expect_not_null(tokenizer);

	memset(&tester, 0, sizeof(tester));
	tester.tokenizerHandler.feed = &Tester_feed;
	tester.count = tokensCount;
	tester.tokens = tokens;
	expect_zero(QxJsonTokenizer_feed(tokenizer, data, wcslen(data)));
	expect_int_equal(tester.index, tester.count);

	QxJsonTokenizer_delete(tokenizer);
	return;
}

static void testArray(void)
{
	QxJsonToken const tokens[] = {
		{ QxJsonTokenBeginArray     , NULL, 0 },
		{ QxJsonTokenValuesSeparator, NULL, 0 },
		{ QxJsonTokenEndArray       , NULL, 0 }
	};
	testGeneric(L"[ , ]", tokens, ARRAY_SIZE(tokens));
	return;
}

static void testObject(void)
{
	QxJsonToken const tokens[] = {
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
	QxJsonToken const token = { QxJsonTokenNull, NULL, 0 };
	testGeneric(L"null", &token, 1);
	return;
}

static void testTrue(void)
{
	QxJsonToken const token = { QxJsonTokenTrue, NULL, 0 };
	testGeneric(L"true", &token, 1);
	return;
}

static void testFalse(void)
{
	QxJsonToken const token = { QxJsonTokenFalse, NULL, 0 };
	testGeneric(L"false", &token, 1);
	return;
}

static void testString(void)
{
	QxJsonToken const tokens[] = {
		{ QxJsonTokenString, L""          , 0 },
		{ QxJsonTokenString, L"QxJson"    , 6 },
		{ QxJsonTokenString, L"[\r,\n,\t]", 7 }
	};
	testGeneric(L"\"\"" L"\"QxJson\"" L"\"[\\r,\\n,\\t]\"",
		tokens, ARRAY_SIZE(tokens));
	return;
}

static void testNumber(void)
{
	QxJsonToken const tokens[] = {
		{ QxJsonTokenNumber, L"3.1415E-6", 9 }
	};
	testGeneric(L"3.1415E-6 ", tokens, ARRAY_SIZE(tokens));
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
	testNumber();
	return EXIT_SUCCESS;
}
