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
 * vacuum.h
 *	  header file for postgres vacuum cleaner and statistics analyzer
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/vacuum.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef VACUUM_H
#define VACUUM_H

#include "access/htup.h"
#include "catalog/pg_class.h"
#include "catalog/pg_statistic.h"
#include "catalog/pg_type.h"
#include "parser/parse_node.h"
#include "storage/buf.h"
#include "storage/lock.h"
#include "utils/relcache.h"

/*
 * Flags for amparallelvacuumoptions to control the participation of bulkdelete
 * and vacuumcleanup in parallel vacuum.
 */

/*
 * Both bulkdelete and vacuumcleanup are disabled by default.  This will be
 * used by IndexAM's that don't want to or cannot participate in parallel
 * vacuum.  For example, if an index AM doesn't have a way to communicate the
 * index statistics allocated by the first ambulkdelete call to the subsequent
 * ones until amvacuumcleanup, the index AM cannot participate in parallel
 * vacuum.
 */
#define VACUUM_OPTION_NO_PARALLEL			0

/*
 * bulkdelete can be performed in parallel.  This option can be used by
 * IndexAm's that need to scan the index to delete the tuples.
 */
#define VACUUM_OPTION_PARALLEL_BULKDEL		(1 << 0)

/*
 * vacuumcleanup can be performed in parallel if bulkdelete is not performed
 * yet.  This will be used by IndexAM's that can scan the index if the
 * bulkdelete is not performed.
 */
#define VACUUM_OPTION_PARALLEL_COND_CLEANUP	(1 << 1)

/*
 * vacuumcleanup can be performed in parallel even if bulkdelete has already
 * processed the index.  This will be used by IndexAM's that scan the index
 * during the cleanup phase of index irrespective of whether the index is
 * already scanned or not during bulkdelete phase.
 */
#define VACUUM_OPTION_PARALLEL_CLEANUP		(1 << 2)

/* value for checking vacuum flags */
#define VACUUM_OPTION_MAX_VALID_VALUE		((1 << 3) - 1)

/*----------
 * ANALYZE builds one of these structs for each attribute (column) that is
 * to be analyzed.  The struct and subsidiary data are in anl_context,
 * so they live until the end of the ANALYZE operation.
 *
 * The type-specific typanalyze function is passed a pointer to this struct
 * and must return true to continue analysis, false to skip analysis of this
 * column.  In the true case it must set the compute_stats and minrows fields,
 * and can optionally set extra_data to pass additional info to compute_stats.
 * minrows is its request for the minimum number of sample rows to be gathered
 * (but note this request might not be honored, eg if there are fewer rows
 * than that in the table).
 *
 * The compute_stats routine will be called after sample rows have been
 * gathered.  Aside from this struct, it is passed:
 *		fetchfunc: a function for accessing the column values from the
 *				   sample rows
 *		samplerows: the number of sample tuples
 *		totalrows: estimated total number of rows in relation
 * The fetchfunc may be called with rownum running from 0 to samplerows-1.
 * It returns a Datum and an isNull flag.
 *
 * compute_stats should set stats_valid true if it is able to compute
 * any useful statistics.  If it does, the remainder of the struct holds
 * the information to be stored in a pg_statistic row for the column.  Be
 * careful to allocate any pointed-to data in anl_context, which will NOT
 * be CurrentMemoryContext when compute_stats is called.
 *
 * Note: all comparisons done for statistical purposes should use the
 * underlying column's collation (attcollation), except in situations
 * where a noncollatable container type contains a collatable type;
 * in that case use the type's default collation.  Be sure to record
 * the appropriate collation in stacoll.
 *----------
 */
typedef struct VacAttrStats *VacAttrStatsP;

typedef Datum (*AnalyzeAttrFetchFunc) (VacAttrStatsP stats, int rownum,
									   bool *isNull);

typedef void (*AnalyzeAttrComputeStatsFunc) (VacAttrStatsP stats,
											 AnalyzeAttrFetchFunc fetchfunc,
											 int samplerows,
											 double totalrows);

