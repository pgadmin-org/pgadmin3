//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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

// App headers
#include "pgAdmin3.h"
#include "pgDefs.h"
#include "frmMain.h"
#include "menu.h"

#include "frmEditGrid.h"
#include "dlgEditGridOptions.h"
#include "pgTable.h"
#include "pgView.h"

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
class dlgEditGridOptions : public wxDialog
{
public:

	// Construction
    dlgEditGridOptions(frmEditGrid *parent, ctlSQLGrid *grid);

private:

	void OnCancel(wxCommandEvent &ev);
	void OnOK(wxCommandEvent &ev);
	void OnRemove(wxCommandEvent &ev);
	void OnAsc(wxCommandEvent &ev);
	void OnDesc(wxCommandEvent &ev);
    void OnCboColumnsChange(wxCommandEvent &ev);
    void OnLstSortColsChange(wxCommandEvent &ev);
	frmEditGrid *parent;
	ctlSQLGrid *editGrid;

	// Macros
	DECLARE_EVENT_TABLE()
};

#endif