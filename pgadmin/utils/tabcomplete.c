//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - Tab completion
// 
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// tab-complete.cpp - Functions interfacing with tab-complete from psql
//
//////////////////////////////////////////////////////////////////////////

/*
 * BUG: Must compile as C and not C++, because of
 * http://support.microsoft.com/default.aspx?scid=kb%3Ben-us%3B315481
 * Adds a bit of C<->C++ cruft...
 */

#define _CRT_NONSTDC_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libpq-fe.h>
#include <ctype.h>

/*
 * Callbacks to the C++ world
 */
char *pg_query_to_single_ordered_string(char *query, void *dbptr);


/*
 * Global vars for readline emulation
 */
static char *rl_line_buffer;

/*
 * Macros to ease typing present in psql, rewritten for our API
 */
#define COMPLETE_WITH_QUERY(query) \
	do { matches = complete_from_query(text,query,NULL,dbptr); } while (0)
#define COMPLETE_WITH_SCHEMA_QUERY(query, addon) \
	do { matches = complete_from_schema_query(text,&query, addon, dbptr); } while (0)
#define COMPLETE_WITH_LIST(list) \
	do { matches = complete_from_list(text,list); } while (0)
#define COMPLETE_WITH_CONST(str) \
	do { matches = complete_from_const(text,str); } while (0)
#define COMPLETE_WITH_ATTR(table) \
	do { matches = complete_from_query(text,Query_for_list_of_attributes, table, dbptr); } while (0)

/*
 * Functions used by the tab completion in psql normally found in pgport
 */
static void *pg_malloc(size_t size)
{
	return malloc(size);
}

static int pg_strcasecmp(const char *s1, const char *s2)
{
#ifdef WIN32
	return _stricmp(s1, s2);
#else
    return strcasecmp(s1, s2);
#endif
}

static int pg_strncasecmp(const char *s1, const char *s2, int len)
{
#ifdef WIN32
	return _strnicmp(s1, s2, len);
#else
    return strncasecmp(s1, s2, len);
#endif
}

/*
 * Return the word (space delimited) before point. Set skip > 0 to
 * skip that many words; e.g. skip=1 finds the word before the
 * previous one. Return value is NULL or a malloc'ed string.
 *
 * Copied directly from psql.
 */
static char *
previous_word(int point, int skip)
{
	int			i,
				start = 0,
				end = -1,
				inquotes = 0;
	char	   *s;

	while (skip-- >= 0)
	{
		/* first we look for a space before the current word */
		for (i = point; i >= 0; i--)
			if (rl_line_buffer[i] == ' ')
				break;

		/* now find the first non-space which then constitutes the end */
		for (; i >= 0; i--)
			if (rl_line_buffer[i] != ' ')
			{
				end = i;
				break;
			}

		/*
		 * If no end found we return null, because there is no word before the
		 * point
		 */
		if (end == -1)
			return NULL;

		/*
		 * Otherwise we now look for the start. The start is either the last
		 * character before any space going backwards from the end, or it's
		 * simply character 0
		 */
		for (start = end; start > 0; start--)
		{
			if (rl_line_buffer[start] == '"')
				inquotes = !inquotes;
			if ((rl_line_buffer[start - 1] == ' ') && inquotes == 0)
				break;
		}

		point = start;
	}

	/* make a copy */
	s = (char *)pg_malloc(end - start + 2);

	strncpy(s, &rl_line_buffer[start], end - start + 1);
	s[end - start + 1] = '\0';

	return s;
}

/* Find the parenthesis after the last word */
/* Copied directly from psql */
static int find_open_parenthesis(int end)
{
	int i = end-1;
	
	while((rl_line_buffer[i]!=' ')&&(i>=0))
	{
		if (rl_line_buffer[i]=='(') return 1;
		i--;
	}
	while((rl_line_buffer[i]==' ')&&(i>=0))
	{
		i--;
	}
	if (rl_line_buffer[i]=='(')
	{
		return 1;       
	}
	return 0;

}


