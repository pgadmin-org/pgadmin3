//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENREFERENCE_H_
#define PGSGENREFERENCE_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsThread;

class pgsGenReference : public pgsExpression
{

private:

	const pgsExpression *m_table;
	const pgsExpression *m_column;
	const pgsExpression *m_sequence;
	const pgsExpression *m_seed;

	pgsThread *m_app;

public:

	pgsGenReference(const pgsExpression *table, const pgsExpression *column,
	                const pgsExpression *sequence, const pgsExpression *seed,
	                pgsThread *app = 0);

	virtual ~pgsGenReference();

	virtual pgsExpression *clone() const;

	pgsGenReference(const pgsGenReference &that);

	pgsGenReference &operator =(const pgsGenReference &that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSGENREFERENCE_H_*/
