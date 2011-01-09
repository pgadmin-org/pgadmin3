/*-------------------------------------------------------------------------
 *
 * keywords.h
 *	  lexical token lookup for reserved words in postgres SQL
 *
 *
 * Portions Copyright (c) 1996-2006, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * $PostgreSQL: pgsql/src/include/parser/keywords.h,v 1.21 2006/03/05 15:58:57 momjian Exp $
 *
 *-------------------------------------------------------------------------
 */

///////////////////////////////////////////////////////////////////////////
//
// pgAdmin note: This file is based on src/include/parser/keywords.h from
//               PostgreSQL, but with the token code entry removed.
//
//               This file is under the BSD licence, per PostgreSQL.
///////////////////////////////////////////////////////////////////////////

#ifndef KEYWORDS_H
#define KEYWORDS_H

/* Keyword categories --- should match lists in gram.y */
#define UNRESERVED_KEYWORD		0
#define COL_NAME_KEYWORD		1
#define TYPE_FUNC_NAME_KEYWORD	2
#define RESERVED_KEYWORD		3

typedef struct ScanKeyword
{
	const char *name;			/* in lower case */
	int		category;		/* see codes above */
} ScanKeyword;

extern const ScanKeyword *ScanKeywordLookup(const char *text);

extern const ScanKeyword ScanKeywords[];
extern const ScanKeyword ScanKeywordsExtra[];
extern const int NumScanKeywords;
extern const int NumScanKeywordsExtra;
#endif   /* KEYWORDS_H */
