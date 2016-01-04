//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSASSIGNTORECORD_H_
#define PGSASSIGNTORECORD_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsAssign.h"

class pgsAssignToRecord : public pgsAssign
{

private:

	const pgsExpression *m_line;
	const pgsExpression *m_column;

public:

	pgsAssignToRecord(const wxString &name, const pgsExpression *line,
	                  const pgsExpression *column, const pgsExpression *var);

	virtual ~pgsAssignToRecord();

	pgsAssignToRecord(const pgsAssignToRecord &that);

	pgsAssignToRecord &operator=(const pgsAssignToRecord &that);

	virtual pgsExpression *clone() const;

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSASSIGNTORECORD_H_*/
