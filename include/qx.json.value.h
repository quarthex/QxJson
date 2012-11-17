/**
 * @file qx.json.value.h
 * @brief Header file of the QxJsonValue class
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_VALUE
#define _H_QX_JSON_VALUE

#include "qx.json.macro.h"

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
	QxJsonValueTypeString,
	QxJsonValueTypeArray,
	QxJsonValueTypeObject
} QxJsonValueType;

/**
 * @brief Increment the reference counter of a value.
 * @param[in|out] value The value.
 */
QX_API void qxJsonValueRef(QxJsonValue *value);

/**
 * @brief Decrement the reference counter of a value.
 * @param[in|out] value The value.
 *
 * When the reference counter reach zero, the value is freed.
 */
QX_API void qxJsonValueUnref(QxJsonValue *value);

/**
 * @brief Get the type of the value.
 * @param[in] The value.
 * @return The unique identifier of the type of the value.
 */
QX_API QxJsonValueType qxJsonValueType(QxJsonValue const *value);

/**
 * @brief Test that a JSON value has the requiered type.
 * @param value The value to be tested.
 * @param type The requiered type.
 * @return 0 if the type has not the requiered type.
 */
#define QX_JSON_IS(value, type) \
	(qxJsonValueType(value) == QxJsonValueType##type)

/**
 * @brief Dynamically cast a value into the given type.
 * @param value The value to be casted.
 * @param type The required type.
 * @return The casted value on success or a null pointer otherwise.
 */
#define QX_JSON_CAST(value, type) \
	((QxJson##type *)(QX_JSON_IS((value), type) ? (value) : 0))

#endif /* _H_QX_JSON_VALUE */

