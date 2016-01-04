//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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

	pgsIdent(const wxString &name);

	virtual ~pgsIdent();

	/* pgsIdent(const pgsIdent & that); */

	/* pgsIdent & operator=(const pgsIdent & that); */

	virtual pgsExpression *clone() const;

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

public:

	static const wxString m_now;

};

#endif /*PGSIDENT_H_*/
