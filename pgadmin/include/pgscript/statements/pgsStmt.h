//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsStmt.h,v 1.3 2008/08/10 19:50:52 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
	
	pgsThread * m_app;

public:

	pgsStmt(pgsThread * app = 0);

	virtual ~pgsStmt();
	
	void set_position(int line);

	int line() const;

	virtual void eval(pgsVarMap & vars) const = 0;

private:

	pgsStmt(const pgsStmt & that);

	pgsStmt & operator=(const pgsStmt & that);

};

#endif /*PGSSTMT_H_*/
