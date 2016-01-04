//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSCOLUMNS_H_
#define PGSCOLUMNS_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsColumns : public pgsExpression
{

private:

	wxString m_name;

public:

	pgsColumns(const wxString &name);

	virtual ~pgsColumns();

	/* pgsColumns(const pgsColumns & that); */

	/* pgsColumns & operator=(const pgsColumns & that); */

	virtual pgsExpression *clone() const;

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSCOLUMNS_H_*/
