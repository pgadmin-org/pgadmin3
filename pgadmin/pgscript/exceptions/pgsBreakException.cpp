//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsBreakException.cpp,v 1.3 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/exceptions/pgsBreakException.h"

pgsBreakException::pgsBreakException() :
	pgsException()
{
	
}

pgsBreakException::~pgsBreakException()
{
	
}

const wxString pgsBreakException::message() const
{
	return wxT("BREAK");
}
