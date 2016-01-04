//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENINT_H_
#define PGSGENINT_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsGenInt : public pgsExpression
{

private:

	const pgsExpression *m_min;
	const pgsExpression *m_max;
	const pgsExpression *m_sequence;
	const pgsExpression *m_seed;

public:

	pgsGenInt(const pgsExpression *min, const pgsExpression *max,
	          const pgsExpression *sequence, const pgsExpression *seed);

	virtual ~pgsGenInt();

	virtual pgsExpression *clone() const;

	pgsGenInt(const pgsGenInt &that);

	pgsGenInt &operator =(const pgsGenInt &that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSGENINT_H_*/
