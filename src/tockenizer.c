/**
 * @file tockenizer.c
 * @brief Source file of the JSON tockenizer.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../include/qx.json.tockenizer.h"
#include "tools/queue.h"

#define ALLOC(Type) ((Type *)malloc(sizeof(Type)))
#define unused(x) ((void)(x))
#define MEMORY_ALLOC_SIZE 512

/* Some shortcuts */
typedef QxJsonTockenizer Tockenizer;
typedef QxJsonTocken     Tocken;
typedef QxJsonTockenType TockenType;

typedef struct Callbacks Callbacks;
struct Callbacks
{
	int (*const write)(Tockenizer *self, wchar_t character);
	int (*const flush)(Tockenizer *self);
};

struct QxJsonTockenizer
{
	/* Tockens queue */
	Queue tockens;

	/* Parsing working variable */
	size_t size;

	/* Parsing state */
	Callbacks const *callbacks;

	/* char buffer */
	char *charBuffer;
	size_t charSize;
	size_t charAlloc;

	/* wchar_t buffer */
	wchar_t *wcharBuffer;
	size_t wcharAlloc;
	size_t wcharSize;
};

static Callbacks const callbacksDefault;

Tockenizer *qxJsonTockenizerNew(void)
{
	Tockenizer *const instance = ALLOC(Tockenizer);

	if (instance)
	{
		memset(instance, 0, sizeof(Tockenizer));
		instance->callbacks = &callbacksDefault;
	}

	return instance;
}

void qxJsonTockenizerDelete(Tockenizer *self)
{
	Tocken *tocken = NULL;
	assert(self != NULL);

	for (tocken = queueTail(&self->tockens); tocken;)
	{
		free(tocken);
		queuePop(&self->tockens);
	}

	if (self->charBuffer)
	{
		free(self->charBuffer);
	}

	if (self->wcharBuffer)
	{
		free(self->wcharBuffer);
	}

	free(self);
	return;
}

int qxJsonTockenizerWrite(Tockenizer *self, wchar_t const *data, size_t size)
{
	wchar_t const *const end = data + size;
	int error = 0;
	assert(self != NULL);

	if (!data || !size)
	{
		/* Invalid argument */
		return -1;
	}

	for (; !error && data != end; ++data)
	{
		assert(self->callbacks != NULL);
		assert(self->callbacks->write != NULL);
		error = self->callbacks->write(self, *data);
	}

	return error;
}

int qxJsonTockenizerFlush(Tockenizer *self)
{
	assert(self != NULL);
	return self->callbacks->flush(self);
}

int qxJsonTockenizerNextTocken(Tockenizer *self, Tocken *tocken)
{
	Tocken *tail = NULL;
	assert(self != NULL);
	tail = queueTail(&self->tockens);

	if (tail)
	{
		memcpy(tocken, tail, sizeof(Tocken));
		free(tail);
		queuePop(&self->tockens);
		return 1;
	}

	/* Empty tocken */
	return 0;
}

/* Private functions */

static Callbacks const callbacksNull;
static Callbacks const callbacksTrue;
static Callbacks const callbacksFalse;
static Callbacks const callbacksString;

static int flushFail(Tockenizer *tockenizer)
{
	/* Unflushabled */
	unused(tockenizer);
	return -1;
}

static int pushTocken(Tockenizer *tockenizer, TockenType type,
	wchar_t const *data, size_t size)
{
	Tocken *const tocken = ALLOC(Tocken);

	if (!tocken) /* Failed to allocate memory */
	{
		return -1;
	}

	tocken->type = type;
	tocken->data = data;
	tocken->size = size;
	return queuePush(&tockenizer->tockens, tocken);
}

