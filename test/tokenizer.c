/**
 * @file qx.test.tokenizer.c
 * @brief Testing source file of the QxJsonTokenizer class.
 * @author Romain DEOUX
 */

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <qx.json.tokenizer.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#include "expect.h"

static void testGeneric(wchar_t const *data,
	QxJsonToken const *tokens, size_t tokensCount)
{
	QxJsonToken token;
	size_t index = 0;
	QxJsonTokenizer *const tokenizer = QxJsonTokenizer_new();

	expect_not_null(tokenizer);
	expect_zero(QxJsonTokenizer_resetStream(tokenizer, data, wcslen(data)));

	for (; index < tokensCount; ++index)
	{
		expect_int_equal(QxJsonTokenizer_nextToken(tokenizer, &token), 1);
		expect_int_equal(token.type, tokens[index].type);
		expect_int_equal(token.size, tokens[index].size);

		if (token.size)
		{
			expect_not_null(token.data);
			expect_zero(memcmp(token.data, tokens[index].data,
				token.size * sizeof(wchar_t)));
		}
	}

	expect_zero(QxJsonTokenizer_nextToken(tokenizer, &token));

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
