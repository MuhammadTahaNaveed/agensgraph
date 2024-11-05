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
* pthread-win32.c
*	 partial pthread implementation for win32
*
* Copyright (c) 2004-2022, PostgreSQL Global Development Group
* IDENTIFICATION
*	src/interfaces/libpq/pthread-win32.c
*
*-------------------------------------------------------------------------
*/

#include "postgres_fe.h"

#include "pthread-win32.h"

DWORD
pthread_self(void)
{
	return GetCurrentThreadId();
}

void
pthread_setspecific(pthread_key_t key, void *val)
{
}

void *
pthread_getspecific(pthread_key_t key)
{
	return NULL;
}

int
pthread_mutex_init(pthread_mutex_t *mp, void *attr)
{
	mp->initstate = 0;
	return 0;
}

int
pthread_mutex_lock(pthread_mutex_t *mp)
{
	/* Initialize the csection if not already done */
	if (mp->initstate != 1)
	{
		LONG		istate;

		while ((istate = InterlockedExchange(&mp->initstate, 2)) == 2)
			Sleep(0);			/* wait, another thread is doing this */
		if (istate != 1)
			InitializeCriticalSection(&mp->csection);
		InterlockedExchange(&mp->initstate, 1);
	}
	EnterCriticalSection(&mp->csection);
	return 0;
}

int
pthread_mutex_unlock(pthread_mutex_t *mp)
{
	if (mp->initstate != 1)
		return EINVAL;
	LeaveCriticalSection(&mp->csection);
	return 0;
}
