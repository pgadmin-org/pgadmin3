//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsLower.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSLOWER_H_
#define PGSLOWER_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsLower : public pgsOperation
{
	
public:

	pgsLower(const pgsExpression * left, const pgsExpression * right);

	virtual ~pgsLower();

	virtual pgsExpression * clone() const;

	pgsLower(const pgsLower & that);

	pgsLower & operator =(const pgsLower & that);

	virtual wxString value() const;
	
	virtual pgsOperand eval(pgsVarMap & vars) const;
	
};

#endif /*PGSLOWER_H_*/
