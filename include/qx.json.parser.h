/**
 * @file qx.json.parser.h
 * @brief Header file of the QxJsonParser class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_PARSER
#define _H_QX_JSON_PARSER

#include <stddef.h>

#include "qx.json.tokenizer.h"
#include "qx.json.value.h"

/**
 * @brief The QxJsonParser class.
 */
typedef struct QxJsonParser QxJsonParser;

/*! Value description processed by a value factory */
typedef struct QxJsonValueSpec QxJsonValueSpec;
struct QxJsonValueSpec
{
	/*! Depth of the value to be created (0 for root values) */
	unsigned int depth;

	/*! Type of the value */
	QxJsonValueType type;

	/*! Informations needed to create the value */
	union
	{
		struct { wchar_t const *data; size_t size; } string;
		struct { wchar_t const *data; size_t size; } number;
		struct { void **values; size_t size; } array;
		struct { void **keys; void **values; size_t size; } object;
	} data;
};

/**
 * @brief Interface of a value factory.
 */
typedef void *(*QxJsonValueFactory)(QxJsonValueSpec const *spec, void *userPtr);

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
 * @param token The new token.
 * @return 0 on success.
 */
QX_API int QxJsonParser_feed(QxJsonParser *self, QxJsonToken const *token);

/**
 * @brief Ends the stream parsing.
 * @param self The parser instance.
 * @param value The parsed value if any.
 * @return 0 on success.
 */
QX_API int QxJsonParser_end(QxJsonParser *self, QxJsonValue **value);

#endif /* _H_QX_JSON_PARSER */
