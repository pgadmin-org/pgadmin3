//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSARITHMETICEXCEPTION_H_
#define PGSARITHMETICEXCEPTION_H_

#include "pgscript/pgScript.h"
#include "pgscript/exceptions/pgsException.h"

class pgsArithmeticException : public pgsException
{

protected:

	const wxString m_left;
	const wxString m_right;

public:

	pgsArithmeticException(const wxString &left, const wxString &right);

	virtual ~pgsArithmeticException();

	virtual const wxString message() const;

};

#endif /*PGSARITHMETICEXCEPTION_H_*/
