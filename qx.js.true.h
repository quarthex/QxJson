/**
 * @file qx.js.true.h
 * @brief Header file of the QxJsTrue class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JS_TRUE
#define _H_QX_JS_TRUE

#include "qx.macro.h"
#include "qx.js.value.h"

/**
 * @brief Class that handle a JavaScript true value.
 */
typedef struct QxJsTrue QxJsTrue;

/**
 * @brief Create a new true value.
 * @return A JavaScript true value.
 */
QX_API QxJsValue *qxJsTrueNew(void);

#endif /* _H_QX_JS_FALSE */

