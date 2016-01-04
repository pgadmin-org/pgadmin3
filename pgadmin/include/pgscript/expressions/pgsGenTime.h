//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENTIME_H_
#define PGSGENTIME_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsGenTime : public pgsExpression
{

private:

	const pgsExpression *m_min;
	const pgsExpression *m_max;
	const pgsExpression *m_sequence;
	const pgsExpression *m_seed;

public:

	pgsGenTime(const pgsExpression *min, const pgsExpression *max,
	           const pgsExpression *sequence, const pgsExpression *seed);

	virtual ~pgsGenTime();

	virtual pgsExpression *clone() const;

	pgsGenTime(const pgsGenTime &that);

	pgsGenTime &operator =(const pgsGenTime &that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSGENTIME_H_*/
