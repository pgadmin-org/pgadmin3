//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmQuery.cpp - SQL Query Box
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/busyinfo.h>
#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include <wx/dnd.h>
#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/aui/aui.h>
#include <wx/bmpcbox.h>

// App headers
#include "frm/frmMain.h"
#include "frm/frmQuery.h"
#include "frm/menu.h"
#include "ctl/explainCanvas.h"
#include "db/pgConn.h"

#include "ctl/ctlMenuToolbar.h"
#include "ctl/ctlSQLResult.h"
#include "dlg/dlgSelectConnection.h"
#include "dlg/dlgAddFavourite.h"
#include "dlg/dlgManageFavourites.h"
#include "dlg/dlgManageMacros.h"
#include "frm/frmReport.h"
#include "gqb/gqbViewController.h"
#include "gqb/gqbModel.h"
#include "gqb/gqbViewPanels.h"
#include "gqb/gqbEvents.h"
#include "schema/pgDatabase.h"
#include "schema/pgFunction.h"
#include "schema/pgTable.h"
#include "schema/pgView.h"
#include "schema/gpExtTable.h"
#include "schema/pgServer.h"
#include "utils/favourites.h"
#include "utils/sysLogger.h"
#include "utils/sysSettings.h"
#include "utils/utffile.h"
#include "pgscript/pgsApplication.h"

// Icons
#include "images/sql-32.xpm"

// Bitmaps
#include "images/file_new.xpm"
#include "images/file_open.xpm"
#include "images/file_save.xpm"
#include "images/clip_cut.xpm"
#include "images/clip_copy.xpm"
#include "images/clip_paste.xpm"
#include "images/edit_clear.xpm"
#include "images/edit_find.xpm"
#include "images/edit_undo.xpm"
#include "images/edit_redo.xpm"
#include "images/query_execute.xpm"
#include "images/query_pgscript.xpm"
#include "images/query_execfile.xpm"
#include "images/query_explain.xpm"
#include "images/query_cancel.xpm"
#include "images/help.xpm"
#include "images/gqbJoin.xpm"

#define CTRLID_CONNECTION       4200
#define CTRLID_DATABASELABEL    4201

#define XML_FROM_WXSTRING(s) ((const xmlChar *)(const char *)s.mb_str(wxConvUTF8))
#define WXSTRING_FROM_XML(s) wxString((char *)s, wxConvUTF8)
#define XML_STR(s) ((const xmlChar *)s)

// Initialize execution 'mutex'. As this will always run in the
// main thread, there aren't any real concurrency issues, so
// a simple flag will suffice.
// Required because the pgScript parser isn't currently thread-safe :-(
bool    frmQuery::ms_pgScriptRunning = false;

BEGIN_EVENT_TABLE(frmQuery, pgFrame)
	EVT_ERASE_BACKGROUND(           frmQuery::OnEraseBackground)
	EVT_SIZE(                       frmQuery::OnSize)
	EVT_COMBOBOX(CTRLID_CONNECTION, frmQuery::OnChangeConnection)
	EVT_COMBOBOX(CTL_SQLQUERYCBOX,  frmQuery::OnChangeQuery)
	EVT_CLOSE(                      frmQuery::OnClose)
	EVT_SET_FOCUS(                  frmQuery::OnSetFocus)
	EVT_MENU(MNU_NEW,               frmQuery::OnNew)
	EVT_MENU(MNU_OPEN,              frmQuery::OnOpen)
	EVT_MENU(MNU_SAVE,              frmQuery::OnSave)
	EVT_MENU(MNU_SAVEAS,            frmQuery::OnSaveAs)
	EVT_MENU(MNU_EXPORT,            frmQuery::OnExport)
	EVT_MENU(MNU_SAVEAS_IMAGE_GQB,     frmQuery::SaveExplainAsImage)
	EVT_MENU(MNU_SAVEAS_IMAGE_EXPLAIN, frmQuery::SaveExplainAsImage)
	EVT_MENU(MNU_EXIT,              frmQuery::OnExit)
	EVT_MENU(MNU_CUT,               frmQuery::OnCut)
	EVT_MENU(MNU_COPY,              frmQuery::OnCopy)
	EVT_MENU(MNU_PASTE,             frmQuery::OnPaste)
	EVT_MENU(MNU_CLEAR,             frmQuery::OnClear)
	EVT_MENU(MNU_FIND,              frmQuery::OnSearchReplace)
	EVT_MENU(MNU_UNDO,              frmQuery::OnUndo)
	EVT_MENU(MNU_REDO,              frmQuery::OnRedo)
	EVT_MENU(MNU_EXECUTE,           frmQuery::OnExecute)
	EVT_MENU(MNU_EXECPGS,           frmQuery::OnExecScript)
	EVT_MENU(MNU_EXECFILE,          frmQuery::OnExecFile)
	EVT_MENU(MNU_EXPLAIN,           frmQuery::OnExplain)
	EVT_MENU(MNU_EXPLAINANALYZE,    frmQuery::OnExplain)
	EVT_MENU(MNU_BUFFERS,           frmQuery::OnBuffers)
	EVT_MENU(MNU_CANCEL,            frmQuery::OnCancel)
	EVT_MENU(MNU_AUTOROLLBACK,      frmQuery::OnAutoRollback)
	EVT_MENU(MNU_CONTENTS,          frmQuery::OnContents)
	EVT_MENU(MNU_HELP,              frmQuery::OnHelp)
	EVT_MENU(MNU_CLEARHISTORY,      frmQuery::OnClearHistory)
	EVT_MENU(MNU_SAVEHISTORY,       frmQuery::OnSaveHistory)
	EVT_MENU(MNU_SELECTALL,         frmQuery::OnSelectAll)
	EVT_MENU(MNU_QUICKREPORT,       frmQuery::OnQuickReport)
	EVT_MENU(MNU_AUTOINDENT,        frmQuery::OnAutoIndent)
	EVT_MENU(MNU_WORDWRAP,          frmQuery::OnWordWrap)
	EVT_MENU(MNU_SHOWINDENTGUIDES,  frmQuery::OnShowIndentGuides)
	EVT_MENU(MNU_SHOWWHITESPACE,    frmQuery::OnShowWhitespace)
	EVT_MENU(MNU_SHOWLINEENDS,      frmQuery::OnShowLineEnds)
	EVT_MENU(MNU_SHOWLINENUMBER,    frmQuery::OnShowLineNumber)
	EVT_MENU(MNU_FAVOURITES_ADD,    frmQuery::OnAddFavourite)
	EVT_MENU(MNU_FAVOURITES_MANAGE, frmQuery::OnManageFavourites)
	EVT_MENU(MNU_MACROS_MANAGE,     frmQuery::OnMacroManage)
	EVT_MENU(MNU_DATABASEBAR,       frmQuery::OnToggleDatabaseBar)
	EVT_MENU(MNU_TOOLBAR,           frmQuery::OnToggleToolBar)
	EVT_MENU(MNU_SCRATCHPAD,        frmQuery::OnToggleScratchPad)
	EVT_MENU(MNU_OUTPUTPANE,        frmQuery::OnToggleOutputPane)
	EVT_MENU(MNU_DEFAULTVIEW,       frmQuery::OnDefaultView)
	EVT_MENU(MNU_BLOCK_INDENT,      frmQuery::OnBlockIndent)
	EVT_MENU(MNU_BLOCK_OUTDENT,     frmQuery::OnBlockOutDent)
	EVT_MENU(MNU_UPPER_CASE,        frmQuery::OnChangeToUpperCase)
	EVT_MENU(MNU_LOWER_CASE,        frmQuery::OnChangeToLowerCase)
	EVT_MENU(MNU_COMMENT_TEXT,      frmQuery::OnCommentText)
	EVT_MENU(MNU_UNCOMMENT_TEXT,    frmQuery::OnUncommentText)
	EVT_MENU(MNU_LF,                frmQuery::OnSetEOLMode)
	EVT_MENU(MNU_CRLF,              frmQuery::OnSetEOLMode)
	EVT_MENU(MNU_CR,                frmQuery::OnSetEOLMode)
	EVT_MENU_RANGE(MNU_FAVOURITES_MANAGE + 1, MNU_FAVOURITES_MANAGE + 999, frmQuery::OnSelectFavourite)
	EVT_MENU_RANGE(MNU_MACROS_MANAGE + 1, MNU_MACROS_MANAGE + 99, frmQuery::OnMacroInvoke)
	EVT_ACTIVATE(                   frmQuery::OnActivate)
	EVT_STC_MODIFIED(CTL_SQLQUERY,  frmQuery::OnChangeStc)
	EVT_STC_UPDATEUI(CTL_SQLQUERY,  frmQuery::OnPositionStc)
	EVT_AUI_PANE_CLOSE(             frmQuery::OnAuiUpdate)
	EVT_TIMER(CTL_TIMERSIZES,       frmQuery::OnAdjustSizesTimer)
	EVT_TIMER(CTL_TIMERFRM,         frmQuery::OnTimer)
// These fire when the queries complete
	EVT_MENU(QUERY_COMPLETE,        frmQuery::OnQueryComplete)
	EVT_MENU(PGSCRIPT_COMPLETE,     frmQuery::OnScriptComplete)
	EVT_NOTEBOOK_PAGE_CHANGED(CTL_NTBKCENTER, frmQuery::OnChangeNotebook)
	EVT_SPLITTER_SASH_POS_CHANGED(GQB_HORZ_SASH, frmQuery::OnResizeHorizontally)
	EVT_BUTTON(CTL_DELETECURRENTBTN, frmQuery::OnDeleteCurrent)
	EVT_BUTTON(CTL_DELETEALLBTN,     frmQuery::OnDeleteAll)
END_EVENT_TABLE()

class DnDFile : public wxFileDropTarget
{
public:
	DnDFile(frmQuery *fquery)
	{
		m_fquery = fquery;
	}

	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
	{
		size_t nFiles = filenames.GetCount();
		if ((int) nFiles > 1)
			wxLogError(_("Drag one file at a time"));
		else if ((int) nFiles == 1)
		{
			wxString str;
			bool modeUnicode = settings->GetUnicodeFile();
			wxUtfFile file(filenames[0], wxFile::read, modeUnicode ? wxFONTENCODING_UTF8 : wxFONTENCODING_DEFAULT);

			if (file.IsOpened())
				file.Read(str);

			if (!str.IsEmpty() && !m_fquery->CheckChanged(true))
			{
				m_fquery->SetLastPath(filenames[0]);
				m_fquery->SetQueryText(str);
				m_fquery->ColouriseQuery(0, str.Length());
				wxSafeYield();                            // needed to process sqlQuery modify event
				m_fquery->SetChanged(false);
				m_fquery->setExtendedTitle();
				m_fquery->SetLineEndingStyle();
				m_fquery->UpdateRecentFiles(true);
				m_fquery->UpdateAllRecentFiles();
			}
		}
		return true;
	}

private:
	frmQuery *m_fquery;
};


