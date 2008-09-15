//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSNOT_H_
#define PGSNOT_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsNot : public pgsOperation
{
	
public:

	pgsNot(const pgsExpression * left);

	virtual ~pgsNot();

	virtual pgsExpression * clone() const;

	pgsNot(const pgsNot & that);

	pgsNot & operator =(const pgsNot & that);

	virtual wxString value() const;
	
	virtual pgsOperand eval(pgsVarMap & vars) const;
	
};

#endif /*PGSNOT_H_*/
