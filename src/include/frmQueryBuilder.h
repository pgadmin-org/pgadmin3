//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmQueryBuilder.h - The query builder main form
//
//////////////////////////////////////////////////////////////////////////

#ifndef frmQueryBuilder_H
#define frmQueryBuilder_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/laywin.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/toolbar.h>
#include <wx/tbarsmpl.h>
#include <wx/imaglist.h>
#include <wx/textctrl.h>
#include <wx/grid.h>
#include <wx/minifram.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/stc/stc.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgAddTableView.h"
#include "frmChildTableViewFrame.h"
#include "pgConn.h"
#include "pgDatabase.h"
#include "pgSet.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"

class frmQueryBuilder : public wxMDIParentFrame
{
public:

	// Construction / Deconstruction
    frmQueryBuilder(wxWindow* parent, pgDatabase *database);
    ~frmQueryBuilder();

	// Controls
    wxStatusBar *statusBar;

   	// Data
	wxList m_children;
	wxArrayString m_names;
	wxArrayString m_aliases;
	pgDatabase *database;
	pgServer *m_server;

	// Methods
	wxString GetTableViewAlias(wxString firstname, 
		wxString newname = "", int postfix = 1);
	void AddColumn(frmChildTableViewFrame *frame,int item);

private:

	// Controls
    wxMenuBar *menuBar;
    wxMenu *fileMenu, *toolsMenu, *viewMenu, *helpMenu, *datagramContextMenu;
    wxToolBar *toolBar;

	wxSashLayoutWindow *sashwindow;

	wxNotebook *notebook;
	wxGrid *design, *data;
	wxTextCtrl *sql;
	dlgAddTableView *addtableview;

	// Methods
	void DrawTablesAndViews();
	void UpdateGridTables(frmChildTableViewFrame *frame);
	void UpdateGridColumns(frmChildTableViewFrame *frame, int item);
	void UpdateGridExpressions();
	void BuildQuery();
	void RunQuery();

	// Events
	void OnSize(wxSizeEvent& event);
	void OnSashDrag(wxSashEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAddTableView();

#ifdef __WXMSW__
    void OnContextMenu(wxContextMenuEvent& event)
        { OnRightClick(ScreenToClient(event.GetPosition())); }
#else
    void OnRightUp(wxMouseEvent& event)
        { OnRightClick(event.GetPosition()); }
#endif

	void OnRightClick(wxPoint& point);

	void OnNotebookPageChanged(wxNotebookEvent& event);

	// Control Enumeration
	enum
	{
		CTL_DESIGNPANEL = 1000,
		CTL_SQLPANEL = 1001,
		CTL_DATAPANEL = 1002,
		ID_NOTEBOOK = 1003
	};

	// Menu options
	enum
	{
		MNU_EXIT = 2000,
		MNU_ADDTABLEVIEW = 2001,
	};

	// Sash Windows
	enum
	{
		ID_SASH_WINDOW_BOTTOM = 3000
	};

	// Macros
    DECLARE_EVENT_TABLE()
};

#endif
