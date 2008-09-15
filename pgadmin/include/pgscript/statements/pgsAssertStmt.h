//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsAssertStmt.h,v 1.2 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSASSERTSTMT_H_
#define PGSASSERTSTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

class pgsAssertStmt : public pgsStmt
{

private:

	const pgsExpression * m_cond;

public:

	pgsAssertStmt(const pgsExpression * cond, pgsThread * app = 0);

	virtual ~pgsAssertStmt();

	virtual void eval(pgsVarMap & vars) const;

private:

	pgsAssertStmt(const pgsAssertStmt & that);

	pgsAssertStmt & operator=(const pgsAssertStmt & that);

};

#endif /*PGSASSERTSTMT_H_*/
