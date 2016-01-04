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
#include "pgscript/statements/pgsStatements.h"
#include "pgscript/expressions/pgsExpressions.h"
#include "pgscript/objects/pgsObjects.h"
#include "pgscript/utilities/pgsContext.h"

%}

/*** YACC/Bison declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* Start symbol is named "start" */
%start translation_unit

/* Write out a header file containing the token defines */
%defines

/* Use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* Namespace to enclose parser in */
%name-prefix="pgscript"

/* Set the parser's class identifier */
%define "parser_class_name" "pgsParser"

/* Keep track of the current position within the input */
%locations
%initial-action
{
	// Initialize the initial location object
	@$.begin.filename = @$.end.filename;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class pgsDriver & driver }

/* Verbose error messages */
%error-verbose

%token PGS_END	0			"END OF FILE"

%token PGS_WHILE			"WHILE"
%token PGS_BREAK			"BREAK"
%token PGS_RETURN			"RETURN"
%token PGS_CONTINUE			"CONTINUE"
%token PGS_IF				"IF"
%token PGS_ELSE				"ELSE"
%token PGS_WAITFOR			"WAITFOR"
%token PGS_AS				"AS"

%token PGS_OPEN				"BEGIN (BLOCK)"
%token PGS_CLOSE			"END (BLOCK)"

%token PGS_ASSERT			"ASSERT"
%token PGS_PRINT			"PRINT"
%token PGS_LOG				"LOG"

%token PGS_CNT_COLUMNS		"COLUMNS"
%token PGS_CNT_LINES		"LINES"
%token PGS_TRIM				"TRIM"
%token PGS_RM_LINE			"RMLINE"
%token PGS_CAST				"CAST"

%token PGS_RECORD			"RECORD"

%token PGS_INTEGER			"INTEGER"
%token PGS_REAL				"REAL"
%token PGS_STRING			"STRING"
%token PGS_REGEX			"REGEX"
%token PGS_FILE				"FILE"
%token PGS_DATE				"DATE"
%token PGS_TIME				"TIME"
%token PGS_DATE_TIME		"DATETIME"
%token PGS_REFERENCE		"REFERENCE"

%token PGS_LE_OP			"<="
%token PGS_GE_OP			">="
%token PGS_EQ_OP			"="
%token PGS_AE_OP			"~="
%token PGS_NE_OP			"<>"
%token PGS_AND_OP			"AND"
%token PGS_OR_OP			"OR"
%token PGS_NOT_OP			"NOT"

%token PGS_UNKNOWN			"character"

%right PGS_ELSE

%union
{
	const wxString * str;
	int integer;
	pgsExpression * expr;
	pgsStmt * stmt;
	pgsStmtList * stmt_list;
}

%token PGS_SET_ASSIGN		"SET @VARIABLE"
%token PGS_DECLARE_ASSGN	"DECLARE @VARIABLE"

%token<str> PGS_ABORT		"ABORT"
%token<str> PGS_ALTER		"ALTER"
%token<str> PGS_ANALYZE		"ANALYZE"
%token<str> PGS_BEGIN		"BEGIN"
%token<str> PGS_CHECKPOINT	"CHECKPOINT"
%token<str> PGS_CLOSE_ST	"CLOSE"
%token<str> PGS_CLUSTER		"CLUSTER"
%token<str> PGS_COMMENT		"COMMENT"
%token<str> PGS_COMMIT		"COMMIT"
%token<str> PGS_COPY		"COPY"
%token<str> PGS_CREATE		"CREATE"
%token<str> PGS_DEALLOCATE	"DEALLOCATE"
%token<str> PGS_DECLARE		"DECLARE"
%token<str> PGS_DELETE		"DELETE"
%token<str> PGS_DISCARD		"DISCARD"
%token<str> PGS_DROP		"DROP"
%token<str> PGS_END_ST		"END"
%token<str> PGS_EXECUTE		"EXECUTE"
%token<str> PGS_EXPLAIN		"EXPLAIN"
%token<str> PGS_FETCH		"FETCH"
%token<str> PGS_GRANT		"GRANT"
%token<str> PGS_INSERT		"INSERT"
%token<str> PGS_LISTEN		"LISTEN"
%token<str> PGS_LOAD		"LOAD"
%token<str> PGS_LOCK		"LOCK"
%token<str> PGS_MOVE		"MOVE"
%token<str> PGS_NOTIFY		"NOTIFY"
%token<str> PGS_PREPARE		"PREPARE"
%token<str> PGS_REASSIGN	"REASSIGN"
%token<str> PGS_REINDEX		"REINDEX"
%token<str> PGS_RELEASE		"RELEASE"
%token<str> PGS_RESET		"RESET"
%token<str> PGS_REVOKE		"REVOKE"
%token<str> PGS_ROLLBACK	"ROLLBACK"
%token<str> PGS_SAVEPOINT	"SAVEPOINT"
%token<str> PGS_SELECT		"SELECT"
%token<str> PGS_SET			"SET"
%token<str> PGS_SHOW		"SHOW"
%token<str> PGS_START		"START"
%token<str> PGS_TRUNCATE	"TRUNCATE"
%token<str> PGS_UNLISTEN	"UNLISTEN"
%token<str> PGS_UPDATE		"UPDATE"
%token<str> PGS_VACUUM		"VACUUM"
%token<str> PGS_VALUES		"VALUES"

