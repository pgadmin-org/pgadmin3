//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsUtilities.h,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
