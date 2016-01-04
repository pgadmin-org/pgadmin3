//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSTRIM_H_
#define PGSTRIM_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsTrim : public pgsExpression
{

private:

	const pgsExpression *m_exp;

public:

	pgsTrim(const pgsExpression *exp);

	virtual ~pgsTrim();

	pgsTrim(const pgsTrim &that);

	pgsTrim &operator=(const pgsTrim &that);

	virtual pgsExpression *clone() const;

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSTRIM_H_*/
