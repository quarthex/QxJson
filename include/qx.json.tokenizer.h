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
 * @brief Interface of a tokens handler
 */
typedef struct QxJsonTokenizerHandler QxJsonTokenizerHandler;
struct QxJsonTokenizerHandler
{
	int(*feed)(QxJsonTokenizerHandler *self, QxJsonToken const *token);
};

/**
 * @brief Create a new tokenizer.
 * @param[int] handler The tokens handler.
 * @return A tokenizer on success. A null pointer otherwise.
 */
QX_API QxJsonTokenizer *QxJsonTokenizer_new(QxJsonTokenizerHandler *handler);

/**
 * @brief Delete a tokenizer.
 * @param[out] self The tokenizer to be freed.
 */
QX_API void QxJsonTokenizer_delete(QxJsonTokenizer *self);

/**
 * @brief Feed new data to the tokenizer.
 * @param[out] self The tokenizer instance.
 * @param[in] data A buffer containing the new data.
 * @param[in] size The amount of characters (not the bytes count).
 * @return 0 on success.
 */
QX_API int QxJsonTokenizer_feed(QxJsonTokenizer *self,
	wchar_t const *data, size_t size);

/**
 * @brief Notify the tokenizer about the end of the data stream.
 * @param[out] self The tokenizer instance.
 * @return 0 on success.
 */
QX_API int QxJsonTokenizer_end(QxJsonTokenizer *self);

#endif /* _H_QX_JSON_TOKENIZER */
