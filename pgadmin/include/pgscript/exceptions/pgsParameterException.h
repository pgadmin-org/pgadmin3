//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSPARAMETEREXCEPTION_H_
#define PGSPARAMETEREXCEPTION_H_

#include "pgscript/pgScript.h"
#include "pgscript/exceptions/pgsException.h"

class pgsParameterException : public pgsException
{

protected:

	const wxString m_message;

public:

	pgsParameterException(const wxString &message = wxT("unknown"));

	virtual ~pgsParameterException();

	virtual const wxString message() const;

};

#endif /*PGSPARAMETEREXCEPTION_H_*/
