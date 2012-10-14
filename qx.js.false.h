/**
 * @file qx.js.false.h
 * @brief Header file of the QxJsFalse class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JS_FALSE
#define _H_QX_JS_FALSE

#include "qx.macro.h"
#include "qx.js.value.h"

/**
 * @brief Class that handle a JavaScript false value.
 */
typedef struct QxJsFalse QxJsFalse;

/**
 * @brief Create a new false value.
 * @return A JavaScript false value.
 */
QX_API QxJsValue *qxJsFalseNew(void);

#endif /* _H_QX_JS_FALSE */

