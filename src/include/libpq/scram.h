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
 * scram.h
 *	  Interface to libpq/scram.c
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/libpq/scram.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_SCRAM_H
#define PG_SCRAM_H

#include "lib/stringinfo.h"
#include "libpq/libpq-be.h"
#include "libpq/sasl.h"

/* SASL implementation callbacks */
extern PGDLLIMPORT const pg_be_sasl_mech pg_be_scram_mech;

/* Routines to handle and check SCRAM-SHA-256 secret */
extern char *pg_be_scram_build_secret(const char *password);
extern bool parse_scram_secret(const char *secret, int *iterations, char **salt,
							   uint8 *stored_key, uint8 *server_key);
extern bool scram_verify_plain_password(const char *username,
										const char *password, const char *secret);

#endif							/* PG_SCRAM_H */
