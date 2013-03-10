#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <qx.json.tokenizer.h>

#include "expect.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static void testData(QxJsonTokenizer *tokenizer);

int main(void)
{
	QxJsonTokenizer *tokenizer;
	int error;
	wchar_t *data;
	char const json[] =
		"{"
		"    \"firstName\": \"John\","
		"    \"lastName\": \"Smith\","
		"    \"age\": 25,"
		"    \"address\": {"
		"        \"streetAddress\": \"21 2nd Street\","
		"        \"city\": \"New York\","
		"        \"state\": \"NY\","
		"        \"postalCode\": \"10021\""
		"    },"
		"    \"phoneNumber\": ["
		"        {"
		"            \"type\": \"home\","
		"            \"number\": \"212 555-1234\""
		"        },"
		"        {"
		"            \"type\": \"fax\","
		"            \"number\": \"646 555-4567\""
		"        }"
		"    ]"
		"}";

	/* Convert the content to wide string */
	data = (wchar_t *)malloc(sizeof(json) * sizeof(wchar_t));
	expect_not_null(data);
	error = mbstowcs(data, json, sizeof(json));
	expect_int_not_equal(error, -1);

	/* Create the tokenizer */
	tokenizer = QxJsonTokenizer_new();
	expect_not_null(tokenizer);

	/* Input the steam */
	error = QxJsonTokenizer_resetStream(tokenizer, data, error);
	expect_zero(error);

	/* Assert the content */
	testData(tokenizer);

	/* Free */
	free(data);
	QxJsonTokenizer_delete(tokenizer);
	return EXIT_SUCCESS;
}

static void testData(QxJsonTokenizer *tokenizer)
{
	QxJsonToken tokens[] = {
	/* 0 */	{QxJsonTokenBeginObject, NULL, 0},
	/* 1 */		{QxJsonTokenString, L"firstName", 5 + 4},
	/* 2 */		{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 3 */		{QxJsonTokenString, L"John", 4},
	/* 4 */		{QxJsonTokenValuesSeparator, NULL, 0},

	/* 5 */		{QxJsonTokenString, L"lastName", 4 + 4},
	/* 6 */		{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 7 */		{QxJsonTokenString, L"Smith", 5},
	/* 8 */		{QxJsonTokenValuesSeparator, NULL, 0},

	/* 9 */		{QxJsonTokenString, L"age", 3},
	/* 10 */		{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 11 */		{QxJsonTokenNumber, L"25", 2},
	/* 12 */		{QxJsonTokenValuesSeparator, NULL, 0},

	/* 13 */		{QxJsonTokenString, L"address", 7},
	/* 14 */		{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 15 */		{QxJsonTokenBeginObject, NULL, 0},
	/* 16 */			{QxJsonTokenString, L"streetAddress", 6 + 7},
	/* 17 */			{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 18 */			{QxJsonTokenString, L"21 2nd Street", 2 + 1 + 3 + 1 + 6},
	/* 19 */			{QxJsonTokenValuesSeparator, NULL, 0},

	/* 20 */			{QxJsonTokenString, L"city", 4},
	/* 21 */			{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 22 */			{QxJsonTokenString, L"New York", 3 + 1 + 4},
	/* 23 */			{QxJsonTokenValuesSeparator, NULL, 0},

	/* 24 */			{QxJsonTokenString, L"state", 5},
	/* 25 */			{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 26 */			{QxJsonTokenString, L"NY", 2},
	/* 27 */			{QxJsonTokenValuesSeparator, NULL, 0},

	/* 28 */			{QxJsonTokenString, L"postalCode", 6 + 4},
	/* 29 */			{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 30 */			{QxJsonTokenString, L"10021", 5},
	/* 31 */		{QxJsonTokenEndObject, NULL, 0},
	/* 32 */		{QxJsonTokenValuesSeparator, NULL, 0},

	/* 33 */		{QxJsonTokenString, L"phoneNumber", 5 + 6},
	/* 34 */		{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 35 */		{QxJsonTokenBeginArray, NULL, 0},
	/* 36 */			{QxJsonTokenBeginObject, NULL, 0},
	/* 37 */				{QxJsonTokenString, L"type", 4},
	/* 38 */				{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 39 */				{QxJsonTokenString, L"home", 4},
	/* 40 */				{QxJsonTokenValuesSeparator, NULL, 0},

	/* 41 */				{QxJsonTokenString, L"number", 6},
	/* 42 */				{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 43 */				{QxJsonTokenString, L"212 555-1234", 3 + 1 + 3 + 1 + 4},
	/* 44 */			{QxJsonTokenEndObject, NULL, 0},
	/* 45 */			{QxJsonTokenValuesSeparator, NULL, 0},

	/* 46 */			{QxJsonTokenBeginObject, NULL, 0},
	/* 47 */				{QxJsonTokenString, L"type", 4},
	/* 48 */				{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 49 */				{QxJsonTokenString, L"fax", 3},
	/* 50 */				{QxJsonTokenValuesSeparator, NULL, 0},

	/* 51 */				{QxJsonTokenString, L"number", 6},
	/* 52 */				{QxJsonTokenNameValueSeparator, NULL, 0},
	/* 53 */				{QxJsonTokenString, L"646 555-4567", 3 + 1 + 3 + 1 + 4},
	/* 54 */			{QxJsonTokenEndObject, NULL, 0},
	/* 55 */		{QxJsonTokenEndArray, NULL, 0},
	/* 56 */	{QxJsonTokenEndObject, NULL, 0},
	};
	QxJsonToken token;
	unsigned int index;

	for (index = 0; index < ARRAY_SIZE(tokens); ++index)
	{
		expect_int_equal(QxJsonTokenizer_nextToken(tokenizer, &token), 1);
		expect_int_equal(token.type, tokens[index].type);
		expect_int_equal(token.size, tokens[index].size);

		if (tokens[index].data == NULL)
		{
			expect_null(token.data);
		}
		else
		{
			expect_not_null(token.data);
			expect_wstr_equal(token.data, tokens[index].data);
		}
	}

	expect_int_equal(QxJsonTokenizer_nextToken(tokenizer, &token), 0);
}