typedef struct VacAttrStats
{
	/*
	 * These fields are set up by the main ANALYZE code before invoking the
	 * type-specific typanalyze function.
	 *
	 * Note: do not assume that the data being analyzed has the same datatype
	 * shown in attr, ie do not trust attr->atttypid, attlen, etc.  This is
	 * because some index opclasses store a different type than the underlying
	 * column/expression.  Instead use attrtypid, attrtypmod, and attrtype for
	 * information about the datatype being fed to the typanalyze function.
	 * Likewise, use attrcollid not attr->attcollation.
	 */
	Form_pg_attribute attr;		/* copy of pg_attribute row for column */
	Oid			attrtypid;		/* type of data being analyzed */
	int32		attrtypmod;		/* typmod of data being analyzed */
	Form_pg_type attrtype;		/* copy of pg_type row for attrtypid */
	Oid			attrcollid;		/* collation of data being analyzed */
	MemoryContext anl_context;	/* where to save long-lived data */

	/*
	 * These fields must be filled in by the typanalyze routine, unless it
	 * returns false.
	 */
	AnalyzeAttrComputeStatsFunc compute_stats;	/* function pointer */
	int			minrows;		/* Minimum # of rows wanted for stats */
	void	   *extra_data;		/* for extra type-specific data */

	/*
	 * These fields are to be filled in by the compute_stats routine. (They
	 * are initialized to zero when the struct is created.)
	 */
	bool		stats_valid;
	float4		stanullfrac;	/* fraction of entries that are NULL */
	int32		stawidth;		/* average width of column values */
	float4		stadistinct;	/* # distinct values */
	int16		stakind[STATISTIC_NUM_SLOTS];
	Oid			staop[STATISTIC_NUM_SLOTS];
	Oid			stacoll[STATISTIC_NUM_SLOTS];
	int			numnumbers[STATISTIC_NUM_SLOTS];
	float4	   *stanumbers[STATISTIC_NUM_SLOTS];
	int			numvalues[STATISTIC_NUM_SLOTS];
	Datum	   *stavalues[STATISTIC_NUM_SLOTS];

	/*
	 * These fields describe the stavalues[n] element types. They will be
	 * initialized to match attrtypid, but a custom typanalyze function might
	 * want to store an array of something other than the analyzed column's
	 * elements. It should then overwrite these fields.
	 */
	Oid			statypid[STATISTIC_NUM_SLOTS];
	int16		statyplen[STATISTIC_NUM_SLOTS];
	bool		statypbyval[STATISTIC_NUM_SLOTS];
	char		statypalign[STATISTIC_NUM_SLOTS];

	/*
	 * These fields are private to the main ANALYZE code and should not be
	 * looked at by type-specific functions.
	 */
	int			tupattnum;		/* attribute number within tuples */
	HeapTuple  *rows;			/* access info for std fetch function */
	TupleDesc	tupDesc;
	Datum	   *exprvals;		/* access info for index fetch function */
	bool	   *exprnulls;
	int			rowstride;
} VacAttrStats;

typedef enum VacuumOption
{
	VACOPT_VACUUM = 1 << 0,		/* do VACUUM */
	VACOPT_ANALYZE = 1 << 1,	/* do ANALYZE */
	VACOPT_VERBOSE = 1 << 2,	/* print progress info */
	VACOPT_FREEZE = 1 << 3,		/* FREEZE option */
	VACOPT_FULL = 1 << 4,		/* FULL (non-concurrent) vacuum */
	VACOPT_SKIP_LOCKED = 1 << 5,	/* skip if cannot get lock */
	VACOPT_SKIPTOAST = 1 << 6,	/* don't process the TOAST table, if any */
	VACOPT_DISABLE_PAGE_SKIPPING = 1 << 7	/* don't skip any pages */
} VacuumOption;

/*
 * A ternary value used by vacuum parameters.
 *
 * DEFAULT value is used to determine the value based on other
 * configurations, e.g. reloptions.
 */
typedef enum VacOptTernaryValue
{
	VACOPT_TERNARY_DEFAULT = 0,
	VACOPT_TERNARY_DISABLED,
	VACOPT_TERNARY_ENABLED,
} VacOptTernaryValue;

