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

/*
 * src/interfaces/ecpg/include/sqlda-compat.h
 */

#ifndef ECPG_SQLDA_COMPAT_H
#define ECPG_SQLDA_COMPAT_H

struct sqlvar_compat
{
	short		sqltype;		/* variable type				*/
	int			sqllen;			/* length in bytes				*/
	char	   *sqldata;		/* pointer to data				*/
	short	   *sqlind;			/* pointer to indicator			*/
	char	   *sqlname;		/* variable name				*/
	char	   *sqlformat;		/* reserved for future use		*/
	short		sqlitype;		/* ind variable type			*/
	short		sqlilen;		/* ind length in bytes			*/
	char	   *sqlidata;		/* ind data pointer				*/
	int			sqlxid;			/* extended id type				*/
	char	   *sqltypename;	/* extended type name			*/
	short		sqltypelen;		/* length of extended type name */
	short		sqlownerlen;	/* length of owner name			*/
	short		sqlsourcetype;	/* source type for distinct of built-ins */
	char	   *sqlownername;	/* owner name					*/
	int			sqlsourceid;	/* extended id of source type	*/

	/*
	 * sqlilongdata is new.  It supports data that exceeds the 32k limit.
	 * sqlilen and sqlidata are for backward compatibility and they have
	 * maximum value of <32K.
	 */
	char	   *sqlilongdata;	/* for data field beyond 32K	*/
	int			sqlflags;		/* for internal use only		*/
	void	   *sqlreserved;	/* reserved for future use		*/
};

struct sqlda_compat
{
	short		sqld;
	struct sqlvar_compat *sqlvar;
	char		desc_name[19];	/* descriptor name				*/
	short		desc_occ;		/* size of sqlda structure		*/
	struct sqlda_compat *desc_next; /* pointer to next sqlda struct */
	void	   *reserved;		/* reserved for future use */
};

#endif							/* ECPG_SQLDA_COMPAT_H */