frmQuery::frmQuery(frmMain *form, const wxString &_title, pgConn *_conn, const wxString &query, const wxString &file)
	: pgFrame(NULL, _title),
	  timer(this, CTL_TIMERFRM),
	  pgScript(new pgsApplication(_conn)),
	  pgsStringOutput(&pgsOutputString),
	  pgsOutput(pgsStringOutput, wxEOL_UNIX),
	  pgsTimer(new pgScriptTimer(this))
{
	pgScript->SetCaller(this, PGSCRIPT_COMPLETE);

	mainForm = form;
	conn = _conn;

	loading = true;
	closing = false;

	dlgName = wxT("frmQuery");
	recentKey = wxT("RecentFiles");
	RestorePosition(100, 100, 600, 500, 450, 300);

	explainCanvas = NULL;

	// notify wxAUI which frame to use
	manager.SetManagedWindow(this);
	manager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG);

	SetMinSize(wxSize(450, 300));

	SetIcon(wxIcon(sql_32_xpm));
	wxWindowBase::SetFont(settings->GetSystemFont());
	menuBar = new wxMenuBar();

	fileMenu = new wxMenu();
	recentFileMenu = new wxMenu();
	fileMenu->Append(MNU_NEW, _("&New window\tCtrl-N"), _("Open a new query window"));
	fileMenu->Append(MNU_OPEN, _("&Open...\tCtrl-O"),   _("Open a query file"));
	fileMenu->Append(MNU_SAVE, _("&Save\tCtrl-S"),      _("Save current file"));
	saveasImageMenu = new wxMenu();
	saveasImageMenu->Append(MNU_SAVEAS, _("Query (text)"), _("Save file under new name"));
	saveasImageMenu->Append(MNU_SAVEAS_IMAGE_GQB, _("Graphical Query (image)"), _("Save Graphical Query as an image"));
	saveasImageMenu->Append(MNU_SAVEAS_IMAGE_EXPLAIN, _("Explain (image)"), _("Save output of Explain as an image"));
	fileMenu->Append(wxID_ANY, _("Save as"), saveasImageMenu);
	fileMenu->AppendSeparator();
	fileMenu->Append(MNU_EXPORT, _("&Export..."),  _("Export data to file"));
	fileMenu->Append(MNU_QUICKREPORT, _("&Quick report..."),  _("Run a quick report..."));
	fileMenu->AppendSeparator();
	fileMenu->Append(MNU_RECENT, _("&Recent files"), recentFileMenu);
	fileMenu->Append(MNU_EXIT, _("E&xit\tCtrl-W"), _("Exit query window"));

	menuBar->Append(fileMenu, _("&File"));

	lineEndMenu = new wxMenu();
	lineEndMenu->AppendRadioItem(MNU_LF, _("Unix (LF)"), _("Use Unix style line endings"));
	lineEndMenu->AppendRadioItem(MNU_CRLF, _("DOS (CRLF)"), _("Use DOS style line endings"));
	lineEndMenu->AppendRadioItem(MNU_CR, _("Mac (CR)"), _("Use Mac style line endings"));

	editMenu = new wxMenu();
	editMenu->Append(MNU_UNDO, _("&Undo\tCtrl-Z"), _("Undo last action"), wxITEM_NORMAL);
	editMenu->Append(MNU_REDO, _("&Redo\tCtrl-Y"), _("Redo last action"), wxITEM_NORMAL);
	editMenu->AppendSeparator();
	editMenu->Append(MNU_CUT, _("Cu&t\tCtrl-X"), _("Cut selected text to clipboard"), wxITEM_NORMAL);
	editMenu->Append(MNU_COPY, _("&Copy\tCtrl-C"), _("Copy selected text to clipboard"), wxITEM_NORMAL);
	editMenu->Append(MNU_PASTE, _("&Paste\tCtrl-V"), _("Paste selected text from clipboard"), wxITEM_NORMAL);
	editMenu->Append(MNU_CLEAR, _("C&lear window"), _("Clear edit window"), wxITEM_NORMAL);
	editMenu->AppendSeparator();
	editMenu->Append(MNU_FIND, _("&Find and Replace\tCtrl-F"), _("Find and replace text"), wxITEM_NORMAL);
	editMenu->AppendSeparator();
	editMenu->Append(MNU_AUTOINDENT, _("&Auto indent"), _("Automatically indent text to the same level as the preceding line"), wxITEM_CHECK);

	//  editMenu->AppendSeparator();
	formatMenu = new wxMenu();
	formatMenu->Append(MNU_UPPER_CASE, _("&Upper case\tCtrl-U"), _("Change the selected text to upper case"));
	formatMenu->Append(MNU_LOWER_CASE, _("&Lower case\tCtrl-Shift-U"), _("Change the selected text to lower case"));
	formatMenu->AppendSeparator();
	formatMenu->Append(MNU_BLOCK_INDENT, _("Block &Indent\tTab"), _("Indent the selected block"));
	formatMenu->Append(MNU_BLOCK_OUTDENT, _("Block &Outdent\tShift-Tab"), _("Outdent the selected block"));
	formatMenu->Append(MNU_COMMENT_TEXT, _("Co&mment Text\tCtrl-K"), _("Comment out the selected text"));
	formatMenu->Append(MNU_UNCOMMENT_TEXT, _("Uncomme&nt Text\tCtrl-Shift-K"), _("Uncomment the selected text"));
	editMenu->AppendSubMenu(formatMenu, _("F&ormat"));
	editMenu->Append(MNU_LINEENDS, _("&Line ends"), lineEndMenu);

	menuBar->Append(editMenu, _("&Edit"));

	queryMenu = new wxMenu();
	queryMenu->Append(MNU_EXECUTE, _("&Execute\tF5"), _("Execute query"));
	queryMenu->Append(MNU_EXECPGS, _("Execute &pgScript\tF6"), _("Execute pgScript"));
	queryMenu->Append(MNU_EXECFILE, _("Execute to file"), _("Execute query, write result to file"));
	queryMenu->Append(MNU_EXPLAIN, _("E&xplain\tF7"), _("Explain query"));
	queryMenu->Append(MNU_EXPLAINANALYZE, _("Explain analyze\tShift-F7"), _("Explain and analyze query"));


	wxMenu *eo = new wxMenu();
	eo->Append(MNU_VERBOSE, _("Verbose"), _("Explain verbose query"), wxITEM_CHECK);
	eo->Append(MNU_COSTS, _("Costs"), _("Explain analyze query with (or without costs)"), wxITEM_CHECK);
	eo->Append(MNU_BUFFERS, _("Buffers"), _("Explain analyze query with (or without buffers)"), wxITEM_CHECK);
	queryMenu->Append(MNU_EXPLAINOPTIONS, _("Explain &options"), eo, _("Options modifying Explain output"));
	queryMenu->AppendSeparator();
	queryMenu->Append(MNU_SAVEHISTORY, _("Save history"), _("Save history of executed commands."));
	queryMenu->Append(MNU_CLEARHISTORY, _("Clear history"), _("Clear history window."));
	queryMenu->AppendSeparator();
	queryMenu->Append(MNU_AUTOROLLBACK, _("&Auto-Rollback"), _("Rollback the current transaction if an error is detected"), wxITEM_CHECK);
	queryMenu->AppendSeparator();
	queryMenu->Append(MNU_CANCEL, _("&Cancel\tAlt-Break"), _("Cancel query"));
	menuBar->Append(queryMenu, _("&Query"));

	favouritesMenu = new wxMenu();
	favouritesMenu->Append(MNU_FAVOURITES_ADD, _("Add favourite..."), _("Add current query to favourites"));
	favouritesMenu->Append(MNU_FAVOURITES_MANAGE, _("Manage favourites..."), _("Edit and delete favourites"));
	favouritesMenu->AppendSeparator();
	favourites = 0L;
	UpdateFavouritesList();
	menuBar->Append(favouritesMenu, _("Fav&ourites"));

	macrosMenu = new wxMenu();
	macrosMenu->Append(MNU_MACROS_MANAGE, _("Manage macros..."), _("Edit and delete macros"));
	macrosMenu->AppendSeparator();
	macros = 0L;
	UpdateMacrosList();
	menuBar->Append(macrosMenu, _("&Macros"));

	// View menu
	viewMenu = new wxMenu();
	viewMenu->Append(MNU_DATABASEBAR, _("&Database bar\tCtrl-Alt-B"), _("Show or hide the database selection bar."), wxITEM_CHECK);
	viewMenu->Append(MNU_OUTPUTPANE, _("&Output pane\tCtrl-Alt-O"), _("Show or hide the output pane."), wxITEM_CHECK);
	viewMenu->Append(MNU_SCRATCHPAD, _("S&cratch pad\tCtrl-Alt-S"), _("Show or hide the scratch pad."), wxITEM_CHECK);
	viewMenu->Append(MNU_TOOLBAR, _("&Tool bar\tCtrl-Alt-T"), _("Show or hide the tool bar."), wxITEM_CHECK);
	viewMenu->AppendSeparator();
	viewMenu->Append(MNU_SHOWINDENTGUIDES, _("&Indent guides"), _("Enable or disable display of indent guides"), wxITEM_CHECK);
	viewMenu->Append(MNU_SHOWLINEENDS, _("&Line ends"), _("Enable or disable display of line ends"), wxITEM_CHECK);
	viewMenu->Append(MNU_SHOWWHITESPACE, _("&Whitespace"), _("Enable or disable display of whitespaces"), wxITEM_CHECK);
	viewMenu->Append(MNU_WORDWRAP, _("&Word wrap"), _("Enable or disable word wrapping"), wxITEM_CHECK);
	viewMenu->Append(MNU_SHOWLINENUMBER, _("&Line number"), _("Enable or disable display of line number"), wxITEM_CHECK);
	viewMenu->AppendSeparator();
	viewMenu->Append(MNU_DEFAULTVIEW, _("&Default view\tCtrl-Alt-V"),     _("Restore the default view."));

	menuBar->Append(viewMenu, _("&View"));

	wxMenu *helpMenu = new wxMenu();
	helpMenu->Append(MNU_CONTENTS, _("&Help"),                 _("Open the helpfile."));
	helpMenu->Append(MNU_HELP, _("&SQL Help\tF1"),                _("Display help on SQL commands."));

	menuBar->Append(helpMenu, _("&Help"));

	SetMenuBar(menuBar);

	queryMenu->Check(MNU_VERBOSE, settings->GetExplainVerbose());
	queryMenu->Check(MNU_COSTS, settings->GetExplainCosts());
	queryMenu->Check(MNU_BUFFERS, settings->GetExplainBuffers());

	UpdateRecentFiles();

	wxAcceleratorEntry entries[12];

	entries[0].Set(wxACCEL_CTRL,                (int)'E',      MNU_EXECUTE);
	entries[1].Set(wxACCEL_CTRL,                (int)'O',      MNU_OPEN);
	entries[2].Set(wxACCEL_CTRL,                (int)'S',      MNU_SAVE);
	entries[3].Set(wxACCEL_CTRL,                (int)'F',      MNU_FIND);
	entries[4].Set(wxACCEL_CTRL,                (int)'R',      MNU_REPLACE);
	entries[5].Set(wxACCEL_NORMAL,              WXK_F5,        MNU_EXECUTE);
	entries[6].Set(wxACCEL_NORMAL,              WXK_F7,        MNU_EXPLAIN);
	entries[7].Set(wxACCEL_ALT,                 WXK_PAUSE,     MNU_CANCEL);
	entries[8].Set(wxACCEL_CTRL,                (int)'A',       MNU_SELECTALL);
	entries[9].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);
	entries[10].Set(wxACCEL_CTRL,               (int)'N',      MNU_NEW);
	entries[11].Set(wxACCEL_CTRL,               WXK_F6,        MNU_EXECPGS);

	wxAcceleratorTable accel(12, entries);
	SetAcceleratorTable(accel);

	queryMenu->Enable(MNU_CANCEL, false);

	int iWidths[6] = {0, -1, 40, 150, 80, 80};
	statusBar = CreateStatusBar(6);
	SetStatusBarPane(-1);
	SetStatusWidths(6, iWidths);
	SetStatusText(_("ready"), STATUSPOS_MSGS);

	toolBar = new ctlMenuToolbar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);

	toolBar->SetToolBitmapSize(wxSize(16, 16));

	toolBar->AddTool(MNU_NEW, _("New"), wxBitmap(file_new_xpm), _("New window"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_OPEN, _("Open"), wxBitmap(file_open_xpm), _("Open file"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_SAVE, _("Save"), wxBitmap(file_save_xpm), _("Save file"), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_CUT, _("Cut"), wxBitmap(clip_cut_xpm), _("Cut selected text to clipboard"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_COPY, _("Copy"), wxBitmap(clip_copy_xpm), _("Copy selected text to clipboard"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_PASTE, _("Paste"), wxBitmap(clip_paste_xpm), _("Paste selected text from clipboard"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_CLEAR, _("Clear window"), wxBitmap(edit_clear_xpm), _("Clear edit window"), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_UNDO, _("Undo"), wxBitmap(edit_undo_xpm), _("Undo last action"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_REDO, _("Redo"), wxBitmap(edit_redo_xpm), _("Redo last action"), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_FIND, _("Find"), wxBitmap(edit_find_xpm), _("Find and replace text"), wxITEM_NORMAL);
	toolBar->AddSeparator();

	toolBar->AddTool(MNU_EXECUTE, _("Execute"), wxBitmap(query_execute_xpm), _("Execute query"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_EXECPGS, _("Execute pgScript"), wxBitmap(query_pgscript_xpm), _("Execute pgScript"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_EXECFILE, _("Execute to file"), wxBitmap(query_execfile_xpm), _("Execute query, write result to file"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_EXPLAIN, _("Explain"), wxBitmap(query_explain_xpm), _("Explain query"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_CANCEL, _("Cancel"), wxBitmap(query_cancel_xpm), _("Cancel query"), wxITEM_NORMAL);
	toolBar->AddSeparator();

	toolBar->AddTool(MNU_HELP, _("Help"), wxBitmap(help_xpm), _("Display help on SQL commands."), wxITEM_NORMAL);
	toolBar->Realize();

	// Add the database selection bar
	cbConnection = new wxBitmapComboBox(this, CTRLID_CONNECTION, wxEmptyString, wxDefaultPosition, wxSize(-1, -1), NULL, wxCB_READONLY | wxCB_DROPDOWN);
	cbConnection->Append(conn->GetName(), CreateBitmap(GetServerColour(conn)), (void *)conn);
	cbConnection->Append(_("<new connection>"), wxNullBitmap, (void *)0);

	//Create SQL editor notebook
	sqlNotebook = new wxNotebook(this, CTL_NTBKCENTER, wxDefaultPosition, wxDefaultSize);

	// Create panel for query
	wxPanel *pnlQuery = new wxPanel(sqlNotebook);

	// Create the outer box sizer
	wxBoxSizer *boxQuery = new wxBoxSizer(wxVERTICAL);

	// Create the inner box sizer
	// This one will contain the combobox, and the two buttons
	wxBoxSizer *boxHistory = new wxBoxSizer(wxHORIZONTAL);

	// Query combobox
	sqlQueries = new wxComboBox(pnlQuery, CTL_SQLQUERYCBOX, wxT(""), wxDefaultPosition, wxDefaultSize, NULL, wxCB_DROPDOWN | wxCB_READONLY);
	sqlQueries->SetToolTip(_("Previous queries"));
	LoadQueries();
	boxHistory->Add(sqlQueries, 1, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL, 1);

	// Delete Current button
	btnDeleteCurrent = new wxButton(pnlQuery, CTL_DELETECURRENTBTN, _("Delete"));
	btnDeleteCurrent->Enable(false);
	boxHistory->Add(btnDeleteCurrent, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);

	// Delete All button
	btnDeleteAll = new wxButton(pnlQuery, CTL_DELETEALLBTN, _("Delete All"));
	btnDeleteAll->Enable(sqlQueries->GetCount() > 0);
	boxHistory->Add(btnDeleteAll, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);

	boxQuery->Add(boxHistory, 0, wxEXPAND | wxALL, 1);

	// Create the other inner box sizer
	// This one will contain the SQL box
	wxBoxSizer *boxSQL = new wxBoxSizer(wxHORIZONTAL);

	// Query box
	sqlQuery = new ctlSQLBox(pnlQuery, CTL_SQLQUERY, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_RICH2);
	sqlQuery->SetDatabase(conn);
	sqlQuery->SetMarginWidth(1, 16);
	sqlQuery->SetDropTarget(new DnDFile(this));
	SetEOLModeDisplay(sqlQuery->GetEOLMode());
	boxSQL->Add(sqlQuery, 1, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 1);

	boxQuery->Add(boxSQL, 1, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 1);

	// Auto-sizing
	pnlQuery->SetSizer(boxQuery);
	boxQuery->Fit(pnlQuery);

	// Results pane
	outputPane = new wxNotebook(this, CTL_NTBKGQB, wxDefaultPosition, wxSize(500, 300));
	sqlResult = new ctlSQLResult(outputPane, conn, CTL_SQLRESULT, wxDefaultPosition, wxDefaultSize);
	explainCanvas = new ExplainCanvas(outputPane);
	msgResult = new wxTextCtrl(outputPane, CTL_MSGRESULT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
	msgResult->SetFont(settings->GetSQLFont());
	msgHistory = new wxTextCtrl(outputPane, CTL_MSGHISTORY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
	msgHistory->SetFont(settings->GetSQLFont());

	// Graphical Canvas
	// initialize values
	model = new gqbModel();
	controller = new gqbController(model, sqlNotebook, outputPane, wxSize(GQB_MIN_WIDTH, GQB_MIN_HEIGHT));
	firstTime = true;                             // Inform to GQB that the tree of table haven't filled.
	gqbUpdateRunning = false;                      // Are we already updating the SQL query - event recursion protection.
	adjustSizesTimer = NULL;                      // Timer used to avoid a bug when close outputPane

	// Setup SQL editor notebook NBP_SQLEDTR
	sqlNotebook->AddPage(pnlQuery, _("SQL Editor"));
	sqlNotebook->AddPage(controller->getViewContainer(), _("Graphical Query Builder"));
	sqlNotebook->SetSelection(0);

	outputPane->AddPage(sqlResult, _("Data Output"));
	outputPane->AddPage(explainCanvas, _("Explain"));
	outputPane->AddPage(msgResult, _("Messages"));
	outputPane->AddPage(msgHistory, _("History"));

	sqlQuery->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));
	sqlResult->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));
	msgResult->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));
	msgHistory->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));

	// Now, the scratchpad
	scratchPad = new wxTextCtrl(this, CTL_SCRATCHPAD, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxHSCROLL);

	// Kickstart wxAUI
	manager.AddPane(toolBar, wxAuiPaneInfo().Name(wxT("toolBar")).Caption(_("Tool bar")).ToolbarPane().Top().LeftDockable(false).RightDockable(false));
	manager.AddPane(cbConnection, wxAuiPaneInfo().Name(wxT("databaseBar")).Caption(_("Database bar")).ToolbarPane().Top().LeftDockable(false).RightDockable(false));
	manager.AddPane(outputPane, wxAuiPaneInfo().Name(wxT("outputPane")).Caption(_("Output pane")).Bottom().MinSize(wxSize(200, 100)).BestSize(wxSize(550, 300)));
	manager.AddPane(scratchPad, wxAuiPaneInfo().Name(wxT("scratchPad")).Caption(_("Scratch pad")).Right().MinSize(wxSize(100, 100)).BestSize(wxSize(250, 200)));
	manager.AddPane(sqlNotebook, wxAuiPaneInfo().Name(wxT("sqlQuery")).Caption(_("SQL query")).Center().CaptionVisible(false).CloseButton(false).MinSize(wxSize(200, 100)).BestSize(wxSize(350, 200)));

	// Now load the layout
	wxString perspective;
	settings->Read(wxT("frmQuery/Perspective-") + wxString(FRMQUERY_PERSPECTIVE_VER), &perspective, FRMQUERY_DEFAULT_PERSPECTIVE);
	manager.LoadPerspective(perspective, true);

	// and reset the captions for the current language
	manager.GetPane(wxT("toolBar")).Caption(_("Tool bar"));
	manager.GetPane(wxT("databaseBar")).Caption(_("Database bar"));
	manager.GetPane(wxT("sqlQuery")).Caption(_("SQL query"));
	manager.GetPane(wxT("outputPane")).Caption(_("Output pane"));
	manager.GetPane(wxT("scratchPad")).Caption(_("Scratch pad"));


	// Sync the View menu options
	viewMenu->Check(MNU_DATABASEBAR, manager.GetPane(wxT("databaseBar")).IsShown());
	viewMenu->Check(MNU_TOOLBAR, manager.GetPane(wxT("toolBar")).IsShown());
	viewMenu->Check(MNU_OUTPUTPANE, manager.GetPane(wxT("outputPane")).IsShown());
	viewMenu->Check(MNU_SCRATCHPAD, manager.GetPane(wxT("scratchPad")).IsShown());

	// tell the manager to "commit" all the changes just made
	manager.Update();

	bool bVal;

	// Auto-rollback
	settings->Read(wxT("frmQuery/AutoRollback"), &bVal, false);
	queryMenu->Check(MNU_AUTOROLLBACK, bVal);

	// Auto indent
	settings->Read(wxT("frmQuery/AutoIndent"), &bVal, true);
	editMenu->Check(MNU_AUTOINDENT, bVal);
	if (bVal)
		sqlQuery->SetAutoIndent(true);
	else
		sqlQuery->SetAutoIndent(false);

	// Word wrap
	settings->Read(wxT("frmQuery/WordWrap"), &bVal, false);
	viewMenu->Check(MNU_WORDWRAP, bVal);
	if (bVal)
		sqlQuery->SetWrapMode(wxSTC_WRAP_WORD);
	else
		sqlQuery->SetWrapMode(wxSTC_WRAP_NONE);

	// Indent Guides
	settings->Read(wxT("frmQuery/ShowIndentGuides"), &bVal, false);
	viewMenu->Check(MNU_SHOWINDENTGUIDES, bVal);
	if (bVal)
		sqlQuery->SetIndentationGuides(true);
	else
		sqlQuery->SetIndentationGuides(false);

	// Whitespace
	settings->Read(wxT("frmQuery/ShowWhitespace"), &bVal, false);
	viewMenu->Check(MNU_SHOWWHITESPACE, bVal);
	if (bVal)
		sqlQuery->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
	else
		sqlQuery->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);

	// Line ends
	settings->Read(wxT("frmQuery/ShowLineEnds"), &bVal, false);
	viewMenu->Check(MNU_SHOWLINEENDS, bVal);
	if (bVal)
		sqlQuery->SetViewEOL(1);
	else
		sqlQuery->SetViewEOL(0);

	// Line number
	settings->Read(wxT("frmQuery/ShowLineNumber"), &bVal, false);
	viewMenu->Check(MNU_SHOWLINENUMBER, bVal);

	if (!file.IsEmpty() && wxFileName::FileExists(file))
	{
		wxFileName fn = file;
		lastFilename = fn.GetFullName();
		lastDir = fn.GetPath();
		lastPath = fn.GetFullPath();
		OpenLastFile();
	}
	else
		sqlQuery->SetText(query);

	sqlQuery->Colourise(0, query.Length());

	changed = !query.IsNull() && settings->GetStickySql();
	if (changed)
		setExtendedTitle();
	updateMenu();
	queryMenu->Enable(MNU_SAVEHISTORY, false);
	queryMenu->Enable(MNU_CLEARHISTORY, false);
	setTools(false);
	lastFileFormat = settings->GetUnicodeFile();

	msgResult->SetMaxLength(0L);
	msgHistory->SetMaxLength(0L);
}


frmQuery::~frmQuery()
{
	closing = true;

	// Save frmQuery Perspective
	settings->Write(wxT("frmQuery/Perspective-") + wxString(FRMQUERY_PERSPECTIVE_VER), manager.SavePerspective());

	// Uninitialize wxAUIManager
	manager.UnInit();

	if(sqlNotebook)
	{
		delete sqlNotebook;
		sqlNotebook = NULL;
	}
	if(controller)
	{
		delete controller;
		controller = NULL;
	}
	if(model)
	{
		delete model;
		model = NULL;
	}
	if(adjustSizesTimer)
	{
		delete adjustSizesTimer;
		adjustSizesTimer = NULL;
	}

	while (cbConnection->GetCount() > 1)
	{
		delete (pgConn *)cbConnection->GetClientData(0);
		cbConnection->Delete(0);
	}

	if (favourites)
	{
		delete favourites;
		favourites = NULL;
	}

	if (pgsTimer)
	{
		delete pgsTimer;
		pgsTimer = NULL;
	}

	if (pgScript)
	{
		delete pgScript;
		pgScript = NULL;
	}

	if (mainForm)
		mainForm->RemoveFrame(this);
}


void frmQuery::OnExit(wxCommandEvent &event)
{
	closing = true;
	Close();
}


void frmQuery::OnEraseBackground(wxEraseEvent &event)
{
	event.Skip();
}


void frmQuery::OnSize(wxSizeEvent &event)
{
	event.Skip();
}


void frmQuery::OnToggleScratchPad(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_SCRATCHPAD))
		manager.GetPane(wxT("scratchPad")).Show(true);
	else
		manager.GetPane(wxT("scratchPad")).Show(false);
	manager.Update();
}


