//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENREAL_H_
#define PGSGENREAL_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsGenReal : public pgsExpression
{

private:

	const pgsExpression *m_min;
	const pgsExpression *m_max;
	const pgsExpression *m_precision;
	const pgsExpression *m_sequence;
	const pgsExpression *m_seed;

public:

	pgsGenReal(const pgsExpression *min, const pgsExpression *max,
	           const pgsExpression *precision,
	           const pgsExpression *sequence, const pgsExpression *seed);

	virtual ~pgsGenReal();

	virtual pgsExpression *clone() const;

	pgsGenReal(const pgsGenReal &that);

	pgsGenReal &operator =(const pgsGenReal &that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSGENREAL_H_*/
