/**
 * @file qx.test.tockenizer.c
 * @brief Testing source file of the QxJsonTockenizer class.
 * @author Romain DEOUX
 */

#include <qx.json.tockenizer.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define WCSLEN(str) (ARRAY_SIZE(str) - 1)

#include "assert.h"

static int testNew(void)
{
	QxJsonTockenizer *const tockenizer = qxJsonTockenizerNew();
	QX_ASSERT(tockenizer != NULL);
	qxJsonTockenizerDelete(tockenizer);
	return 0;
}

static int testArray(void)
{
	QxJsonTockenizer *const tockenizer = qxJsonTockenizerNew();
	QxJsonTocken tocken;
	wchar_t const text[] = L"[ , ]";
	QxJsonTockenType const types[] = {
		QxJsonTockenBeginArray,
		QxJsonTockenValuesSeparator,
		QxJsonTockenEndArray
	};
	size_t index = 0;

	qxJsonTockenizerWrite(tockenizer, text, WCSLEN(text));

	for (; index < ARRAY_SIZE(types); ++index)
	{
		QX_ASSERT(qxJsonTockenizerNextTocken(tockenizer, &tocken) == 1);
		QX_ASSERT(tocken.type == types[index]);
	}

	QX_ASSERT(qxJsonTockenizerNextTocken(tockenizer, &tocken) == 0);
	qxJsonTockenizerDelete(tockenizer);
	return 0;
}

static int testObject(void)
{
	QxJsonTockenizer *const tockenizer = qxJsonTockenizerNew();
	QxJsonTocken tocken;
	wchar_t const text[] = L"{ : , : }";
	QxJsonTockenType const types[] = {
		QxJsonTockenBeginObject,
		QxJsonTockenNameValueSeparator,
		QxJsonTockenValuesSeparator,
		QxJsonTockenNameValueSeparator,
		QxJsonTockenEndObject,
	};
	size_t index = 0;

	qxJsonTockenizerWrite(tockenizer, text, WCSLEN(text));

	for (; index < ARRAY_SIZE(types); ++index)
	{
		QX_ASSERT(qxJsonTockenizerNextTocken(tockenizer, &tocken) == 1);
		QX_ASSERT(tocken.type == types[index]);
	}

	QX_ASSERT(qxJsonTockenizerNextTocken(tockenizer, &tocken) == 0);
	qxJsonTockenizerDelete(tockenizer);
	return 0;
}


static int testNull(void)
{
	QxJsonTockenizer *const tockenizer = qxJsonTockenizerNew();
	QxJsonTocken tocken;

	qxJsonTockenizerWrite(tockenizer, L"null", 4);
	QX_ASSERT(qxJsonTockenizerNextTocken(tockenizer, &tocken) == 1);
	QX_ASSERT(tocken.type == QxJsonTockenNull);
	QX_ASSERT(qxJsonTockenizerNextTocken(tockenizer, &tocken) == 0);

	qxJsonTockenizerDelete(tockenizer);
	return 0;
}

int main(void)
{
	testNew();
	testArray();
	testObject();
	testNull();
	return EXIT_SUCCESS;
}

