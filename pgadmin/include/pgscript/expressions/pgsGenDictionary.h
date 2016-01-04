//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSGENDICTIONARY_H_
#define PGSGENDICTIONARY_H_

#include "pgscript/pgScript.h"
#include "pgscript/expressions/pgsExpression.h"

class pgsGenDictionary : public pgsExpression
{

private:

	const pgsExpression *m_file_path;
	const pgsExpression *m_sequence;
	const pgsExpression *m_seed;
	const pgsExpression *m_wx_conv;

public:

	pgsGenDictionary(const pgsExpression *file_path,
	                 const pgsExpression *sequence,
	                 const pgsExpression *seed, const pgsExpression *wx_conv);

	virtual ~pgsGenDictionary();

	virtual pgsExpression *clone() const;

	pgsGenDictionary(const pgsGenDictionary &that);

	pgsGenDictionary &operator =(const pgsGenDictionary &that);

public:

	virtual wxString value() const;

	virtual pgsOperand eval(pgsVarMap &vars) const;

};

#endif /*PGSGENDICTIONARY_H_*/
