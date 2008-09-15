//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsGenRegex.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENREGEX_H_
#define PGSGENREGEX_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsGenRegex : public pgsExpression
{

private:

	const pgsExpression * m_regex;
	const pgsExpression * m_seed;

public:

	pgsGenRegex(const pgsExpression * regex, const pgsExpression * seed);

	virtual ~pgsGenRegex();

	virtual pgsExpression * clone() const;

	pgsGenRegex(const pgsGenRegex & that);

	pgsGenRegex & operator =(const pgsGenRegex & that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap & vars) const;

};

#endif /*PGSGENREGEX_H_*/
