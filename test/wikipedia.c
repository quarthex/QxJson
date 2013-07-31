/**
 * @file wikipedia.c
 * @brief Testing use case using the Wikipedia JSON example.
 * @author Romain DEOUX
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <qx.json.tokenizer.h>

#include "expect.h"

struct Wikipedia
{
	char buffer[1024];
	size_t size;
};
typedef struct Wikipedia Wikipedia;

static void ascii2unicode(wchar_t *dest, char const *src, size_t size)
{
	for (; size; ++dest, ++src, --size)
		*dest = (wchar_t)*src;
}

static void unicode2ascii(char *dest, wchar_t const *src, size_t size)
{
	for (; size; ++dest, ++src, --size)
		*dest = (char)*src;
}

static char const *typeIdentifier(QxJsonTokenType type)
{
	switch (type)
	{
	case QxJsonTokenString:             return "s ";
	case QxJsonTokenNumber:             return "n ";
	case QxJsonTokenFalse:              return "f";
	case QxJsonTokenTrue:               return "t";
	case QxJsonTokenNull:               return "n";
	case QxJsonTokenBeginArray:         return "ba";
	case QxJsonTokenValuesSeparator:    return "vs";
	case QxJsonTokenEndArray:           return "ea";
	case QxJsonTokenBeginObject:        return "bo";
	case QxJsonTokenNameValueSeparator: return "nvs";
	case QxJsonTokenEndObject:          return "eo";
	}

	expect_ok(0);
	return NULL;
}

static int Wikipedia_feed(QxJsonToken const *token, void *userData)
{
	Wikipedia *const self = (Wikipedia *)userData;

	self->size += sprintf(self->buffer + self->size, typeIdentifier(token->type));

	switch (token->type)
	{
	case QxJsonTokenString:
	case QxJsonTokenNumber:
		expect_not_zero(token->size);
		expect_not_null(token->data);
		unicode2ascii(self->buffer + self->size, token->data, token->size);
		self->size += token->size;
		break;

	default:
		expect_zero(token->size);
		expect_null(token->data);
	}

	self->buffer[self->size] = '\n';
	++self->size;
	return 0;
}

int main(void)
{
	int fd;
	char buffer[512];
	wchar_t wbuffer[sizeof(buffer)];
	ssize_t bufferSize;
	Wikipedia wikipedia;
	QxJsonTokenizer *tokenizer;

	/* Create the tokenizer */
	wikipedia.size = 0;
	tokenizer = QxJsonTokenizer_new(&Wikipedia_feed, &wikipedia);
	expect_not_null(tokenizer);

	/* Read the JSON file */
	fd = open("wikipedia.json", O_RDONLY);
	expect_ok(fd >= 0);
	bufferSize = read(fd, buffer, sizeof(buffer));
	expect_ok(bufferSize > 0);
	close(fd);

	/* Feed the tokenizer */
	ascii2unicode(wbuffer, buffer, bufferSize);
	expect_zero(QxJsonTokenizer_feed(tokenizer, wbuffer, bufferSize));
	expect_zero(QxJsonTokenizer_end(tokenizer));

	/* Read the parsed file */
	fd = open("wikipedia.parsed", O_RDONLY);
	expect_ok(fd >= 0);
	bufferSize = read(fd, buffer, sizeof(buffer));
	expect_ok(bufferSize > 0);
	close(fd);

	/* Compare */
	expect_int_equal(bufferSize, wikipedia.size);
	buffer[bufferSize] = '\0';
	expect_str_equal(buffer, wikipedia.buffer);
	expect_zero(memcmp(buffer, wikipedia.buffer, bufferSize));

	return EXIT_SUCCESS;
}
