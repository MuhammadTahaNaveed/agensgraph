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
 * md5.h
 *	  Interface to libpq/md5.c
 *
 * These definitions are needed by both frontend and backend code to work
 * with MD5-encrypted passwords.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/common/md5.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_MD5_H
#define PG_MD5_H

#define MD5_PASSWD_CHARSET	"0123456789abcdef"
#define MD5_PASSWD_LEN	35

extern bool pg_md5_hash(const void *buff, size_t len, char *hexsum);
extern bool pg_md5_binary(const void *buff, size_t len, void *outbuf);
extern bool pg_md5_encrypt(const char *passwd, const char *salt,
						   size_t salt_len, char *buf);

#endif
