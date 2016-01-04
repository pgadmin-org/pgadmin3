//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/utilities/pgsUtilities.h"

wxString pgsUtilities::uniform_line_returns(wxString s)
{
	s.Replace(wxT("\r\n"), wxT("\n"));
	s.Replace(wxT("\r"), wxT("\n"));
	return s;
}

wxString pgsUtilities::escape_quotes(wxString s)
{
	s.Replace(wxT("\\"), wxT("\\\\"));
	s.Replace(wxT("'"), wxT("''"));
	return s;
}

wxString pgsUtilities::unescape_quotes(wxString s)
{
	s.Replace(wxT("''"), wxT("'"));
	s.Replace(wxT("\\'"), wxT("'"));
	return s;
}
