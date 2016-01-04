//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSIDENTRECORD_H_
#define PGSIDENTRECORD_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsIdent.h"

class pgsIdentRecord : public pgsIdent
{

private:

	const pgsExpression *m_line;
	const pgsExpression *m_column;

public:

	pgsIdentRecord(const wxString &name, const pgsExpression *line,
	               const pgsExpression *column = 0);

	virtual ~pgsIdentRecord();

	pgsIdentRecord(const pgsIdentRecord &that);

	pgsIdentRecord &operator=(const pgsIdentRecord &that);

	virtual pgsExpression *clone() const;

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSIDENTRECORD_H_*/
