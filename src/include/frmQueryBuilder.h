//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "frmChildTableViewFrame.h"
#include "dlgAddTableView.h"
#include "ctlSQLResult.h"

struct JoinStruct
{
		wxString left;
		wxString right;
		int leftcolumn;
		int rightcolumn;
		wxString jointype;
		wxArrayString conditions;
		int conditionct;
		wxString joinop;
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
    wxLongLong elapsedQuery, elapsedRetrieve;

	// Methods
	wxString GetTableViewAlias(wxString firstname, 
		wxString newname = wxT(""), int postfix = 1);
	void AddColumn(frmChildTableViewFrame *frame,int item);
	void UpdateGridTables(frmChildTableViewFrame *frame);
	void DeleteChild(wxString talias);
	frmChildTableViewFrame *GetFrameFromAlias(wxString alias);

private:

	// Data
	dlgAddTableView *addtableview;
	frmMain *m_mainForm;

    wxString title, lastFilename, lastDir, lastPath;

    bool changed, aborted, lastFileFormat;

	// Controls
    wxMenuBar *menuBar;
    wxMenu *fileMenu,
        *recentFileMenu,
		*toolsMenu,  
		*helpMenu, 
		*datagramContextMenu,
		*queryMenu;

    wxToolBar *toolBar;
	wxSashLayoutWindow *m_sashwindow;
	wxSplitterWindow *splitter;
    wxTextCtrl *msgResult, *msgHistory;
	wxNotebook *notebook;
	wxGrid *design;
    ctlSQLResult *data;
	ctlSQLBox *sql;

	// Methods
    void setTools(const bool running);
	void UpdateGridColumns(frmChildTableViewFrame *frame, int item,
		bool _FORCE = FALSE, int _FORCEROW = 0);
	void RunQuery(const wxString &query, int resultToRetrieve=0, bool singleResult=false, const int queryOffset=0);
	void VerifyExpression(int row);
	wxString RebuildCondition(wxString condition, int row, bool &errout);
	int FindLeftmostTable();
	bool IsTableLeftOnly(wxString tablename);
	virtual wxMDIClientWindow* OnCreateClient();
	wxString BuildTableJoin(int table, int indent);
    void BuildQuery();

	// Events
    void OnClose(wxCloseEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnExecute(wxCommandEvent& event);
    void OnExplain(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnChange(wxNotifyEvent& event);

	void OnSize(wxSizeEvent& event);
	void OnSashDrag(wxSashEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAddTableView(wxCommandEvent& event);
    void OnSaveHistory(wxCommandEvent& event);
    void OnClearHistory(wxCommandEvent& event);

    void setExtendedTitle();
    void showMessage(const wxString& msg, const wxString &msgShort=wxT(""));

#ifdef __WXMSW__
    void OnContextMenu(wxContextMenuEvent& event)
        { wxPoint pt=ScreenToClient(event.GetPosition()); ExecRightClick(pt); }
#else
    void OnRightUp(wxMouseEvent& event)
        { wxPoint pt=event.GetPosition(); ExecRightClick(pt); }
#endif
	void OnCellSelect(wxGridEvent& event);
	void OnNotebookPageChanged(wxNotebookEvent& event);
	void OnCellChoice(wxCommandEvent& event);
	void OnCellChange(wxGridEvent& event);


   	void ExecRightClick(wxPoint& point);

	// Control Enumeration
	enum
	{
		CTL_DESIGNPANEL = 1000,
		CTL_SQLPANEL,
		CTL_DATAPANEL,
		ID_NOTEBOOK
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

////////////////////////////////////////////////////////////////////////////////
// Class Definition
////////////////////////////////////////////////////////////////////////////////
class DnDDesign : public wxTextDropTarget
{

public:

	DnDDesign(wxFrame *frame) 
	{ 
		m_frame = frame; 
	}

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text);
	
private:

    wxFrame *m_frame;
};


#endif