void frmQuery::OnToggleDatabaseBar(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_DATABASEBAR))
		manager.GetPane(wxT("databaseBar")).Show(true);
	else
		manager.GetPane(wxT("databaseBar")).Show(false);
	manager.Update();
}


void frmQuery::OnToggleToolBar(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_TOOLBAR))
		manager.GetPane(wxT("toolBar")).Show(true);
	else
		manager.GetPane(wxT("toolBar")).Show(false);
	manager.Update();
}


void frmQuery::OnToggleOutputPane(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_OUTPUTPANE))
	{
		manager.GetPane(wxT("outputPane")).Show(true);
	}
	else
	{
		manager.GetPane(wxT("outputPane")).Show(false);
	}
	manager.Update();
	adjustGQBSizes();
}


void frmQuery::OnAuiUpdate(wxAuiManagerEvent &event)
{
	if(event.pane->name == wxT("databaseBar"))
	{
		viewMenu->Check(MNU_DATABASEBAR, false);
	}
	else if(event.pane->name == wxT("toolBar"))
	{
		viewMenu->Check(MNU_TOOLBAR, false);
	}
	else if(event.pane->name == wxT("outputPane"))
	{
		viewMenu->Check(MNU_OUTPUTPANE, false);
		if(!adjustSizesTimer)
			adjustSizesTimer = new wxTimer(this, CTL_TIMERSIZES);
		adjustSizesTimer->Start(500);
	}
	else if(event.pane->name == wxT("scratchPad"))
	{
		viewMenu->Check(MNU_SCRATCHPAD, false);
	}
	event.Skip();
}


void frmQuery::OnDefaultView(wxCommandEvent &event)
{
	manager.LoadPerspective(FRMQUERY_DEFAULT_PERSPECTIVE, true);

	// Reset the captions for the current language
	manager.GetPane(wxT("toolBar")).Caption(_("Tool bar"));
	manager.GetPane(wxT("databaseBar")).Caption(_("Database bar"));
	manager.GetPane(wxT("sqlQuery")).Caption(_("SQL query"));
	manager.GetPane(wxT("outputPane")).Caption(_("Output pane"));
	manager.GetPane(wxT("scratchPad")).Caption(_("Scratch pad"));

	manager.Update();

	// Sync the View menu options
	viewMenu->Check(MNU_DATABASEBAR, manager.GetPane(wxT("databaseBar")).IsShown());
	viewMenu->Check(MNU_TOOLBAR, manager.GetPane(wxT("toolBar")).IsShown());
	viewMenu->Check(MNU_OUTPUTPANE, manager.GetPane(wxT("outputPane")).IsShown());
	viewMenu->Check(MNU_SCRATCHPAD, manager.GetPane(wxT("scratchPad")).IsShown());
}


void frmQuery::OnAutoRollback(wxCommandEvent &event)
{
	queryMenu->Check(MNU_AUTOROLLBACK, event.IsChecked());

	settings->Write(wxT("frmQuery/AutoRollback"), queryMenu->IsChecked(MNU_AUTOROLLBACK));
}


void frmQuery::OnAutoIndent(wxCommandEvent &event)
{
	editMenu->Check(MNU_AUTOINDENT, event.IsChecked());

	settings->Write(wxT("frmQuery/AutoIndent"), editMenu->IsChecked(MNU_AUTOINDENT));

	if (editMenu->IsChecked(MNU_AUTOINDENT))
		sqlQuery->SetAutoIndent(true);
	else
		sqlQuery->SetAutoIndent(false);
}


void frmQuery::OnWordWrap(wxCommandEvent &event)
{
	viewMenu->Check(MNU_WORDWRAP, event.IsChecked());

	settings->Write(wxT("frmQuery/WordWrap"), viewMenu->IsChecked(MNU_WORDWRAP));

	if (viewMenu->IsChecked(MNU_WORDWRAP))
		sqlQuery->SetWrapMode(wxSTC_WRAP_WORD);
	else
		sqlQuery->SetWrapMode(wxSTC_WRAP_NONE);
}


void frmQuery::OnShowIndentGuides(wxCommandEvent &event)
{
	viewMenu->Check(MNU_SHOWINDENTGUIDES, event.IsChecked());

	settings->Write(wxT("frmQuery/ShowIndentGuides"), viewMenu->IsChecked(MNU_SHOWINDENTGUIDES));

	if (viewMenu->IsChecked(MNU_SHOWINDENTGUIDES))
		sqlQuery->SetIndentationGuides(true);
	else
		sqlQuery->SetIndentationGuides(false);
}


void frmQuery::OnShowWhitespace(wxCommandEvent &event)
{
	viewMenu->Check(MNU_SHOWWHITESPACE, event.IsChecked());

	settings->Write(wxT("frmQuery/ShowWhitespace"), viewMenu->IsChecked(MNU_SHOWWHITESPACE));

	if (viewMenu->IsChecked(MNU_SHOWWHITESPACE))
		sqlQuery->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
	else
		sqlQuery->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);
}


void frmQuery::OnShowLineEnds(wxCommandEvent &event)
{
	viewMenu->Check(MNU_SHOWLINEENDS, event.IsChecked());

	settings->Write(wxT("frmQuery/ShowLineEnds"), viewMenu->IsChecked(MNU_SHOWLINEENDS));

	if (viewMenu->IsChecked(MNU_SHOWLINEENDS))
		sqlQuery->SetViewEOL(1);
	else
		sqlQuery->SetViewEOL(0);
}


void frmQuery::OnShowLineNumber(wxCommandEvent &event)
{
	viewMenu->Check(MNU_SHOWLINENUMBER, event.IsChecked());

	settings->Write(wxT("frmQuery/ShowLineNumber"), viewMenu->IsChecked(MNU_SHOWLINENUMBER));

	sqlQuery->UpdateLineNumber();
}


void frmQuery::OnActivate(wxActivateEvent &event)
{
	if (event.GetActive())
		updateMenu();
	event.Skip();
}


void frmQuery::OnExport(wxCommandEvent &ev)
{
	sqlResult->Export();
}


void frmQuery::Go()
{
	cbConnection->SetSelection(0L);
	wxCommandEvent ev;
	OnChangeConnection(ev);

	Show(true);
	sqlQuery->SetFocus();
	loading = false;
}


typedef struct __sqltokenhelp
{
	const wxChar *token;
	const wxChar *page;
	int type;
} SqlTokenHelp;

