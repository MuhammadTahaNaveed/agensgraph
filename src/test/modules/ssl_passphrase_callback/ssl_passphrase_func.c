/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*-------------------------------------------------------------------------
 *
 * ssl_passphrase_func.c
 *
 * Loadable PostgreSQL module fetch an ssl passphrase for the server cert.
 * instead of calling an external program. This implementation just hands
 * back the configured password rot13'd.
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include <float.h>
#include <stdio.h>

#include "libpq/libpq.h"
#include "libpq/libpq-be.h"
#include "utils/guc.h"

PG_MODULE_MAGIC;

void		_PG_init(void);
void		_PG_fini(void);

static char *ssl_passphrase = NULL;

/* callback function */
static int	rot13_passphrase(char *buf, int size, int rwflag, void *userdata);

/* hook function to set the callback */
static void set_rot13(SSL_CTX *context, bool isServerStart);

/*
 * Module load callback
 */
void
_PG_init(void)
{
	/* Define custom GUC variable. */
	DefineCustomStringVariable("ssl_passphrase.passphrase",
							   "passphrase before transformation",
							   NULL,
							   &ssl_passphrase,
							   NULL,
							   PGC_SIGHUP,
							   0,	/* no flags required */
							   NULL,
							   NULL,
							   NULL);
	if (ssl_passphrase)
		openssl_tls_init_hook = set_rot13;
}

void
_PG_fini(void)
{
	/* do  nothing yet */
}

static void
set_rot13(SSL_CTX *context, bool isServerStart)
{
	/* warn if the user has set ssl_passphrase_command */
	if (ssl_passphrase_command[0])
		ereport(WARNING,
				(errmsg("ssl_passphrase_command setting ignored by ssl_passphrase_func module")));

	SSL_CTX_set_default_passwd_cb(context, rot13_passphrase);
}

static int
rot13_passphrase(char *buf, int size, int rwflag, void *userdata)
{

	Assert(ssl_passphrase != NULL);
	StrNCpy(buf, ssl_passphrase, size);
	for (char *p = buf; *p; p++)
	{
		char		c = *p;

		if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
			*p = c + 13;
		else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
			*p = c - 13;
	}

	return strlen(buf);

}
