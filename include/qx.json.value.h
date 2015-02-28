/**
 * @file qx.json.value.h
 * @brief Header file of the QxJsonValue class
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_VALUE
#define _H_QX_JSON_VALUE

#include <stddef.h>

#include "qx.json.macro.h"

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
 * @brief Variant type that handle any JavaScript value kind.
 */
typedef struct QxJsonValue QxJsonValue;

/**
 * @brief Increment the reference counter of a value.
 * @param value The value.
 */
QX_API void QxJsonValue_retains(QxJsonValue *self);

/**
 * @brief Decrement the reference counter of a value.
 * @param value The value.
 *
 * When the reference counter reach zero, the value is freed.
 */
QX_API void QxJsonValue_release(QxJsonValue *self);

/**
 * @brief Get the type of the value.
 * @param The value.
 * @return The unique identifier of the type of the value.
 */
QX_API QxJsonValueType QxJsonValue_type(QxJsonValue const *self);

/**
 * @brief Test that a JSON value has the requiered type.
 * @param value The value to be tested.
 * @param type The requiered type.
 * @return 0 if the type has not the requiered type.
 */
#define QX_JSON_IS(value, type) \
	(QxJsonValue_type(value) == QxJsonValueType##type)
#define QX_JSON_IS_ARRAY(value)  QX_JSON_IS((value), Array)
#define QX_JSON_IS_FALSE(value)  QX_JSON_IS((value), False)
#define QX_JSON_IS_NULL(value)   QX_JSON_IS((value), Null)
#define QX_JSON_IS_NUMBER(value) QX_JSON_IS((value), Number)
#define QX_JSON_IS_OBJECT(value) QX_JSON_IS((value), Object)
#define QX_JSON_IS_STRING(value) QX_JSON_IS((value), String)
#define QX_JSON_IS_TRUE(value)   QX_JSON_IS((value), True)

/**
 * @brief Get the size of the value.
 * @param self The value.
 * @return The size of the value.
 *
 * array:  The number of items.
 * object: The number of key/value pairs.
 * string: The length of the string.
 * false/null/number/true: 0.
 */
QX_API size_t QxJsonValue_size(QxJsonValue const *self);

/* Array */

/**
 * @brief Create a new array value.
 * @return A JavaScript array value.
 */
QX_API QxJsonValue *QxJsonValue_arrayNew(void);

/**
 * @brief Append a value to the array.
 * @param array The array.
 * @param value The value to be appended.
 * @return 0 on success.
 *
 * The reference counter of the value is incremented.
 */
QX_API int QxJsonValue_arrayAppend(QxJsonValue *self, QxJsonValue *value);

/**
 * @brief Append a value to the array.
 * @param array The array.
 * @param value The value to be appended.
 * @return 0 on success.
 *
 * The reference counter of the value is not incremented.
 */
QX_API int QxJsonValue_arrayAppendNew(QxJsonValue *self, QxJsonValue *value);

/**
 * @brief Prepend a value to the array.
 * @param array The array.
 * @param value The value to be prepended.
 * @return 0 on success.
 *
 * The reference counter of the value is incremented.
 */
QX_API int QxJsonValue_arrayPrepend(QxJsonValue *self, QxJsonValue *value);

/**
 * @brief Prepend a value to the array.
 * @param array The array.
 * @param value The value to be prepended.
 * @return 0 on success.
 *
 * The reference counter of the value is not incremented.
 */
QX_API int QxJsonValue_arrayPrependNew(QxJsonValue *self, QxJsonValue *value);

/**
 * @brief Insert a value into the array.
 * @param array The array.
 * @param index The index where the value must be inserted.
 * @param value The value to insert.
 * @return 0 on success.
 *
 * The reference counter of the value is incremented.
 */
QX_API int QxJsonValue_arrayInsert(QxJsonValue *self, size_t index, QxJsonValue *value);

/**
 * @brief Insert a value into the array.
 * @param array The array.
 * @param index The index where the value must be inserted.
 * @param value The value to insert.
 * @return 0 on success.
 *
 * The reference counter of the value is not incremented.
 */
QX_API int QxJsonValue_arrayInsertNew(QxJsonValue *self, size_t index, QxJsonValue *value);

/**
 * @brief Get a value in the array.
 * @param self  The array.
 * @param index The index of the value in the array.
 * @return The value on success. A null pointer otherwise.
 */
QX_API QxJsonValue const *QxJsonValue_arrayGet(QxJsonValue const *self, size_t index);

/**
 * @brief Walk throught an array.
 * @param self     The array.
 * @param callback A function called for each value.
 * @param ptr      A custome pointer forwarded to the callback.
 * @return 0 on success.
 *
 * @c callback is called once for each value of the array.
 * @c callback should return a non zero value to stop the walking loop.
 */
QX_API int QxJsonValue_arrayEach(QxJsonValue *self,
	int (*callback)(size_t index, QxJsonValue *value, void *ptr), void *ptr);

/* False */

/**
 * @brief Create a new false value.
 * @return A JavaScript false value.
 */
QX_API QxJsonValue *QxJsonValue_falseNew(void);

/* Null */

/**
 * @brief Create a new null value.
 * @return A JavaScript null value.
 */
QX_API QxJsonValue *QxJsonValue_nullNew(void);

/* Number */

/**
 * @brief Create a new number value.
 * @param value A finite floating point value.
 * @return A JavaScript number value.
 */
QX_API QxJsonValue *QxJsonValue_numberNew(double value);

/**
 * @brief Get the number value.
 * @brief self A value where type equals QxJsonValueTypeNumber.
 * @return A double valid value or NaN if the value have not the right type.
 */
QX_API double QxJsonValue_numberValue(QxJsonValue const *self);

/* Object */

/**
 * @brief Create a new object value.
 * @return A JavaScript object value.
 */
QX_API QxJsonValue *QxJsonValue_objectNew(void);

/**
 * @brief Associate a value to a key in the object.
 * @param object The object to be modified.
 * @param key The altered key.
 * @param value The associated value.
 * @return 0 if the value have successfully been associated to the key.
 * @warning The internal reference counter of the value is not incremented.
 */
QX_API int QxJsonValue_objectSet(QxJsonValue *self, QxJsonValue *key, QxJsonValue * value);

/**
 * @brief Remove a key from the object.
 * @param object The object to be modified.
 * @param key The key to be removed.
 * @return 0 if the key have successfully be removed or if it did not exists.
 */
QX_API int QxJsonValue_objectUnset(QxJsonValue *self, QxJsonValue *key);

/**
 * @brief Get a value from an object.
 * @param self  The object.
 * @param key   The key of the value to get.
 * @param value The output value.
 * @return 0 on success.
 */
QX_API int QxJsonValue_objectGet(QxJsonValue *self,
	QxJsonValue const *key, QxJsonValue **value);

/**
 * @brief Walk through an object.
 * @param self     The object.
 * @param callback A function called for each key/value pair.
 * @param ptr      A custome pointer forwarded to the callback.
 * @return 0 on success.
 *
 * @c callback is called once for each key/value pait of the object.
 * @c callback should return a non zero value to stop the walking loop.
 */
QX_API int QxJsonValue_objectEach(QxJsonValue *self,
	int (*callback)(QxJsonValue const *key, QxJsonValue *value, void *ptr),
	void *ptr);

/* String */

/**
 * @brief Create a new string value.
 * @return A JavaScript string value.
 */
QX_API QxJsonValue *QxJsonValue_stringNew(wchar_t const *data, size_t size);

/**
 * @brief Get the string value.
 * @brief self A value where type equals QxJsonValueTypeString.
 * @return A pointer to the wide string data or NULL if the value have not the
 *         right type.
 */
QX_API wchar_t const *QxJsonValue_stringValue(QxJsonValue const *self);

/* True */

/**
 * @brief Create a new true value.
 * @return A JavaScript true value.
 */
QX_API QxJsonValue *QxJsonValue_trueNew(void);

#endif /* _H_QX_JSON_VALUE */
