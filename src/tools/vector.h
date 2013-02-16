/**
 * @file vector.h
 * @brief Header file of the QxVector class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_VECTOR
#define _H_QX_VECTOR

typedef struct QxVector QxVector;
struct QxVector
{
	void *data;
	size_t size;
	size_t alloc;
};

/**
 * @brief Initialize a vector.
 * @param self The vector to be initialized.
 */
void qxVectorInit(QxVector *self);

/**
 * @brief Free the content of the vector.
 * @param self The vector to be finalized.
 *
 * Once finalized, a dynamically allocated vector can be freed without memory
 * leaks.
 */
void qxVectorFinalize(QxVector *self);

/**
 * @brief Append data at the end of the vector.
 * @param self     The vector.
 * @param data     The address of the data to be appended.
 * @param dataSize The size of the data.
 * @return 0 on success.
 */
int qxVectorAppend(QxVector *self, void *data, size_t dataSize);

#endif /* _H_QX_VECTOR */
