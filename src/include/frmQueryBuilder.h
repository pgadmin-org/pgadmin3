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
	wxArrayPtrVoid m_children;
	wxArrayString m_names;
	wxArrayString m_aliases;
	pgDatabase *m_database;
	pgServer *m_server;

	// Methods
	wxString GetTableViewAlias(wxString firstname, 
		wxString newname = "", int postfix = 1);
	void AddColumn(frmChildTableViewFrame *frame,int item);
	void UpdateGridTables(frmChildTableViewFrame *frame);

private:

	// Data
    bool m_changed;
	wxString m_lastFilename;
	wxString m_lastDir;
	wxString m_lastPath;

	// Controls
    wxMenuBar *menuBar;
    wxMenu *fileMenu, 
		*toolsMenu, 
		*viewMenu, 
		*helpMenu, 
		*datagramContextMenu,
		*queryMenu;

    wxToolBar *toolBar;
	wxSashLayoutWindow *m_sashwindow;
	wxNotebook *notebook;
	wxGrid *design, *data;
	ctlSQLBox *sql;

	// Dialogs
	dlgAddTableView *addtableview;

	// Methods
    void setTools(const bool running);
	void DrawTablesAndViews();
	void UpdateGridColumns(frmChildTableViewFrame *frame, int item,
		bool _FORCE = FALSE, int _FORCEROW = 0);
	void BuildQuery();
	void RunQuery();
	frmChildTableViewFrame *GetFrameFromAlias(wxString alias);
	void VerifyExpression(int row);
	wxString RebuildCondition(wxString condition, 
		bool &errout);

	// Events
    void OnClose(wxCloseEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnExecute(wxCommandEvent& event);
    void OnExplain(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnChange(wxNotifyEvent& event);

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
	void OnCellSelect(wxGridEvent& event);
	void OnNotebookPageChanged(wxNotebookEvent& event);
	void OnCellChoice(wxCommandEvent& event);
	void OnCellChange(wxGridEvent& event);

	// Control Enumeration
	enum
	{
		CTL_DESIGNPANEL = 1000,
		CTL_SQLPANEL,
		CTL_DATAPANEL,
		ID_NOTEBOOK
	};

	// Menu options
	enum
	{
		MNU_EXIT = 2000,
		MNU_ADDTABLEVIEW,
		MNU_OPEN,
		MNU_SAVE,
		MNU_SAVEAS,
		MNU_CANCEL,
		MNU_EXECUTE,
		MNU_EXPLAIN,
		MNU_QUERYBUILDER,
		MNU_QUERYANALYZER
	};

	// Button Enumeration
	enum
	{
	   BTN_OPEN = 3000,
	   BTN_SAVE,
	   BTN_EXECUTE,
	   BTN_EXPLAIN,
	   BTN_CANCEL
	};

	// Sash Windows
	enum
	{
		ID_SASH_WINDOW_BOTTOM = 4000
	};

	// Design Columns
	enum
	{
		DESIGN_OUTPUT = 0,
		DESIGN_TABLE = 1,
		DESIGN_COLUMN = 2,
		DESIGN_EXPRESSION = 3,
		DESIGN_ALIAS = 4,
		DESIGN_AGGREGATE = 5,
		DESIGN_CONDITION = 6,
		DESIGN_OR1 = 7,
		DESIGN_OR2 = 8,
		DESIGN_OR3 = 9,
		DESIGN_OR4 = 10,
		DESIGN_OR5 = 11
	};

	// Position of status line fields
	enum
	{
	   STATUSPOS_MSGS = 1,
	   STATUSPOS_ROWS,
	   STATUSPOS_SECS
	};

	// Macros
    DECLARE_EVENT_TABLE()
};

#endif
