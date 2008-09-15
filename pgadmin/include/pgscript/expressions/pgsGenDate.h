//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENDATE_H_
#define PGSGENDATE_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsGenDate : public pgsExpression
{

private:

	const pgsExpression * m_min;
	const pgsExpression * m_max;
	const pgsExpression * m_sequence;
	const pgsExpression * m_seed;

public:

	pgsGenDate(const pgsExpression * min, const pgsExpression * max,
			const pgsExpression * sequence, const pgsExpression * seed);

	virtual ~pgsGenDate();

	virtual pgsExpression * clone() const;

	pgsGenDate(const pgsGenDate & that);

	pgsGenDate & operator =(const pgsGenDate & that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap & vars) const;

};

#endif /*PGSGENDATE_H_*/
