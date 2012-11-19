/**
 * @file qx.json.object.h
 * @brief Header file of the QxJsonNull class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_OBJECT
#define _H_QX_JSON_OBJECT

#include "qx.json.macro.h"
#include "qx.json.string.h"
#include "qx.json.value.h"

#define QX_JSON_IS_OBJECT(value) QX_JSON_IS((value), Object)
#define QX_JSON_OBJECT(value) QX_JSON_CAST((value), Object)

/**
 * @brief Class that handle a JavaScript object value.
 */
typedef struct QxJsonObject QxJsonObject;

/**
 * @brief Create a new object value.
 * @return A JavaScript object value.
 */
QX_API QxJsonValue *qxJsonObjectNew(void);

/**
 * @brief Associate a value to a key in the object.
 * @param[out] object The object to be modified.
 * @param[in] key The altered key.
 * @param[in] value The associated value.
 * @return 0 if the value have successfully been associated to the key.
 * @warning The internal reference counter of the value is not incremented.
 */
QX_API int qxJsonObjectSet(QxJsonObject *object, QxJsonString *key,
							QxJsonValue * value);

/**
 * @brief Remove a key from the object.
 * @param[out] object The object to be modified.
 * @param[in] key The key to be removed.
 * @return 0 if the key have successfully be removed or if it did not exists.
 */
QX_API int qxJsonObjectUnset(QxJsonObject *object, QxJsonString *key);

/**
 * @brief Return the key count of the object.
 * @param[in] object The object.
 * @return The amount of keys the object handles.
 */
QX_API size_t qxJsonObjectSize(QxJsonObject *object);

#endif /* _H_QX_JSON_OBJECT */

