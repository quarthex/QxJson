/**
 * @file tockenizer.c
 * @brief Source file of the JSON tockenizer.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../include/qx.json.tockenizer.h"
#include "queue.h"

#define ALLOC(Type) ((Type *)malloc(sizeof(Type)))
#define unused(x) ((void)(x))

/* Some shortcuts */
typedef QxJsonTockenizer Tockenizer;
typedef QxJsonTocken     Tocken;
typedef QxJsonTockenType TockenType;

struct QxJsonTockenizer
{
	Queue tockens;
	Tocken *tocken;
	wchar_t *buffer;
	size_t bufferAlloc;
	size_t bufferSize;
	int (*write)(Tockenizer *self, wchar_t character);
	int (*flush)(Tockenizer *self);
};

static int writeDefault(Tockenizer *self, wchar_t character);
static int flushDefault(Tockenizer *self);

Tockenizer *qxJsonTockenizerNew(void)
{
	Tockenizer *const instance = ALLOC(Tockenizer);

	if (instance)
	{
		memset(instance, 0, sizeof(Tockenizer));

		instance->buffer = (wchar_t *)malloc(sizeof(wchar_t) * 512);

		if (!instance->buffer) /* Failed to allocate memory */
		{
			free(instance);
			return NULL;
		}

		instance->bufferAlloc = 512;
		instance->write = writeDefault;
		instance->flush = flushDefault;
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

	free(self->buffer);
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
		error = self->write(self, *data);
	}

	return error;
}

int qxJsonTockenizerFlush(Tockenizer *self)
{
	assert(self != NULL);
	return self->flush(self);
}

int qxJsonTockenizerNextTocken(Tockenizer *self, Tocken *tocken)
{
	Tocken *tail = NULL;
	assert(self != NULL);
	tail = queueTail(&self->tockens);

	if (tail)
	{
		memcpy(tocken, tail, sizeof(Tocken));
		queuePop(&self->tockens);
		return 1;
	}

	/* Empty tocken */
	return 0;
}

/* Private functions */

static int writeNull(Tockenizer *tockenizer, wchar_t character);
static int flushNull(Tockenizer *tockenizer);

static int pushTocken(Tockenizer *tockenizer, TockenType type)
{
	Tocken *const tocken = ALLOC(Tocken);

	if (!tocken) /* Failed to allocate memory */
	{
		return -1;
	}

	tocken->type = type;
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
		return -1; /* Not yet implemented */

	case L',':
		return pushTocken(tockenizer, QxJsonTockenValuesSeparator);

	case L'-':
		return -1; /* Not yet implemented */

	case L'0':
		return -1; /* Not yet implemented */

	case L':':
		return pushTocken(tockenizer, QxJsonTockenNameValueSeparator);

	case L'[':
		return pushTocken(tockenizer, QxJsonTockenBeginArray);

	case L']':
		return pushTocken(tockenizer, QxJsonTockenEndArray);

	case L'f':
		/* Not yet implemented */
		return -1;

	case L'n':
		memcpy(tockenizer->buffer, L"ull", 3 * sizeof(wchar_t));
		tockenizer->write = writeNull;
		tockenizer->flush = flushNull;
		return 0;

	case L't':
		return -1; /* Not yet implemented */

	case L'{':
		return pushTocken(tockenizer, QxJsonTockenBeginObject);

	case L'}':
		return pushTocken(tockenizer, QxJsonTockenEndObject);

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

static int writeNull(Tockenizer *tockenizer, wchar_t character)
{
	if (character != tockenizer->buffer[tockenizer->bufferSize])
	{
		/* Unexpecting character */
		return -1;
	}

	if (tockenizer->bufferSize == 2) /* Last character */
	{
		tockenizer->write = writeDefault;
		tockenizer->flush = flushDefault;
		tockenizer->bufferSize = 0;
		return pushTocken(tockenizer, QxJsonTockenNull);
	}

	++tockenizer->bufferSize;
	return 0;
}

static int flushNull(Tockenizer *tockenizer)
{
	/* Unflushable */
	unused(tockenizer);
	return -1;
}

