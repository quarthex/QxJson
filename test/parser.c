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
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	expect_zero(QxJsonParser_feed(parser, L"[]", 2));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_ARRAY(root));
	expect_zero(QxJsonValue_size(root));
	QxJsonValue_release(root);
}

static void testNestedArray(void)
{
	QxJsonParser *parser;
	QxJsonValue *root = NULL;
	QxJsonValue const *child;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	expect_zero(QxJsonParser_feed(parser, L"[[[]]]", 6));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_ARRAY(root));
	expect_int_equal(QxJsonValue_size(root), 1);

	child = QxJsonValue_arrayGet(root, 0);
	expect_not_null(child);
	expect_ok(QX_JSON_IS_ARRAY(child));
	expect_int_equal(QxJsonValue_size(child), 1);

	child = QxJsonValue_arrayGet(child, 0);
	expect_not_null(child);
	expect_ok(QX_JSON_IS_ARRAY(child));
	expect_zero(QxJsonValue_size(child));

	QxJsonValue_release(root);
}

static void testFalse(void)
{
	QxJsonParser *parser;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	expect_zero(QxJsonParser_feed(parser, L"false", 5));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_FALSE(root));
	QxJsonValue_release(root);
}

static void testNull(void)
{
	QxJsonParser *parser;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	expect_zero(QxJsonParser_feed(parser, L"null", 4));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_NULL(root));
	QxJsonValue_release(root);
}

static void testNumber(void)
{
	QxJsonParser *parser;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	expect_zero(QxJsonParser_feed(parser, L"3.1415", 6));

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
	wchar_t const *text = L"{\"key\": null}";
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	expect_zero(QxJsonParser_feed(parser, text, wcslen(text)));

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
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	expect_zero(QxJsonParser_feed(parser, L"\"string\"", 8));

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
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new();
	expect_not_null(parser);

	expect_zero(QxJsonParser_feed(parser, L"true", 4));

	expect_zero(QxJsonParser_end(parser, &root));
	QxJsonParser_release(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_TRUE(root));
	QxJsonValue_release(root);
}

static void testPartialTocken(void)
{
	/* The parser should raise an error when a unexpected token begins to
	 * be fed.
	 *
	 * Tokens that can be partially parsed are:
	 *      - strings
	 *      - numbers
	 *      - false/true/null
	 */

	wchar_t const *testCases[] = {
		L"[0 \"",       /* '[' <value> <string>  */
		L"[0 0",        /* '[' <value> <number>  */
		L"[0 f",        /* '[' <value> 'false'   */
		L"[0 t",        /* '[' <value> 'true'    */
		L"[0 n",        /* '[' <value> 'null'    */
		L"{0",          /* '{' <number>          */
		L"{f",          /* '{' 'false'           */
		L"{t",          /* '{' 'true'            */
		L"{n",          /* '{' 'null'            */
		L"{\"foo\" \"", /* '{' <string> <string> */
		L"{\"foo\" 0",  /* '{' <string> <number> */
		L"{\"foo\" f",  /* '{' <string> 'false'  */
		L"{\"foo\" t",  /* '{' <string> 'true'   */
		L"{\"foo\" n",  /* '{' <string> 'null'   */
	};
	size_t idx;

	for (idx = 0; idx < sizeof(testCases) /  sizeof(testCases[0]); ++idx)
	{
		wchar_t const *testCase = testCases[idx];
		QxJsonParser *parser;

		parser = QxJsonParser_new();
		expect_not_null(parser);

		expect_not_zero(QxJsonParser_feed(parser, testCase, wcslen(testCase)));

		QxJsonParser_release(parser);
	}
}

int main(void)
{
	testArray();
	testNestedArray();
	testFalse();
	testNull();
	testNumber();
	testObject();
	testString();
	testTrue();
	testPartialTocken();
	return EXIT_SUCCESS;
}
