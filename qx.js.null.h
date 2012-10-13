/**
 * @file qx.js.null.h
 * @brief Header file of the QxJsNull class.
 * @author Romain Déoux <romain.deoux@gmail.com>
 */

#ifndef _H_QX_JS_NULL
#define _H_QX_JS_NULL

#include "qx.macro.h"
#include "qx.js.value.h"

/**
 * @brief Class that handle a JavaScript null value.
 */
typedef struct QxJsNull QxJsNull;

/**
 * @brief Create a new null value.
 * @return A JavaScript null value.
 */
QX_API QxJsValue *qxJsNullNew(void);

#endif /* _H_QX_JS_NULL */

