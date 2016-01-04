//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSASSERTEXCEPTION_H_
#define PGSASSERTEXCEPTION_H_

#include "pgscript/pgScript.h"
#include "pgscript/exceptions/pgsException.h"

class pgsAssertException : public pgsException
{

protected:

	const wxString m_message;

public:

	pgsAssertException(const wxString &message);

	virtual ~pgsAssertException();

	virtual const wxString message() const;

};

#endif /*PGSASSERTEXCEPTION_H_*/
