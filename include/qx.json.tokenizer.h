/**
 * @file qx.json.tokenizer.h
 * @brief Header file ot the QxJsonTokenizer class.
 * @author Romain DEOUX
 */

#ifndef _H_QX_JSON_TOKENIZER
#define _H_QX_JSON_TOKENIZER

#include <stddef.h>
#include "qx.json.macro.h"

/**
 * @brief JSON stream tokenizer.
 */
typedef struct QxJsonTokenizer QxJsonTokenizer;

/**
 * @brief The type of a JSON token.
 */
enum QxJsonTokenType
{
	QxJsonTokenString,
	QxJsonTokenNumber,
	QxJsonTokenFalse,
	QxJsonTokenTrue,
	QxJsonTokenNull,
	QxJsonTokenBeginArray,
	QxJsonTokenValuesSeparator,
	QxJsonTokenEndArray,
	QxJsonTokenBeginObject,
	QxJsonTokenNameValueSeparator,
	QxJsonTokenEndObject
};
typedef enum QxJsonTokenType QxJsonTokenType;

/**
 * @brief A JSON token raised by the tokenizer.
 */
struct QxJsonToken
{
	QxJsonTokenType type;
	wchar_t const *data;
	size_t size;
};
typedef struct QxJsonToken QxJsonToken;

/**
 * @brief Create a new tokenizer.
 * @return A tokenizer on success. A null pointer otherwise.
 */
QX_API QxJsonTokenizer *QxJsonTokenizer_new(void);

/**
 * @brief Delete a tokenizer.
 * @param[out] self The tokenizer to be freed.
 */
QX_API void QxJsonTokenizer_delete(QxJsonTokenizer *self);

/**
 * @brief Reset the stream to be tokenized.
 * @param     self The tokenizer.
 * @param[in] data The stream data.
 * @param[in] size The size of the stream.
 * @return 0 on success. -1 otherwise.
 */
QX_API int QxJsonTokenizer_resetStream(QxJsonTokenizer *self,
	wchar_t const *data, size_t size);

/**
 * @brief Retrieve the next token of the JSON stream.
 * @param self The tokenizer.
 * @param[out] token The next token of the stream.
 * @return 1 on success. 0 if there is no more token. An error code otherwise.
 */
QX_API int QxJsonTokenizer_nextToken(QxJsonTokenizer *self, QxJsonToken *token);

#endif /* _H_QX_JSON_TOKENIZER */
