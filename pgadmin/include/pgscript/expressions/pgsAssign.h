//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSASSIGN_H_
#define PGSASSIGN_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsAssign : public pgsExpression
{

protected:

	wxString m_name;
	const pgsExpression *m_var;

public:

	pgsAssign(const wxString &name, const pgsExpression *var);

	virtual ~pgsAssign();

	pgsAssign(const pgsAssign &that);

	pgsAssign &operator=(const pgsAssign &that);

	virtual pgsExpression *clone() const;

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSASSIGN_H_*/
