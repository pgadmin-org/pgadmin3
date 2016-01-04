//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/exceptions/pgsCastException.h"

pgsCastException::pgsCastException(const wxString &value, const wxString &type) :
	pgsException(), m_value(value), m_type(type)
{

}

pgsCastException::~pgsCastException()
{

}

const wxString pgsCastException::message() const
{
	return wxString() << PGSOUTEXCEPTION <<
	       wxString::Format(_("Cast Exception - Cannot convert '%s' to '%s'"),
	                        m_value.c_str(), m_type.c_str());
}
