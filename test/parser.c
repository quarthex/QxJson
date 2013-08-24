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

static void *factory(QxJsonValueSpec const *spec, void *userPtr)
{
	void **const root = (void **)userPtr;
	void *value = NULL;
	size_t index;

	expect_null(*root);

	switch (spec->type)
	{
	case QxJsonValueTypeString:
		value = QxJsonValue_stringNew(spec->data.string.data, spec->data.string.size);
		break;

	case QxJsonValueTypeNumber:
		value = QxJsonValue_numberNew(wcstod(spec->data.number.data, NULL));
		break;

	case QxJsonValueTypeFalse:
		value = QxJsonValue_falseNew();
		break;

	case QxJsonValueTypeTrue:
		value = QxJsonValue_trueNew();
		break;

	case QxJsonValueTypeNull:
		value = QxJsonValue_nullNew();
		break;

	case QxJsonValueTypeArray:
		value = QxJsonValue_arrayNew();

		for (index = 0; index < spec->data.array.size; ++index)
			QxJsonValue_arrayAppendNew(value, (QxJsonValue *)spec->data.array.values[index]);

		break;

	case QxJsonValueTypeObject:
		value = QxJsonValue_objectNew();

		for (index = 0; index < spec->data.object.size; ++index)
		{
			QxJsonValue_objectSet(value,
				(QxJsonValue *)spec->data.object.keys[index],
				(QxJsonValue *)spec->data.object.values[index]);
			QxJsonValue_decref((QxJsonValue *)spec->data.object.keys[index]);
			QxJsonValue_decref((QxJsonValue *)spec->data.object.values[index]);
		}

		break;
	}

	if (spec->depth == 0)
		*root = value;

	return value;
}

static void testArray(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new(&factory, &root);
	expect_not_null(parser);

	token.type = QxJsonTokenBeginArray;
	token.size = 0;
	token.data = NULL;
	expect_zero(QxJsonParser_feed(parser, &token));

	token.type = QxJsonTokenEndArray;
	token.size = 0;
	token.data = NULL;
	expect_zero(QxJsonParser_feed(parser, &token));

	QxJsonParser_delete(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_ARRAY(root));
	expect_zero(QxJsonValue_size(root));
	QxJsonValue_decref(root);
}

static void testFalse(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new(&factory, &root);
	expect_not_null(parser);

	token.type = QxJsonTokenFalse;
	expect_zero(QxJsonParser_feed(parser, &token));

	QxJsonParser_delete(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_FALSE(root));
	QxJsonValue_decref(root);
}

static void testNull(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new(&factory, &root);
	expect_not_null(parser);

	token.type = QxJsonTokenNull;
	expect_zero(QxJsonParser_feed(parser, &token));

	QxJsonParser_delete(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_NULL(root));
	QxJsonValue_decref(root);
}

static void testNumber(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new(&factory, &root);
	expect_not_null(parser);

	token.type = QxJsonTokenNumber;
	token.data = L"3.1415";
	token.size = 6;
	expect_zero(QxJsonParser_feed(parser, &token));

	QxJsonParser_delete(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_NUMBER(root));
	expect_double_equal(QxJsonValue_numberValue(root), 3.1415);
	QxJsonValue_decref(root);
}

static void testObject(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new(&factory, &root);
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

	QxJsonParser_delete(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_OBJECT(root));
	expect_int_equal(QxJsonValue_size(root), 1);
	QxJsonValue_decref(root);
}

static void testString(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new(&factory, &root);
	expect_not_null(parser);

	token.type = QxJsonTokenString;
	token.data = L"string";
	token.size = 6;
	expect_zero(QxJsonParser_feed(parser, &token));

	QxJsonParser_delete(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_STRING(root));
	expect_int_equal(QxJsonValue_size(root), 6);
	expect_wstr_equal(QxJsonValue_stringValue(root), L"string");
	QxJsonValue_decref(root);
}

static void testTrue(void)
{
	QxJsonParser *parser;
	QxJsonToken token;
	QxJsonValue *root = NULL;

	parser = QxJsonParser_new(&factory, &root);
	expect_not_null(parser);

	token.type = QxJsonTokenTrue;
	expect_zero(QxJsonParser_feed(parser, &token));

	QxJsonParser_delete(parser);

	expect_not_null(root);
	expect_ok(QX_JSON_IS_TRUE(root));
	QxJsonValue_decref(root);
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
