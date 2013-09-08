/**
 * @file parser.c
 * @brief Testing sources of the QxJsonParser class.
 * @author Romain DEOUX
 */

#include <stdlib.h>
#include <wchar.h>

#include <qx.json.parser.h>
#include <qx.json.value.h>

#include "expect.h"

static void testArray(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	token.type = QxJsonTokenBeginArray;
	token.size = 0;
	token.data = NULL;
	expect_zero(QxJsonParser_feed(parser, &token));

	token.type = QxJsonTokenEndArray;
	token.size = 0;
	token.data = NULL;
	expect_zero(QxJsonParser_feed(parser, &token));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_ARRAY(root));
	expect_zero(QxJsonValue_size(root));
	QxJsonValue_release(root);
}

static void testFalse(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	token.type = QxJsonTokenFalse;
	expect_zero(QxJsonParser_feed(parser, &token));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_FALSE(root));
	QxJsonValue_release(root);
}

static void testNull(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	token.type = QxJsonTokenNull;
	expect_zero(QxJsonParser_feed(parser, &token));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_NULL(root));
	QxJsonValue_release(root);
}

static void testNumber(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	token.type = QxJsonTokenNumber;
	token.data = L"3.1415";
	token.size = 6;
	expect_zero(QxJsonParser_feed(parser, &token));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_NUMBER(root));
	expect_double_equal(QxJsonValue_numberValue(root), 3.1415);
	QxJsonValue_release(root);
}

static void testObject(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	token.type = QxJsonTokenBeginObject;
	token.size = 0;
	token.data = NULL;
	expect_zero(QxJsonParser_feed(parser, &token));

	token.type = QxJsonTokenString;
	token.size = 3;
	token.data = L"key";
	expect_zero(QxJsonParser_feed(parser, &token));

	token.type = QxJsonTokenNameValueSeparator;
	expect_zero(QxJsonParser_feed(parser, &token));

	token.type = QxJsonTokenNull;
	expect_zero(QxJsonParser_feed(parser, &token));

	token.type = QxJsonTokenEndObject;
	token.size = 0;
	token.data = NULL;
	expect_zero(QxJsonParser_feed(parser, &token));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_OBJECT(root));
	expect_int_equal(QxJsonValue_size(root), 1);
	QxJsonValue_release(root);
}

static void testString(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	token.type = QxJsonTokenString;
	token.data = L"string";
	token.size = 6;
	expect_zero(QxJsonParser_feed(parser, &token));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_STRING(root));
	expect_int_equal(QxJsonValue_size(root), 6);
	expect_wstr_equal(QxJsonValue_stringValue(root), L"string");
	QxJsonValue_release(root);
}

static void testTrue(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	token.type = QxJsonTokenTrue;
	expect_zero(QxJsonParser_feed(parser, &token));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_TRUE(root));
	QxJsonValue_release(root);
}

int main(void)
{
	testArray();
	testFalse();
	testNull();
	testNumber();
	testObject();
	testString();
	testTrue();
	return EXIT_SUCCESS;
}
