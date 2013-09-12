/**
 * @file qx.json.parser.h
 * @brief Header file of the QxJsonParser class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_PARSER
#define _H_QX_JSON_PARSER

#include <stddef.h>

#include "qx.json.value.h"

/**
 * @brief The QxJsonParser class.
 */
typedef struct QxJsonParser QxJsonParser;

/**
 * @brief Create a new parser.
 * @return A parser instance.
 */
QX_API QxJsonParser *QxJsonParser_new(void);

/**
 * @brief Destroy a parser.
 * @param self The instance to be destroyed.
 */
QX_API void QxJsonParser_release(QxJsonParser *self);

/**
 * @brief Feed the parser with a new token.
 * @param self The parser instance.
 * @param data Unicode chunck.
 * @param size Size of the unicode chunck.
 * @return 0 on success.
 */
QX_API int QxJsonParser_feed(QxJsonParser *self,
	wchar_t const *data, size_t size);

/**
 * @brief Ends the stream parsing.
 * @param self The parser instance.
 * @param value The parsed value if any.
 * @return 0 on success.
 */
QX_API int QxJsonParser_end(QxJsonParser *self, QxJsonValue **value);

#endif /* _H_QX_JSON_PARSER */
