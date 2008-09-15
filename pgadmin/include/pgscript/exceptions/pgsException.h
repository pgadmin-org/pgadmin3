//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSEXCEPTION_H_
#define PGSEXCEPTION_H_

#include "pgscript/pgScript.h"

class pgsException
{
	
protected:
	
	pgsException();
	
public:
	
	virtual ~pgsException();
	
	virtual const wxString message() const = 0;
	
};

#endif /*PGSEXCEPTION_H_*/
