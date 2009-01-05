//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/exceptions/pgsAssertException.h"

pgsAssertException::pgsAssertException(const wxString & message) :
	m_message(message)
{
	
}

pgsAssertException::~pgsAssertException()
{

}

const wxString pgsAssertException::message() const
{
	return wxString() << wxT("[EXCEPT] Assert Exception - ") << m_message;
}
