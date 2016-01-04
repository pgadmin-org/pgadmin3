//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSBREAKEXCEPTION_H_
#define PGSBREAKEXCEPTION_H_

#include "pgscript/pgScript.h"
#include "pgscript/exceptions/pgsException.h"

class pgsBreakException : public pgsException
{

public:

	pgsBreakException();

	virtual ~pgsBreakException();

	virtual const wxString message() const;

};

#endif /*PGSBREAKEXCEPTION_H_*/
