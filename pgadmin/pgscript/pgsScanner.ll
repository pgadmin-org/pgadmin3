L	[a-zA-Z_@#]
D	[0-9]
E	[e-eE-E][+-]?{D}+

%{ /*** C/C++ Declarations ***/

//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// 
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////

#include "pgscript/pgScript.h"
#include "pgscript/parser.tab.hh"
#include "pgscript/utilities/pgsScanner.h"

/* Import the parser's token type into a local typedef */
typedef pgscript::pgsParser::token token;
typedef pgscript::pgsParser::token_type token_type;

/* Work around an incompatibility in flex (at least versions 2.5.31 through
 * 2.5.33): it generates code that does not conform to C89.  See Debian bug
 * 333231 <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.  */
#undef	yywrap
#define	yywrap()	1

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::PGS_END

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* Enable C++ scanner class generation */
%option c++

/* Change the name of the scanner class. Results in "pgsFlexLexer" */
%option prefix="pgs"
	
/* Case insensitive */
%option case-insensitive

/* The manual says "somewhat more optimized" */
%option batch

/* Prevent isatty warning in VC++ */
%option never-interactive

/* For using start conditions */
%option stack

/* No support for include files is planned */
%option noyywrap

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%x SC_COMMENT
%x SC_QUERY
%x SC_DOLLAR
%x SC_STRING

%% /*** Regular Expressions Part ***/

 /* Code to place at the beginning of yylex() */
%{
	// Reset location
	yylloc->step();
%}

<INITIAL,SC_QUERY>"--".*$	{ /* Ignore SQL comment */ }
<INITIAL>"/*"		{ comment_caller = INITIAL; BEGIN(SC_COMMENT); }
<SC_QUERY>"/*"		{ comment_caller = SC_QUERY; BEGIN(SC_COMMENT); }

<INITIAL>{
"WHILE"				{ return token::PGS_WHILE; }
"BREAK"				{ return token::PGS_BREAK; }
"RETURN"			{ return token::PGS_RETURN; }
"CONTINUE"			{ return token::PGS_CONTINUE; }
"IF"				{ return token::PGS_IF; }
"ELSE"				{ return token::PGS_ELSE; }
"WAITFOR"			{ return token::PGS_WAITFOR; }
"AS"				{ return token::PGS_AS; }

"ASSERT"			{ return token::PGS_ASSERT; }
"GO"				{ /* Ignore it */ }
"PRINT"				{ return token::PGS_PRINT; }

"COLUMNS"			{ return token::PGS_CNT_COLUMNS; }
"LINES"				{ return token::PGS_CNT_LINES; }
"TRIM"				{ return token::PGS_TRIM; }
"RMLINE"			{ return token::PGS_RM_LINE; }
"CAST"				{ return token::PGS_CAST; }

"RECORD"			{ return token::PGS_RECORD; }

"INTEGER"			{ return token::PGS_INTEGER; }
"REAL"				{ return token::PGS_REAL; }
"STRING"			{ return token::PGS_STRING; }
"REGEX"				{ return token::PGS_REGEX; }
"FILE"				{ return token::PGS_FILE; }
"DATE"				{ return token::PGS_DATE; }
"TIME"				{ return token::PGS_TIME; }
"DATETIME"			{ return token::PGS_DATE_TIME; }
"REFERENCE"			{ return token::PGS_REFERENCE; }


"SET"[ \t]+"@"		{ unput('@'); yylloc->end.columns(-1);
					  return token::PGS_SET_ASSIGN; }
"DECLARE"[ \t]+"@"	{ unput('@'); yylloc->end.columns(-1);
					  return token::PGS_DECLARE_ASSGN; }

"BEGIN"				{ /* Block opening */ return token::PGS_OPEN; }
"END"				{ /* Block closing */ return token::PGS_CLOSE; }

"@"({L}|{D})*		{ yylval->str = pnew wxString(yytext, m_conv);
					  return token::PGS_IDENTIFIER; }

{D}+				{ yylval->str = pnew wxString(yytext, m_conv);
					  return token::PGS_VAL_INT; }

{D}*"."{D}+({E})?	{ yylval->str = pnew wxString(yytext, m_conv);
					  return token::PGS_VAL_REAL; }
{D}+{E}				{ yylval->str = pnew wxString(yytext, m_conv);
					  return token::PGS_VAL_REAL; }
{D}+"."{D}*({E})?	{ yylval->str = pnew wxString(yytext, m_conv);
					  return token::PGS_VAL_REAL; }

"AND"				{ return token::PGS_AND_OP; }
"OR"				{ return token::PGS_OR_OP; }
"<="				{ return token::PGS_LE_OP; }
">="				{ return token::PGS_GE_OP; }
"="					{ return token::PGS_EQ_OP; }
"~="				{ return token::PGS_AE_OP; }
"<>"				{ return token::PGS_NE_OP; }
";"					{ return wx_static_cast(token_type, ';'); }
("{"|"<%")			{ return wx_static_cast(token_type, '{'); }
("}"|"%>")			{ return wx_static_cast(token_type, '}'); }
":"					{ return wx_static_cast(token_type, ':'); }
"("					{ return wx_static_cast(token_type, '('); }
")"					{ return wx_static_cast(token_type, ')'); }
("["|"<:")			{ return wx_static_cast(token_type, '['); }
("]"|":>")			{ return wx_static_cast(token_type, ']'); }
"."					{ return wx_static_cast(token_type, '.'); }
","					{ return wx_static_cast(token_type, ','); }
"NOT"				{ return token::PGS_NOT_OP; }
"-"					{ return wx_static_cast(token_type, '-'); }
"+"					{ return wx_static_cast(token_type, '+'); }
"*"					{ return wx_static_cast(token_type, '*'); }
"/"					{ return wx_static_cast(token_type, '/'); }
"%"					{ return wx_static_cast(token_type, '%'); }
"<"					{ return wx_static_cast(token_type, '<'); }
">"					{ return wx_static_cast(token_type, '>'); }

"'"					{ string_caller = INITIAL; BEGIN(SC_STRING); }

"ABORT"				{ query += yytext; query_token = token::PGS_ABORT;
					  BEGIN(SC_QUERY); }
"ALTER"				{ query += yytext; query_token = token::PGS_ALTER;
					  BEGIN(SC_QUERY); }
"ANALYZE"			{ query += yytext; query_token = token::PGS_ANALYZE;
					  BEGIN(SC_QUERY); }
"BEGIN"[ \t]+"TRAN"	{ query += yytext; query_token = token::PGS_BEGIN;
					  BEGIN(SC_QUERY); }
"BEGIN"[ \t]+"WORK"	{ query += yytext; query_token = token::PGS_BEGIN;
					  BEGIN(SC_QUERY); }
"CHECKPOINT"		{ query += yytext; query_token = token::PGS_CHECKPOINT;
					  BEGIN(SC_QUERY); }
"CLOSE"				{ query += yytext; query_token = token::PGS_CLOSE_ST;
					  BEGIN(SC_QUERY); }
"CLUSTER"			{ query += yytext; query_token = token::PGS_CLUSTER;
					  BEGIN(SC_QUERY); }
"COMMENT"			{ query += yytext; query_token = token::PGS_COMMENT;
					  BEGIN(SC_QUERY); }
"COMMIT"			{ query += yytext; query_token = token::PGS_COMMIT;
					  BEGIN(SC_QUERY); }
"COPY"				{ query += yytext; query_token = token::PGS_COPY;
					  BEGIN(SC_QUERY); }
"CREATE"			{ query += yytext; query_token = token::PGS_CREATE;
					  BEGIN(SC_QUERY); }
"DEALLOCATE"		{ query += yytext; query_token = token::PGS_DEALLOCATE;
					  BEGIN(SC_QUERY); }
"DECLARE"			{ query += yytext; query_token = token::PGS_DECLARE;
					  BEGIN(SC_QUERY); }
"DELETE"			{ query += yytext; query_token = token::PGS_DELETE;
					  BEGIN(SC_QUERY); }
"DISCARD"			{ query += yytext; query_token = token::PGS_DISCARD;
					  BEGIN(SC_QUERY); }
"DROP"				{ query += yytext; query_token = token::PGS_DROP;
					  BEGIN(SC_QUERY); }
"END"[ \t]+"TRANS"	{ query += yytext; query_token = token::PGS_END_ST;
					  BEGIN(SC_QUERY); }
"END"[ \t]+"WORK"	{ query += yytext; query_token = token::PGS_END_ST;
					  BEGIN(SC_QUERY); }
"EXECUTE"			{ query += yytext; query_token = token::PGS_EXECUTE;
					  BEGIN(SC_QUERY); }
"EXPLAIN"			{ query += yytext; query_token = token::PGS_EXPLAIN;
					  BEGIN(SC_QUERY); }
"FETCH"				{ query += yytext; query_token = token::PGS_FETCH;
					  BEGIN(SC_QUERY); }
"GRANT"				{ query += yytext; query_token = token::PGS_GRANT;
					  BEGIN(SC_QUERY); }
"INSERT"			{ query += yytext; query_token = token::PGS_INSERT;
					  BEGIN(SC_QUERY); }
"LISTEN"			{ query += yytext; query_token = token::PGS_LISTEN;
					  BEGIN(SC_QUERY); }
"LOAD"				{ query += yytext; query_token = token::PGS_LOAD;
					  BEGIN(SC_QUERY); }
"LOCK"				{ query += yytext; query_token = token::PGS_LOCK;
					  BEGIN(SC_QUERY); }
"MOVE"				{ query += yytext; query_token = token::PGS_MOVE;
					  BEGIN(SC_QUERY); }
"NOTIFY"			{ query += yytext; query_token = token::PGS_NOTIFY;
					  BEGIN(SC_QUERY); }
"PREPARE"			{ query += yytext; query_token = token::PGS_PREPARE;
					  BEGIN(SC_QUERY); }
"REASSIGN"			{ query += yytext; query_token = token::PGS_REASSIGN;
					  BEGIN(SC_QUERY); }
"REINDEX"			{ query += yytext; query_token = token::PGS_REINDEX;
					  BEGIN(SC_QUERY); }
"RELEASE"			{ query += yytext; query_token = token::PGS_RELEASE;
					  BEGIN(SC_QUERY); }
"RESET"				{ query += yytext; query_token = token::PGS_RESET;
					  BEGIN(SC_QUERY); }
"REVOKE"			{ query += yytext; query_token = token::PGS_REVOKE;
					  BEGIN(SC_QUERY); }
"ROLLBACK"			{ query += yytext; query_token = token::PGS_ROLLBACK;
					  BEGIN(SC_QUERY); }
"SAVEPOINT"			{ query += yytext; query_token = token::PGS_SAVEPOINT;
					  BEGIN(SC_QUERY); }
"SELECT"			{ query += yytext; query_token = token::PGS_SELECT;
					  BEGIN(SC_QUERY); }
"SET"				{ query += yytext; query_token = token::PGS_SET;
					  BEGIN(SC_QUERY); }
"SHOW"				{ query += yytext; query_token = token::PGS_SHOW;
					  BEGIN(SC_QUERY); }
"START"				{ query += yytext; query_token = token::PGS_START;
					  BEGIN(SC_QUERY); }
"TRUNCATE"			{ query += yytext; query_token = token::PGS_TRUNCATE;
					  BEGIN(SC_QUERY); }
"UNLISTEN"			{ query += yytext; query_token = token::PGS_UNLISTEN;
					  BEGIN(SC_QUERY); }
"UPDATE"			{ query += yytext; query_token = token::PGS_UPDATE;
					  BEGIN(SC_QUERY); }
"VACUUM"			{ query += yytext; query_token = token::PGS_VACUUM;
					  BEGIN(SC_QUERY); }
"VALUES"			{ query += yytext; query_token = token::PGS_VALUES;
					  BEGIN(SC_QUERY); }

[ \t\v\f]+			{ }
\r					{ yylloc->step(); }
\n					{ yylloc->lines(yyleng); yylloc->step(); }
.					{ return token::PGS_UNKNOWN; }
}

<SC_QUERY>{
"'"					{ query += yytext; string_caller = SC_QUERY; BEGIN(SC_STRING); }
\$({L}|{D})*\$		{ query += yytext; dollar = yytext; BEGIN(SC_DOLLAR); }
";"					{ yylval->str = pnew wxString(query.c_str(), m_conv);
					  query.clear(); unput(';'); yylloc->end.columns(-1);
					  BEGIN(INITIAL); m_parent = 0; return query_token; }
"("					{ ++m_parent; query += yytext; }
")"					{
						--m_parent;
						if (m_parent == -1)
						{
							yylval->str = pnew wxString(query.c_str(), m_conv);
							query.clear(); unput(')'); yylloc->end.columns(-1);
							BEGIN(INITIAL); m_parent = 0; return query_token;
						}
						else
						{
							query += yytext;
						}
					}
\r					{ query += yytext; yylloc->step(); }
\n					{ query += yytext; yylloc->lines(yyleng); yylloc->step(); }
<<EOF>>				{ yylval->str = pnew wxString(query.c_str(), m_conv);
					  query.clear(); m_parent = 0; return query_token; }
.					{ yylloc->columns(columns(*yytext)); query += yytext; }
}

<SC_DOLLAR>{
\$({L}|{D})*\$		{ query += yytext;
					  if (std::string(yytext) == dollar) BEGIN(SC_QUERY); }
\r					{ query += yytext; yylloc->step(); }
\n					{ query += yytext; yylloc->lines(yyleng); yylloc->step(); }
<<EOF>>				{ query += yytext; yylval->str = pnew wxString(query.c_str(), m_conv);
					  query.clear(); m_parent = 0; return query_token; }
.					{ yylloc->columns(columns(*yytext)); query += yytext; }
}

<SC_COMMENT>{
"*/"				{ BEGIN(comment_caller); }
\r					{ yylloc->step(); }
\n					{ yylloc->lines(yyleng); yylloc->step(); }
.					{ yylloc->columns(columns(*yytext)); }
}

<SC_STRING>{
"''"				{
						if (string_caller == SC_QUERY)
							query += yytext;
						else
							str += "'";
					}
\\.					{
						if (string_caller == SC_QUERY)
							query += yytext;
						else
							str += *(yytext + 1);
						yylloc->columns(columns(*(yytext + 1)));
					}
"'"					{
						if (string_caller == SC_QUERY)
						{
							query += yytext;
							BEGIN(string_caller);
						}
						else
						{
							yylval->str = pnew wxString(str.c_str(), m_conv);
							str.clear();
							BEGIN(string_caller);
							return token::PGS_VAL_STR;
						}
					}
\r					{
						if (string_caller == SC_QUERY)
							query += yytext;
						else
							str += yytext;
						yylloc->step();
					}
\n					{
						if (string_caller == SC_QUERY)
							query += yytext;
						else
							str += yytext;
						yylloc->lines(yyleng); yylloc->step();
					}
<<EOF>>				{
						if (string_caller == SC_QUERY)
						{
							query += yytext; yylval->str = pnew wxString(query.c_str(), m_conv);
							query.clear(); m_parent = 0; return query_token;
						}
						else
						{
							yylval->str = pnew wxString(str.c_str(), m_conv);
							str.clear();
							BEGIN(string_caller);
							return token::PGS_VAL_STR;
						}
					}
.					{ 
						if (string_caller == SC_QUERY)
							query += yytext;
						else
							str += yytext;
						yylloc->columns(columns(*yytext));
					}
}

%% /*** Additional Code ***/

namespace pgscript
{

pgsScanner::pgsScanner(wxMBConv & conv, std::istream * in, std::ostream * out) :
	pgsFlexLexer(in, out), m_parent(0), m_conv(conv)
{

}

pgsScanner::~pgsScanner()
{

}

void pgsScanner::set_debug(bool b)
{
	yy_flex_debug = b;
}

int pgsScanner::columns(const char & c)
{
	if ((c & 0xF0) == 0xF0) // 4 bytes
		return -3;
	else if ((c & 0xE0) == 0xE0) // 3 bytes
		return -2;
	else if ((c & 0xC0) == 0xC0) // 2 bytes
		return -1;
	else return 0;
}

}

/* This implementation of pgsFlexLexer::yylex() is required to fill the
 * vtable of the class pgsFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the pgsScanner class instead. */

#ifdef yylex
#undef yylex
#endif

int pgsFlexLexer::yylex()
{
	return 0;
}
