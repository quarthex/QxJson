/**
 * @file qx.json.object.h
 * @brief Header file of the QxJsonNull class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_OBJECT
#define _H_QX_JSON_OBJECT

#include "qx.json.macro.h"
#include "qx.json.value.h"

#define QX_JSON_IS_OBJECT(value) QX_JSON_IS((value), Object)

/**
 * @brief Class that handle a JavaScript object value.
 */
typedef struct QxJsonObject QxJsonObject;

/**
 * @brief Create a new object value.
 * @return A JavaScript object value.
 */
QX_API QxJsonValue *qxJsonObjectNew(void);

#endif /* _H_QX_JSON_OBJECT */

