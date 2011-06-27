/*-------------------------------------------------------------------------
 *
 * keywords.c
 *	  lexical token lookup for reserved words in PostgreSQL
 *
 * Portions Copyright (c) 1996-2006, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  $PostgreSQL: pgsql/src/backend/parser/keywords.c,v 1.177 2006/10/07 21:51:02 petere Exp $
 *
 *-------------------------------------------------------------------------
 */

///////////////////////////////////////////////////////////////////////////
//
// pgAdmin note: This file is based on src/backend/parser/keywords.c and
//               src/backend/parser/kwlookup.c from PostgreSQL, but extended
//               to support EntepriseDB and Greenplum.
//
//               This file is under the BSD licence, per PostgreSQL.
///////////////////////////////////////////////////////////////////////////

#include "postgres.h"
#include "parser/keywords.h"

/*
 * List of (keyword-name, keyword-token-value) pairs.
 */
#define PG_KEYWORD(a,b,c) {a,c},
const ScanKeyword ScanKeywords[] = {
#include <parser/kwlist.h>
};
const int NumScanKeywords = lengthof(ScanKeywords);

/*
 * Additional pairs here. They need to live in a separate array since
 * the ScanKeywords array needs to be sorted!
 *
 * !!WARNING!!: This list must be sorted, because binary
 *              search is used to locate entries.
 */
#define PG_KEYWORD2(a,b) {a,b},
const ScanKeyword ScanKeywordsExtra[] = {
	PG_KEYWORD2("connect", RESERVED_KEYWORD)
	PG_KEYWORD2("convert", RESERVED_KEYWORD)
	PG_KEYWORD2("distributed", UNRESERVED_KEYWORD)
	PG_KEYWORD2("exec", RESERVED_KEYWORD)
	PG_KEYWORD2("log", UNRESERVED_KEYWORD)
	PG_KEYWORD2("long", RESERVED_KEYWORD)
	PG_KEYWORD2("minus", RESERVED_KEYWORD)
	PG_KEYWORD2("nocache", RESERVED_KEYWORD)
	PG_KEYWORD2("number", RESERVED_KEYWORD)
	PG_KEYWORD2("package", RESERVED_KEYWORD)
	PG_KEYWORD2("pls_integer", RESERVED_KEYWORD)
	PG_KEYWORD2("raw", RESERVED_KEYWORD)
	PG_KEYWORD2("return", RESERVED_KEYWORD)
	PG_KEYWORD2("smalldatetime", RESERVED_KEYWORD)
	PG_KEYWORD2("smallfloat", RESERVED_KEYWORD)
	PG_KEYWORD2("smallmoney", RESERVED_KEYWORD)
	PG_KEYWORD2("sysdate", RESERVED_KEYWORD)
	PG_KEYWORD2("systimestap", RESERVED_KEYWORD)
	PG_KEYWORD2("tinyint", RESERVED_KEYWORD)
	PG_KEYWORD2("tinytext", RESERVED_KEYWORD)
	PG_KEYWORD2("varchar2", RESERVED_KEYWORD)
};
const int NumScanKeywordsExtra = lengthof(ScanKeywordsExtra);

/*
 * ScanKeywordLookup - see if a given word is a keyword
 *
 * Returns a pointer to the ScanKeyword table entry, or NULL if no match.
 *
 * The match is done case-insensitively.  Note that we deliberately use a
 * dumbed-down case conversion that will only translate 'A'-'Z' into 'a'-'z',
 * even if we are in a locale where tolower() would produce more or different
 * translations.  This is to conform to the SQL99 spec, which says that
 * keywords are to be matched in this way even though non-keyword identifiers
 * receive a different case-normalization mapping.
 */
const ScanKeyword *
ScanKeywordLookup(const char *text)
{
    int			len,
                i;
    char		word[NAMEDATALEN];
    const ScanKeyword *low;
    const ScanKeyword *high;

    len = strlen(text);
    /* We assume all keywords are shorter than NAMEDATALEN. */
    if (len >= NAMEDATALEN)
        return NULL;

    /*
     * Apply an ASCII-only downcasing.	We must not use tolower() since it may
     * produce the wrong translation in some locales (eg, Turkish).
     */
    for (i = 0; i < len; i++)
    {
        char		ch = text[i];

        if (ch >= 'A' && ch <= 'Z')
            ch += 'a' - 'A';
        word[i] = ch;
    }
    word[len] = '\0';

    /*
     * Now do a binary search using plain strcmp() comparison.
     */
    low = &ScanKeywords[0];
    high = endof(ScanKeywords) - 1;
    while (low <= high)
    {
        const ScanKeyword *middle;
        int			difference;

        middle = low + (high - low) / 2;
        difference = strcmp(middle->name, word);
        if (difference == 0)
            return middle;
        else if (difference < 0)
            low = middle + 1;
        else
            high = middle - 1;
    }

	/*
	 * If not found, also do a binary search in the list of extra
	 * keywords.
	 */
    low = &ScanKeywordsExtra[0];
    high = endof(ScanKeywordsExtra) - 1;
    while (low <= high)
    {
        const ScanKeyword *middle;
        int			difference;

        middle = low + (high - low) / 2;
        difference = strcmp(middle->name, word);
        if (difference == 0)
            return middle;
        else if (difference < 0)
            low = middle + 1;
        else
            high = middle - 1;
    }

    return NULL;
}
