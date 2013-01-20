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
	QxJsonTockenValuesSeparator,
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
};
typedef struct QxJsonTocken QxJsonTocken;

/**
 * @brief Create a new tockenizer.
 * @return A tockenizer on success. A null pointer otherwise.
 */
QX_API QxJsonTockenizer *qxJsonTockenizerNew(void);

/**
 * @brief Delete a tockenizer.
 * @param[out] tockenizer The tockenizer to be freed.
 */
QX_API void qxJsonTockenizerDelete(QxJsonTockenizer *tockenizer);

/**
 * @brief Write data to the tockenizer.
 * @param[out] tockenizer The tockenizer.
 * @param[in] data The unicode data to be written.
 * @param[in] size The characters count.
 * @return 0 on success.
 */
QX_API int qxJsonTockenizerWrite(QxJsonTockenizer *tockenizer,
	wchar_t const *data, size_t size);

/**
 * @brief Flush the JSON stream.
 * @param[out] tockenizer The tockenizer.
 * @return 0 on success.
 *
 * All pending characters are treated as if the end of the stream were reached.
 * For instance, it might be usefull to end up plain decimal number parsing.
 */
QX_API int qxJsonTockenizerFlush(QxJsonTockenizer *tockenizer);

/**
 * @brief Get the next tocken in the pending tocken list of the tockenizer.
 * @param[in] tockenizer The tockenizer.
 * @param[out] tocken A valid pointer to a QxJsonTocken instance.
 * @return 1 if a tocken was successfully copied to @c tocken.
 *         0 if the tockens list is empty.
 *         Any other return values indicate a failure.
 */
QX_API int qxJsonTockenizerNextTocken(QxJsonTockenizer *tockenizer,
	QxJsonTocken *tocken);

#endif /* _H_QX_JSON_TOCKENIZER */

