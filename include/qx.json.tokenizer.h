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
	wchar_t *data;
	size_t size;
};
typedef struct QxJsonToken QxJsonToken;

/**
 * @brief Create a new tokenizer.
 * @return A tokenizer on success. A null pointer otherwise.
 */
QX_API QxJsonTokenizer *QxJsonTokenizer_new(void);

/**
 * @brief Associate an handler to the tokenizer.
 * @param[out] self The tokenizer.
 * @param[in] callback The address of a function.
 * @param[in|out] userData The user pointer passed to the handler.
 *
 * Each time the tokenizer raise a token, it calls the @c callback function
 * with two parameters: a read-only instance of the token and the user pointer.
 *
 * The data addressed by @c token->data are not persistent.
 *
 * The user pointer can be used to pass additional parameters to the callback
 * like, for instance, contextual informations.
 */
QX_API void QxJsonTokenizer_setHandler(QxJsonTokenizer *self,
	int(*callback)(QxJsonToken const *token, void *userData), void *userData);

/**
 * @brief Delete a tokenizer.
 * @param[out] self The tokenizer to be freed.
 */
QX_API void QxJsonTokenizer_delete(QxJsonTokenizer *self);

/**
 * @brief Write data to the tokenizer.
 * @param[out] self The tokenizer.
 * @param[in] data The unicode data to be written.
 * @param[in] size The characters count.
 * @return 0 on success.
 */
QX_API int QxJsonTokenizer_write(QxJsonTokenizer *self,
	wchar_t const *data, size_t size);

/**
 * @brief Flush the JSON stream.
 * @param[out] self The tokenizer.
 * @return 0 on success.
 *
 * All pending characters are treated as if the end of the stream were reached.
 * For instance, it might be usefull to end up plain decimal number parsing.
 */
QX_API int QxJsonTokenizer_flush(QxJsonTokenizer *self);

#endif /* _H_QX_JSON_TOKENIZER */
