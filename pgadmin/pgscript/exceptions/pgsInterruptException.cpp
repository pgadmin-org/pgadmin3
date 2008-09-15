//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsInterruptException.cpp,v 1.5 2008/08/13 13:24:02 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/exceptions/pgsInterruptException.h"

pgsInterruptException::pgsInterruptException() :
	pgsException()
{
	
}

pgsInterruptException::~pgsInterruptException()
{
	
}

const wxString pgsInterruptException::message() const
{
	return wxT("[EXCEPT] pgScript interrupted");
}
