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
	size_t index;
	size_t count;
	QxJsonToken const *tokens;
};
typedef struct Tester Tester;

int Tester_feed(QxJsonToken const *token, void *userData)
{
	Tester *const self = (Tester *)userData;
	QxJsonToken const *pattern;

	expect_not_null(self);
	expect_not_null(token);
	expect_int_not_equal(self->index, self->count);

	pattern = &self->tokens[self->index];
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

	++self->index;
	return 0;
}

static void testGeneric(char const *data,
	QxJsonToken const *tokens, size_t tokensCount)
{
	Tester tester;
	QxJsonAsciiTokenizer *tokenizer;

	tokenizer = QxJsonAsciiTokenizer_new(&Tester_feed, &tester);
	expect_not_null(tokenizer);

	tester.index = 0;
	tester.count = tokensCount;
	tester.tokens = tokens;
	expect_zero(QxJsonAsciiTokenizer_feed(tokenizer, data, strlen(data)));
	expect_zero(QxJsonAsciiTokenizer_end(tokenizer));
	expect_int_equal(tester.index, tester.count);

	QxJsonAsciiTokenizer_delete(tokenizer);
	return;
}

static void testArray(void)
{
	QxJsonToken const tokens[] = {
		{ QxJsonTokenBeginArray     , NULL, 0 },
		{ QxJsonTokenValuesSeparator, NULL, 0 },
		{ QxJsonTokenEndArray       , NULL, 0 }
	};
	testGeneric("[ , ]", tokens, ARRAY_SIZE(tokens));
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
	testGeneric("{ : , : }", tokens, ARRAY_SIZE(tokens));
	return;
}

static void testNull(void)
{
	QxJsonToken const token = { QxJsonTokenNull, NULL, 0 };
	testGeneric("null", &token, 1);
	return;
}

static void testTrue(void)
{
	QxJsonToken const token = { QxJsonTokenTrue, NULL, 0 };
	testGeneric("true", &token, 1);
	return;
}

static void testFalse(void)
{
	QxJsonToken const token = { QxJsonTokenFalse, NULL, 0 };
	testGeneric("false", &token, 1);
	return;
}

static void testString(void)
{
	QxJsonToken const tokens[] = {
		{ QxJsonTokenString, L""          , 0 },
		{ QxJsonTokenString, L"QxJson"    , 6 },
		{ QxJsonTokenString, L"[\r,\n,\t]", 7 }
	};
	testGeneric("\"\"" "\"QxJson\"" "\"[\\r,\\n,\\t]\"",
		tokens, ARRAY_SIZE(tokens));
	return;
}

static void testNumber(void)
{
	QxJsonToken const tokens[] = {
		{ QxJsonTokenNumber, L"3.1415E-6", 9 }
	};
	testGeneric("3.1415E-6 ", tokens, ARRAY_SIZE(tokens));
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
