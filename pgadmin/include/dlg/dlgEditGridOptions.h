//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
#include "ctl/wxgridsel.h"
#endif

#include <wx/generic/gridctrl.h>

#include "dlg/dlgClasses.h"
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

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	void OnFilterChange(wxStyledTextEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	void OnClose(wxCloseEvent &ev);
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
