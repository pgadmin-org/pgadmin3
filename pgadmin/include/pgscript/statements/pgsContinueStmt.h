//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSCONTINUESTMT_H_
#define PGSCONTINUESTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

class pgsContinueStmt : public pgsStmt
{

public:

	pgsContinueStmt(pgsThread *app = 0);

	virtual ~pgsContinueStmt();

	virtual void eval(pgsVarMap &vars) const;

private:

	pgsContinueStmt(const pgsContinueStmt &that);

	pgsContinueStmt &operator=(const pgsContinueStmt &that);

};

#endif /*PGSCONTINUESTMT_H_*/
