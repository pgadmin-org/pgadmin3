//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsGenTime.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENTIME_H_
#define PGSGENTIME_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsGenTime : public pgsExpression
{

private:

	const pgsExpression * m_min;
	const pgsExpression * m_max;
	const pgsExpression * m_sequence;
	const pgsExpression * m_seed;

public:

	pgsGenTime(const pgsExpression * min, const pgsExpression * max,
			const pgsExpression * sequence, const pgsExpression * seed);

	virtual ~pgsGenTime();

	virtual pgsExpression * clone() const;

	pgsGenTime(const pgsGenTime & that);

	pgsGenTime & operator =(const pgsGenTime & that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap & vars) const;

};

#endif /*PGSGENTIME_H_*/
