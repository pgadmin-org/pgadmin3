//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: postgres.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// postgres.h - dummy include
//
//////////////////////////////////////////////////////////////////////////


#include <string.h>

#define lengthof(array) (sizeof (array) / sizeof ((array)[0]))
#define endof(array)	(&array[lengthof(array)])



// to suppress much stuff in parse.h
#define YYTOKENTYPE
#define YYSTYPE int


#define NAMEDATALEN 32
