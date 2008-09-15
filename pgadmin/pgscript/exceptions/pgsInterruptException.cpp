//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
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
