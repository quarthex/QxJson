/**
 * @file qx.json.parser.h
 * @brief Header file of the QxJsonParser class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_PARSER
#define _H_QX_JSON_PARSER

#include <stddef.h>

#include "qx.json.tokenizer.h"

/**
 * @brief The QxJsonParser class.
 */
typedef struct QxJsonParser QxJsonParser;

/**
 * @brief The unique identifier of a value type.
 */
typedef enum QxJsonValueType
{
	QxJsonValueTypeNull,
	QxJsonValueTypeTrue,
	QxJsonValueTypeFalse,
	QxJsonValueTypeNumber,
	QxJsonValueTypeString,
	QxJsonValueTypeArray,
	QxJsonValueTypeObject
} QxJsonValueType;

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
	} spec;
};

/**
 * @brief Interface of a value factory.
 */
typedef void *(*QxJsonValueFactory)(QxJsonValueSpec const *spec, void *userPtr);

/**
 * @brief Create a new parser.
 * @param factory A value factory.
 * @param userPtr Custom pointer forwarded to the factory.
 * @return A parser instance.
 *
 * If the factory return a NULL value, the parser fails.
 * The parser is not responsible of the memory managment of the created values.
 */
QxJsonParser *QxJsonParser_new(QxJsonValueFactory factory, void *userPtr);

/**
 * @brief Destroy a parser.
 * @param self The instance to be destroyed.
 */
void QxJsonParser_delete(QxJsonParser *self);

/**
 * @brief Feed the parser with a new token.
 * @param self The parser instance.
 * @param token The new token.
 * @return 0 on success.
 */
int QxJsonParser_feed(QxJsonParser *self, QxJsonToken const *token);

#endif /* _H_QX_JSON_PARSER */
