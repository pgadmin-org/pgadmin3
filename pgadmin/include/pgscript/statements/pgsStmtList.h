//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSSTMTLIST_H_
#define PGSSTMTLIST_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

WX_DECLARE_LIST(pgsStmt, pgsListStmt);

class pgsStmtList : public pgsStmt
{

private:

	pgsListStmt m_stmt_list;

	pgsOutputStream &m_cout;

public:

	static bool m_exception_thrown;

public:

	pgsStmtList(pgsOutputStream &cout, pgsThread *app = 0);

	virtual ~pgsStmtList();

	virtual void eval(pgsVarMap &vars) const;

	void insert_front(pgsStmt *stmt);

	void insert_back(pgsStmt *stmt);

private:

	pgsStmtList(const pgsStmtList &that);

	pgsStmtList &operator=(const pgsStmtList &that);

};

#endif /*PGSSTMTLIST_H_*/
