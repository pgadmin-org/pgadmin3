//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsIdent.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSIDENT_H_
#define PGSIDENT_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsIdent : public pgsExpression
{

protected:

	wxString m_name;

public:

	pgsIdent(const wxString & name);

	virtual ~pgsIdent();

	/* pgsIdent(const pgsIdent & that); */

	/* pgsIdent & operator=(const pgsIdent & that); */

	virtual pgsExpression * clone() const;
	
	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap & vars) const;
	
public:
	
	static const wxString m_now;

};

#endif /*PGSIDENT_H_*/
