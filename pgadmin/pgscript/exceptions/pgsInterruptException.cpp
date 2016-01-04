//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
	return wxString() << PGSOUTEXCEPTION << _("pgScript interrupted");
}
