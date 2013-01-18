/**
 * @file tockenizer.c
 * @brief Source file of the JSON tockenizer.
 * @author Romain DEOUX
 */

#include <stdlib.h>

#include "../include/qx.json.tockenizer.h"

struct QxJsonTockenizer
{
	int(*callback)(QxJsonTocken const *);
	void *userData;
};

typedef QxJsonTockenizer Tockenizer;

Tockenizer *qxJsonTockenizerNew(int(*callback)(QxJsonTocken const *),
	void *userData)
{
	Tockenizer *const instance = (Tockenizer *)malloc(sizeof(Tockenizer));

	if (instance)
	{
		instance->callback = callback;
		instance->userData = userData;
	}

	return instance;
}

void qxJsonTockenizerDelete(Tockenizer *tockenizer)
{
	if (tockenizer)
	{
		free(tockenizer);
	}

	return;
}

int qxJsonTockenizerWrite(Tockenizer *tockenizer, wchar_t const *data,
	size_t length)
{
	/* Not yet implemented */
	return -1;
}

int qxJsonTockenizerFlush(Tockenizer *tockenizer)
{
	/* Not yet implemented */
	return -1;
}

