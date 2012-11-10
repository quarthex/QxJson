/**
 * @file qx.json.null.h
 * @brief Header file of the QxJsonNull class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_NULL
#define _H_QX_JSON_NULL

#include "qx.macro.h"
#include "qx.json.value.h"

#define QX_JSON_IS_NULL(value) QX_JSON_IS((value), Null)

/**
 * @brief Class that handle a JavaScript null value.
 */
typedef struct QxJsonNull QxJsonNull;

/**
 * @brief Create a new null value.
 * @return A JavaScript null value.
 */
QX_API QxJsonValue *qxJsonNullNew(void);

#endif /* _H_QX_JSON_NULL */

