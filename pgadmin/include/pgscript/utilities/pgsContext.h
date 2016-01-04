//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSCONTEXT_H_
#define PGSCONTEXT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmtList.h"

WX_DECLARE_LIST(pgsExpression, pgsListExpression);

class pgsThread;

/** pgsContext is kind of a util class used during script parsing.
 * It is used in pgsParser.yy. Some members are made public because they need
 * to be easily available in order to be passed to some other objects. */
class pgsContext
{

private:

	wxArrayString m_columns;

	/** List of temporary expressions or variables. */
	pgsListExpression m_vars;

	/** List of temporary statements. */
	pgsListStmt m_stmts;

public:

	/** For writing to the output. */
	pgsOutputStream &m_cout;

public:

	//////////////////////////////
	// Constructor & destructor //
	//////////////////////////////

	pgsContext(pgsOutputStream &cout);

	~pgsContext();

	///////////////////////////////
	// Methods generating values //
	///////////////////////////////

	/** Generates a pgsNumber with value '0' and put it on stack. */
	pgsVariable *zero();

	/** Generates a pgsNumber with value '0' and put it on stack. */
	pgsVariable *one();

	/** Generates a pgsNumber with value now() and put it on stack. */
	pgsVariable *seed();

	/** Generates a pgsString with the locale encoding and put it on stack. */
	pgsVariable *encoding();

	/** Generates an empty statement list and put it on stack. */
	pgsStmtList *stmt_list(pgsThread *app = 0);

	////////////////////////////////////////////////
	// For managing a new record declaration list //
	////////////////////////////////////////////////

	/** Adds a column name to the column list. */
	void add_column(const wxString &column);

	/** Retrieves the column list. */
	const wxArrayString &columns();

	/** Clears the column list. */
	void clear_columns();


	/////////////////////////////////////////////
	// For managing stacks of temporary values //
	/////////////////////////////////////////////

	/** Adds a pgsExpression on stack. */
	void push_var(pgsExpression *var);

	/** Removes the last pgsExpression on stack. */
	void pop_var();

	/** Gives the number of pgsExpression on stack. */
	size_t size_vars() const;

	/** Adds a pgsStmt on stack. */
	void push_stmt(pgsStmt *stmt);

	/** Removes the last pgsStmt on stack. */
	void pop_stmt();

	/** Gives the number of pgsStmt on stack. */
	size_t size_stmts() const;

	/** When an error occurs in the parser this method must be called in order
	 * to free the memory (i.e the temporary pgsExpression & pgsStmt). */
	void clear_stacks();

private:

	pgsContext(const pgsContext &that);

	pgsContext &operator=(const pgsContext &that);

};

#endif /*PGSCONTEXT_H_*/
