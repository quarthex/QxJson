/**
 * @file qx.json.tockenizer.h
 * @brief Header file ot the QxJsonTockenizer class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_TOCKENIZER
#define _H_QX_JSON_TOCKENIZER

#include <stddef.h>
#include "qx.json.macro.h"

/**
 * @brief JSON stream tockenizer.
 */
typedef struct QxJsonTockenizer QxJsonTockenizer;

/**
 * @brief The type of a JSON tocken.
 */
enum QxJsonTockenType
{
	QxJsonTockenString,
	QxJsonTockenNumber,
	QxJsonTockenFalse,
	QxJsonTockenTrue,
	QxJsonTockenNull,
	QxJsonTockenBeginArray,
	QxJsonTockenValueSeparator,
	QxJsonTockenEndArray,
	QxJsonTockenBeginObject,
	QxJsonTockenNameValueSeparator,
	QxJsonTockenEndObject
};
typedef enum QxJsonTockenType QxJsonTockenType;

/**
 * @brief A JSON tocken raised by the tockenizer.
 */
struct QxJsonTocken
{
	QxJsonTockenType type;
	wchar_t const *data;
	size_t length;
	void *userData;
};
typedef struct QxJsonTocken QxJsonTocken;

/**
 * @brief Create a new tockenizer.
 * @param[in]  callback A pointer to a function called each time a new tocken
 *                      has been detected.
 * @param[out] userData An user defined pointer.
 * @return A tockenizer on success. A null pointer otherwise.
 *
 * If the called function returns a non 0 result, it is considered as a failure
 * and the value is returned by the caller (qxJsonTockenizerWrite or
 * qxJsonTockenizerFlush).
 */
QX_API QxJsonTockenizer *qxJsonTockenizerNew(
	int(*callback)(QxJsonTocken const *tocken), void *userData);

/**
 * @brief Delete a tockenizer.
 * @param[out] tockenizer The tockenizer to be freed.
 *
 * Memory that could be allocated for the user pointer will not be freed.
 */
QX_API void qxJsonTockenizerDelete(QxJsonTockenizer *tockenizer);

/**
 * @brief Write data to the tockenizer.
 * @param[out] tockenizer The tockenizer.
 * @param[in] data The unicode data to be written.
 * @param[in] length The characters count.
 * @return 0 on success.
 */
QX_API int qxJsonTockenizerWrite(QxJsonTockenizer *tockenizer,
	wchar_t const *data, size_t length);

/**
 * @brief Flush the JSON stream.
 * @param[out] tockenizer The tockenizer.
 * @return 0 on success.
 *
 * All pending characters are treated as if the end of the stream were reached.
 * For instance, it might be usefull to end up plain decimal number parsing.
 */
QX_API int qxJsonTockenizerFlush(QxJsonTockenizer *tockenizer);

#endif /* _H_QX_JSON_TOCKENIZER */

