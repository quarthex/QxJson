/**
 * @file qx.json.true.h
 * @brief Header file of the QxJsonTrue class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_TRUE
#define _H_QX_JSON_TRUE

#include "qx.json.macro.h"
#include "qx.json.value.h"

#define QX_JSON_IS_TRUE(value) QX_JSON_IS((value), True)

/**
 * @brief Class that handle a JavaScript true value.
 */
typedef struct QxJsonTrue QxJsonTrue;

/**
 * @brief Create a new true value.
 * @return A JavaScript true value.
 */
QX_API QxJsonValue *qxJsonTrueNew(void);

#endif /* _H_QX_JSON_TRUE */
