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
 * walmethods.h
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *		  src/bin/pg_basebackup/walmethods.h
 *-------------------------------------------------------------------------
 */


typedef void *Walfile;

typedef enum
{
	CLOSE_NORMAL,
	CLOSE_UNLINK,
	CLOSE_NO_RENAME
} WalCloseMethod;

/*
 * A WalWriteMethod structure represents the different methods used
 * to write the streaming WAL as it's received.
 *
 * All methods that have a failure return indicator will set state
 * allowing the getlasterror() method to return a suitable message.
 * Commonly, errno is this state (or part of it); so callers must take
 * care not to clobber errno between a failed method call and use of
 * getlasterror() to retrieve the message.
 */
typedef struct WalWriteMethod WalWriteMethod;
struct WalWriteMethod
{
	/*
	 * Open a target file. Returns Walfile, or NULL if open failed. If a temp
	 * suffix is specified, a file with that name will be opened, and then
	 * automatically renamed in close(). If pad_to_size is specified, the file
	 * will be padded with NUL up to that size, if supported by the Walmethod.
	 */
	Walfile		(*open_for_write) (const char *pathname, const char *temp_suffix, size_t pad_to_size);

	/*
	 * Close an open Walfile, using one or more methods for handling automatic
	 * unlinking etc. Returns 0 on success, other values for error.
	 */
	int			(*close) (Walfile f, WalCloseMethod method);

	/* Check if a file exist */
	bool		(*existsfile) (const char *pathname);

	/* Return the size of a file, or -1 on failure. */
	ssize_t		(*get_file_size) (const char *pathname);

	/*
	 * Return the name of the current file to work on in pg_malloc()'d string,
	 * without the base directory.  This is useful for logging.
	 */
	char	   *(*get_file_name) (const char *pathname, const char *temp_suffix);

	/* Return the level of compression */
	int			(*compression) (void);

	/*
	 * Write count number of bytes to the file, and return the number of bytes
	 * actually written or -1 for error.
	 */
	ssize_t		(*write) (Walfile f, const void *buf, size_t count);

	/* Return the current position in a file or -1 on error */
	off_t		(*get_current_pos) (Walfile f);

	/*
	 * fsync the contents of the specified file. Returns 0 on success.
	 */
	int			(*sync) (Walfile f);

	/*
	 * Clean up the Walmethod, closing any shared resources. For methods like
	 * tar, this includes writing updated headers. Returns true if the
	 * close/write/sync of shared resources succeeded, otherwise returns false
	 * (but the resources are still closed).
	 */
	bool		(*finish) (void);

	/* Return a text for the last error in this Walfile */
	const char *(*getlasterror) (void);
};

/*
 * Available WAL methods:
 *	- WalDirectoryMethod - write WAL to regular files in a standard pg_wal
 *	- WalTarMethod       - write WAL to a tarfile corresponding to pg_wal
 *						   (only implements the methods required for pg_basebackup,
 *						   not all those required for pg_receivewal)
 */
WalWriteMethod *CreateWalDirectoryMethod(const char *basedir,
										 int compression, bool sync);
WalWriteMethod *CreateWalTarMethod(const char *tarbase, int compression, bool sync);

/* Cleanup routines for previously-created methods */
void		FreeWalDirectoryMethod(void);
void		FreeWalTarMethod(void);
