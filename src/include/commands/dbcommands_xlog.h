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
 * dbcommands_xlog.h
 *		Database resource manager XLOG definitions (create/drop database).
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/dbcommands_xlog.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef DBCOMMANDS_XLOG_H
#define DBCOMMANDS_XLOG_H

#include "access/xlogreader.h"
#include "lib/stringinfo.h"

/* record types */
#define XLOG_DBASE_CREATE_FILE_COPY		0x00
#define XLOG_DBASE_CREATE_WAL_LOG		0x10
#define XLOG_DBASE_DROP					0x20

/*
 * Single WAL record for an entire CREATE DATABASE operation. This is used
 * by the FILE_COPY strategy.
 */
typedef struct xl_dbase_create_file_copy_rec
{
	Oid			db_id;
	Oid			tablespace_id;
	Oid			src_db_id;
	Oid			src_tablespace_id;
} xl_dbase_create_file_copy_rec;

/*
 * WAL record for the beginning of a CREATE DATABASE operation, when the
 * WAL_LOG strategy is used. Each individual block will be logged separately
 * afterward.
 */
typedef struct xl_dbase_create_wal_log_rec
{
	Oid			db_id;
	Oid			tablespace_id;
} xl_dbase_create_wal_log_rec;

typedef struct xl_dbase_drop_rec
{
	Oid			db_id;
	int			ntablespaces;	/* number of tablespace IDs */
	Oid			tablespace_ids[FLEXIBLE_ARRAY_MEMBER];
} xl_dbase_drop_rec;
#define MinSizeOfDbaseDropRec offsetof(xl_dbase_drop_rec, tablespace_ids)

extern void dbase_redo(XLogReaderState *rptr);
extern void dbase_desc(StringInfo buf, XLogReaderState *rptr);
extern const char *dbase_identify(uint8 info);

#endif							/* DBCOMMANDS_XLOG_H */
