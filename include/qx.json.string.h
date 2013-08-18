/**
 * @file qx.json.string.h
 * @brief Header file of the QxJsonString class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_STRING
#define _H_QX_JSON_STRING

#include <stddef.h>
#include "qx.json.macro.h"
#include "qx.json.value.h"

#define QX_JSON_IS_STRING(value) QX_JSON_IS((value), String)
#define QX_JSON_STRING(value) QX_JSON_CAST((value), String)

/**
 * @brief Class that handle a JavaScript string value.
 */
typedef struct QxJsonString QxJsonString;

/**
 * @brief Native type of a JavaScript string.
 */
typedef wchar_t const *qx_json_string_t;

/**
 * @brief Create a new string value.
 * @param[in] data The unicode data.
 * @param[in] size The amount of characters.
 * @return A JavaScript string value.
 * @warning If @c data is a null pointer, a null pointer is returned.
 * @warning An overvalued @c size may lead to unexpected results.
 */
QX_API QxJsonValue *QxJsonString_new(qx_json_string_t data, size_t size);

/**
 * @brief Get the native data of the string.
 * @param[in] string The string.
 * @return The native data of the input string.
 * @warning If the input string is a null pointer, a null pointer is returned.
 */
QX_API qx_json_string_t QxJsonString_data(QxJsonString const *string);

/**
 * @brief Get the size of the string.
 * @param[in] string The string.
 * @return The size of the input string.
 * @warning If the input string is a null pointer, 0 is returned.
 */
QX_API size_t QxJsonString_size(QxJsonString const *string);

#endif /* _H_QX_JSON_STRING */
