//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSCAST_H_
#define PGSCAST_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsCast : public pgsExpression
{

private:

	int m_cast_type;

	const pgsExpression *m_var;

public:

	pgsCast(const int &cast_type, const pgsExpression *var);

	virtual ~pgsCast();

	virtual pgsExpression *clone() const;

	pgsCast(const pgsCast &that);

	pgsCast &operator=(const pgsCast &that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSCAST_H_*/
