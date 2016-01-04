//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSWHILESTMT_H_
#define PGSWHILESTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

class pgsWhileStmt : public pgsStmt
{

private:

	const pgsExpression *m_cond;
	const pgsStmt *m_stmt_list;

public:

	pgsWhileStmt(const pgsExpression *cond, const pgsStmt *stmt_list,
	             pgsThread *app = 0);

	virtual ~pgsWhileStmt();

	virtual void eval(pgsVarMap &vars) const;

private:

	pgsWhileStmt(const pgsWhileStmt &that);

	pgsWhileStmt &operator=(const pgsWhileStmt &that);

};

#endif /*PGSWHILESTMT_H_*/
