//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSPROGRAM_H_
#define PGSPROGRAM_H_

#include "pgscript/pgScript.h"
#include "pgscript/objects/pgsVariable.h"

#include <wx/thread.h>

class pgsStmtList;

class pgsProgram
{

private:

	pgsVarMap &m_vars;

public:

	pgsProgram(pgsVarMap &vars);

	~pgsProgram();

	void dump();

	static void dump(const pgsVarMap &vars);

	void eval(pgsStmtList *stmt_list);

private:

	pgsProgram(const pgsProgram &that);

	pgsProgram &operator=(const pgsProgram &that);

};

#endif /*PGSPROGRAM_H_*/