/*
 * Parameters customizing behavior of VACUUM and ANALYZE.
 *
 * Note that at least one of VACOPT_VACUUM and VACOPT_ANALYZE must be set
 * in options.
 */
typedef struct VacuumParams
{
	int			options;		/* bitmask of VacuumOption */
	int			freeze_min_age; /* min freeze age, -1 to use default */
	int			freeze_table_age;	/* age at which to scan whole table */
	int			multixact_freeze_min_age;	/* min multixact freeze age, -1 to
											 * use default */
	int			multixact_freeze_table_age; /* multixact age at which to scan
											 * whole table */
	bool		is_wraparound;	/* force a for-wraparound vacuum */
	int			log_min_duration;	/* minimum execution threshold in ms at
									 * which  verbose logs are activated, -1
									 * to use default */
	VacOptTernaryValue index_cleanup;	/* Do index vacuum and cleanup,
										 * default value depends on reloptions */
	VacOptTernaryValue truncate;	/* Truncate empty pages at the end,
									 * default value depends on reloptions */

	/*
	 * The number of parallel vacuum workers.  0 by default which means choose
	 * based on the number of indexes.  -1 indicates parallel vacuum is
	 * disabled.
	 */
	int			nworkers;
} VacuumParams;

/* GUC parameters */
extern PGDLLIMPORT int default_statistics_target;	/* PGDLLIMPORT for PostGIS */
extern int	vacuum_freeze_min_age;
extern int	vacuum_freeze_table_age;
extern int	vacuum_multixact_freeze_min_age;
extern int	vacuum_multixact_freeze_table_age;

/* Variables for cost-based parallel vacuum */
extern pg_atomic_uint32 *VacuumSharedCostBalance;
extern pg_atomic_uint32 *VacuumActiveNWorkers;
extern int	VacuumCostBalanceLocal;


/* in commands/vacuum.c */
extern void ExecVacuum(ParseState *pstate, VacuumStmt *vacstmt, bool isTopLevel);
extern void vacuum(List *relations, VacuumParams *params,
				   BufferAccessStrategy bstrategy, bool isTopLevel);
extern void vac_open_indexes(Relation relation, LOCKMODE lockmode,
							 int *nindexes, Relation **Irel);
extern void vac_close_indexes(int nindexes, Relation *Irel, LOCKMODE lockmode);
extern double vac_estimate_reltuples(Relation relation,
									 BlockNumber total_pages,
									 BlockNumber scanned_pages,
									 double scanned_tuples);
extern void vac_update_relstats(Relation relation,
								BlockNumber num_pages,
								double num_tuples,
								BlockNumber num_all_visible_pages,
								bool hasindex,
								TransactionId frozenxid,
								MultiXactId minmulti,
								bool in_outer_xact);
extern void vacuum_set_xid_limits(Relation rel,
								  int freeze_min_age, int freeze_table_age,
								  int multixact_freeze_min_age,
								  int multixact_freeze_table_age,
								  TransactionId *oldestXmin,
								  TransactionId *freezeLimit,
								  TransactionId *xidFullScanLimit,
								  MultiXactId *multiXactCutoff,
								  MultiXactId *mxactFullScanLimit);
extern void vac_update_datfrozenxid(void);
extern void vacuum_delay_point(void);
extern bool vacuum_is_relation_owner(Oid relid, Form_pg_class reltuple,
									 int options);
extern Relation vacuum_open_relation(Oid relid, RangeVar *relation,
									 int options, bool verbose, LOCKMODE lmode);

/* in commands/analyze.c */
extern void analyze_rel(Oid relid, RangeVar *relation,
						VacuumParams *params, List *va_cols, bool in_outer_xact,
						BufferAccessStrategy bstrategy);
extern bool std_typanalyze(VacAttrStats *stats);

/* in utils/misc/sampling.c --- duplicate of declarations in utils/sampling.h */
extern double anl_random_fract(void);
extern double anl_init_selection_state(int n);
extern double anl_get_next_S(double t, int n, double *stateptr);

#endif							/* VACUUM_H */