SqlTokenHelp sqlTokenHelp[] =
{
	{ wxT("ABORT"), 0, 0},
	{ wxT("ALTER"), 0, 2},
	{ wxT("ANALYZE"), 0, 0},
	{ wxT("BEGIN"), 0, 0},
	{ wxT("CHECKPOINT"), 0, 0},
	{ wxT("CLOSE"), 0, 0},
	{ wxT("CLUSTER"), 0, 0},
	{ wxT("COMMENT"), 0, 0},
	{ wxT("COMMIT"), 0, 0},
	{ wxT("COPY"), 0, 0},
	{ wxT("CREATE"), 0, 1},
	{ wxT("DEALLOCATE"), 0, 0},
	{ wxT("DECLARE"), 0, 0},
	{ wxT("DELETE"), 0, 0},
	{ wxT("DROP"), 0, 1},
	{ wxT("END"), 0, 0},
	{ wxT("EXECUTE"), 0, 0},
	{ wxT("EXPLAIN"), 0, 0},
	{ wxT("FETCH"), 0, 0},
	{ wxT("GRANT"), 0, 0},
	{ wxT("INSERT"), 0, 0},
	{ wxT("LISTEN"), 0, 0},
	{ wxT("LOAD"), 0, 0},
	{ wxT("LOCK"), 0, 0},
	{ wxT("MOVE"), 0, 0},
	{ wxT("NOTIFY"), 0, 0},
	{ wxT("END"), 0, 0},
	// { wxT("PREPARE"), 0, 0},  handled individually
	{ wxT("REINDEX"), 0, 0},
	{ wxT("RELEASE"), wxT("pg/sql-release-savepoint"), 0},
	{ wxT("RESET"), 0, 0},
	{ wxT("REVOKE"), 0, 0},
	// { wxT("ROLLBACK"), 0, 0}, handled individually
	{ wxT("SAVEPOINT"), 0, 0},
	{ wxT("SELECT"), 0, 0},
	{ wxT("SET"), 0, 0},
	{ wxT("SHOW"), 0, 0},
	{ wxT("START"), wxT("pg/sql-start-transaction"), 0},
	{ wxT("TRUNCATE"), 0, 0},
	{ wxT("UNLISTEN"), 0, 0},
	{ wxT("UPDATE"), 0, 0},
	{ wxT("VACUUM"), 0, 0},

	{ wxT("AGGREGATE"), 0, 11},
	{ wxT("CAST"), 0, 11},
	{ wxT("CONSTRAINT"), 0, 11},
	{ wxT("CONVERSION"), 0, 11},
	{ wxT("DATABASE"), 0, 12},
	{ wxT("DOMAIN"), 0, 11},
	{ wxT("FUNCTION"), 0, 11},
	{ wxT("GROUP"), 0, 12},
	{ wxT("INDEX"), 0, 11},
	{ wxT("LANGUAGE"), 0, 11},
	{ wxT("OPERATOR"), 0, 11},
	{ wxT("ROLE"), 0, 11},
	{ wxT("RULE"), 0, 11},
	{ wxT("SCHEMA"), 0, 11},
	{ wxT("SEQUENCE"), 0, 11},
	{ wxT("TABLE"), 0, 12},
	{ wxT("TABLESPACE"), 0, 12},
	{ wxT("TRIGGER"), 0, 12},
	{ wxT("TYPE"), 0, 11},
	{ wxT("USER"), 0, 12},
	{ wxT("VIEW"), 0, 11},
	{ wxT("EXTTABLE"), 0, 12},
	{ 0, 0 }
};

void frmQuery::OnContents(wxCommandEvent &event)
{
	DisplayHelp(wxT("query"), HELP_PGADMIN);
}


void frmQuery::OnChangeConnection(wxCommandEvent &ev)
{
	// On Solaris, this event seems to get fired when the form closes(!!)
	if(!IsVisible() && !loading)
		return;

	unsigned int sel = cbConnection->GetSelection();
	if (sel == cbConnection->GetCount() - 1)
	{
		// new Connection
		dlgSelectConnection dlg(this, mainForm);
		int rc = dlg.Go(conn, cbConnection);
		if (rc == wxID_OK)
		{
			bool createdNewConn;
			wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Query Tool");
			pgConn *newconn = dlg.CreateConn(applicationname, createdNewConn);
			if (newconn && createdNewConn)
			{
				cbConnection->Insert(newconn->GetName(), CreateBitmap(GetServerColour(newconn)), sel, (void *)newconn);
				cbConnection->SetSelection(sel);
				OnChangeConnection(ev);
			}
			else
				rc = wxID_CANCEL;
		}
		if (rc != wxID_OK)
		{
			unsigned int i;
			for (i = 0 ; i < sel ; i++)
			{
				if (cbConnection->GetClientData(i) == conn)
				{
					cbConnection->SetSelection(i);
					break;
				}
			}
		}
	}
	else
	{
		conn = (pgConn *)cbConnection->GetClientData(sel);
		sqlResult->SetConnection(conn);
		title = wxT("Query - ") + cbConnection->GetValue();
		setExtendedTitle();

		//Refresh GQB Tree if used
		if(conn && !firstTime)
		{
			controller->getTablesBrowser()->refreshTables(conn);
			controller->getView()->Refresh();
		}
	}
}


void frmQuery::OnHelp(wxCommandEvent &event)
{
	wxString page;
	wxString query = sqlQuery->GetSelectedText();
	if (query.IsNull())
		query = sqlQuery->GetText();

	query.Trim(false);

	if (!query.IsEmpty())
	{
		wxStringTokenizer tokens(query);
		query = tokens.GetNextToken();

		if (query.IsSameAs(wxT("PREPARE"), false))
		{
			if (tokens.GetNextToken().IsSameAs(wxT("TRANSACTION"), false))
				page = wxT("sql-prepare-transaction");
			else
				page = wxT("sql-prepare");
		}
		else if (query.IsSameAs(wxT("ROLLBACK"), false))
		{
			if (tokens.GetNextToken().IsSameAs(wxT("PREPARED"), false))
				page = wxT("sql-rollback-prepared");
			else
				page = wxT("sql-rollback");
		}
		else
		{
			SqlTokenHelp *sth = sqlTokenHelp;
			while (sth->token)
			{
				if (sth->type < 10 && query.IsSameAs(sth->token, false))
				{
					if (sth->page)
						page = sth->page;
					else
						page = wxT("sql-") + query.Lower();

					if (sth->type)
					{
						int type = sth->type + 10;

						query = tokens.GetNextToken();
						sth = sqlTokenHelp;
						while (sth->token)
						{
							if (sth->type >= type && query.IsSameAs(sth->token, false))
							{
								if (sth->page)
									page += sth->page;
								else
									page += query.Lower();
								break;
							}
							sth++;
						}
						if (!sth->token)
							page = wxT("sql-commands");
					}
					break;
				}
				sth++;
			}
		}
	}
	if (page.IsEmpty())
		page = wxT("sql-commands");

	if (conn->GetIsEdb())
		DisplayHelp(page, HELP_ENTERPRISEDB);
	else if (conn->GetIsGreenplum())
		DisplayHelp(page, HELP_GREENPLUM);
	else
		DisplayHelp(page, HELP_POSTGRESQL);
}


void frmQuery::OnSaveHistory(wxCommandEvent &event)
{
#ifdef __WXMSW__
	wxFileDialog *dlg = new wxFileDialog(this, _("Save history"), lastDir, wxEmptyString,
	                                     _("Log files (*.log)|*.log|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
#else
	wxFileDialog *dlg = new wxFileDialog(this, _("Save history"), lastDir, wxEmptyString,
	                                     _("Log files (*.log)|*.log|All files (*)|*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
#endif
	if (dlg->ShowModal() == wxID_OK)
	{
		if (!FileWrite(dlg->GetPath(), msgHistory->GetValue(), false))
		{
			wxLogError(__("Could not write the file %s: Errcode=%d."), dlg->GetPath().c_str(), wxSysErrorCode());
		}
	}
	delete dlg;

}

void frmQuery::OnChangeNotebook(wxNotebookEvent &event)
{
	if(sqlNotebook && sqlNotebook->GetPageCount() >= 2)
	{

		if (event.GetSelection() == 0)
		{
			queryMenu->SetHelpString(MNU_EXECUTE, _("Execute query"));
			queryMenu->SetHelpString(MNU_EXECFILE, _("Execute query, write result to file"));
			toolBar->SetToolShortHelp(MNU_EXECUTE, _("Execute query"));
			toolBar->SetToolShortHelp(MNU_EXECFILE, _("Execute query, write result to file"));
			viewMenu->Enable(MNU_OUTPUTPANE, true);
			viewMenu->Enable(MNU_SCRATCHPAD, true);

			// Reset the panes
			if (viewMenu->IsChecked(MNU_OUTPUTPANE))
				manager.GetPane(wxT("outputPane")).Show(true);
			if (viewMenu->IsChecked(MNU_SCRATCHPAD))
				manager.GetPane(wxT("scratchPad")).Show(true);
			manager.Update();

			updateFromGqb(false);
		}
		else
		{
			manager.GetPane(wxT("outputPane")).Show(false);
			manager.GetPane(wxT("scratchPad")).Show(false);
			manager.Update();
			viewMenu->Enable(MNU_OUTPUTPANE, false);
			viewMenu->Enable(MNU_SCRATCHPAD, false);

			if(firstTime)        //Things that should be done on first click on GQB
			{
				// Menu
				queryMenu->Append(MNU_EXECUTE, _("Generate SQL from Graphical Query Builder Model"));
				queryMenu->SetHelpString(MNU_EXECFILE, _("Generate SQL from Graphical Query Builder Model"));
				toolBar->SetToolShortHelp(MNU_EXECUTE, _("Generate SQL from Graphical Query Builder Model"));
				toolBar->SetToolShortHelp(MNU_EXECFILE, _("Generate SQL from Graphical Query Builder Model"));

				// Size, and pause to allow the window to draw
				adjustGQBSizes();
				wxTheApp->Yield(true);

				// Database related Stuffs.
				// Create a server object and connect it.
				controller->getTablesBrowser()->refreshTables(conn);
				firstTime = false;
			}
		}
	}
}


void frmQuery::OnSetFocus(wxFocusEvent &event)
{
	sqlQuery->SetFocus();
	event.Skip();
}


void frmQuery::OnClearHistory(wxCommandEvent &event)
{
	queryMenu->Enable(MNU_SAVEHISTORY, false);
	queryMenu->Enable(MNU_CLEARHISTORY, false);
	msgHistory->Clear();
}


void frmQuery::OnFocus(wxFocusEvent &ev)
{
	if (wxDynamicCast(this, wxFrame))
		updateMenu(ev.GetEventObject());
	else
	{
		frmQuery *wnd = (frmQuery *)GetParent();

		if (wnd)
			wnd->OnFocus(ev);
	}
	ev.Skip();
}


void frmQuery::OnCut(wxCommandEvent &ev)
{
	if (currentControl() == sqlQuery)
	{
		sqlQuery->Cut();
		updateMenu();
	}
}


wxWindow *frmQuery::currentControl()
{
	wxWindow *wnd = FindFocus();
	if (wnd == outputPane)
	{
		switch (outputPane->GetSelection())
		{
			case 0:
				wnd = sqlResult;
				break;
			case 1:
				wnd = explainCanvas;
				break;
			case 2:
				wnd = msgResult;
				break;
			case 3:
				wnd = msgHistory;
				break;
		}
	}
	return wnd;

}


void frmQuery::OnCopy(wxCommandEvent &ev)
{
	wxWindow *wnd = currentControl();

	if (wnd == sqlQuery)
		sqlQuery->Copy();
	else if (wnd == msgResult)
		msgResult->Copy();
	else if (wnd == msgHistory)
		msgHistory->Copy();
	else if (wnd == scratchPad)
		scratchPad->Copy();
	else
	{
		wxWindow *obj = wnd;

		while (obj != NULL)
		{
			if (obj == sqlResult)
			{
				sqlResult->Copy();
				break;
			}
			obj = obj->GetParent();
		}
	}
	updateMenu();
}


void frmQuery::OnPaste(wxCommandEvent &ev)
{
	if (currentControl() == sqlQuery)
		sqlQuery->Paste();
	else if (currentControl() == scratchPad)
		scratchPad->Paste();
}


void frmQuery::OnClear(wxCommandEvent &ev)
{
	wxWindow *wnd = currentControl();

	if (wnd == sqlQuery)
		sqlQuery->ClearAll();
	else if (wnd == msgResult)
		msgResult->Clear();
	else if (wnd == msgHistory)
		msgHistory->Clear();
	else if (wnd == scratchPad)
		scratchPad->Clear();
}


void frmQuery::OnSelectAll(wxCommandEvent &ev)
{
	wxWindow *wnd = currentControl();

	if (wnd == sqlQuery)
		sqlQuery->SelectAll();
	else if (wnd == msgResult)
		msgResult->SelectAll();
	else if (wnd == msgHistory)
		msgHistory->SelectAll();
	else if (wnd == sqlResult)
		sqlResult->SelectAll();
	else if (wnd == scratchPad)
		scratchPad->SelectAll();
	else if (wnd->GetParent() == sqlResult)
		sqlResult->SelectAll();
}


void frmQuery::OnSearchReplace(wxCommandEvent &ev)
{
	sqlQuery->OnSearchReplace(ev);
}


void frmQuery::OnUndo(wxCommandEvent &ev)
{
	sqlQuery->Undo();
}


void frmQuery::OnRedo(wxCommandEvent &ev)
{
	sqlQuery->Redo();
}


void frmQuery::setExtendedTitle()
{
	wxString chgStr;
	if (changed)
		chgStr = wxT(" *");

	if (lastPath.IsNull())
		SetTitle(title + chgStr);
	else
	{
		SetTitle(title + wxT(" - [") + lastPath + wxT("]") + chgStr);
	}
}


void frmQuery::updateMenu(wxObject *obj)
{
	bool canCut = false;
	bool canPaste = false;
	bool canUndo = false;
	bool canRedo = false;
	bool canClear = false;
	bool canFind = false;
	bool canAddFavourite = false;
	bool canSaveExplain = false;
	bool canSaveGQB = false;

	wxAuiFloatingFrame *fp = wxDynamicCastThis(wxAuiFloatingFrame);
	if (fp)
		return;

	if (closing)
		return;

	if (obj != msgResult && obj != msgHistory)
	{
		if (sqlQuery)
		{
			canUndo = sqlQuery->CanUndo();
			canRedo = sqlQuery->CanRedo();
			canPaste = sqlQuery->CanPaste();
			canAddFavourite = (sqlQuery->GetLength() > 0);
		}

		canCut = true;
		canClear = true;
		canFind = true;
	}
	else if (obj == msgResult || obj == msgHistory)
	{
		canClear = true;
	}

	canSaveExplain = explainCanvas->GetDiagram()->GetCount() > 0;
	canSaveGQB = controller->getView()->canSaveAsImage();

	toolBar->EnableTool(MNU_UNDO, canUndo);
	editMenu->Enable(MNU_UNDO, canUndo);

	toolBar->EnableTool(MNU_REDO, canRedo);
	editMenu->Enable(MNU_REDO, canRedo);

	toolBar->EnableTool(MNU_PASTE, canPaste);
	editMenu->Enable(MNU_PASTE, canPaste);

	toolBar->EnableTool(MNU_CUT, canCut);
	editMenu->Enable(MNU_CUT, canCut);

	toolBar->EnableTool(MNU_CLEAR, canClear);
	editMenu->Enable(MNU_CLEAR, canClear);

	toolBar->EnableTool(MNU_FIND, canFind);
	editMenu->Enable(MNU_FIND, canFind);

	favouritesMenu->Enable(MNU_FAVOURITES_ADD, canAddFavourite);

}


void frmQuery::UpdateFavouritesList()
{
	if (favourites)
		delete favourites;

	favourites = queryFavouriteFileProvider::LoadFavourites(true);

	while (favouritesMenu->GetMenuItemCount() > 3)
	{
		favouritesMenu->Destroy(favouritesMenu->GetMenuItems()[3]);
	}

	favourites->AppendAllToMenu(favouritesMenu, MNU_FAVOURITES_MANAGE + 1);
}


void frmQuery::UpdateMacrosList()
{
	if (macros)
		delete macros;

	macros = queryMacroFileProvider::LoadMacros(true);

	while (macrosMenu->GetMenuItemCount() > 2)
	{
		macrosMenu->Destroy(macrosMenu->GetMenuItems()[2]);
	}

	macros->AppendAllToMenu(macrosMenu, MNU_MACROS_MANAGE + 1);
}


void frmQuery::OnAddFavourite(wxCommandEvent &event)
{
	if (sqlQuery->GetText().Trim().IsEmpty())
		return;
	if (dlgAddFavourite(this, favourites).AddFavourite(sqlQuery->GetText()))
	{
		// Added a favourite, so save
		queryFavouriteFileProvider::SaveFavourites(favourites);
		mainForm->UpdateAllFavouritesList();
	}
}


void frmQuery::OnManageFavourites(wxCommandEvent &event)
{
	int r = dlgManageFavourites(this, favourites).ManageFavourites();
	if (r == 1)
	{
		// Changed something, so save
		queryFavouriteFileProvider::SaveFavourites(favourites);
	}
	if (r == 1 || r == -1)
	{
		// Changed something requiring rollback
		mainForm->UpdateAllFavouritesList();
	}
}


void frmQuery::OnSelectFavourite(wxCommandEvent &event)
{
	queryFavouriteItem *fav;

	fav = favourites->FindFavourite(event.GetId());
	if (!fav)
		return;

	if (!sqlQuery->GetText().Trim().IsEmpty())
	{
		int r = wxMessageDialog(this, _("Replace current query?"), _("Confirm replace"), wxYES_NO | wxCANCEL | wxICON_QUESTION).ShowModal();
		if (r == wxID_CANCEL)
			return;
		else if (r == wxID_YES)
			sqlQuery->ClearAll();
		else
		{
			if (sqlQuery->GetText().Last() != '\n')
				sqlQuery->AddText(wxT("\n"));     // Add a newline after the last query
		}
	}
	sqlQuery->AddText(fav->GetContents());
}


bool frmQuery::CheckChanged(bool canVeto)
{
	if (changed && settings->GetAskSaveConfirmation())
	{
		wxString fn;
		if (!lastPath.IsNull())
			fn = wxString::Format(_("The text in file %s has changed.\nDo you want to save changes?"), lastPath.c_str());
		else
			fn = _("The text has changed.\nDo you want to save changes?");
		wxMessageDialog msg(this, fn, _("Query"),
		                    wxYES_NO | wxICON_EXCLAMATION |
		                    (canVeto ? wxCANCEL : 0));

		wxCommandEvent noEvent;
		switch (msg.ShowModal())
		{
			case wxID_YES:
				if (lastPath.IsNull())
					OnSaveAs(noEvent);
				else
					OnSave(noEvent);

				return changed;

			case wxID_CANCEL:
				return true;
		}
	}
	return false;
}


void frmQuery::OnClose(wxCloseEvent &event)
{
	if (queryMenu->IsEnabled(MNU_CANCEL))
	{
		if (event.CanVeto())
		{
			wxMessageDialog msg(this, _("A query is running. Do you wish to cancel it?"), _("Query"),
			                    wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);

			if (msg.ShowModal() == wxID_NO)
			{
				event.Veto();
				return;
			}
		}

		wxCommandEvent ev;
		OnCancel(ev);
	}

	while (sqlResult->RunStatus() == CTLSQL_RUNNING)
	{
		wxLogInfo(wxT("SQL Query box: Waiting for query to abort"));
		wxSleep(1);
	}

	if (CheckChanged(event.CanVeto()) && event.CanVeto())
	{
		event.Veto();
		return;
	}

	closing = true;

	// Reset the panes
	if (viewMenu->IsChecked(MNU_OUTPUTPANE))
		manager.GetPane(wxT("outputPane")).Show(true);
	if (viewMenu->IsChecked(MNU_SCRATCHPAD))
		manager.GetPane(wxT("scratchPad")).Show(true);
	manager.Update();

	Hide();

	sqlQuery->Disconnect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));
	sqlResult->Disconnect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));
	msgResult->Disconnect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));
	msgHistory->Disconnect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(frmQuery::OnFocus));

	controller->nullView();                   //to avoid bug on *nix when deleting controller

	settings->SetExplainVerbose(queryMenu->IsChecked(MNU_VERBOSE));
	settings->SetExplainCosts(queryMenu->IsChecked(MNU_COSTS));
	settings->SetExplainBuffers(queryMenu->IsChecked(MNU_BUFFERS));

	sqlResult->Abort();                           // to make sure conn is unused

	Destroy();

}


