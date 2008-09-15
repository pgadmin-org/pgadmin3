//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsCast.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
	
	const pgsExpression * m_var;

public:

	pgsCast(const int & cast_type, const pgsExpression * var);

	virtual ~pgsCast();
	
	virtual pgsExpression * clone() const;

	pgsCast(const pgsCast & that);

	pgsCast & operator=(const pgsCast & that);
	
public:
	
	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap & vars) const;

};

#endif /*PGSCAST_H_*/
