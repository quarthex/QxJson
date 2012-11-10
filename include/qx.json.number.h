/**
 * @file qx.json.number.h
 * @brief Header file of the QxJsonNumber class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_NUMBER
#define _H_QX_JSON_NUMBER

#include "qx.macro.h"
#include "qx.json.value.h"

#define QX_JSON_IS_NUMBER(value) QX_JSON_IS((value), Number)
#define QX_JSON_NUMBER(value) QX_JSON_CAST((value), Number)

/**
 * @brief Class that handle a JavaScript number value.
 */
typedef struct QxJsonNumber QxJsonNumber;

/**
 * @brief Native type of a JavaScript number.
 */
typedef double qx_json_number_t;

/**
 * @brief Create a new number value.
 * @return A JavaScript number value.
 */
QX_API QxJsonValue *qxJsonNumberNew(qx_json_number_t number);

/**
 * @brief Get the native value of the number.
 * @param[in] value The number.
 * @return The native value of the input number.
 * @warning If the input number is a null pointer, 0 is returned.
 */
QX_API qx_json_number_t qxJsonNumberValue(QxJsonNumber const *number);

#endif /* _H_QX_JSON_NUMBER */

