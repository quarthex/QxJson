/**
 * @file queue.h
 * @brief Header file of the internal Queue class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_QUEUE
#define _H_QX_QUEUE

typedef struct QueueNode QueueNode;
struct QueueNode
{
	void *data;
	QueueNode *next;
};

typedef struct Queue Queue;
struct Queue
{
	QueueNode *head;
	QueueNode *tail;
};

int queuePush(Queue *self, void *data);
void *queueTail(Queue const *self);
void queuePop(Queue *self);

#endif /* _H_QX_QUEUE */
