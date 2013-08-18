/**
 * @file parser.c
 * @brief Testing sources of the QxJsonParser class.
 * @author Romain DEOUX
 */

#include <stdlib.h>
#include <wchar.h>

#include <qx.json.parser.h>
#include <qx.json.string.h>
#include <qx.json.number.h>
#include <qx.json.false.h>
#include <qx.json.true.h>
#include <qx.json.null.h>
#include <qx.json.array.h>
#include <qx.json.object.h>

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
		value = QxJsonString_new(spec->spec.string.data, spec->spec.string.size);
		break;

	case QxJsonValueTypeNumber:
		value = QxJsonNumber_new(wcstod(spec->spec.number.data, NULL));
		break;

	case QxJsonValueTypeFalse:
		value = QxJsonFalse_new();
		break;

	case QxJsonValueTypeTrue:
		value = QxJsonTrue_new();
		break;

	case QxJsonValueTypeNull:
		value = QxJsonNull_new();
		break;

	case QxJsonValueTypeArray:
		value = QxJsonArray_new();

		for (index = 0; index < spec->spec.array.size; ++index)
			QxJsonArray_appendNew(QX_JSON_ARRAY(value), (QxJsonValue *)spec->spec.array.values[index]);

		break;

	case QxJsonValueTypeObject:
		value = QxJsonObject_new();

		for (index = 0; index < spec->spec.object.size; ++index)
		{
			QxJsonObject_set(QX_JSON_OBJECT(value),
				QX_JSON_STRING((QxJsonValue *)spec->spec.object.keys[index]),
				(QxJsonValue *)spec->spec.object.values[index]);
			QxJsonValue_decref((QxJsonValue *)spec->spec.object.keys[index]);
			QxJsonValue_decref((QxJsonValue *)spec->spec.object.values[index]);
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
	expect_zero(QxJsonArray_size(QX_JSON_ARRAY(root)));
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
	expect_double_equal(QxJsonNumber_value(QX_JSON_NUMBER(root)), 3.1415);
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
	expect_int_equal(QxJsonObject_size(QX_JSON_OBJECT(root)), 1);
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
	expect_int_equal(QxJsonString_size(QX_JSON_STRING(root)), 6);
	expect_wstr_equal(QxJsonString_data(QX_JSON_STRING(root)), L"string");
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
