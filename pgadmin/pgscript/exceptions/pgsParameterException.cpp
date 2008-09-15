//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsParameterException.cpp,v 1.6 2008/08/13 13:24:02 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/exceptions/pgsParameterException.h"

pgsParameterException::pgsParameterException(const wxString & message) :
	pgsException(), m_message(message)
{
	
}

pgsParameterException::~pgsParameterException()
{
	
}

const wxString pgsParameterException::message() const
{
	wxString message(m_message);
	message.Replace(wxT("\n"), wxT("\n         "));
	return wxString() << wxT("[EXCEPT] Parameter Exception - Some parameters ")
			<< wxT("are invalid:\n>> ") << message;
}
