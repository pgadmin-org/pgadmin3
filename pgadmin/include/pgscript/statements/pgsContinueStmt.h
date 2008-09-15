//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsContinueStmt.h,v 1.2 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSCONTINUESTMT_H_
#define PGSCONTINUESTMT_H_

#include "pgscript/pgScript.h"
#include "pgscript/statements/pgsStmt.h"

class pgsContinueStmt : public pgsStmt
{

public:

	pgsContinueStmt(pgsThread * app = 0);

	virtual ~pgsContinueStmt();

	virtual void eval(pgsVarMap & vars) const;
	
private:
	
	pgsContinueStmt(const pgsContinueStmt & that);

	pgsContinueStmt & operator=(const pgsContinueStmt & that);

};

#endif /*PGSCONTINUESTMT_H_*/
