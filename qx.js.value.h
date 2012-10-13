/**
 * @file qx.js.value.h
 * @brief Header file of the QxJsValue class
 * @author Romain Déoux <romain.deoux@gmail.com>
 */

#ifndef _H_QX_JS_VALUE
#define _H_QX_JS_VALUE

#include "qx.macro.h"

/**
 * @brief Variant type that handle any JavaScript value kind.
 */
typedef struct QxJsValue QxJsValue;

/**
 * @brief The unique identifier of a value type.
 */
typedef enum QxJsValueType
{
	QxJsValueTypeNull
} QxJsValueType;

/**
 * @brief Increment the reference counter of a value.
 * @param[in|out] value The value.
 */
QX_API void qxJsValueIncRef(QxJsValue *value);

/**
 * @brief Decrement the reference counter of a value.
 * @param[in|out] value The value.
 *
 * When the reference counter reach zero, the value is freed.
 */
QX_API void qxJsValueDecRef(QxJsValue *value);

/**
 * @brief Get the type of the value.
 * @param[in] The value.
 * @return The unique identifier of the type of the value.
 */
QX_API QxJsValueType qxJsValueType(QxJsValue const *value);

#endif /* _H_QX_JS_VALUE */

