//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsDocmgr.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

#ifndef WSDOCMGRH
#define WSDOCMGRH

#include <wx/docview.h>

class wsDocMgr : public wxDocManager
{
public:

	wsDocMgr() : wxDocManager() {}

	virtual wxDocTemplate * SelectDocumentPath( wxDocTemplate ** templates, int noTemplates, wxString& path, long flags, bool save );

private:

	wxString selectFunctionFromServer();

};

#endif
