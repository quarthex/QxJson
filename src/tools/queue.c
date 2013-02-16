/**
 * @file queue.c
 * @brief Source file of the internal Queue class.
 * @author Romain DEOUX
 */

#include <assert.h>
#include <stdlib.h>

#include "queue.h"

int queuePush(Queue *self, void *data)
{
	QueueNode *node = NULL;
	assert(self != NULL);
	node = (QueueNode *)malloc(sizeof(QueueNode));

	if (!node)
	{
		return -1;
	}

	node->data = data;
	node->next = NULL;

	if (self->head) /* The queue is not empty */
	{
		self->head->next = node;
		self->head = node;
	}
	else
	{
		self->head = node;
		self->tail = node;
	}

	return 0;
}

void *queueTail(Queue const *self)
{
	assert(self != NULL);
	return self->tail ? self->tail->data : NULL;
}

void queuePop(Queue *self)
{
	QueueNode *node = NULL;
	assert(self != NULL);
	node = self->tail;

	if (node)
	{
		/* Non empty queue */
		self->tail = node->next;

		if (!self->tail) /* Now, the queue is empty */
		{
			self->head = NULL;
		}

		free(node);
	}

	return;
}
