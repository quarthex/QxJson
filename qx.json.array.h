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

#endif /* _H_QX_JSON_ARRAY */

