/**
 * @file vector.c
 * @brief Source file of the QxVector class.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

void qxVectorInit(QxVector *self)
{
	assert(self != NULL);
	memset(self, 0, sizeof(QxVector));
	return;
}

void qxVectorFinalize(QxVector *self)
{
	assert(self != NULL);

	if (self->data)
	{
		assert(self->alloc != 0);
		free(self->data);
		qxVectorInit(self);
	}

	return;
}

int qxVectorAppend(QxVector *self, void *data, size_t dataSize)
{
	void *dataTmp;
	assert(self != NULL);

	if (self->size + dataSize > self->alloc)
	{
		self->alloc += 512;
		dataTmp = realloc(self->data, self->alloc);

		if (!dataTmp)
		{
			/* Memory allocation failed */
			self->alloc -= 512;
			return -1;
		}

		self->data = dataTmp;
	}

	memcpy((char *)self->data + self->size, data, dataSize);
	self->size += dataSize;
	return 0;
}
