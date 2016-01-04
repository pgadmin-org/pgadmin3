//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSEQUAL_H_
#define PGSEQUAL_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsOperation.h"

class pgsEqual : public pgsOperation
{

private:

	bool m_case_sensitive;

public:

	pgsEqual(const pgsExpression *left, const pgsExpression *right,
	         bool case_sensitive = true);

	virtual ~pgsEqual();

	virtual pgsExpression *clone() const;

	pgsEqual(const pgsEqual &that);

	pgsEqual &operator =(const pgsEqual &that);

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSEQUAL_H_*/
