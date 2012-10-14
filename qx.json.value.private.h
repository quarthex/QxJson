/**
 * @file qx.json.value.private.h
 * @brief Private header of the QxJsonValue class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_VALUE_PRIVATE_H
#define _H_QX_JSON_VALUE_PRIVATE_H

#include <stddef.h>

#include "qx.json.value.h"

/**
 * @brief Internal introspection class for the values.
 */
typedef struct QxJsonValueClass
{
	/**
	 * @brief Free any resource used by the value.
	 * @param[in|out] value The value to be finalized.
	 */
	void (*finalize)(QxJsonValue *value);

	/**
	 * @param The type of the value.
	 */
	QxJsonValueType type;
} QxJsonValueClass;

struct QxJsonValue
{
	/**
	 * @brief The internal class of the value.
	 */
	QxJsonValueClass const *klass;

	/**
	 * The reference counter.
	 */
	size_t ref;
};

#endif /* _H_QX_JSON_VALUE_PRIVATE_H */

