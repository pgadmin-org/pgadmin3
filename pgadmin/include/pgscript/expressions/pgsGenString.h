//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENSTRING_H_
#define PGSGENSTRING_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsGenString : public pgsExpression
{

private:

	const pgsExpression *m_min;
	const pgsExpression *m_max;
	const pgsExpression *m_nb_words;
	const pgsExpression *m_seed;

public:

	pgsGenString(const pgsExpression *min, const pgsExpression *max,
	             const pgsExpression *nb_words, const pgsExpression *seed);

	virtual ~pgsGenString();

	virtual pgsExpression *clone() const;

	pgsGenString(const pgsGenString &that);

	pgsGenString &operator =(const pgsGenString &that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSGENSTRING_H_*/