void frmQuery::OnChangeStc(wxStyledTextEvent &event)
{
	// The STC seems to fire this event even if it loses focus. Fortunately,
	// that seems to be m_modificationType == 512.
	if (!changed && event.m_modificationType != 512)
	{
		changed = true;
		setExtendedTitle();
	}
	updateMenu();
}


void frmQuery::OnPositionStc(wxStyledTextEvent &event)
{
	wxString pos;
	pos.Printf(_("Ln %d Col %d Ch %d"), sqlQuery->LineFromPosition(sqlQuery->GetCurrentPos()) + 1, sqlQuery->GetColumn(sqlQuery->GetCurrentPos()) + 1, sqlQuery->GetCurrentPos() + 1);
	SetStatusText(pos, STATUSPOS_POS);
}


void frmQuery::OpenLastFile()
{
	wxString str;
	bool modeUnicode = settings->GetUnicodeFile();
	wxUtfFile file(lastPath, wxFile::read, modeUnicode ? wxFONTENCODING_UTF8 : wxFONTENCODING_DEFAULT);

	if (file.IsOpened())
		file.Read(str);

	if (!str.IsEmpty())
	{
		sqlQuery->SetText(str);
		sqlQuery->Colourise(0, str.Length());
		wxSafeYield();                            // needed to process sqlQuery modify event
		changed = false;
		setExtendedTitle();
		SetLineEndingStyle();
		UpdateRecentFiles(true);
		if(mainForm != NULL)
		{
			mainForm->UpdateAllRecentFiles();
		}
	}
}


void frmQuery::UpdateAllRecentFiles()
{
	mainForm->UpdateAllRecentFiles();
}

void frmQuery::OnNew(wxCommandEvent &event)
{
	frmQuery *fq = new frmQuery(mainForm, wxEmptyString, conn->Duplicate(), wxEmptyString);
	if (mainForm)
		mainForm->AddFrame(fq);
	fq->Go();
}


void frmQuery::OnOpen(wxCommandEvent &event)
{
	if (CheckChanged(true))
		return;

#ifdef __WXMSW__
	wxFileDialog dlg(this, _("Open query file"), lastDir, wxT(""),
	                 _("Query files (*.sql)|*.sql|pgScript files (*.pgs)|*.pgs|All files (*.*)|*.*"), wxFD_OPEN);
#else
	wxFileDialog dlg(this, _("Open query file"), lastDir, wxT(""),
	                 _("Query files (*.sql)|*.sql|pgScript files (*.pgs)|*.pgs|All files (*)|*"), wxFD_OPEN);
#endif

	if (dlg.ShowModal() == wxID_OK)
	{
		lastFilename = dlg.GetFilename();
		lastDir = dlg.GetDirectory();
		lastPath = dlg.GetPath();
		OpenLastFile();
	}
}


void frmQuery::OnSave(wxCommandEvent &event)
{
	bool modeUnicode = settings->GetUnicodeFile();

	if (lastPath.IsNull())
	{
		OnSaveAs(event);
		return;
	}

	wxUtfFile file(lastPath, wxFile::write, modeUnicode ? wxFONTENCODING_UTF8 : wxFONTENCODING_DEFAULT);
	if (file.IsOpened())
	{
		if ((file.Write(sqlQuery->GetText()) == 0) && (!modeUnicode))
			wxMessageBox(_("Query text incomplete.\nQuery contained characters that could not be converted to the local charset.\nPlease correct the data or try using UTF8 instead."));
		file.Close();
		changed = false;
		setExtendedTitle();
		UpdateRecentFiles();
	}
	else
	{
		wxLogError(__("Could not write the file %s: Errcode=%d."), lastPath.c_str(), wxSysErrorCode());
	}
}


// Set the line ending style based on the current document.
void frmQuery::SetLineEndingStyle()
{
	// Detect the file mode
	wxRegEx *reLF = new wxRegEx(wxT("[^\r]\n"), wxRE_NEWLINE);
	wxRegEx *reCRLF = new wxRegEx(wxT("\r\n"), wxRE_NEWLINE);
	wxRegEx *reCR = new wxRegEx(wxT("\r[^\n]"), wxRE_NEWLINE);

	bool haveLF = reLF->Matches(sqlQuery->GetText());
	bool haveCRLF = reCRLF->Matches(sqlQuery->GetText());
	bool haveCR = reCR->Matches(sqlQuery->GetText());
	int mode = GetLineEndingStyle();

	if ((haveLF && haveCR) ||
	        (haveLF && haveCRLF) ||
	        (haveCR && haveCRLF))
	{
		wxMessageBox(_("This file contains mixed line endings. They will be converted to the current setting."), _("Warning"), wxICON_INFORMATION);
		sqlQuery->ConvertEOLs(mode);
		changed = true;
		setExtendedTitle();
		updateMenu();
	}
	else
	{
		if (haveLF)
			mode = wxSTC_EOL_LF;
		else if (haveCRLF)
			mode = wxSTC_EOL_CRLF;
		else if (haveCR)
			mode = wxSTC_EOL_CR;
	}

	// Now set the status text, menu options, and the mode
	sqlQuery->SetEOLMode(mode);
	switch(mode)
	{

		case wxSTC_EOL_LF:
			lineEndMenu->Check(MNU_LF, true);
			SetStatusText(_("Unix"), STATUSPOS_FORMAT);
			break;

		case wxSTC_EOL_CRLF:
			lineEndMenu->Check(MNU_CRLF, true);
			SetStatusText(_("DOS"), STATUSPOS_FORMAT);
			break;

		case wxSTC_EOL_CR:
			lineEndMenu->Check(MNU_CR, true);
			SetStatusText(_("Mac"), STATUSPOS_FORMAT);
			break;

		default:
			wxLogError(wxT("Someone created a new line ending style! Run, run for your lives!!"));
	}

	delete reCRLF;
	delete reCR;
	delete reLF;
}


// Get the line ending style
int frmQuery::GetLineEndingStyle()
{
	if (lineEndMenu->IsChecked(MNU_LF))
		return wxSTC_EOL_LF;
	else if (lineEndMenu->IsChecked(MNU_CRLF))
		return wxSTC_EOL_CRLF;
	else if (lineEndMenu->IsChecked(MNU_CR))
		return wxSTC_EOL_CR;
	else
		return sqlQuery->GetEOLMode();
}


// User-set the current EOL mode for the form
void frmQuery::OnSetEOLMode(wxCommandEvent &event)
{
	int mode = GetLineEndingStyle();
	sqlQuery->ConvertEOLs(mode);
	sqlQuery->SetEOLMode(mode);
	settings->SetLineEndingType(mode);

	SetEOLModeDisplay(mode);

	if (!changed)
	{
		changed = true;
		setExtendedTitle();
	}

	pgScript->SetConnection(conn);
}


// Display the EOL mode settings on the form
void frmQuery::SetEOLModeDisplay(int mode)
{
	switch(mode)
	{

		case wxSTC_EOL_LF:
			lineEndMenu->Check(MNU_LF, true);
			SetStatusText(_("Unix"), STATUSPOS_FORMAT);
			break;

		case wxSTC_EOL_CRLF:
			lineEndMenu->Check(MNU_CRLF, true);
			SetStatusText(_("DOS"), STATUSPOS_FORMAT);
			break;

		case wxSTC_EOL_CR:
			lineEndMenu->Check(MNU_CR, true);
			SetStatusText(_("Mac"), STATUSPOS_FORMAT);
			break;

		default:
			wxLogError(wxT("Someone created a new line ending style! Run, run for your lives!!"));
	}
}


