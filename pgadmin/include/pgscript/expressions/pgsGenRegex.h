//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENREGEX_H_
#define PGSGENREGEX_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsGenRegex : public pgsExpression
{

private:

	const pgsExpression *m_regex;
	const pgsExpression *m_seed;

public:

	pgsGenRegex(const pgsExpression *regex, const pgsExpression *seed);

	virtual ~pgsGenRegex();

	virtual pgsExpression *clone() const;

	pgsGenRegex(const pgsGenRegex &that);

	pgsGenRegex &operator =(const pgsGenRegex &that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSGENREGEX_H_*/