/*
 * Forward declarations
 */
static char *complete_from_list(const char *text, const char * const *list);
static char *complete_from_const(const char *text, const char *string);
static char *complete_from_query(const char *text, const char *query, const char *addon, void *dbptr);
static char *complete_from_schema_query(const char *text, const void* query, const char *addon, void *dbptr);
static char *complete_create_command(char *text);
static char *complete_filename();

/*
 * Include the main tab completion functionality from psql
 */
#include "tab-complete.inc"


/*
 * Completion functions mimicking those from psql, only returning a single space separated
 * string instead of being called multiple times by the readline library.
 */
static char *_complete_from_list(const char *text, const char * const *list, int casesensitive)
{
	 int string_length = strlen(text);
	 int size = 0;
	 int i;
	 char *r;

	 for (i = 0; list[i] != NULL; i++)
	 {
		 if (casesensitive && strncmp(text, list[i], string_length) == 0)
			 size += strlen(list[i]);
		 if (!casesensitive && pg_strncasecmp(text, list[i], string_length) == 0)
			 size += strlen(list[i]);
	 }
	 
	 if (size == 0)
		 return NULL;

	 r = calloc(size + i*2 + 1, 1);
	 for (i = 0; list[i] != NULL; i++)
	 {
		 if ((casesensitive && strncmp(text, list[i], string_length) == 0)
			 ||
			 (!casesensitive && pg_strncasecmp(text, list[i], string_length) == 0))
		 {
			strcat(r,list[i]);
			strcat(r," \t");
		 }
	 }
	 r[strlen(r)-1] = '\0'; /* Chop of trailing space */
	 return r;
}

static char *complete_from_list(const char *text, const char * const *list)
{
	 char *r = _complete_from_list(text, list, 1);
	 if (r)
		 return r;
	 return _complete_from_list(text, list, 0);
}


static char *complete_from_const(const char *text, const char *string)
{
	return strdup(string);
}

