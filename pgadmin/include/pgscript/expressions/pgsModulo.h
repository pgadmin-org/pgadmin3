//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsModulo.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSMODULO_H_
#define PGSMODULO_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsModulo : public pgsOperation
{
	
public:

	pgsModulo(const pgsExpression * left, const pgsExpression * right);

	virtual ~pgsModulo();

	virtual pgsExpression * clone() const;

	pgsModulo(const pgsModulo & that);

	pgsModulo & operator =(const pgsModulo & that);
	
	virtual wxString value() const;
	
	virtual pgsOperand eval(pgsVarMap & vars) const;
	
};

#endif /*PGSMODULO_H_*/
