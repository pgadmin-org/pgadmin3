//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSEXECUTE_H_
#define PGSEXECUTE_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsOutputStream;
class pgsThread;

class pgsExecute : public pgsExpression
{

private:

	wxString m_query;

	pgsOutputStream *m_cout;

	pgsThread *m_app;

public:

	pgsExecute(const wxString &query, pgsOutputStream *cout = 0,
	           pgsThread *app = 0);

	virtual ~pgsExecute();

	/* pgsExecute(const pgsExecute & that); */

	pgsExecute &operator=(const pgsExecute &that);

	virtual pgsExpression *clone() const;

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSEXECUTE_H_*/