static char *_complete_from_query(const char *text, const char *query, const SchemaQuery *squery, const char *addon, void *dbptr)
{
	int string_length = strlen(text);
	char *e_text;
	char *complete_query = NULL;
	char *t;

	e_text = malloc(string_length*2+1);
	PQescapeString(e_text, text, string_length);

	if (query != NULL)
	{
		/* Normal query */
		int bufsize = 1024;
		char *e_addon;

		/* Normal query needs escaped string */
		if (addon)
		{
			e_addon = malloc(strlen(addon)*2+1);
			PQescapeString(e_addon, addon, strlen(addon));
		}
		else
			e_addon = strdup("");

		while (1)
		{
			int r;
			complete_query = realloc(complete_query, bufsize);
#ifdef WIN32
			r = _snprintf(complete_query, bufsize, query, string_length, e_text, e_addon);
#else
            r = snprintf(complete_query, bufsize, query, string_length, e_text, e_addon);
#endif
			if (r < 0 || r >= bufsize)
				bufsize *= 2;
			else
				break;
		}
		free(e_addon);
	}
	else
	{
		/* Schema query */
		char *selcondition = NULL;
		char *viscondition = NULL;
		char *suppress_str = NULL;
		const char *qualresult;
		int bufsize = 2048;

		if (squery->selcondition)
		{
			selcondition = malloc(strlen(squery->selcondition)+10);
			sprintf(selcondition,"%s AND ",squery->selcondition);
		}
		else
			selcondition = strdup("");

		if (squery->viscondition)
		{
			viscondition = malloc(strlen(squery->viscondition)+10);
			sprintf(viscondition, " AND %s",squery->viscondition);
		}
		else
			viscondition = strdup("");

		if (strcmp(squery->catname,"pg_catalog.pg_class c") == 0 &&
			strncmp(text, "pg_", 3) != 0)
			suppress_str = " AND c.relnamespace <> (SELECT oid FROM pg_catalog.pg_namespace WHERE nspname = 'pg_catalog')";
		else
			suppress_str = "";

		qualresult = squery->qualresult;
		if (qualresult == NULL)
			qualresult = squery->result;

		while (1)
		{
			int r;
			complete_query = realloc(complete_query, bufsize);
#ifdef WIN32
            r = _snprintf(complete_query, bufsize, 
#else
            r = snprintf(complete_query, bufsize,
#endif
				"SELECT %s FROM %s WHERE %s substring(%s,1,%d)='%s' %s %s "
				"\nUNION\n"
				"SELECT pg_catalog.quote_ident(n.nspname) || '.' FROM pg_catalog.pg_namespace n WHERE substring(pg_catalog.quote_ident(n.nspname) || '.',1,%d)='%s' AND (SELECT pg_catalog.count(*) FROM pg_catalog.pg_namespace WHERE substring(pg_catalog.quote_ident(nspname) || '.',1,%d)= substring('%s',1,pg_catalog.length(pg_catalog.quote_ident(nspname))+1))>1"
				"\nUNION\n"
				"SELECT pg_catalog.quote_ident(n.nspname) || '.' || %s FROM %s, pg_catalog.pg_namespace n WHERE %s = n.oid AND %s substring(pg_catalog.quote_ident(n.nspname) || '.' || %s,1,%d)='%s' AND substring(pg_catalog.quote_ident(n.nspname) || '.',1,%d) = substring('%s',1,pg_catalog.length(pg_catalog.quote_ident(n.nspname))+1) AND (SELECT pg_catalog.count(*) FROM pg_catalog.pg_namespace WHERE substring(pg_catalog.quote_ident(nspname) || '.',1,%d) = substring('%s',1,pg_catalog.length(pg_catalog.quote_ident(nspname))+1)) = 1"
				"\n%s",
				squery->result,
				squery->catname,
				selcondition,
				squery->result,
				string_length,
				e_text,
				viscondition,
				suppress_str,
				string_length,
				e_text,
				string_length,
				e_text,
				qualresult,
				squery->catname,
				squery->namespace,
				selcondition,
				qualresult,
				string_length,
				e_text,
				string_length,
				e_text,
				string_length,
				e_text,
				addon?addon:"");
			
			if (r < 0 || r >= bufsize)
				bufsize *= 2;
			else
				break;
			
		}
		
		free(viscondition);
		free(selcondition);

	}
	t = pg_query_to_single_ordered_string(complete_query, dbptr);
	if (complete_query)
		free(complete_query);
	free(e_text);
	return t;
}

static char *complete_from_query(const char *text, const char *query, const char *addon, void *dbptr)
{
	return _complete_from_query(text, query, NULL, addon, dbptr);
}

static char *complete_from_schema_query(const char *text, const void* query, const char *addon, void *dbptr)
{
	/* query really is a SchemaQuery, but we don't know about SchemaQuery early enough. */
	return _complete_from_query(text, NULL, (SchemaQuery *)query, addon, dbptr);
}

static char *complete_create_command(char *text)
{
	 int string_length = strlen(text);
	 int size = 0;
	 int i;
	 char *r;

	 for (i = 0; words_after_create[i].name != NULL; i++)
	 {
		 if (pg_strncasecmp(text, words_after_create[i].name, string_length) == 0)
			 size += strlen(words_after_create[i].name);
	 }

	 r = calloc(size + i*2 + 1, 1);
	 for (i = 0; words_after_create[i].name != NULL; i++)
	 {
		 if (pg_strncasecmp(text, words_after_create[i].name, string_length) == 0)
		 {
			strcat(r,words_after_create[i].name);
			strcat(r," \t");
		 }
	 }
	 r[strlen(r)-1] = '\0'; /* Chop of trailing space */
	 return r;
}

/* Not implemented */
static char *complete_filename()
{
	return NULL;
}


/*
 * Entrypoint from the C++ world
 */
char *tab_complete(const char *allstr, const int startptr, const int endptr, void *dbptr)
{
	rl_line_buffer = (char *)allstr;
	return psql_completion((char *)(allstr + startptr), startptr,endptr,dbptr);
}
