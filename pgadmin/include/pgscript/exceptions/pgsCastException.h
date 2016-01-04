//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSCASTEXCEPTION_H_
#define PGSCASTEXCEPTION_H_

#include "pgscript/pgScript.h"
#include "pgscript/exceptions/pgsException.h"

class pgsCastException : public pgsException
{

protected:

	const wxString m_value;
	const wxString m_type;

public:

	pgsCastException(const wxString &value, const wxString &type);

	virtual ~pgsCastException();

	virtual const wxString message() const;

};

#endif /*PGSCASTEXCEPTION_H_*/
