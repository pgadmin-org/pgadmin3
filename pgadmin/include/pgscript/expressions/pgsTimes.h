//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSTIMES_H_
#define PGSTIMES_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsTimes : public pgsOperation
{

public:

	pgsTimes(const pgsExpression *left, const pgsExpression *right);

	virtual ~pgsTimes();

	virtual pgsExpression *clone() const;

	pgsTimes(const pgsTimes &that);

	pgsTimes &operator =(const pgsTimes &that);

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSTIMES_H_*/
