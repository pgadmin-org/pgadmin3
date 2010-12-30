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
 * Load the yytokentype list of symbols from PostgreSQL
 */
#undef YYTOKENTYPE
#include <server/parser/gram.h>

/*
 * For non-PostgreSQL types, define an enumb with their values here
 * NOTE! Make sure the value doesn't conflict with what's in
 * server/parser/gram.h - as the PostgreSQL parser grows larger the
 * highest numbers will increase.
 */
enum yytokentype_extra {
 /* The following additions are keywords in EnterpriseDB */
 CONNECT_EDB = 800,
 CONVERT_EDB = 801,
 MINUS_EDB = 802,
 NUMBER_EDB = 803,
 PACKAGE_EDB = 804,
 SMALLDATETIME_EDB = 805,
 SMALLFLOAT_EDB = 806,
 SMALLMONEY_EDB = 807,
 TINYINT_EDB = 808,
 TINYTEXT_EDB = 809,
 VARCHAR2_EDB = 810,
 EXEC_EDB = 811,
 LONG_EDB = 812,
 NOCACHE_EDB = 813,
 PLS_INTEGER_EDB = 814,
 RAW_EDB = 815,
 RETURN_EDB = 816,
 SYSDATE_EDB = 817,
 SYSTIMESTAMP_EDB = 818,

 /* The following additions are keywords in Greenplum Database */
 DISTRIBUTED_GP = 900,
 LOG_P_GP = 901
};


/*
 * List of (keyword-name, keyword-token-value) pairs.
 */
#define PG_KEYWORD(a,b,c) {a,b,c},
const ScanKeyword ScanKeywords[] = {
#include <server/parser/kwlist.h>
};
const int NumScanKeywords = lengthof(ScanKeywords);

/*
 * Additional pairs here. They need to live in a separate array since
 * the ScanKeywords array needs to be sorted!
 *
 * !!WARNING!!: This list must be sorted, because binary
 *              search is used to locate entries.
 */
const ScanKeyword ScanKeywordsExtra[] = {
	PG_KEYWORD("connect", CONNECT_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("convert", CONVERT_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("distributed", DISTRIBUTED_GP, UNRESERVED_KEYWORD)
	PG_KEYWORD("exec", EXEC_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("log", LOG_P_GP, UNRESERVED_KEYWORD)
	PG_KEYWORD("long", LONG_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("minus", MINUS_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("nocache", NOCACHE_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("number", NUMBER_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("package", PACKAGE_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("pls_integer", PLS_INTEGER_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("raw", RAW_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("return", RETURN_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("smalldatetime", SMALLDATETIME_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("smallfloat", SMALLFLOAT_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("smallmoney", SMALLMONEY_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("sysdate", SYSDATE_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("systimestap", SYSTIMESTAMP_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("tinyint", TINYINT_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("tinytext", TINYTEXT_EDB, RESERVED_KEYWORD)
	PG_KEYWORD("varchar2", VARCHAR2_EDB, RESERVED_KEYWORD)
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
