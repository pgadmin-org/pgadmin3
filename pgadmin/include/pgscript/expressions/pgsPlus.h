//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsPlus.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSPLUS_H_
#define PGSPLUS_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsPlus : public pgsOperation
{
	
public:

	pgsPlus(const pgsExpression * left, const pgsExpression * right);

	virtual ~pgsPlus();

	virtual pgsExpression * clone() const;
	
	pgsPlus(const pgsPlus & that);

	pgsPlus & operator =(const pgsPlus & that);
	
	virtual wxString value() const;
	
	virtual pgsOperand eval(pgsVarMap & vars) const;
	
};

#endif /*PGSPLUS_H_*/
