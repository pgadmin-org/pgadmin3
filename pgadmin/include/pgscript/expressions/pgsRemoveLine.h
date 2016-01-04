//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSREMOVELINE_H_
#define PGSREMOVELINE_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsRemoveLine : public pgsExpression
{

private:

	wxString m_rec;
	const pgsExpression *m_line;

public:

	pgsRemoveLine(const wxString &rec, const pgsExpression *line);

	virtual ~pgsRemoveLine();

	pgsRemoveLine(const pgsRemoveLine &that);

	pgsRemoveLine &operator=(const pgsRemoveLine &that);

	virtual pgsExpression *clone() const;

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSREMOVELINE_H_*/
