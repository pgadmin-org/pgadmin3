//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSINTERRUPTEXCEPTION_H_
#define PGSINTERRUPTEXCEPTION_H_

#include "pgscript/pgScript.h"
#include "pgscript/exceptions/pgsException.h"

class pgsInterruptException : public pgsException
{

public:

	pgsInterruptException();

	virtual ~pgsInterruptException();

	virtual const wxString message() const;

};

#endif /*PGSINTERRUPTEXCEPTION_H_*/
