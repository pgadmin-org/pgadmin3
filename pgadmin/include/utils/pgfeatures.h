//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgfeatures.h PostgreSQL features constants
//
//////////////////////////////////////////////////////////////////////////


#ifndef __FEATURE_H
#define __FEATURE_H

enum
{
	FEATURE_INITIALIZED = 0,
	FEATURE_SIZE,
	FEATURE_FILEREAD,
	FEATURE_ROTATELOG,
	FEATURE_POSTMASTER_STARTTIME,
	FEATURE_TERMINATE_BACKEND,
	FEATURE_RELOAD_CONF,
	FEATURE_PGSTATTUPLE,
	FEATURE_PGSTATINDEX,
	FEATURE_FUNCTION_DEFAULTS,
	FEATURE_LAST
};

#endif
