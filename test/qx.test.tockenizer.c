/**
 * @file qx.test.tockenizer.c
 * @brief Testing source file of the QxJsonTockenizer class.
 * @author Romain DEOUX
 */

#include <qx.json.tockenizer.h>

#include "qx.assert.h"

static int tockenizerCallback(QxJsonTocken const *tocken)
{
	return -1;
}

int main(void)
{
	QxJsonTockenizer *const tockenizer = qxJsonTockenizerNew(tockenizerCallback, NULL);
	QX_ASSERT(tockenizer != NULL);
	qxJsonTockenizerDelete(tockenizer);
	return EXIT_SUCCESS;
}

