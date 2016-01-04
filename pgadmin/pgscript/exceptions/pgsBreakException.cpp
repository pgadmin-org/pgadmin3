//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
