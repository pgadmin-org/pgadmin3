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
#include <wx/grid.h>

// App headers
#include "frmMain.h"
#include "dlgAddTableView.h"
#include "frmChildTableViewFrame.h"

struct JoinStruct
{
		wxString left;
		wxString right;
		int leftcolumn;
		int rightcolumn;
		wxString jointype;
		wxArrayString conditions;
		int conditionct;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class frmQueryBuilder : public wxMDIParentFrame
{
public:

	// Construction / Deconstruction
    frmQueryBuilder(frmMain *form, pgDatabase *database);
    ~frmQueryBuilder();

	// Controls
    wxStatusBar *statusBar;

   	// Data
	wxArrayPtrVoid m_children;
	wxArrayPtrVoid m_joins; 
	wxArrayString m_names;
	wxArrayString m_aliases;
	pgDatabase *m_database;
	pgServer *m_server;

	// Methods
	wxString GetTableViewAlias(wxString firstname, 
		wxString newname = "", int postfix = 1);
	void AddColumn(frmChildTableViewFrame *frame,int item);
	void UpdateGridTables(frmChildTableViewFrame *frame);
	void DeleteChild(wxString talias);
	frmChildTableViewFrame *GetFrameFromAlias(wxString alias);

private:

	// Data
	frmMain *m_mainForm;
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
	void UpdateGridColumns(frmChildTableViewFrame *frame, int item,
		bool _FORCE = FALSE, int _FORCEROW = 0);
	void BuildQuery();
	void RunQuery();
	void VerifyExpression(int row);
	wxString RebuildCondition(wxString condition, 
		bool &errout);

	virtual wxMDIClientWindow* OnCreateClient();

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
        { wxPoint pt=event.GetPosition(); OnRightClick(pt); }
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

public:

	// Position of status line fields
	enum
	{
	   STATUSPOS_MSGS = 1,
	   STATUSPOS_ROWS,
	   STATUSPOS_SECS
	};

private:

	// Macros
    DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class myClientWindow : public wxMDIClientWindow
{

private:

	// Events
	void OnPaint(wxPaintEvent& event);

	// Methods
	void DrawRotatedText(wxPaintDC *dc, const char* str, 
		float x, float y, float angle);

	// Macros
    DECLARE_EVENT_TABLE()
};

#endif

