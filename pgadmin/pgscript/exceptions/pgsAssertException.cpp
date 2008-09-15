//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsAssertException.cpp,v 1.4 2008/08/13 13:24:02 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
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
