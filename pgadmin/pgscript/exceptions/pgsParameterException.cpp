//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "utils/misc.h"
#include "pgscript/exceptions/pgsParameterException.h"

pgsParameterException::pgsParameterException(const wxString &message) :
	pgsException(), m_message(message)
{

}

pgsParameterException::~pgsParameterException()
{

}

const wxString pgsParameterException::message() const
{
	wxString message(m_message);
	message.Replace(wxT("\n"), wxT("\n") + generate_spaces(PGSOUTEXCEPTION.Length()));
	message.Prepend(wxT(">> "));
	message.Prepend(generate_spaces(PGSOUTEXCEPTION.Length()));
	return wxString() << PGSOUTEXCEPTION <<
	       wxString::Format(_("Parameter Exception - Some parameters are invalid:\n%s"),
	                        message.c_str());
}
