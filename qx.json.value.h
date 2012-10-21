/**
 * @file qx.json.value.h
 * @brief Header file of the QxJsonValue class
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_VALUE
#define _H_QX_JSON_VALUE

#include "qx.macro.h"

/**
 * @brief Variant type that handle any JavaScript value kind.
 */
typedef struct QxJsonValue QxJsonValue;

/**
 * @brief The unique identifier of a value type.
 */
typedef enum QxJsonValueType
{
	QxJsonValueTypeNull,
	QxJsonValueTypeTrue,
	QxJsonValueTypeFalse,
	QxJsonValueTypeNumber,
	QxJsonValueTypeString
} QxJsonValueType;

#define qxJsonValueIs(value, Type) \
	(qxJsonValueType(value) == (QxJsonValueType##Type))
#define qxJsonValueIsNull(value)   qxJsonValueIs(value, Null)
#define qxJsonValueIsTrue(value)   qxJsonValueIs(value, True)
#define qxJsonValueIsFalse(value)  qxJsonValueIs(value, False)
#define qxJsonValueIsNumber(value) qxJsonValueIs(value, Number)
#define qxJsonValueIsString(value) qxJsonValueIs(value, String)

/**
 * @brief Increment the reference counter of a value.
 * @param[in|out] value The value.
 */
QX_API void qxJsonValueIncRef(QxJsonValue *value);

/**
 * @brief Decrement the reference counter of a value.
 * @param[in|out] value The value.
 *
 * When the reference counter reach zero, the value is freed.
 */
QX_API void qxJsonValueDecRef(QxJsonValue *value);

/**
 * @brief Get the type of the value.
 * @param[in] The value.
 * @return The unique identifier of the type of the value.
 */
QX_API QxJsonValueType qxJsonValueType(QxJsonValue const *value);

#endif /* _H_QX_JSON_VALUE */