static int writeDefault(Tockenizer *tockenizer, wchar_t character)
{
	switch (character)
	{
	case L'\t':
	case L'\n':
	case L'\r':
	case L' ':
		/* Ignored white space */
		return 0;

	case L'"':
		tockenizer->size = 0;
		tockenizer->callbacks = &callbacksString;
		return 0;

	case L',':
		return pushTocken(tockenizer, QxJsonTockenValuesSeparator, NULL, 0);

	case L'-':
		return -1; /* Not yet implemented */

	case L'0':
		return -1; /* Not yet implemented */

	case L':':
		return pushTocken(tockenizer, QxJsonTockenNameValueSeparator, NULL, 0);

	case L'[':
		return pushTocken(tockenizer, QxJsonTockenBeginArray, NULL, 0);

	case L']':
		return pushTocken(tockenizer, QxJsonTockenEndArray, NULL, 0);

	case L'f':
		tockenizer->size = 0;
		tockenizer->callbacks = &callbacksFalse;
		return 0;

	case L'n':
		tockenizer->size = 0;
		tockenizer->callbacks = &callbacksNull;
		return 0;

	case L't':
		tockenizer->size = 0;
		tockenizer->callbacks = &callbacksTrue;
		return 0;

	case L'{':
		return pushTocken(tockenizer, QxJsonTockenBeginObject, NULL, 0);

	case L'}':
		return pushTocken(tockenizer, QxJsonTockenEndObject, NULL, 0);

	default:
		if (character >= L'1' && character <= L'9')
		{
			return -1; /* Not yet implemented */
		}
	}

	/* Unexpected character */
	return -1;
}

static int flushDefault(Tockenizer *tockenizer)
{
	/* Idle state, nothing in the buffer */
	unused(tockenizer);
	return 0;
}

static Callbacks const callbacksDefault = { writeDefault, flushDefault };

static int writeNull(Tockenizer *tockenizer, wchar_t character)
{
	static wchar_t const tocken[3] = L"ull";

	if (character != tocken[tockenizer->size])
	{
		/* Unexpecting character */
		return -1;
	}

	if (tockenizer->size == 2) /* Last character */
	{
		tockenizer->callbacks = &callbacksDefault;
		return pushTocken(tockenizer, QxJsonTockenNull, NULL, 0);
	}

	++tockenizer->size;
	return 0;
}

static Callbacks const callbacksNull = { writeNull, flushFail };

static int writeTrue(Tockenizer *tockenizer, wchar_t character)
{
	static wchar_t const tocken[3] = L"rue";

	if (character != tocken[tockenizer->size])
	{
		/* Unexpecting character */
		return -1;
	}

	if (tockenizer->size == 2) /* Last character */
	{
		tockenizer->callbacks = &callbacksDefault;
		return pushTocken(tockenizer, QxJsonTockenTrue, NULL, 0);
	}

	++tockenizer->size;
	return 0;
}

static Callbacks const callbacksTrue = { writeTrue, flushFail };

static int writeFalse(Tockenizer *tockenizer, wchar_t character)
{
	static wchar_t const tocken[4] = L"alse";

	if (character != tocken[tockenizer->size])
	{
		/* Unexpecting character */
		return -1;
	}

	if (tockenizer->size == 3) /* Last character */
	{
		tockenizer->callbacks = &callbacksDefault;
		return pushTocken(tockenizer, QxJsonTockenFalse, NULL, 0);
	}

	++tockenizer->size;
	return 0;
}

static Callbacks const callbacksFalse = { writeFalse, flushFail };

static int writeString(Tockenizer *tockenizer, wchar_t character)
{
	wchar_t *dataTmp;
	int pushTockenResult;

	if (character == L'"')
	{
		/* End of the string => push it to the tockens queue */
		pushTockenResult = pushTocken(tockenizer, QxJsonTockenString,
			tockenizer->wcharBuffer + tockenizer->wcharSize, tockenizer->size);

		if (pushTockenResult == 0)
		{
			tockenizer->wcharSize += tockenizer->size;
		}

		return pushTockenResult;
	}

	if (tockenizer->wcharSize + tockenizer->size == tockenizer->wcharAlloc)
	{
		tockenizer->wcharAlloc += MEMORY_ALLOC_SIZE;
		dataTmp = realloc(tockenizer->wcharBuffer,
			tockenizer->wcharAlloc * sizeof(wchar_t));

		if (dataTmp == NULL)
		{
			return -1; /* Failed to allocate more memory */
		}

		tockenizer->wcharBuffer = dataTmp;
	}

	tockenizer->wcharBuffer[tockenizer->wcharSize + tockenizer->size] = character;
	++tockenizer->size;
	return 0;
}

static Callbacks const callbacksString = { writeString, flushFail };