void frmQuery::OnSaveAs(wxCommandEvent &event)
{
#ifdef __WXMSW__
	wxFileDialog *dlg = new wxFileDialog(this, _("Save query file as"), lastDir, lastFilename,
	                                     _("Query files (*.sql)|*.sql|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
#else
	wxFileDialog *dlg = new wxFileDialog(this, _("Save query file as"), lastDir, lastFilename,
	                                     _("Query files (*.sql)|*.sql|All files (*)|*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
#endif
	if (dlg->ShowModal() == wxID_OK)
	{
		lastFilename = dlg->GetFilename();
		lastDir = dlg->GetDirectory();
		lastPath = dlg->GetPath();
		switch (dlg->GetFilterIndex())
		{
			case 0:
#ifdef __WXMAC__
				if (!lastPath.Contains(wxT(".")))
					lastPath += wxT(".sql");
#endif
				break;
			case 1:
#ifdef __WXMAC__
				if (!lastPath.Contains(wxT(".")))
					lastPath += wxT(".sql");
#endif
				break;
			default:
				break;
		}

		lastFileFormat = settings->GetUnicodeFile();

		wxUtfFile file(lastPath, wxFile::write, lastFileFormat ? wxFONTENCODING_UTF8 : wxFONTENCODING_DEFAULT);
		if (file.IsOpened())
		{
			if ((file.Write(sqlQuery->GetText()) == 0) && (!lastFileFormat))
				wxMessageBox(_("Query text incomplete.\nQuery contained characters that could not be converted to the local charset.\nPlease correct the data or try using UTF8 instead."));
			file.Close();
			changed = false;
			setExtendedTitle();
			UpdateRecentFiles();
			fileMenu->Enable(MNU_RECENT, (recentFileMenu->GetMenuItemCount() > 0));
		}
		else
		{
			wxLogError(__("Could not write the file %s: Errcode=%d."), lastPath.c_str(), wxSysErrorCode());
		}
	}
	delete dlg;
}


void frmQuery::OnQuickReport(wxCommandEvent &event)
{
	wxDateTime now = wxDateTime::Now();

	frmReport *rep = new frmReport(this);

	rep->XmlAddHeaderValue(wxT("generated"), now.Format(wxT("%c")));
	rep->XmlAddHeaderValue(wxT("database"), conn->GetName());

	rep->SetReportTitle(_("Quick report"));

	int section = rep->XmlCreateSection(_("Query results"));

	rep->XmlAddSectionTableFromGrid(section, sqlResult);

	wxString stats;
	stats.Printf(wxT("%d rows with %d columns retrieved."), sqlResult->NumRows(), sqlResult->GetNumberCols());

	rep->XmlSetSectionTableInfo(section, stats);

	wxString query = sqlQuery->GetSelectedText();
	if (query.IsNull())
		query = sqlQuery->GetText();

	rep->XmlSetSectionSql(section, query);

	rep->ShowModal();
}


void frmQuery::OnCancel(wxCommandEvent &event)
{
	toolBar->EnableTool(MNU_CANCEL, false);
	queryMenu->Enable(MNU_CANCEL, false);
	SetStatusText(_("Cancelling."), STATUSPOS_MSGS);

	if (sqlResult->RunStatus() == CTLSQL_RUNNING)
		sqlResult->Abort();
	else if (pgScript->IsRunning())
		pgScript->Terminate();

	QueryExecInfo *qi = (QueryExecInfo *)event.GetClientData();
	if (qi)
		delete qi;

	aborted = true;
}


void frmQuery::OnExplain(wxCommandEvent &event)
{
	if(sqlNotebook->GetSelection() == 1)
	{
		if (!updateFromGqb(true))
			return;
	}

	wxString query = sqlQuery->GetSelectedText();
	if (query.IsNull())
		query = sqlQuery->GetText();

	if (query.IsNull())
		return;
	wxString sql;
	int resultToRetrieve = 1;
	bool verbose = queryMenu->IsChecked(MNU_VERBOSE);
	bool analyze = event.GetId() == MNU_EXPLAINANALYZE;

	if (analyze)
	{
		sql += wxT("\nBEGIN;\n");
		resultToRetrieve++;
	}
	sql += wxT("EXPLAIN ");
	if (conn->BackendMinimumVersion(9, 0))
	{
		bool costs = queryMenu->IsChecked(MNU_COSTS);
		bool buffers = queryMenu->IsChecked(MNU_BUFFERS);

		sql += wxT("(");
		if (analyze)
			sql += wxT("ANALYZE on, ");
		else
			sql += wxT("ANALYZE off, ");
		if (verbose)
			sql += wxT("VERBOSE on, ");
		else
			sql += wxT("VERBOSE off, ");
		if (costs)
			sql += wxT("COSTS on, ");
		else
			sql += wxT("COSTS off, ");
		if (buffers)
			sql += wxT("BUFFERS on ");
		else
			sql += wxT("BUFFERS off ");
		sql += wxT(")");
	}
	else
	{
		if (analyze)
			sql += wxT("ANALYZE ");
		if (verbose)
			sql += wxT("VERBOSE ");
	}

	int offset = sql.Length();

	sql += query;

	if (analyze)
	{
		// Bizarre bug fix - if we append a rollback directly after -- it'll crash!!
		// Add a \n first.
		sql += wxT("\n;\nROLLBACK;");
	}

	execQuery(sql, resultToRetrieve, true, offset, false, true, verbose);
}

void frmQuery::OnBuffers(wxCommandEvent &event)
{
	queryMenu->Enable(MNU_EXPLAIN, !queryMenu->IsChecked(MNU_BUFFERS));
	toolBar->EnableTool(MNU_EXPLAIN, !queryMenu->IsChecked(MNU_BUFFERS));

	settings->SetExplainBuffers(queryMenu->IsChecked(MNU_BUFFERS));
}

// Update the main SQL query from the GQB if desired
bool frmQuery::updateFromGqb(bool executing)
{
	if (closing)
		return false;

	// Make sure this doesn't get call recursively through an event
	if (gqbUpdateRunning)
		return false;
	updateMenu();

	gqbUpdateRunning = true;

	// Execute Generation of SQL sentence from GQB
	bool canGenerate = false;
	wxString newQuery = controller->generateSQL();

	// If the new query is empty, don't do anything
	if (newQuery.IsEmpty())
	{
		if (controller->getTableCount() > 0)
		{
			wxMessageBox(_("No SQL query was generated."), _("Graphical Query Builder"), wxICON_INFORMATION);
		}
		gqbUpdateRunning = false;
		return false;
	}

	// Only prompt the user if the dirty flag is set, and the textbox is not empty, and the query has changed.
	if(changed && !sqlQuery->GetText().Trim().IsEmpty() && sqlQuery->GetText() != newQuery + wxT("\n"))
	{
		wxString fn;
		if (executing)
			fn = _("The generated SQL query has changed.\nDo you want to update it and execute the query?");
		else
			fn = _("The generated SQL query has changed.\nDo you want to update it?");

		wxMessageDialog msg(this, fn, _("Query"), wxYES_NO | wxICON_EXCLAMATION);
		if(msg.ShowModal() == wxID_YES && changed)
		{
			canGenerate = true;
		}
		else
		{
			gqbUpdateRunning = false;
		}
	}
	else
	{
		canGenerate = true;
	}

	if(canGenerate)
	{
		sqlQuery->ClearAll();
		sqlQuery->AddText(newQuery + wxT("\n"));
		sqlQuery->Colourise(0, sqlQuery->GetText().Length());
		sqlNotebook->SetSelection(0);
		changed = true;

		gqbUpdateRunning = false;
		return true;
	}

	return false;
}

void frmQuery::OnExecute(wxCommandEvent &event)
{
	if(sqlNotebook->GetSelection() == 1)
	{
		if (!updateFromGqb(true))
			return;
	}

	wxString query = sqlQuery->GetSelectedText();
	if (query.IsNull())
		query = sqlQuery->GetText();

	if (query.IsNull())
		return;

	execQuery(query);
	sqlQuery->SetFocus();
}


void frmQuery::OnExecScript(wxCommandEvent &event)
{
	// Get the script
	wxString query = sqlQuery->GetSelectedText();
	if (query.IsNull())
		query = sqlQuery->GetText();
	if (query.IsNull())
		return;

	// Make sure pgScript is not already running
	// Required because the pgScript parser isn't currently thread-safe :-(
	if (frmQuery::ms_pgScriptRunning == true)
	{
		wxMessageBox(_("pgScript already running."), _("Concurrent execution of pgScripts is not supported at this time."), wxICON_WARNING);
		return;
	}
	frmQuery::ms_pgScriptRunning = true;

	// Clear markers and indicators
	sqlQuery->MarkerDeleteAll(0);
	sqlQuery->StartStyling(0, wxSTC_INDICS_MASK);
	sqlQuery->SetStyling(sqlQuery->GetText().Length(), 0);

	// Menu stuff to initialize
	setTools(true);
	queryMenu->Enable(MNU_SAVEHISTORY, true);
	queryMenu->Enable(MNU_CLEARHISTORY, true);

	// Window stuff
	explainCanvas->Clear();
	msgResult->Clear();
	outputPane->SetSelection(2);

	// Status text
	SetStatusText(wxT(""), STATUSPOS_SECS);
	SetStatusText(_("pgScript is running."), STATUSPOS_MSGS);
	SetStatusText(wxT(""), STATUSPOS_ROWS);

	// History
	msgHistory->AppendText(_("-- Executing pgScript\n"));
	Update();
	wxTheApp->Yield(true);

	// Timer
	startTimeQuery = wxGetLocalTimeMillis();
	timer.Start(10);

	// Delete previous variables
	pgScript->ClearSymbols();

	// Parse script. Note that we add \n so the parse can correctly identify
	// a comment on the last line of the query.
	pgScript->ParseString(query + wxT("\n"), pgsOutput);
	pgsTimer->Start(20);
	aborted = false;
}



void frmQuery::OnExecFile(wxCommandEvent &event)
{
	if(sqlNotebook->GetSelection() == 1)
	{
		if (!updateFromGqb(true))
			return;
	}

	wxString query = sqlQuery->GetSelectedText();
	if (query.IsNull())
		query = sqlQuery->GetText();

	if (query.IsNull())
		return;

	execQuery(query, 0, false, 0, true);
	sqlQuery->SetFocus();
}


void frmQuery::OnMacroManage(wxCommandEvent &event)
{
	int r = dlgManageMacros(this, mainForm, macros).ManageMacros();
	if (r == 1)
	{
		// Changed something, so save
		queryMacroFileProvider::SaveMacros(macros);
	}
	if (r == -1 || r == 1)
	{
		// Changed something requiring rollback
		mainForm->UpdateAllMacrosList();
	}

}


void frmQuery::OnMacroInvoke(wxCommandEvent &event)
{
	queryMacroItem *mac;

	mac = macros->FindMacro(event.GetId());
	if (!mac)
		return;

	wxString query = mac->GetQuery();
	if (query.IsEmpty())
		return;            // do not execute empty query

	if (query.Find(wxT("$SELECTION$")) != wxNOT_FOUND)
	{
		wxString selection = sqlQuery->GetSelectedText();
		if (selection.IsEmpty())
		{
			wxMessageBox(_("This macro includes a text substitution. Please select some text in the SQL pane and re-run the macro."), _("Execute macro"), wxICON_EXCLAMATION);
			return;
		}
		query.Replace(wxT("$SELECTION$"), selection);
	}
	execQuery(query);
	sqlQuery->SetFocus();
}


void frmQuery::setTools(const bool running)
{
	toolBar->EnableTool(MNU_EXECUTE, !running);
	toolBar->EnableTool(MNU_EXECPGS, !running);
	toolBar->EnableTool(MNU_EXECFILE, !running);
	toolBar->EnableTool(MNU_EXPLAIN, (!running && !settings->GetExplainBuffers()));
	toolBar->EnableTool(MNU_CANCEL, running);
	queryMenu->Enable(MNU_EXECUTE, !running);
	queryMenu->Enable(MNU_EXECPGS, !running);
	queryMenu->Enable(MNU_EXECFILE, !running);
	queryMenu->Enable(MNU_EXPLAIN, (!running && !settings->GetExplainBuffers()));
	queryMenu->Enable(MNU_EXPLAINANALYZE, !running);
	queryMenu->Enable(MNU_CANCEL, running);
	fileMenu->Enable(MNU_EXPORT, sqlResult->CanExport());
	fileMenu->Enable(MNU_QUICKREPORT, sqlResult->CanExport());
	fileMenu->Enable(MNU_RECENT, (recentFileMenu->GetMenuItemCount() > 0));
	sqlQuery->EnableAutoComp(running);
}


void frmQuery::showMessage(const wxString &msg, const wxString &msgShort)
{
	msgResult->AppendText(msg + wxT("\n"));
	msgHistory->AppendText(msg + wxT("\n"));
	wxString str;
	if (msgShort.IsNull())
		str = msg;
	else
		str = msgShort;
	str.Replace(wxT("\n"), wxT(" "));
	SetStatusText(str, STATUSPOS_MSGS);
}


void frmQuery::execQuery(const wxString &query, int resultToRetrieve, bool singleResult, const int queryOffset, bool toFile, bool explain, bool verbose)
{
	setTools(true);
	queryMenu->Enable(MNU_SAVEHISTORY, true);
	queryMenu->Enable(MNU_CLEARHISTORY, true);

	explainCanvas->Clear();

	// Clear markers and indicators
	sqlQuery->MarkerDeleteAll(0);
	sqlQuery->StartStyling(0, wxSTC_INDICS_MASK);
	sqlQuery->SetStyling(sqlQuery->GetText().Length(), 0);

	if (!changed)
		setExtendedTitle();

	aborted = false;

	QueryExecInfo *qi = new QueryExecInfo();
	qi->queryOffset = queryOffset;
	qi->toFileExportForm = NULL;
	qi->singleResult = singleResult;
	qi->explain = explain;
	qi->verbose = verbose;

	if (toFile)
	{
		qi->toFileExportForm = new frmExport(this);
		if (qi->toFileExportForm->ShowModal() != wxID_OK)
		{
			delete qi;
			setTools(false);
			aborted = true;
			return;
		}
	}

	// We must do this lot before the query starts, otherwise
	// it might not happen once the main thread gets busy with
	// other stuff.
	SetStatusText(wxT(""), STATUSPOS_SECS);
	SetStatusText(_("Query is running."), STATUSPOS_MSGS);
	SetStatusText(wxT(""), STATUSPOS_ROWS);
	msgResult->Clear();

	msgHistory->AppendText(_("-- Executing query:\n"));
	msgHistory->AppendText(query);
	msgHistory->AppendText(wxT("\n"));
	Update();
	wxTheApp->Yield(true);

	startTimeQuery = wxGetLocalTimeMillis();
	timer.Start(10);

	if (sqlResult->Execute(query, resultToRetrieve, this, QUERY_COMPLETE, qi) >= 0)
	{
		// Return and wait for the result
		return;
	}

	completeQuery(false, false, false);
}


// When the query completes, it raises an event which we process here.
void frmQuery::OnQueryComplete(wxCommandEvent &ev)
{
	QueryExecInfo *qi = (QueryExecInfo *)ev.GetClientData();

	bool done = false;

	while (sqlResult->RunStatus() == CTLSQL_RUNNING)
	{
		wxTheApp->Yield(true);
	}

	while (pgScript->IsRunning())
	{
		wxLogInfo(wxT("SQL Query box: Waiting for script to abort"));
		wxSleep(1);
	}

	timer.Stop();

	wxString str;
	str = sqlResult->GetMessagesAndClear();
	msgResult->AppendText(str);
	msgHistory->AppendText(str);

	elapsedQuery = wxGetLocalTimeMillis() - startTimeQuery;
	SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);

	if (sqlResult->RunStatus() != PGRES_TUPLES_OK)
	{
		outputPane->SetSelection(2);
		if (sqlResult->RunStatus() == PGRES_COMMAND_OK)
		{
			done = true;

			int insertedCount = sqlResult->InsertedCount();
			OID insertedOid = sqlResult->InsertedOid();
			if (insertedCount < 0)
			{
				showMessage(wxString::Format(_("Query returned successfully with no result in %s ms."),
				                             elapsedQuery.ToString().c_str()), _("OK."));
			}
			else if (insertedCount == 1 && insertedOid)
			{
				showMessage(wxString::Format(_("Query returned successfully: one row with OID %d inserted, %s ms execution time."),
				                             insertedOid, elapsedQuery.ToString().c_str()),
				            wxString::Format(_("One Row with OID %d inserted."), insertedOid));
			}
			else
			{
				showMessage(wxString::Format(
				                wxPLURAL(
				                    "Query returned successfully: %d row affected, %s ms execution time.",
				                    "Query returned successfully: %d rows affected, %s ms execution time.",
				                    insertedCount),
				                insertedCount, elapsedQuery.ToString().c_str()),
				            wxString::Format(wxPLURAL("%d row affected.", "%d rows affected.", insertedCount), insertedCount));
			}
		}
		else
		{
			pgError err = sqlResult->GetResultError();
			wxString errMsg = err.formatted_msg;
			wxLogQuietError(wxT("%s"), conn->GetLastError().Trim().c_str());

			long errPos;
			err.statement_pos.ToLong(&errPos);

			showMessage(wxString::Format(wxT("********** %s **********\n"), _("Error")));
			showMessage(errMsg);

			if (errPos > 0)
			{
				int selStart = sqlQuery->GetSelectionStart(), selEnd = sqlQuery->GetSelectionEnd();
				if (selStart == selEnd)
					selStart = 0;

				errPos -= qi->queryOffset;        // do not count EXPLAIN or similar

				// Set an indicator on the error word (break on any kind of bracket, a space or full stop)
				int sPos = errPos + selStart - 1, wEnd = 1;
				sqlQuery->StartStyling(sPos, wxSTC_INDICS_MASK);
				while(sqlQuery->GetCharAt(sPos + wEnd) != ' ' &&
				        sqlQuery->GetCharAt(sPos + wEnd) != '(' &&
				        sqlQuery->GetCharAt(sPos + wEnd) != '{' &&
				        sqlQuery->GetCharAt(sPos + wEnd) != '[' &&
				        sqlQuery->GetCharAt(sPos + wEnd) != '.' &&
				        (unsigned int)(sPos + wEnd) < sqlQuery->GetText().Length())
					wEnd++;
				sqlQuery->SetStyling(wEnd, wxSTC_INDIC0_MASK);

				int line = 0, maxLine = sqlQuery->GetLineCount();
				while (line < maxLine && sqlQuery->GetLineEndPosition(line) < errPos + selStart + 1)
					line++;
				if (line < maxLine)
				{
					sqlQuery->GotoPos(sPos);
					sqlQuery->MarkerAdd(line, 0);

					if (!changed)
						setExtendedTitle();

					sqlQuery->EnsureVisible(line);
				}
			}
		}
	}
	else
	{
		done = true;
		outputPane->SetSelection(0);
		long rowsTotal = sqlResult->NumRows();

		if (qi->toFileExportForm)
		{
			SetStatusText(wxString::Format(wxPLURAL("%d row.", "%d rows.", rowsTotal), rowsTotal), STATUSPOS_ROWS);

			if (rowsTotal)
			{
				SetStatusText(_("Writing data."), STATUSPOS_MSGS);

				toolBar->EnableTool(MNU_CANCEL, false);
				queryMenu->Enable(MNU_CANCEL, false);
				SetCursor(*wxHOURGLASS_CURSOR);

				if (sqlResult->ToFile(qi->toFileExportForm))
					SetStatusText(_("Data written to file."), STATUSPOS_MSGS);
				else
					SetStatusText(_("Data export aborted."), STATUSPOS_MSGS);
				SetCursor(wxNullCursor);
			}
			else
				SetStatusText(_("No data to export."), STATUSPOS_MSGS);
		}
		else
		{
			if (qi->singleResult)
			{
				sqlResult->DisplayData(true);

				showMessage(wxString::Format(
				                wxPLURAL("%d row retrieved.", "%d rows retrieved.", sqlResult->NumRows()),
				                sqlResult->NumRows()), _("OK."));
			}
			else
			{
				SetStatusText(wxString::Format(wxPLURAL("Retrieving data: %d row.", "Retrieving data: %d rows.", rowsTotal), rowsTotal), STATUSPOS_MSGS);
				wxTheApp->Yield(true);

				sqlResult->DisplayData();

				SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);

				str = _("Total query runtime: ") + elapsedQuery.ToString() + wxT(" ms.\n") ;
				msgResult->AppendText(str);
				msgHistory->AppendText(str);

				showMessage(wxString::Format(wxPLURAL("%ld row retrieved.", "%ld rows retrieved.", sqlResult->NumRows()), sqlResult->NumRows()), _("OK."));
			}
			SetStatusText(wxString::Format(wxPLURAL("%d row.", "%d rows.", rowsTotal), rowsTotal), STATUSPOS_ROWS);
		}
	}

	if (sqlResult->RunStatus() == PGRES_TUPLES_OK || sqlResult->RunStatus() == PGRES_COMMAND_OK)
	{
		if (sqlQuery->GetText().Len() < (unsigned int)settings->GetHistoryMaxQuerySize())
		{
			wxString tmp = sqlQuery->GetText();
			tmp.Replace(wxT("\n"), wxT(" "));
			tmp.Replace(wxT("\r"), wxT(" "));
			sqlQueries->Append(tmp);
			histoQueries.Add(sqlQuery->GetText());
			SaveQueries();
		}

		// Delete an old query if it matches the current one
		unsigned int index = histoQueries.Index(sqlQueries->GetString(sqlQueries->GetCount() - 1), false);
		if (index != (unsigned int)wxNOT_FOUND && index < ((unsigned int)sqlQueries->GetCount() - 1))
		{
			histoQueries.RemoveAt(index);
			sqlQueries->Delete(index);
		}
	}

	// Make sure only the maximum query number is enforced
	while (sqlQueries->GetCount() > (unsigned int)settings->GetHistoryMaxQueries())
	{
		histoQueries.RemoveAt(0);
		sqlQueries->Delete(0);
	}

	SaveQueries();

	completeQuery(done, qi->explain, qi->verbose);
	delete qi;
}


void frmQuery::OnScriptComplete(wxCommandEvent &ev)
{
	// Stop timers
	timer.Stop();
	pgsTimer->Stop();

	// Write output
	writeScriptOutput();

	// Reset tools
	setTools(false);

	// Unlock our pseudo-mutex thingy
	frmQuery::ms_pgScriptRunning = false;

	// Manage timer
	elapsedQuery = wxGetLocalTimeMillis() - startTimeQuery;
	SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);
	SetStatusText(_("pgScript completed."), STATUSPOS_MSGS);
	wxString str = _("Total pgScript runtime: ") + elapsedQuery.ToString() + wxT(" ms.\n\n");
	msgHistory->AppendText(str);

	// Check whether there was an error/exception
	if (pgScript->errorOccurred() && pgScript->errorLine() >= 1)
	{
		// Find out what the line number is
		int selStart = sqlQuery->GetSelectionStart(), selEnd = sqlQuery->GetSelectionEnd();
		if (selStart == selEnd)
			selStart = 0;
		int line = 0, maxLine = sqlQuery->GetLineCount();
		while (line < maxLine && sqlQuery->GetLineEndPosition(line) < selStart)
			line++;
		line += pgScript->errorLine() - 1;

		// Mark the line where the error occurred
		sqlQuery->MarkerAdd(line, 0);

		// Go to that line
		sqlQuery->GotoPos(sqlQuery->GetLineEndPosition(line));
	}
}

void frmQuery::writeScriptOutput()
{
	pgScript->LockOutput();

	wxString output(pgsOutputString);
	pgsOutputString.Clear();
	msgResult->AppendText(output);

	pgScript->UnlockOutput();
}

// Complete the processing of a query
void frmQuery::completeQuery(bool done, bool explain, bool verbose)
{
	// Display async notifications
	pgNotification *notify;
	int notifies = 0;
	notify = conn->GetNotification();
	while (notify)
	{
		wxString notifyStr;
		notifies++;

		if (notify->data.IsEmpty())
			notifyStr.Printf(_("\nAsynchronous notification of '%s' received from backend pid %d"), notify->name.c_str(), notify->pid);
		else
			notifyStr.Printf(_("\nAsynchronous notification of '%s' received from backend pid %d\n   Data: %s"), notify->name.c_str(), notify->pid, notify->data.c_str());

		msgResult->AppendText(notifyStr);
		msgHistory->AppendText(notifyStr);

		notify = conn->GetNotification();
	}

	if (notifies)
	{
		wxString statusMsg = statusBar->GetStatusText(STATUSPOS_MSGS);
		if (statusMsg.Last() == '.')
			statusMsg = statusMsg.Left(statusMsg.Length() - 1);

		SetStatusText(wxString::Format(
		                  wxPLURAL("%s (%d asynchronous notification received).", "%s (%d asynchronous notifications received).", notifies),
		                  statusMsg.c_str(), notifies), STATUSPOS_MSGS);
	}

	msgResult->AppendText(wxT("\n"));
	msgResult->ShowPosition(0);
	msgHistory->AppendText(wxT("\n"));
	msgHistory->ShowPosition(0);

	// If the transaction aborted for some reason, issue a rollback to cleanup.
	if (settings->GetAutoRollback() && conn->GetTxStatus() == PGCONN_TXSTATUS_INERROR)
		conn->ExecuteVoid(wxT("ROLLBACK;"));

	setTools(false);
	fileMenu->Enable(MNU_EXPORT, sqlResult->CanExport());

	if (!IsActive())
		RequestUserAttention();

	if (!viewMenu->IsChecked(MNU_OUTPUTPANE))
	{
		viewMenu->Check(MNU_OUTPUTPANE, true);
		manager.GetPane(wxT("outputPane")).Show(true);
		manager.Update();
	}

	// If this was an EXPLAIN query, process the results
	if (done && explain)
	{
		if (!verbose || conn->BackendMinimumVersion(8, 4))
		{
			int i;
			wxString str;
			if (sqlResult->NumRows() == 1)
			{
				// Avoid shared storage issues with strings
				str.Append(sqlResult->OnGetItemText(0, 0).c_str());
			}
			else
			{
				for (i = 0 ; i < sqlResult->NumRows() ; i++)
				{
					if (i)
						str.Append(wxT("\n"));
					str.Append(sqlResult->OnGetItemText(i, 0));
				}
			}
			explainCanvas->SetExplainString(str);
			outputPane->SetSelection(1);
		}
		updateMenu();
	}

	sqlQuery->SetFocus();
}


void frmQuery::OnTimer(wxTimerEvent &event)
{
	elapsedQuery = wxGetLocalTimeMillis() - startTimeQuery;
	SetStatusText(elapsedQuery.ToString() + wxT(" ms"), STATUSPOS_SECS);

	wxString str = sqlResult->GetMessagesAndClear();
	if (!str.IsEmpty())
	{
		msgResult->AppendText(str + wxT("\n"));
		msgHistory->AppendText(str + wxT("\n"));
	}

	// Increase the granularity for longer running queries
	if (elapsedQuery > 200 && timer.GetInterval() == 10 && timer.IsRunning())
	{
		timer.Stop();
		timer.Start(100);
	}
}

// Adjust sizes of GQB components, Located here because need to
// avoid some issues when implementing inside controller/view Classes
void frmQuery::adjustGQBSizes()
{
	// Get Size (only height) from main Tab with GQB and SQL Editor and adjust the width
	// to desiree, then set [Sash of tablesBrowser | GQB_Canvas]
	manager.Update();
	sqlNotebook->Refresh();
	wxSize s = sqlNotebook->GetSize();
	s.SetWidth(200);
	s.SetHeight(s.GetHeight() - 180);      //re-adjust weight eliminating Horz Sash Position
	controller->getTablesBrowser()->SetSize(s);
	controller->setSashVertPosition(controller->getTablesBrowser()->GetSize().GetWidth());

	// Now Adjust Sash Horizontal
	s = sqlNotebook->GetSize();
	controller->setSashHorizPosition(s.GetHeight() - 150);

	// Adjust GQB grids internal columns sizes
	controller->calcGridColsSizes();
}


// Adjust sizes of GQB components after vertical sash adjustment,
// Located here because need to avoid some issues when implementing
// inside controller/view Classes
void frmQuery::OnResizeHorizontally(wxSplitterEvent &event)
{
	int y = event.GetSashPosition();
	wxSize s = controller->getTablesBrowser()->GetSize();
	s.SetHeight(y);               // re-adjust weight eliminating Horz Sash Position
	controller->getTablesBrowser()->SetSize(s);
}



// This function adjust the GQB Components after an event on the wxAui
// event, it's a workaround because need event finish to work properly
void frmQuery::OnAdjustSizesTimer(wxTimerEvent &event)
{
	adjustGQBSizes();
	adjustSizesTimer->Stop();
}

void frmQuery::OnBlockIndent(wxCommandEvent &event)
{
	if (FindFocus()->GetId() == CTL_SQLQUERY)
		sqlQuery->CmdKeyExecute(wxSTC_CMD_TAB);
	else if (FindFocus()->GetId() == CTL_SCRATCHPAD)
		scratchPad->WriteText(wxT("\t"));
}

void frmQuery::OnBlockOutDent(wxCommandEvent &event)
{
	if (FindFocus()->GetId() == CTL_SQLQUERY)
		sqlQuery->CmdKeyExecute(wxSTC_CMD_BACKTAB);
}

void frmQuery::OnChangeToUpperCase(wxCommandEvent &event)
{
	if (FindFocus()->GetId() == CTL_SQLQUERY)
		sqlQuery->UpperCase();
}

void frmQuery::OnChangeToLowerCase(wxCommandEvent &event)
{
	if (FindFocus()->GetId() == CTL_SQLQUERY)
		sqlQuery->LowerCase();
}

void frmQuery::OnCommentText(wxCommandEvent &event)
{
	if (FindFocus()->GetId() == CTL_SQLQUERY)
		sqlQuery->BlockComment(false);
}

void frmQuery::OnUncommentText(wxCommandEvent &event)
{
	if (FindFocus()->GetId() == CTL_SQLQUERY)
		sqlQuery->BlockComment(true);
}

wxBitmap frmQuery::CreateBitmap(const wxColour &colour)
{
	const int w = 10, h = 10;

	wxMemoryDC dc;
	wxBitmap bmp(w, h);
	dc.SelectObject(bmp);
	if (colour == wxNullColour)
		dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
	else
		dc.SetBrush(wxBrush(colour));
	dc.DrawRectangle(0, 0, w, h);

	return bmp;
}

wxColour frmQuery::GetServerColour(pgConn *connection)
{
	wxColour tmp = wxNullColour;
	if (mainForm != NULL)
	{
		ctlTree *browser = mainForm->GetBrowser();
		wxTreeItemIdValue foldercookie, servercookie;
		wxTreeItemId folderitem, serveritem;
		pgObject *object;
		pgServer *server;

		folderitem = browser->GetFirstChild(browser->GetRootItem(), foldercookie);
		while (folderitem)
		{
			if (browser->ItemHasChildren(folderitem))
			{
				serveritem = browser->GetFirstChild(folderitem, servercookie);
				while (serveritem)
				{
					object = browser->GetObject(serveritem);
					if (object->IsCreatedBy(serverFactory))
					{
						server = (pgServer *)object;
						if (server->GetConnected() &&
						        server->GetConnection()->GetHost() == connection->GetHost() &&
						        server->GetConnection()->GetPort() == connection->GetPort())
						{
							tmp = wxColour(server->GetColour());
						}
					}
					serveritem = browser->GetNextChild(folderitem, servercookie);
				}
			}
			folderitem = browser->GetNextChild(browser->GetRootItem(), foldercookie);
		}
	}
	return tmp;
}

void frmQuery::LoadQueries()
{
	xmlTextReaderPtr reader;

	if (!wxFile::Access(settings->GetHistoryFile(), wxFile::read))
		return;

	reader = xmlReaderForFile((const char *)settings->GetHistoryFile().mb_str(wxConvUTF8), NULL, 0);
	if (!reader)
	{
		wxMessageBox(_("Failed to load the history file!"));
		return;
	}

	while (xmlTextReaderRead(reader))
	{
		wxString nodename = WXSTRING_FROM_XML(xmlTextReaderConstName(reader));

		if (nodename == wxT("histoquery"))
		{
			xmlChar *cont = xmlTextReaderReadString(reader);

			if (!cont)
				continue;

			if (WXSTRING_FROM_XML(cont) != wxT(""))
			{
				wxString query = WXSTRING_FROM_XML(cont);
				wxString tmp = query;
				tmp.Replace(wxT("\n"), wxT(" "));
				tmp.Replace(wxT("\r"), wxT(" "));
				sqlQueries->Append(tmp);
				histoQueries.Add(query);
			}

			xmlFree(cont);
		}
	}

	xmlTextReaderClose(reader);
	xmlFreeTextReader(reader);
	xmlCleanupParser();

	// Make sure only the maximum query number is enforced
	if (sqlQueries->GetCount() > (unsigned int)settings->GetHistoryMaxQueries())
	{
		while (sqlQueries->GetCount() > (unsigned int)settings->GetHistoryMaxQueries())
		{
			histoQueries.RemoveAt(0);
			sqlQueries->Delete(0);
		}
		SaveQueries();
	}

	return;
}


void frmQuery::SaveQueries()
{
	size_t i;
	xmlTextWriterPtr writer;

	writer = xmlNewTextWriterFilename((const char *)settings->GetHistoryFile().mb_str(wxConvUTF8), 0);
	if (!writer)
	{
		wxMessageBox(_("Failed to write to history file!"));
		return;
	}
	xmlTextWriterSetIndent(writer, 1);

	if ((xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL) < 0) ||
	        (xmlTextWriterStartElement(writer, XML_STR("histoqueries")) < 0))
	{
		wxMessageBox(_("Failed to write to history file!"));
		xmlFreeTextWriter(writer);
		return;
	}

	for (i = 0; i < histoQueries.GetCount(); i++)
	{
		xmlTextWriterStartElement(writer, XML_STR("histoquery"));
		xmlTextWriterWriteString(writer, XML_FROM_WXSTRING(histoQueries.Item(i)));
		xmlTextWriterEndElement(writer);
	}

	if (xmlTextWriterEndDocument(writer) < 0)
	{
		wxMessageBox(_("Failed to write to history file!"));
	}

	xmlFreeTextWriter(writer);
}


