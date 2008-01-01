//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// md5.h - MD5 password encryption function definitions
//
// Note: These functions are lifted shamelessly from libpq. As of PG8.2,
//       PQencryptPassword will be exported from libpq, but let's not
//       force everyone to upgrade their libpq just yet.
//////////////////////////////////////////////////////////////////////////

#define MD5_PASSWD_LEN	35

typedef unsigned char uint8;	/* == 8 bits */
typedef unsigned int uint32;	/* == 32 bits */

bool pg_md5_hash(const void *buff, size_t len, char *hexsum);
bool pg_md5_encrypt(const char *passwd, const char *salt, size_t salt_len, char *buf);
