//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/exceptions/pgsCastException.h"

pgsCastException::pgsCastException(const wxString & value, const wxString & type) :
	pgsException(), m_value(value), m_type(type)
{
	
}

pgsCastException::~pgsCastException()
{
	
}

const wxString pgsCastException::message() const
{
	return wxString() << wxT("[EXCEPT] Cast Exception - Cannot convert ")
			<< m_value << wxT(" to ") << m_type;
}
