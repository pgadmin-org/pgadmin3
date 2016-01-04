//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSUTILITIES_H_
#define PGSUTILITIES_H_

#include "pgscript/pgScript.h"

class pgsUtilities
{

public:

	static wxString uniform_line_returns(wxString s);
	static wxString escape_quotes(wxString s);
	static wxString unescape_quotes(wxString s);

};

#endif /*PGSUTILITIES_H_*/
