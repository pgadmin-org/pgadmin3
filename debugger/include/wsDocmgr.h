//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsDocmgr.h 5827 2007-01-04 16:35:14 hiroshi $
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
