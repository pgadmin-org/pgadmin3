//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgEditGridOptions.h - Edit grid options
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DLGEDITGRIDOPTIONS_H
#define __DLGEDITGRIDOPTIONS_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/grid.h>

#ifdef __WX_FULLSOURCE
#include "wx/generic/gridsel.h"
#else
#include "wxgridsel.h"
#endif

#include <wx/generic/gridctrl.h>

#include "dlgClasses.h"
class ctlSQLEditGrid;
class ctlSQLBox;
class pgConn;
class frmEditGrid;
////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////

class dlgEditGridOptions : public pgDialog
{
public:

	// Construction
    dlgEditGridOptions(frmEditGrid *parent, pgConn *conn, const wxString &rel, ctlSQLEditGrid *grid);
    ~dlgEditGridOptions();

private:

	void OnCancel(wxCommandEvent &ev);
	void OnOK(wxCommandEvent &ev);
	void OnRemove(wxCommandEvent &ev);
	void OnAsc(wxCommandEvent &ev);
	void OnDesc(wxCommandEvent &ev);
	void OnValidate(wxCommandEvent &ev);
    void OnCboColumnsChange(wxCommandEvent &ev);
    void OnLstSortColsChange(wxListEvent &ev);
	bool Validate();
	frmEditGrid *parent;
	pgConn *connection;
	wxString relation;
	ctlSQLEditGrid *editGrid;
	ctlSQLBox *filter;
	wxMBConv *conv;

	// Macros
	DECLARE_EVENT_TABLE()
};

#endif
