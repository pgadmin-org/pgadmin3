//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsBreakStmt.h,v 1.2 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSBREAKSTMT_H_
#define PGSBREAKSTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

class pgsBreakStmt : public pgsStmt
{

public:

	pgsBreakStmt(pgsThread * app = 0);

	virtual ~pgsBreakStmt();

	virtual void eval(pgsVarMap & vars) const;
	
private:
	
	pgsBreakStmt(const pgsBreakStmt & that);

	pgsBreakStmt & operator=(const pgsBreakStmt & that);

};

#endif /*PGSBREAKSTMT_H_*/
