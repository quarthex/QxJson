/**
 * @file wikipedia.c
 * @brief Testing use case using the Wikipedia JSON example.
 * @author Romain DEOUX
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <qx.json.parser.h>

#include "expect.h"

static int checkAddress(QxJsonValue const *key, QxJsonValue *value, void *ptr)
{
	wchar_t const *keyStr, *valueStr;

	(void)ptr;
	expect_not_null(key);
	expect_not_null(value);

	expect_int_equal(QxJsonValue_type(key), QxJsonValueTypeString);
	keyStr = QxJsonValue_stringValue(key);
	expect_not_null(keyStr);

	expect_int_equal(QxJsonValue_type(value), QxJsonValueTypeString);
	valueStr = QxJsonValue_stringValue(value);
	expect_not_null(valueStr);

	if (wcscmp(keyStr, L"streetAddress") == 0)
		expect_wstr_equal(valueStr, L"21 2nd Street");
	else if (wcscmp(keyStr, L"city") == 0)
		expect_wstr_equal(valueStr, L"New York");
	else if (wcscmp(keyStr, L"state") == 0)
		expect_wstr_equal(valueStr, L"NY");
	else if (wcscmp(keyStr, L"postalCode") == 0)
		expect_wstr_equal(valueStr, L"10021");
	else
		return -1;

	return 0;
}

static int checkPhoneNumber(size_t index, QxJsonValue *value, void *ptr)
{
	QxJsonValue *key, *subvalue;
	(void)ptr;

	expect_not_null(value);

	switch (index)
	{
	case 0:
		expect_ok(QX_JSON_IS_OBJECT(value));
		expect_int_equal(QxJsonValue_size(value), 2);

		key = QxJsonValue_stringNew(L"type", 4);
		subvalue = NULL;
		expect_zero(QxJsonValue_objectGet(value, key, &subvalue));
		QxJsonValue_release(key);
		expect_not_null(subvalue);
		expect_wstr_equal(QxJsonValue_stringValue(subvalue), L"home");

		key = QxJsonValue_stringNew(L"number", 6);
		subvalue = NULL;
		expect_zero(QxJsonValue_objectGet(value, key, &subvalue));
		QxJsonValue_release(key);
		expect_not_null(subvalue);
		expect_wstr_equal(QxJsonValue_stringValue(subvalue), L"212 555-1234");

		break;

	case 1:
		key = QxJsonValue_stringNew(L"type", 4);
		subvalue = NULL;
		expect_zero(QxJsonValue_objectGet(value, key, &subvalue));
		QxJsonValue_release(key);
		expect_not_null(subvalue);
		expect_wstr_equal(QxJsonValue_stringValue(subvalue), L"fax");

		key = QxJsonValue_stringNew(L"number", 6);
		subvalue = NULL;
		expect_zero(QxJsonValue_objectGet(value, key, &subvalue));
		QxJsonValue_release(key);
		expect_not_null(subvalue);
		expect_wstr_equal(QxJsonValue_stringValue(subvalue), L"646 555-4567");

	default:
		return -1;
	}

	return 0;
}

static int checkRoot(QxJsonValue const *key, QxJsonValue *value, void *ptr)
{
	wchar_t const *keyStr;

	(void)ptr;
	expect_not_null(key);
	expect_not_null(value);

	expect_ok(QX_JSON_IS_STRING(key));
	keyStr = QxJsonValue_stringValue(key);
	expect_not_null(keyStr);

	if (wcscmp(keyStr, L"firstName") == 0)
	{
		expect_ok(QX_JSON_IS_STRING(value));
		expect_wstr_equal(QxJsonValue_stringValue(value), L"John");
	}
	else if (wcscmp(keyStr, L"lastName") == 0)
	{
		expect_ok(QX_JSON_IS_STRING(value));
		expect_wstr_equal(QxJsonValue_stringValue(value), L"Smith");
	}
	else if (wcscmp(keyStr, L"age") == 0)
	{
		expect_ok(QX_JSON_IS_NUMBER(value));
		expect_double_equal(QxJsonValue_numberValue(value), 25.0);
	}
	else if (wcscmp(keyStr, L"address") == 0)
	{
		expect_ok(QX_JSON_IS_OBJECT(value));
		expect_int_equal(QxJsonValue_size(value), 4);
		expect_zero(QxJsonValue_objectEach(value, &checkAddress, NULL));
	}
	else if (wcscmp(keyStr, L"phoneNumber") == 0)
	{
		expect_ok(QX_JSON_IS_ARRAY(value));
		expect_int_equal(QxJsonValue_size(value), 4);
		expect_zero(QxJsonValue_arrayEach(value, &checkPhoneNumber, NULL));
	}
	else
		return -1;

	return 0;
}

int main(void)
{
	FILE *file;
	char buffer[512];
	wchar_t wbuffer[512];
	ssize_t bufferSize;
	QxJsonParser *parser = NULL;
	QxJsonValue *value = NULL;

	/* Create the parser */
	parser = QxJsonParser_new();
	expect_not_null(parser);

	/* Read the JSON file */
	file = fopen("../test/wikipedia.json", "r");
	expect_ok(file != NULL);

	do
	{
		bufferSize = fread(buffer, sizeof(char), sizeof(buffer), file);
		expect_ok(bufferSize >= 0);

		if (bufferSize)
		{
			expect_zero(QxJsonParser_feed(parser, wbuffer, mbstowcs(wbuffer, buffer, bufferSize)));
		}
	}
	while (bufferSize > 0);

	fclose(file);
	expect_zero(QxJsonParser_end(parser, &value));

	/* Check the value */
	expect_ok(QX_JSON_IS_OBJECT(value));
	expect_int_equal(QxJsonValue_size(value), 5);
	expect_zero(QxJsonValue_objectEach(value, &checkRoot, NULL));
	QxJsonValue_release(value);

	return EXIT_SUCCESS;
}
