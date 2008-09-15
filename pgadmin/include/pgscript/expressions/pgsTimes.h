//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsTimes.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSTIMES_H_
#define PGSTIMES_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsTimes : public pgsOperation
{
	
public:

	pgsTimes(const pgsExpression * left, const pgsExpression * right);

	virtual ~pgsTimes();

	virtual pgsExpression * clone() const;
	
	pgsTimes(const pgsTimes & that);

	pgsTimes & operator =(const pgsTimes & that);
	
	virtual wxString value() const;
	
	virtual pgsOperand eval(pgsVarMap & vars) const;
	
};

#endif /*PGSTIMES_H_*/
