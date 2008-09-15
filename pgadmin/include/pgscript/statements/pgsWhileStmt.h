//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsWhileStmt.h,v 1.2 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSWHILESTMT_H_
#define PGSWHILESTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

class pgsWhileStmt : public pgsStmt
{

private:

	const pgsExpression * m_cond;
	const pgsStmt * m_stmt_list;

public:

	pgsWhileStmt(const pgsExpression * cond, const pgsStmt * stmt_list,
			pgsThread * app = 0);

	virtual ~pgsWhileStmt();

	virtual void eval(pgsVarMap & vars) const;

private:

	pgsWhileStmt(const pgsWhileStmt & that);

	pgsWhileStmt & operator=(const pgsWhileStmt & that);

};

#endif /*PGSWHILESTMT_H_*/
