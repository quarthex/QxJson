/**
 * @file qx.js.value.private.h
 * @brief Private header of the QxJsValue class.
 * @author Romain Déoux <romain.deoux@gmail.com>
 */

#ifndef _H_QX_JS_VALUE_PRIVATE_H
#define _H_QX_JS_VALUE_PRIVATE_H

#include <stddef.h>

#include "qx.js.value.h"

/**
 * @brief Internal introspection class for the values.
 */
typedef struct QxJsValueClass
{
	/**
	 * @brief Free any resource used by the value.
	 * @param[in|out] value The value to be finalized.
	 */
	void (*finalize)(QxJsValue *value);

	/**
	 * @param The type of the value.
	 */
	QxJsValueType type;
} QxJsValueClass;

struct QxJsValue
{
	/**
	 * @brief The internal class of the value.
	 */
	QxJsValueClass const *klass;

	/**
	 * The reference counter.
	 */
	size_t ref;
};

#endif /* _H_QX_JS_VALUE_PRIVATE_H */

