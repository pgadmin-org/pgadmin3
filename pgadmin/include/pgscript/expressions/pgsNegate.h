//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsNegate.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSNEGATE_H_
#define PGSNEGATE_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsNegate : public pgsOperation
{
	
public:

	pgsNegate(const pgsExpression * left);

	virtual ~pgsNegate();

	virtual pgsExpression * clone() const;

	pgsNegate(const pgsNegate & that);

	pgsNegate & operator =(const pgsNegate & that);

	virtual wxString value() const;
	
	virtual pgsOperand eval(pgsVarMap & vars) const;
	
};

#endif /*PGSNEGATE_H_*/
