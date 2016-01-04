//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENDATETIME_H_
#define PGSGENDATETIME_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsGenDateTime : public pgsExpression
{

private:

	const pgsExpression *m_min;
	const pgsExpression *m_max;
	const pgsExpression *m_sequence;
	const pgsExpression *m_seed;

public:

	pgsGenDateTime(const pgsExpression *min, const pgsExpression *max,
	               const pgsExpression *sequence, const pgsExpression *seed);

	virtual ~pgsGenDateTime();

	virtual pgsExpression *clone() const;

	pgsGenDateTime(const pgsGenDateTime &that);

	pgsGenDateTime &operator =(const pgsGenDateTime &that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSGENDATETIME_H_*/
