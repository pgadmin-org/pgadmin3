//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgDefs.h PostgreSQL constants
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGDEFS_H
#define PGDEFS_H

#define PGOID_SCHEMA_CATALOG                11L
#define PGOID_SCHEMA_TOAST                  99L
#define PGOID_SCHEMA_PUBLIC                 2200L

#define PGOID_TYPE_SERIAL                   -42L
#define PGOID_TYPE_BOOL                     16L
#define PGOID_TYPE_BYTEA                    17L
#define PGOID_TYPE_CHAR                     18L
#define PGOID_TYPE_NAME                     19L
#define PGOID_TYPE_INT8                     20L
#define PGOID_TYPE_INT2                     21L
#define PGOID_TYPE_INT4                     23L
#define PGOID_TYPE_TEXT                     25L
#define PGOID_TYPE_OID                      26L
#define PGOID_TYPE_TID                      27L
#define PGOID_TYPE_XID                      28L
#define PGOID_TYPE_CID                      29L
#define PGOID_TYPE_FLOAT4                   700L
#define PGOID_TYPE_FLOAT8                   701L
#define PGOID_TYPE_MONEY                    790L
#define PGOID_TYPE_VARCHAR                  1043L
#define PGOID_TYPE_TIMESTAMP                1114L
#define PGOID_TYPE_TIME                     1083L
#define PGOID_TYPE_INTERVAL                 1186L
#define PGOID_TYPE_BIT                      1560L
#define PGOID_TYPE_NUMERIC                  1700L
#define PGOID_TYPE_CSTRING                  2275L
#define PGOID_TYPE_ANY                      2276L
#define PGOID_TYPE_VOID                     2278L
#define PGOID_TYPE_TRIGGER                  2279L
#define PGOID_TYPE_LANGUAGE_HANDLER         2280L


// These constants come from pgsql/src/include/catalog/pg_trigger.h
#define TRIGGER_TYPE_ROW				(1 << 0)
#define TRIGGER_TYPE_BEFORE				(1 << 1)
#define TRIGGER_TYPE_INSERT				(1 << 2)
#define TRIGGER_TYPE_DELETE				(1 << 3)
#define TRIGGER_TYPE_UPDATE				(1 << 4)


#endif