void frmQuery::OnChangeQuery(wxCommandEvent &event)
{
	wxString query = histoQueries.Item(sqlQueries->GetSelection());
	if (query.Length() > 0)
	{
		sqlQuery->SetText(query);
		sqlQuery->Colourise(0, query.Length());
		wxSafeYield();                            // needed to process sqlQuery modify event
		changed = false;
		setExtendedTitle();
		SetLineEndingStyle();
		btnDeleteCurrent->Enable(true);
	}
	btnDeleteAll->Enable(sqlQueries->GetCount() > 0);
}


void frmQuery::OnDeleteCurrent(wxCommandEvent &event)
{

	if ( wxMessageDialog(this,
	                     _("Delete current query from history?"),
	                     _("Confirm deletion"),
	                     wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION).ShowModal() == wxID_YES )
	{
		histoQueries.RemoveAt(sqlQueries->GetSelection());
		sqlQueries->Delete(sqlQueries->GetSelection());
		sqlQueries->SetValue(wxT(""));
		btnDeleteCurrent->Enable(false);
		btnDeleteAll->Enable(sqlQueries->GetCount() > 0);
		SaveQueries();
	}
}


void frmQuery::OnDeleteAll(wxCommandEvent &event)
{

	if ( wxMessageDialog(this,
	                     _("Delete all queries from history?"),
	                     _("Confirm deletion"),
	                     wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION).ShowModal() == wxID_YES )
	{
		histoQueries.Clear();
		sqlQueries->Clear();
		sqlQueries->SetValue(wxT(""));
		btnDeleteCurrent->Enable(false);
		btnDeleteAll->Enable(false);
		SaveQueries();
	}
}