%token<str> PGS_IDENTIFIER	"IDENTIFIER"
%token<str> PGS_VAL_INT		"INTEGER VALUE"
%token<str> PGS_VAL_REAL	"REAL VALUE"
%token<str> PGS_VAL_STR		"STRING VALUE"

%destructor { pdelete($$); } PGS_ABORT
%destructor { pdelete($$); } PGS_ALTER
%destructor { pdelete($$); } PGS_ANALYZE
%destructor { pdelete($$); } PGS_BEGIN
%destructor { pdelete($$); } PGS_CHECKPOINT
%destructor { pdelete($$); } PGS_CLOSE_ST
%destructor { pdelete($$); } PGS_CLUSTER
%destructor { pdelete($$); } PGS_COMMENT
%destructor { pdelete($$); } PGS_COMMIT
%destructor { pdelete($$); } PGS_COPY
%destructor { pdelete($$); } PGS_CREATE
%destructor { pdelete($$); } PGS_DEALLOCATE
%destructor { pdelete($$); } PGS_DECLARE
%destructor { pdelete($$); } PGS_DELETE
%destructor { pdelete($$); } PGS_DISCARD
%destructor { pdelete($$); } PGS_DROP
%destructor { pdelete($$); } PGS_END_ST
%destructor { pdelete($$); } PGS_EXECUTE
%destructor { pdelete($$); } PGS_EXPLAIN
%destructor { pdelete($$); } PGS_FETCH
%destructor { pdelete($$); } PGS_GRANT
%destructor { pdelete($$); } PGS_INSERT
%destructor { pdelete($$); } PGS_LISTEN
%destructor { pdelete($$); } PGS_LOAD
%destructor { pdelete($$); } PGS_LOCK
%destructor { pdelete($$); } PGS_MOVE
%destructor { pdelete($$); } PGS_NOTIFY
%destructor { pdelete($$); } PGS_PREPARE
%destructor { pdelete($$); } PGS_REASSIGN
%destructor { pdelete($$); } PGS_REINDEX
%destructor { pdelete($$); } PGS_RELEASE
%destructor { pdelete($$); } PGS_RESET
%destructor { pdelete($$); } PGS_REVOKE
%destructor { pdelete($$); } PGS_ROLLBACK
%destructor { pdelete($$); } PGS_SAVEPOINT
%destructor { pdelete($$); } PGS_SELECT
%destructor { pdelete($$); } PGS_SET
%destructor { pdelete($$); } PGS_SHOW
%destructor { pdelete($$); } PGS_START
%destructor { pdelete($$); } PGS_TRUNCATE
%destructor { pdelete($$); } PGS_UNLISTEN
%destructor { pdelete($$); } PGS_UPDATE
%destructor { pdelete($$); } PGS_VACUUM
%destructor { pdelete($$); } PGS_VALUES

%destructor { pdelete($$); } PGS_IDENTIFIER
%destructor { pdelete($$); } PGS_VAL_INT
%destructor { pdelete($$); } PGS_VAL_REAL
%destructor { pdelete($$); } PGS_VAL_STR

%type<expr> postfix_expression
%type<expr> unary_expression
%type<expr> cast_expression
%type<expr> multiplicative_expression
%type<expr> additive_expression
%type<expr> relational_expression
%type<expr> equality_expression
%type<expr> logical_and_expression
%type<expr> logical_or_expression
%type<expr> expression
%type<expr> random_generator
%type<expr> sql_expression

