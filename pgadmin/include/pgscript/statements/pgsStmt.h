//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSSTMT_H_
#define PGSSTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/objects/pgsVariable.h"

class pgsThread;

class pgsStmt
{

private:

	unsigned int m_line;

protected:

	pgsThread *m_app;

public:

	pgsStmt(pgsThread *app = 0);

	virtual ~pgsStmt();

	void set_position(int line);

	int line() const;

	virtual void eval(pgsVarMap &vars) const = 0;

private:

	pgsStmt(const pgsStmt &that);

	pgsStmt &operator=(const pgsStmt &that);

};

#endif /*PGSSTMT_H_*/
