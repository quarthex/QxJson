/**
 * @file qx.json.array.h
 * @brief Header file of the QxJsonArray class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_ARRAY
#define _H_QX_JSON_ARRAY

#include "qx.macro.h"
#include "qx.json.value.h"

#define QX_JSON_IS_ARRAY(value) QX_JSON_IS((value), Array)
#define QX_JSON_ARRAY(value) QX_JSON_CAST((value), Array)

/**
 * @brief Class that handle a JavaScript array value.
 */
typedef struct QxJsonArray QxJsonArray;

/**
 * @brief Create a new array value.
 * @return A JavaScript array value.
 */
QX_API QxJsonValue *qxJsonArrayNew(void);

/**
 * @brief Get the array size.
 * @param[in] The array.
 * @return The size of the array.
 * @warning If @c array is a null pointer, 0 is returned.
 */
size_t qxJsonArraySize(QxJsonArray const *array);

/**
 * @brief Append a value to the array.
 * @param[out] array The array.
 * @param[in] value The value to be appended.
 * @return 0 on success.
 *
 * The reference counter of the value is incremented.
 */
int qxJsonArrayAppend(QxJsonArray *array, QxJsonValue *value);

/**
 * @brief Append a value to the array.
 * @param[out] array The array.
 * @param[in] value The value to be appended.
 * @return 0 on success.
 *
 * The reference counter of the value is not incremented.
 */
int qxJsonArrayAppendNew(QxJsonArray *array, QxJsonValue *value);

/**
 * @brief Prepend a value to the array.
 * @param[out] array The array.
 * @param[in] value The value to be prepended.
 * @return 0 on success.
 *
 * The reference counter of the value is incremented.
 */
int qxJsonArrayPrepend(QxJsonArray *array, QxJsonValue *value);

/**
 * @brief Prepend a value to the array.
 * @param[out] array The array.
 * @param[in] value The value to be prepended.
 * @return 0 on success.
 *
 * The reference counter of the value is not incremented.
 */
int qxJsonArrayPrependNew(QxJsonArray *array, QxJsonValue *value);

/**
 * @brief Get a value in the array.
 * @param[in] array The array.
 * @param[in] index The index of the value in the array.
 * @return The value on success. A null pointer otherwise.
 */
QxJsonValue const *qxJsonArrayGet(QxJsonArray *array, size_t index);

#endif /* _H_QX_JSON_ARRAY */

