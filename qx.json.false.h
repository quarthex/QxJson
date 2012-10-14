/**
 * @file qx.json.false.h
 * @brief Header file of the QxJsonFalse class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_FALSE
#define _H_QX_JSON_FALSE

#include "qx.macro.h"
#include "qx.json.value.h"

/**
 * @brief Class that handle a JavaScript false value.
 */
typedef struct QxJsonFalse QxJsonFalse;

/**
 * @brief Create a new false value.
 * @return A JavaScript false value.
 */
QX_API QxJsonValue *qxJsonFalseNew(void);

#endif /* _H_QX_JSON_FALSE */