///////////////////////////////////////////////////////

wxWindow *queryToolBaseFactory::StartDialogSql(frmMain *form, pgObject *obj, const wxString &sql)
{
	pgDatabase *db = obj->GetDatabase();
	wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Query Tool");
	pgConn *conn = db->CreateConn(applicationname);
	if (conn)
	{
		frmQuery *fq = new frmQuery(form, wxEmptyString, conn, sql);
		fq->Go();
		return fq;
	}
	return 0;
}


bool queryToolBaseFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->GetDatabase() && obj->GetDatabase()->GetConnected();
}


bool queryToolDataFactory::CheckEnable(pgObject *obj)
{
	return queryToolBaseFactory::CheckEnable(obj) && !obj->IsCollection() &&
	       (obj->IsCreatedBy(tableFactory) || obj->IsCreatedBy(viewFactory));
}


queryToolFactory::queryToolFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : queryToolBaseFactory(list)
{
	mnu->Append(id, _("&Query tool\tCtrl-E"), _("Execute arbitrary SQL queries."));
	toolbar->AddTool(id, _("Query tool\tCtrl-E"), wxBitmap(sql_32_xpm), _("Execute arbitrary SQL queries."), wxITEM_NORMAL);
}


wxWindow *queryToolFactory::StartDialog(frmMain *form, pgObject *obj)
{
	wxString qry;
	if (settings->GetStickySql())
		qry = obj->GetSql(form->GetBrowser());
	return StartDialogSql(form, obj, qry);
}


queryToolSqlFactory::queryToolSqlFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : queryToolBaseFactory(list)
{
	mnu->Append(id, _("CREATE script"), _("Start Query tool with CREATE script."));
	if (toolbar)
		toolbar->AddTool(id, _("CREATE script"), wxBitmap(sql_32_xpm), _("Start query tool with CREATE script."), wxITEM_NORMAL);
}


wxWindow *queryToolSqlFactory::StartDialog(frmMain *form, pgObject *obj)
{
	return StartDialogSql(form, obj, obj->GetSql(form->GetBrowser()));
}


bool queryToolSqlFactory::CheckEnable(pgObject *obj)
{
	return queryToolBaseFactory::CheckEnable(obj) && obj->CanCreate() && !obj->IsCollection();
}


queryToolSelectFactory::queryToolSelectFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : queryToolDataFactory(list)
{
	mnu->Append(id, _("SELECT script"), _("Start query tool with SELECT script."));
}

bool queryToolSelectFactory::CheckEnable(pgObject *obj)
{
	return queryToolBaseFactory::CheckEnable(obj) && !obj->IsCollection() &&
	       (obj->IsCreatedBy(tableFactory) || obj->IsCreatedBy(viewFactory) || obj->IsCreatedBy(functionFactory));
}

wxWindow *queryToolSelectFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (obj->IsCreatedBy(tableFactory))
	{
		pgTable *table = (pgTable *)obj;
		return StartDialogSql(form, obj, table->GetSelectSql(form->GetBrowser()));
	}
	else if (obj->IsCreatedBy(viewFactory))
	{
		pgView *view = (pgView *)obj;
		return StartDialogSql(form, obj, view->GetSelectSql(form->GetBrowser()));
	}
	else if (obj->IsCreatedBy(extTableFactory))
	{
		gpExtTable *exttable = (gpExtTable *)obj;
		return StartDialogSql(form, obj, exttable->GetSelectSql(form->GetBrowser()));
	}
	else if (obj->IsCreatedBy(functionFactory))
	{
		pgFunction *function = (pgFunction *)obj;
		return StartDialogSql(form, obj, function->GetSelectSql(form->GetBrowser()));
	}
	return 0;
}

queryToolExecFactory::queryToolExecFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : queryToolDataFactory(list)
{
	mnu->Append(id, _("EXEC script"), _("Start query tool with EXEC script."));
}

bool queryToolExecFactory::CheckEnable(pgObject *obj)
{
	return queryToolBaseFactory::CheckEnable(obj) && !obj->IsCollection() && obj->IsCreatedBy(procedureFactory);
}

wxWindow *queryToolExecFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (obj->IsCreatedBy(procedureFactory))
	{
		pgProcedure *procedure = (pgProcedure *)obj;
		return StartDialogSql(form, obj, procedure->GetExecSql(form->GetBrowser()));
	}
	return 0;
}

queryToolDeleteFactory::queryToolDeleteFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : queryToolDataFactory(list)
{
	mnu->Append(id, _("DELETE script"), _("Start query tool with DELETE script."));
}


bool queryToolDeleteFactory::CheckEnable(pgObject *obj)
{
	if (!queryToolDataFactory::CheckEnable(obj))
		return false;
	if (obj->IsCreatedBy(tableFactory))
		return true;
	return false;
}


wxWindow *queryToolDeleteFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (obj->IsCreatedBy(tableFactory))
	{
		pgTable *table = (pgTable *)obj;
		return StartDialogSql(form, obj, table->GetDeleteSql(form->GetBrowser()));
	}
	return 0;
}


queryToolUpdateFactory::queryToolUpdateFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : queryToolDataFactory(list)
{
	mnu->Append(id, _("UPDATE script"), _("Start query tool with UPDATE script."));
}


wxWindow *queryToolUpdateFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (obj->IsCreatedBy(tableFactory))
	{
		pgTable *table = (pgTable *)obj;
		return StartDialogSql(form, obj, table->GetUpdateSql(form->GetBrowser()));
	}
	else if (obj->IsCreatedBy(viewFactory))
	{
		pgView *view = (pgView *)obj;
		return StartDialogSql(form, obj, view->GetUpdateSql(form->GetBrowser()));
	}

	return 0;
}


bool queryToolUpdateFactory::CheckEnable(pgObject *obj)
{
	if (!queryToolDataFactory::CheckEnable(obj))
		return false;
	if (obj->IsCreatedBy(tableFactory))
		return true;
	pgView *view = (pgView *)obj;

	return view->HasUpdateRule();
}


queryToolInsertFactory::queryToolInsertFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : queryToolDataFactory(list)
{
	mnu->Append(id, _("INSERT script"), _("Start query tool with INSERT script."));
}


wxWindow *queryToolInsertFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (obj->IsCreatedBy(tableFactory))
	{
		pgTable *table = (pgTable *)obj;
		return StartDialogSql(form, obj, table->GetInsertSql(form->GetBrowser()));
	}
	else if (obj->IsCreatedBy(viewFactory))
	{
		pgView *view = (pgView *)obj;
		return StartDialogSql(form, obj, view->GetInsertSql(form->GetBrowser()));
	}
	return 0;
}

bool queryToolInsertFactory::CheckEnable(pgObject *obj)
{
	if (!queryToolDataFactory::CheckEnable(obj))
		return false;
	if (obj->IsCreatedBy(tableFactory))
		return true;
	pgView *view = (pgView *)obj;

	return view->HasInsertRule();
}

void frmQuery::SaveExplainAsImage(wxCommandEvent &ev)
{
	wxFileDialog *dlg = new wxFileDialog(this, _("Save Explain As image file"), lastDir, lastFilename,
	                                     wxT("Bitmap files (*.bmp)|*.bmp|JPEG files (*.jpeg)|*.jpeg|PNG files (*.png)|*.png"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg->ShowModal() == wxID_OK)
	{
		lastFilename = dlg->GetFilename();
		lastDir = dlg->GetDirectory();
		lastPath = dlg->GetPath();
		int index = dlg->GetFilterIndex();

		wxString     strType;
		wxBitmapType imgType;
		switch (index)
		{
				// bmp
			case 0:
				strType = wxT(".bmp");
				imgType = wxBITMAP_TYPE_BMP;
				break;
				// jpeg
			case 1:
				strType = wxT(".jpeg");
				imgType = wxBITMAP_TYPE_JPEG;
				break;
				// default (png)
			default:
				// png
			case 2:
				strType = wxT(".png");
				imgType = wxBITMAP_TYPE_PNG;
				break;
		}

		if (!lastPath.Contains(wxT(".")))
			lastPath += strType;

		if (ev.GetId() == MNU_SAVEAS_IMAGE_GQB)
			controller->getView()->SaveAsImage(lastPath, imgType);
		else if (ev.GetId() == MNU_SAVEAS_IMAGE_EXPLAIN)
			explainCanvas->SaveAsImage(lastPath, imgType);
	}
}

///////////////////////////////////////////////////////

pgScriptTimer::pgScriptTimer(frmQuery *parent) :
	m_parent(parent)
{

}

void pgScriptTimer::Notify()
{
	// Write script output
	m_parent->writeScriptOutput();
}

