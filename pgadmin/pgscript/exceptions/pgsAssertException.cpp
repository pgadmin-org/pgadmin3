//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/exceptions/pgsAssertException.h"

pgsAssertException::pgsAssertException(const wxString &message) :
	m_message(message)
{

}

pgsAssertException::~pgsAssertException()
{

}

const wxString pgsAssertException::message() const
{
	return wxString() << PGSOUTEXCEPTION << _("Assert Exception - ") << m_message;
}