%type<str> sql_query
%type<integer> type_name

%type<stmt> statement

%type<stmt> compound_statement
%type<stmt> sql_statement
%type<stmt> selection_statement
%type<stmt> iteration_statement
%type<stmt> procedure_statement
%type<stmt> jump_statement
%type<stmt> declaration_statement
%type<stmt> assign_statement

%type<stmt_list> declaration_list
%type<stmt_list> assign_list
%type<stmt> declaration_element
%type<stmt> assign_element

%type<stmt_list> statement_list

%{

#include "pgscript/utilities/pgsDriver.h"
#include "pgscript/utilities/pgsScanner.h"

/* This "connects" the bison parser in the driver to the flex scanner class
 * object. It defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef  yylex
#define yylex driver.lexer->lex

%}

%% /*** Grammar Rules ***/
	
postfix_expression
	: PGS_IDENTIFIER '[' expression ']' '[' expression ']'
								{
									$$ = pnew pgsIdentRecord(*($1), $3, $6);
									pdelete($1);
									driver.context.pop_var(); driver.context.pop_var(); // $3 & $6
									driver.context.push_var($$);
								}
	| PGS_IDENTIFIER '[' expression ']'
								{
									$$ = pnew pgsIdentRecord(*($1), $3);
									pdelete($1);
									driver.context.pop_var(); // $3
									driver.context.push_var($$);
								}
	| PGS_CNT_LINES '(' PGS_IDENTIFIER ')'
								{
									$$ = pnew pgsLines(*($3));
									pdelete($3);
									driver.context.push_var($$);
								}
	| PGS_CNT_COLUMNS '(' PGS_IDENTIFIER ')'
								{
									$$ = pnew pgsColumns(*($3));
									pdelete($3);
									driver.context.push_var($$);
								}
	| PGS_TRIM '(' expression ')'
								{
									$$ = pnew pgsTrim($3);
									driver.context.pop_var(); // $3
									driver.context.push_var($$); // assert
								}
	| PGS_IDENTIFIER
								{
									$$ = pnew pgsIdent(*($1));
									pdelete($1);
									driver.context.push_var($$);
								}
	| PGS_VAL_INT
								{
									$$ = pnew pgsNumber(*($1), pgsInt);
									pdelete($1);
									driver.context.push_var($$);
								}
	| PGS_VAL_REAL
								{
									$$ = pnew pgsNumber(*($1), pgsReal);
									pdelete($1);
									driver.context.push_var($$);
								}
	| PGS_VAL_STR
								{
									$$ = pnew pgsString(*($1));
									pdelete($1);
									driver.context.push_var($$);
								}
	| '(' PGS_SELECT ')'		{
									$$ = pnew pgsExecute(*($2), &driver.context.m_cout,
											&(driver.thread));
									pdelete($2);
									driver.context.push_var($$); // SQL Expression statement
								}
	| random_generator			{ $$ = $1; }
	| '(' expression ')'		{
									$$ = pnew pgsParenthesis($2);
									driver.context.pop_var(); // $2
									driver.context.push_var($$);
								}
	;

unary_expression
	: postfix_expression		{ $$ = $1; }
	| '+' cast_expression		{ $$ = $2; }
	| '-' cast_expression		{
									$$ = pnew pgsNegate($2);
									driver.context.pop_var(); // $2
									driver.context.push_var($$);
								}
	| PGS_NOT_OP cast_expression
								{
									$$ = pnew pgsNot($2);
									driver.context.pop_var(); // $2
									driver.context.push_var($$);
								}
	;

cast_expression
	: unary_expression			{ $$ = $1; }
	| PGS_CAST '(' cast_expression PGS_AS type_name ')'
								{
									$$ = pnew pgsCast($5, $3);
									driver.context.pop_var(); // $3
									driver.context.push_var($$);
								}
	;	

type_name
	: PGS_INTEGER				{ $$ = pgscript::pgsParser::token::PGS_INTEGER; }
	| PGS_REAL					{ $$ = pgscript::pgsParser::token::PGS_REAL; }
	| PGS_STRING				{ $$ = pgscript::pgsParser::token::PGS_STRING; }
	| PGS_RECORD				{ $$ = pgscript::pgsParser::token::PGS_RECORD; }
	;
	
multiplicative_expression
	: cast_expression			{ $$ = $1; }
	| multiplicative_expression '*' cast_expression
								{
									$$ = pnew pgsTimes($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	| multiplicative_expression '/' cast_expression
								{
									$$ = pnew pgsOver($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	| multiplicative_expression '%' cast_expression
								{
									$$ = pnew pgsModulo($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	;
	
additive_expression
	: multiplicative_expression	{ $$ = $1; }
	| additive_expression '+' multiplicative_expression
								{
									$$ = pnew pgsPlus($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	| additive_expression '-' multiplicative_expression
								{
									$$ = pnew pgsMinus($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	;

relational_expression
	: additive_expression		{ $$ = $1; }
	| relational_expression '<' additive_expression
								{
									$$ = pnew pgsLower($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	| relational_expression '>' additive_expression
								{
									$$ = pnew pgsGreater($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	| relational_expression PGS_LE_OP additive_expression
								{
									$$ = pnew pgsLowerEqual($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	| relational_expression PGS_GE_OP additive_expression
								{
									$$ = pnew pgsGreaterEqual($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	;

equality_expression
	: relational_expression		{ $$ = $1; }
	| equality_expression PGS_EQ_OP relational_expression
								{
									$$ = pnew pgsEqual($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	| equality_expression PGS_AE_OP relational_expression
								{
									$$ = pnew pgsEqual($1, $3, false);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	| equality_expression PGS_NE_OP relational_expression
								{
									$$ = pnew pgsDifferent($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	;

logical_and_expression
	: equality_expression		{ $$ = $1; }
	| logical_and_expression PGS_AND_OP equality_expression
								{
									$$ = pnew pgsAnd($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	;

logical_or_expression
	: logical_and_expression	{ $$ = $1; }
	| logical_or_expression PGS_OR_OP logical_and_expression
								{
									$$ = pnew pgsOr($1, $3);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var($$);
								}
	;

expression
	: logical_or_expression		{
									wxLogScriptVerbose(wxT("%s"), $1->value().c_str());
									$$ = $1;
								}
	;
	
random_generator
	: PGS_INTEGER '(' expression ',' expression ')' 
								{
									$$ = pnew pgsGenInt($3, $5, driver.context.zero(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_INTEGER '(' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenInt($3, $5, $7, driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_INTEGER '(' expression ',' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenInt($3, $5, $7, $9);
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_REAL '(' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenReal($3, $5, $7, driver.context.zero(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_REAL '(' expression ',' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenReal($3, $5, $7, $9, driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_REAL '(' expression ',' expression ',' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenReal($3, $5, $7, $9, $11);
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_STRING '(' expression ',' expression ')'
								{
									$$ = pnew pgsGenString($3, $5, driver.context.one(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_STRING '(' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenString($3, $5, $7, driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_STRING '(' expression ',' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenString($3, $5, $7, $9);
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_REGEX '(' expression ')'
								{
									$$ = pnew pgsGenRegex($3, driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_REGEX '(' expression ',' expression ')'
								{
									$$ = pnew pgsGenRegex($3, $5);
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_FILE '(' expression ')'
								{
									$$ = pnew pgsGenDictionary($3, driver.context.zero(),
											driver.context.seed(), driver.context.encoding());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_FILE '(' expression ',' expression ')'
								{
									$$ = pnew pgsGenDictionary($3, $5, driver.context.seed(),
											driver.context.encoding());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_FILE '(' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenDictionary($3, $5, $7, driver.context.encoding());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_FILE '(' expression ',' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenDictionary($3, $5, $7, $9);
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_DATE '(' expression ',' expression ')'
								{
									$$ = pnew pgsGenDate($3, $5, driver.context.zero(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_DATE '(' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenDate($3, $5, $7, driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_DATE '(' expression ',' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenDate($3, $5, $7, $9);
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_TIME '(' expression ',' expression ')'
								{
									$$ = pnew pgsGenTime($3, $5, driver.context.zero(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_TIME '(' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenTime($3, $5, $7, driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_TIME '(' expression ',' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenTime($3, $5, $7, $9);
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_DATE_TIME '(' expression ',' expression ')'
								{
									$$ = pnew pgsGenDateTime($3, $5, driver.context.zero(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_DATE_TIME '(' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenDateTime($3, $5, $7, driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_DATE_TIME '(' expression ',' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenDateTime($3, $5, $7, $9);
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_REFERENCE '(' expression ',' expression ')'
								{
									$$ = pnew pgsGenReference($3, $5, driver.context.zero(),
											driver.context.seed(), &(driver.thread));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_REFERENCE '(' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenReference($3, $5, $7, driver.context.seed(),
											&(driver.thread));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	| PGS_REFERENCE '(' expression ',' expression ',' expression ',' expression ')'
								{
									$$ = pnew pgsGenReference($3, $5, $7, $9, &(driver.thread));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var($$);
								}
	;

statement
	: compound_statement		{ $$ = $1; }
	| selection_statement		{ $$ = $1; }
	| iteration_statement		{ $$ = $1; }
	| sql_statement ';'			{ $$ = $1; }
	| procedure_statement ';'	{ $$ = $1; }
	| jump_statement ';'		{ $$ = $1; }
	| declaration_statement ';'	{ $$ = $1; }
	| assign_statement ';'		{ $$ = $1; }
	;

statement_list
	: statement					{
									driver.context.pop_stmt(); // $1
									$$ = driver.context.stmt_list(&(driver.thread));
									$$->insert_back($1);
								}
	| statement_list statement	{
									driver.context.pop_stmt(); // $2
									$$ = $1;						
									$$->insert_back($2);
								}
	;
	
compound_statement
	: PGS_OPEN PGS_CLOSE		{
									wxLogScriptVerbose(wxT("BEGIN END"));
									$$ = driver.context.stmt_list(&(driver.thread));
								}
	| PGS_OPEN statement_list PGS_CLOSE
								{
									wxLogScriptVerbose(wxT("BEGIN ... END"));
									$$ = $2;
								}
	;
	
sql_statement
	: sql_expression			{
									wxLogScriptVerbose(wxT("%s"), $1->value().c_str());
									$$ = pnew pgsExpressionStmt($1, &(driver.thread));
									driver.context.pop_var(); // $1
									driver.context.push_stmt($$); // pgsExpressionStmt
									$$->set_position(yyloc.begin.line);
								}
	;
	
sql_expression
	: sql_query 				{
									$$ = pnew pgsExecute(*($1), &driver.context.m_cout,
											&(driver.thread));
									pdelete($1);
									driver.context.push_var($$); // pgsExecute
								}
	;
	
sql_query
	: PGS_ABORT					{ $$ = $1; }
	| PGS_ALTER					{ $$ = $1; }
	| PGS_ANALYZE				{ $$ = $1; }
	| PGS_BEGIN					{ $$ = $1; }
	| PGS_CHECKPOINT			{ $$ = $1; }
	| PGS_CLOSE_ST				{ $$ = $1; }
	| PGS_CLUSTER				{ $$ = $1; }
	| PGS_COMMENT				{ $$ = $1; }
	| PGS_COMMIT				{ $$ = $1; }
	| PGS_COPY					{ $$ = $1; }
	| PGS_CREATE				{ $$ = $1; }
	| PGS_DEALLOCATE			{ $$ = $1; }
	| PGS_DECLARE				{ $$ = $1; }
	| PGS_DELETE				{ $$ = $1; }
	| PGS_DISCARD				{ $$ = $1; }
	| PGS_DROP					{ $$ = $1; }
	| PGS_END_ST				{ $$ = $1; }
	| PGS_EXECUTE				{ $$ = $1; }
	| PGS_EXPLAIN				{ $$ = $1; }
	| PGS_FETCH					{ $$ = $1; }
	| PGS_GRANT					{ $$ = $1; }
	| PGS_INSERT				{ $$ = $1; }
	| PGS_LISTEN				{ $$ = $1; }
	| PGS_LOAD					{ $$ = $1; }
	| PGS_LOCK					{ $$ = $1; }
	| PGS_MOVE					{ $$ = $1; }
	| PGS_NOTIFY				{ $$ = $1; }
	| PGS_PREPARE				{ $$ = $1; }
	| PGS_REASSIGN				{ $$ = $1; }
	| PGS_REINDEX				{ $$ = $1; }
	| PGS_RELEASE				{ $$ = $1; }
	| PGS_RESET					{ $$ = $1; }
	| PGS_REVOKE				{ $$ = $1; }
	| PGS_ROLLBACK				{ $$ = $1; }
	| PGS_SAVEPOINT				{ $$ = $1; }
	| PGS_SELECT				{ $$ = $1; }
	| PGS_SET					{ $$ = $1; }
	| PGS_SHOW					{ $$ = $1; }
	| PGS_START					{ $$ = $1; }
	| PGS_TRUNCATE				{ $$ = $1; }
	| PGS_UNLISTEN				{ $$ = $1; }
	| PGS_UPDATE				{ $$ = $1; }
	| PGS_VACUUM				{ $$ = $1; }
	| PGS_VALUES				{ $$ = $1; }
	;
	
declaration_statement
	: PGS_DECLARE_ASSGN declaration_list
								{
									$$ = $2;
								}
	;
	
declaration_list
	: declaration_element		{
									driver.context.pop_stmt(); // $1
									$$ = driver.context.stmt_list(&(driver.thread));
									$$->insert_back($1);

								}
	| declaration_list ',' declaration_element
								{
									driver.context.pop_stmt(); // $3
									$$ = $1;
									$$->insert_back($3);
								}
	;
	
declaration_element
	: PGS_IDENTIFIER			{
									wxLogScriptVerbose(wxT("DECLARE %s"), $1->c_str());
									
									$$ = pnew pgsExpressionStmt(pnew pgsAssign(*($1),
											pnew pgsString(wxT(""))), &(driver.thread));
									driver.context.push_stmt($$); // pgsExpressionStmt
									$$->set_position(yyloc.begin.line);
									
									pdelete($1);
								}
	|  PGS_IDENTIFIER '{' record_declaration_list '}'
								{
									wxLogScriptVerbose(wxT("DECLARE %s"), $1->c_str());
									
									$$ = pnew pgsDeclareRecordStmt(*($1), driver.context.columns(),
											&(driver.thread));
									driver.context.push_stmt($$); // pgsDeclareRecordStmt
									$$->set_position(yyloc.begin.line);
									
									driver.context.clear_columns();
									pdelete($1);
								}
	;

assign_statement
	: PGS_SET_ASSIGN assign_list
								{
									$$ = $2;
								}
	;

assign_list
	: assign_element			{
									driver.context.pop_stmt(); // $1
									$$ = driver.context.stmt_list(&(driver.thread));
									$$->insert_back($1);
								}
	| assign_list ',' assign_element
								{
									driver.context.pop_stmt(); // $3
									$$ = $1;
									$$->insert_back($3);
								}
	;

assign_element
	: PGS_IDENTIFIER PGS_EQ_OP expression
								{
									wxLogScriptVerbose(wxT("SET %s = %s"), $1->c_str(),
											$3->value().c_str());
									
									$$ = pnew pgsExpressionStmt(pnew pgsAssign(*($1), $3),
											&(driver.thread));
									driver.context.pop_var(); // $3
									driver.context.push_stmt($$); // pgsExpressionStmt
									$$->set_position(yyloc.begin.line);
									
									pdelete($1);
								}
	| PGS_IDENTIFIER '[' expression ']' '[' expression ']' PGS_EQ_OP expression
								{
									wxLogScriptVerbose(wxT("SET %s[%s][%s] = %s"),
											$1->c_str(), $3->value().c_str(),
											$6->value().c_str(), $9->value().c_str());
									
									$$ = pnew pgsExpressionStmt(pnew pgsAssignToRecord(*($1),
											$3, $6, $9), &(driver.thread));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); // $3 & $6 & $9
									driver.context.push_stmt($$); // pgsExpressionStmt
									$$->set_position(yyloc.begin.line);
									
									pdelete($1);
								}
	| PGS_IDENTIFIER PGS_EQ_OP sql_expression
								{
									wxLogScriptVerbose(wxT("SET %s = %s"), $1->c_str(),
											$3->value().c_str());
									
									$$ = pnew pgsExpressionStmt(pnew pgsAssign(*($1), $3),
											&(driver.thread));
									driver.context.pop_var(); // $3
									driver.context.push_stmt($$); // pgsExpressionStmt
									$$->set_position(yyloc.begin.line);
									
									pdelete($1);
								}
	;

selection_statement
	: PGS_IF expression statement %prec PGS_ELSE
								{
									wxLogScriptVerbose(wxT("IF %s"), $2->value().c_str());
									
									$$ = pnew pgsIfStmt($2, $3, driver.context
											.stmt_list(&(driver.thread)), &(driver.thread));
									driver.context.pop_var(); // $2
									driver.context.pop_stmt(); // $3
									driver.context.pop_stmt(); // stmt_list
									driver.context.push_stmt($$); // pgsIfStmt
									$$->set_position(yyloc.begin.line);
								}
	| PGS_IF expression statement PGS_ELSE statement
								{
									wxLogScriptVerbose(wxT("IF %s"), $2->value().c_str());
									
									$$ = pnew pgsIfStmt($2, $3, $5, &(driver.thread));
									driver.context.pop_var(); // $2
									driver.context.pop_stmt(); // $3
									driver.context.pop_stmt(); // $5
									driver.context.push_stmt($$); // pgsIfStmt
									$$->set_position(yyloc.begin.line);
								}
	;

iteration_statement
	: PGS_WHILE expression statement
								{
									wxLogScriptVerbose(wxT("WHILE %s"), $2->value().c_str());
									
									$$ = pnew pgsWhileStmt($2, $3, &(driver.thread));
									driver.context.pop_var(); // $2
									driver.context.pop_stmt(); // $3
									driver.context.push_stmt($$); // pgsWhileStmt
									$$->set_position(yyloc.begin.line);
								}
	;
	
jump_statement
	: PGS_BREAK					{
									wxLogScriptVerbose(wxT("BREAK"));
									
									$$ = pnew pgsBreakStmt(&(driver.thread));
									driver.context.push_stmt($$); // pgsBreakStmt
									$$->set_position(yyloc.begin.line);
								}
	| PGS_RETURN				{
									wxLogScriptVerbose(wxT("RETURN"));
									
									$$ = pnew pgsBreakStmt(&(driver.thread));
									driver.context.push_stmt($$); // pgsBreakStmt
									$$->set_position(yyloc.begin.line);
								}
	| PGS_CONTINUE				{
									wxLogScriptVerbose(wxT("CONTINUE"));
									
									$$ = pnew pgsContinueStmt(&(driver.thread));
									driver.context.push_stmt($$); // pgsContinueStmt
									$$->set_position(yyloc.begin.line);
								}
	;
	
procedure_statement
	: PGS_PRINT expression
								{
									wxLogScriptVerbose(wxT("PRINT %s"), $2->value().c_str());
									
									$$ = pnew pgsPrintStmt($2, driver.context.m_cout,
											&(driver.thread));
									driver.context.pop_var(); // $2
									driver.context.push_stmt($$); // pgsPrintStmt
									$$->set_position(yyloc.begin.line);
								}
	| PGS_ASSERT expression
								{
									wxLogScriptVerbose(wxT("ASSERT %s"), $2->value().c_str());
									
									$$ = pnew pgsAssertStmt($2, &(driver.thread));
									driver.context.pop_var(); // $2
									driver.context.push_stmt($$); // pgsAssertStmt
									$$->set_position(yyloc.begin.line);
								}
	| PGS_RM_LINE '(' PGS_IDENTIFIER '[' expression ']' ')'
								{
									wxLogScriptVerbose(wxT("RMLINE %s[%s]"), $3->c_str(),
											$5->value().c_str());
									
									$$ = pnew pgsExpressionStmt(pnew pgsRemoveLine(*($3), $5),
											&(driver.thread));
									driver.context.pop_var(); // $5
									driver.context.push_stmt($$); // pgsExpressionStmt
									$$->set_position(yyloc.begin.line);
									
									pdelete($3);
								}
	;
	
record_declaration_list
	: PGS_IDENTIFIER			{
									driver.context.add_column(*$1);
									pdelete($1);
								}
	| record_declaration_list ',' PGS_IDENTIFIER
								{
									driver.context.add_column(*$3);
									pdelete($3);
								}
	;

translation_unit
	: PGS_END
	| statement_list PGS_END	{
									driver.program.eval($1);
									
									driver.context.pop_stmt();
									pdelete($1); // delete root statement $1
								}
	;

%% /*** Additional Code ***/

void pgscript::pgsParser::error(const pgsParser::location_type & l,
		const std::string & m)
{
	wxLogScriptVerbose(wxT("EXPR STACK SIZE = %u"), driver.context.size_vars());
	wxLogScriptVerbose(wxT("STMT STACK SIZE = %u"), driver.context.size_stmts());
	driver.context.clear_stacks();
	driver.error(l, wxString(m.c_str(), wxConvUTF8));
}
