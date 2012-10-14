/**
 * @file qx.js.number.h
 * @brief Header file of the QxJsNumber class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JS_NUMBER
#define _H_QX_JS_NUMBER

#include "qx.macro.h"
#include "qx.js.value.h"

#define QX_JS_NUMBER(value) (qxJsValueIsNumber(value) ? (QxJsNumber *)(value) : NULL )

/**
 * @brief Class that handle a JavaScript number value.
 */
typedef struct QxJsNumber QxJsNumber;

/**
 * @brief Native type of a JavaScript number.
 */
typedef double qx_js_number_t;

/**
 * @brief Create a new number value.
 * @return A JavaScript number value.
 */
QX_API QxJsValue *qxJsNumberNew(qx_js_number_t number);

/**
 * @brief Get the native value of the number.
 * @param[in] value The number.
 * @return The native value of the input number.
 * @warning If the input number is a null pointer, 0 is returned.
 */
QX_API qx_js_number_t qxJsNumberValue(QxJsNumber const *number);

#endif /* _H_QX_JS_NUMBER */

