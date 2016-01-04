//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmStatus.cpp - Status Screen
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/image.h>
#include <wx/textbuf.h>
#include <wx/clipbrd.h>
#include <wx/sysopt.h>

// wxAUI
#include <wx/aui/aui.h>

// App headers
#include "frm/frmAbout.h"
#include "frm/frmStatus.h"
#include "frm/frmHint.h"
#include "frm/frmMain.h"
#include "db/pgConn.h"
#include "frm/frmQuery.h"
#include "utils/pgfeatures.h"
#include "schema/pgServer.h"
#include "schema/pgUser.h"
#include "ctl/ctlMenuToolbar.h"
#include "ctl/ctlAuiNotebook.h"
#include "utils/csvfiles.h"

// Icons
#include "images/clip_copy.pngc"
#include "images/readdata.pngc"
#include "images/query_cancel.pngc"
#include "images/terminate_backend.pngc"
#include "images/delete.pngc"
#include "images/storedata.pngc"
#include "images/down.pngc"
#include "images/up.pngc"


#include "db/pgConn.h"


#define CTRLID_DATABASE         4200


BEGIN_EVENT_TABLE(frmStatus, pgFrame)
	EVT_MENU(MNU_EXIT,                            frmStatus::OnExit)

	EVT_MENU(MNU_COPY,                            frmStatus::OnCopy)
	EVT_MENU(MNU_COPY_QUERY,                      frmStatus::OnCopyQuery)

	EVT_MENU(MNU_HELP,                            frmStatus::OnHelp)
	EVT_MENU(MNU_CONTENTS,                        frmStatus::OnContents)
	EVT_MENU(MNU_STATUSPAGE,                      frmStatus::OnToggleStatusPane)
	EVT_MENU(MNU_LOCKPAGE,                        frmStatus::OnToggleLockPane)
	EVT_MENU(MNU_XACTPAGE,                        frmStatus::OnToggleXactPane)
	EVT_MENU(MNU_LOGPAGE,                         frmStatus::OnToggleLogPane)
	EVT_MENU(MNU_TOOLBAR,                         frmStatus::OnToggleToolBar)
	EVT_MENU(MNU_DEFAULTVIEW,                     frmStatus::OnDefaultView)
	EVT_MENU(MNU_HIGHLIGHTSTATUS,                 frmStatus::OnHighlightStatus)

	EVT_AUI_PANE_CLOSE(                           frmStatus::OnPaneClose)

	EVT_COMBOBOX(CTL_RATECBO,                     frmStatus::OnRateChange)
	EVT_MENU(MNU_REFRESH,                         frmStatus::OnRefresh)
	EVT_MENU(MNU_CANCEL,                          frmStatus::OnCancelBtn)
	EVT_MENU(MNU_TERMINATE,                       frmStatus::OnTerminateBtn)
	EVT_MENU(MNU_COMMIT,                          frmStatus::OnCommit)
	EVT_MENU(MNU_ROLLBACK,                        frmStatus::OnRollback)
	EVT_COMBOBOX(CTL_LOGCBO,                      frmStatus::OnLoadLogfile)
	EVT_BUTTON(CTL_ROTATEBTN,                     frmStatus::OnRotateLogfile)

	EVT_TIMER(TIMER_REFRESHUI_ID,                 frmStatus::OnRefreshUITimer)

	EVT_TIMER(TIMER_STATUS_ID,                    frmStatus::OnRefreshStatusTimer)
	EVT_LIST_ITEM_SELECTED(CTL_STATUSLIST,        frmStatus::OnSelStatusItem)
	EVT_LIST_ITEM_DESELECTED(CTL_STATUSLIST,      frmStatus::OnSelStatusItem)
	EVT_LIST_COL_CLICK(CTL_STATUSLIST,            frmStatus::OnSortStatusGrid)
	EVT_LIST_COL_RIGHT_CLICK(CTL_STATUSLIST,      frmStatus::OnRightClickStatusGrid)
	EVT_LIST_COL_END_DRAG(CTL_STATUSLIST,         frmStatus::OnChgColSizeStatusGrid)

	EVT_TIMER(TIMER_LOCKS_ID,                     frmStatus::OnRefreshLocksTimer)
	EVT_LIST_ITEM_SELECTED(CTL_LOCKLIST,          frmStatus::OnSelLockItem)
	EVT_LIST_ITEM_DESELECTED(CTL_LOCKLIST,        frmStatus::OnSelLockItem)
	EVT_LIST_COL_CLICK(CTL_LOCKLIST,              frmStatus::OnSortLockGrid)
	EVT_LIST_COL_RIGHT_CLICK(CTL_LOCKLIST,        frmStatus::OnRightClickLockGrid)
	EVT_LIST_COL_END_DRAG(CTL_LOCKLIST,           frmStatus::OnChgColSizeLockGrid)

	EVT_TIMER(TIMER_XACT_ID,                      frmStatus::OnRefreshXactTimer)
	EVT_LIST_ITEM_SELECTED(CTL_XACTLIST,          frmStatus::OnSelXactItem)
	EVT_LIST_ITEM_DESELECTED(CTL_XACTLIST,        frmStatus::OnSelXactItem)
	EVT_LIST_COL_CLICK(CTL_XACTLIST,              frmStatus::OnSortXactGrid)
	EVT_LIST_COL_RIGHT_CLICK(CTL_XACTLIST,        frmStatus::OnRightClickXactGrid)
	EVT_LIST_COL_END_DRAG(CTL_XACTLIST,           frmStatus::OnChgColSizeXactGrid)

	EVT_TIMER(TIMER_LOG_ID,                       frmStatus::OnRefreshLogTimer)
	EVT_LIST_ITEM_SELECTED(CTL_LOGLIST,           frmStatus::OnSelLogItem)
	EVT_LIST_ITEM_DESELECTED(CTL_LOGLIST,         frmStatus::OnSelLogItem)

	EVT_COMBOBOX(CTRLID_DATABASE,                 frmStatus::OnChangeDatabase)

	EVT_CLOSE(                                    frmStatus::OnClose)
END_EVENT_TABLE();


int frmStatus::cboToRate()
{
	int rate = 0;

	if (cbRate->GetValue() == _("Don't refresh"))
		rate = 0;
	if (cbRate->GetValue() == _("1 second"))
		rate = 1;
	if (cbRate->GetValue() == _("5 seconds"))
		rate = 5;
	if (cbRate->GetValue() == _("10 seconds"))
		rate = 10;
	if (cbRate->GetValue() == _("30 seconds"))
		rate = 30;
	if (cbRate->GetValue() == _("1 minute"))
		rate = 60;
	if (cbRate->GetValue() == _("5 minutes"))
		rate = 300;
	if (cbRate->GetValue() == _("10 minutes"))
		rate = 600;
	if (cbRate->GetValue() == _("30 minutes"))
		rate = 1800;
	if (cbRate->GetValue() == _("1 hour"))
		rate = 3600;

	return rate;
}


wxString frmStatus::rateToCboString(int rate)
{
	wxString rateStr;

	if (rate == 0)
		rateStr = _("Don't refresh");
	if (rate == 1)
		rateStr = _("1 second");
	if (rate == 5)
		rateStr = _("5 seconds");
	if (rate == 10)
		rateStr = _("10 seconds");
	if (rate == 30)
		rateStr = _("30 seconds");
	if (rate == 60)
		rateStr = _("1 minute");
	if (rate == 300)
		rateStr = _("5 minutes");
	if (rate == 600)
		rateStr = _("10 minutes");
	if (rate == 1800)
		rateStr = _("30 minutes");
	if (rate == 3600)
		rateStr = _("1 hour");

	return rateStr;
}


frmStatus::frmStatus(frmMain *form, const wxString &_title, pgConn *conn) : pgFrame(NULL, _title)
{
	wxString initquery;
	bool highlight = false;

	dlgName = wxT("frmStatus");

	loaded = false;

	mainForm = form;
	connection = conn;
	locks_connection = conn;

	statusTimer = 0;
	locksTimer = 0;
	xactTimer = 0;
	logTimer = 0;

	logHasTimestamp = false;
	logFormatKnown = false;

	// Only superusers can set these parameters...
	pgUser *user = new pgUser(connection->GetUser());
	if (user)
	{
		if (user->GetSuperuser())
		{
			// Make the connection quiet on the logs
			if (connection->BackendMinimumVersion(8, 0))
				initquery = wxT("SET log_statement='none';SET log_duration='off';SET log_min_duration_statement=-1;");
			else
				initquery = wxT("SET log_statement='off';SET log_duration='off';SET log_min_duration_statement=-1;");
			connection->ExecuteVoid(initquery, false);
		}
		delete user;
	}

	// Notify wxAUI which frame to use
	manager.SetManagedWindow(this);
	manager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE);

	// Set different window's attributes
	SetTitle(_title);
	appearanceFactory->SetIcons(this);
	RestorePosition(-1, -1, 700, 500, 700, 500);
	SetMinSize(wxSize(700, 500));
	SetFont(settings->GetSystemFont());

	// Build menu bar
	menuBar = new wxMenuBar();

	fileMenu = new wxMenu();
	fileMenu->Append(MNU_EXIT, _("E&xit\tCtrl-W"), _("Exit query window"));

	menuBar->Append(fileMenu, _("&File"));

	editMenu = new wxMenu();
	editMenu->Append(MNU_COPY, _("&Copy\tCtrl-C"), _("Copy selected text to clipboard"), wxITEM_NORMAL);

	menuBar->Append(editMenu, _("&Edit"));

	actionMenu = new wxMenu();
	actionMenu->Append(MNU_REFRESH, _("Refresh\tCtrl-R"), _("Refresh the selected panel"), wxITEM_NORMAL);
	actionMenu->AppendSeparator();
	actionMenu->Append(MNU_COPY_QUERY, _("Copy to query tool\tCtrl-Shift-C"), _("Open the query tool with the selected query"), wxITEM_NORMAL);
	actionMenu->Append(MNU_CANCEL, _("Cancel query\tDel"), _("Cancel the selected query"), wxITEM_NORMAL);
	actionMenu->Append(MNU_TERMINATE, _("Terminate backend\tShift-Del"), _("Terminate the selected backend"), wxITEM_NORMAL);
	actionMenu->AppendSeparator();
	actionMenu->Append(MNU_COMMIT, _("Commit prepared transaction"), _("Commit the selected prepared transaction"), wxITEM_NORMAL);
	actionMenu->Append(MNU_ROLLBACK, _("Rollback prepared transaction"), _("Rollback the selected prepared transaction"), wxITEM_NORMAL);

	menuBar->Append(actionMenu, _("&Action"));

	viewMenu = new wxMenu();
	viewMenu->Append(MNU_STATUSPAGE, _("&Activity\tCtrl-Alt-A"), _("Show or hide the activity tab."), wxITEM_CHECK);
	viewMenu->Append(MNU_LOCKPAGE, _("&Locks\tCtrl-Alt-L"), _("Show or hide the locks tab."), wxITEM_CHECK);
	viewMenu->Append(MNU_XACTPAGE, _("Prepared &Transactions\tCtrl-Alt-T"), _("Show or hide the prepared transactions tab."), wxITEM_CHECK);
	viewMenu->Append(MNU_LOGPAGE, _("Log&file\tCtrl-Alt-F"), _("Show or hide the logfile tab."), wxITEM_CHECK);
	viewMenu->AppendSeparator();
	viewMenu->Append(MNU_TOOLBAR, _("Tool&bar\tCtrl-Alt-B"), _("Show or hide the toolbar."), wxITEM_CHECK);
	viewMenu->Append(MNU_HIGHLIGHTSTATUS, _("Highlight items of the activity list"), _("Highlight or not the items of the activity list."), wxITEM_CHECK);
	viewMenu->AppendSeparator();
	viewMenu->Append(MNU_DEFAULTVIEW, _("&Default view\tCtrl-Alt-V"), _("Restore the default view."));

	menuBar->Append(viewMenu, _("&View"));

	wxMenu *helpMenu = new wxMenu();
	helpMenu->Append(MNU_CONTENTS, _("&Help contents"), _("Open the helpfile."));
	helpMenu->Append(MNU_HELP, _("&Server status help"), _("Display help on this window."));

#ifdef __WXMAC__
	menuFactories = new menuFactoryList();
	aboutFactory *af = new aboutFactory(menuFactories, helpMenu, 0);
	wxApp::s_macAboutMenuItemId = af->GetId();
	menuFactories->RegisterMenu(this, wxCommandEventHandler(pgFrame::OnAction));
#endif

	menuBar->Append(helpMenu, _("&Help"));

	// Setup edit menu
	editMenu->Enable(MNU_COPY, false);

	// Finish menu bar
	SetMenuBar(menuBar);

	// Set statusBar
	statusBar = CreateStatusBar(1);
	SetStatusBarPane(-1);

	// Set up toolbar
	toolBar = new ctlMenuToolbar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	toolBar->SetToolBitmapSize(wxSize(16, 16));
	toolBar->AddTool(MNU_REFRESH, wxEmptyString, *readdata_png_bmp, _("Refresh"), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_COPY, wxEmptyString, *clip_copy_png_bmp, _("Copy selected text to clipboard"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_COPY_QUERY, wxEmptyString, *clip_copy_png_bmp, _("Open the query tool with the selected query"), wxITEM_NORMAL);
	toolBar->AddSeparator();
	toolBar->AddTool(MNU_CANCEL, wxEmptyString, *query_cancel_png_bmp, _("Cancel query"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_TERMINATE, wxEmptyString, *terminate_backend_png_bmp, _("Terminate backend"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_COMMIT, wxEmptyString, *storedata_png_bmp, _("Commit transaction"), wxITEM_NORMAL);
	toolBar->AddTool(MNU_ROLLBACK, wxEmptyString, *delete_png_bmp, _("Rollback transaction"), wxITEM_NORMAL);
	toolBar->AddSeparator();
	cbLogfiles = new wxComboBox(toolBar, CTL_LOGCBO, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL,
	                            wxCB_READONLY | wxCB_DROPDOWN);
	toolBar->AddControl(cbLogfiles);
	btnRotateLog = new wxButton(toolBar, CTL_ROTATEBTN, _("Rotate"));
	toolBar->AddControl(btnRotateLog);
	toolBar->AddSeparator();
	cbRate = new wxComboBox(toolBar, CTL_RATECBO, wxEmptyString, wxDefaultPosition, wxSize(-1, -1), wxArrayString(), wxCB_READONLY | wxCB_DROPDOWN);
	toolBar->AddControl(cbRate);
	toolBar->AddSeparator();
	cbDatabase = new ctlComboBoxFix(toolBar, CTRLID_DATABASE, wxDefaultPosition, wxSize(-1, -1), wxCB_READONLY | wxCB_DROPDOWN);
	toolBar->AddControl(cbDatabase);
	toolBar->Realize();

	// Append items to cbo
	cbRate->Append(_("Don't refresh"));
	cbRate->Append(_("1 second"));
	cbRate->Append(_("5 seconds"));
	cbRate->Append(_("10 seconds"));
	cbRate->Append(_("30 seconds"));
	cbRate->Append(_("1 minute"));
	cbRate->Append(_("5 minutes"));
	cbRate->Append(_("10 minutes"));
	cbRate->Append(_("30 minutes"));
	cbRate->Append(_("1 hour"));

	// Disable toolbar's items
	toolBar->EnableTool(MNU_CANCEL, false);
	toolBar->EnableTool(MNU_TERMINATE, false);
	toolBar->EnableTool(MNU_COMMIT, false);
	toolBar->EnableTool(MNU_ROLLBACK, false);
	actionMenu->Enable(MNU_CANCEL, false);
	actionMenu->Enable(MNU_TERMINATE, false);
	actionMenu->Enable(MNU_COMMIT, false);
	actionMenu->Enable(MNU_ROLLBACK, false);
	cbLogfiles->Enable(false);
	btnRotateLog->Enable(false);

	// Add the database combobox
	pgSet *dataSet1 = connection->ExecuteSet(wxT("SELECT datname FROM pg_database WHERE datallowconn ORDER BY datname"));
	while (!dataSet1->Eof())
	{
		cbDatabase->Append(dataSet1->GetVal(wxT("datname")));
		dataSet1->MoveNext();
	}
	delete dataSet1;

	// Image list for all listviews
	listimages = new wxImageList(13, 8, true, 2);
	listimages->Add(*down_png_ico);
	listimages->Add(*up_png_ico);

	// Create panel
	AddStatusPane();
	AddLockPane();
	AddXactPane();
	AddLogPane();
	manager.AddPane(toolBar, wxAuiPaneInfo().Name(wxT("toolBar")).Caption(_("Tool bar")).ToolbarPane().Top().LeftDockable(false).RightDockable(false));

	// Now load the layout
	wxString perspective;
	settings->Read(wxT("frmStatus/Perspective-") + wxString(FRMSTATUS_PERSPECTIVE_VER), &perspective, FRMSTATUS_DEFAULT_PERSPECTIVE);
	manager.LoadPerspective(perspective, true);

	// Reset the captions for the current language
	manager.GetPane(wxT("toolBar")).Caption(_("Tool bar"));
	manager.GetPane(wxT("Activity")).Caption(_("Activity"));
	manager.GetPane(wxT("Locks")).Caption(_("Locks"));
	manager.GetPane(wxT("Transactions")).Caption(_("Prepared Transactions"));
	manager.GetPane(wxT("Logfile")).Caption(_("Logfile"));

	// Tell the manager to "commit" all the changes just made
	manager.Update();

	// Sync the View menu options
	viewMenu->Check(MNU_STATUSPAGE, manager.GetPane(wxT("Activity")).IsShown());
	viewMenu->Check(MNU_LOCKPAGE, manager.GetPane(wxT("Locks")).IsShown());
	viewMenu->Check(MNU_XACTPAGE, manager.GetPane(wxT("Transactions")).IsShown());
	viewMenu->Check(MNU_LOGPAGE, manager.GetPane(wxT("Logfile")).IsShown());
	viewMenu->Check(MNU_TOOLBAR, manager.GetPane(wxT("toolBar")).IsShown());

	// Read the highlight status checkbox
	settings->Read(wxT("frmStatus/HighlightStatus"), &highlight, true);
	viewMenu->Check(MNU_HIGHLIGHTSTATUS, highlight);

	// Get our PID
	backend_pid = connection->GetBackendPID();

	// Create the refresh timer (quarter of a second)
	// This is a horrible hack to get around the lack of a
	// PANE_ACTIVATED event in wxAUI.
	refreshUITimer = new wxTimer(this, TIMER_REFRESHUI_ID);
	refreshUITimer->Start(250);

	// The selected pane is the log pane by default
	// so enable/disable the widgets according to this
	wxListEvent nullevent;
	OnSelLogItem(nullevent);

	// We're good now
	loaded = true;
}


frmStatus::~frmStatus()
{
	// Delete the refresh timer
	delete refreshUITimer;

	// If the status window wasn't launched in standalone mode...
	if (mainForm)
		mainForm->RemoveFrame(this);

	// Save the window's position
	settings->Write(wxT("frmStatus/Perspective-") + wxString(FRMSTATUS_PERSPECTIVE_VER), manager.SavePerspective());
	manager.UnInit();
	SavePosition();

	// Save the highlight status checkbox
	settings->WriteBool(wxT("frmStatus/HighlightStatus"), viewMenu->IsChecked(MNU_HIGHLIGHTSTATUS));

	// For each current page, save the slider's position and delete the timer
	settings->WriteInt(wxT("frmStatus/RefreshStatusRate"), statusRate);
	delete statusTimer;
	settings->WriteInt(wxT("frmStatus/RefreshLockRate"), locksRate);
	delete locksTimer;
	if (viewMenu->IsEnabled(MNU_XACTPAGE))
	{
		settings->WriteInt(wxT("frmStatus/RefreshXactRate"), xactRate);
		if (xactTimer)
		{
			delete xactTimer;
			xactTimer = NULL;
		}
	}
	if (viewMenu->IsEnabled(MNU_LOGPAGE))
	{
		settings->WriteInt(wxT("frmStatus/RefreshLogRate"), logRate);
		emptyLogfileCombo();
		if (logTimer)
		{
			delete logTimer;
			logTimer = NULL;
		}
	}

	// If connection is still available, delete it
	if (locks_connection && locks_connection != connection)
	{
		if (locks_connection->IsAlive())
			delete locks_connection;
	}
	if (connection)
	{
		if (connection->IsAlive())
			delete connection;
	}
}


void frmStatus::Go()
{
	// Show the window
	Show(true);

	// Send RateChange event to launch each timer
	wxScrollEvent nullScrollEvent;
	if (viewMenu->IsChecked(MNU_STATUSPAGE))
	{
		currentPane = PANE_STATUS;
		cbRate->SetValue(rateToCboString(statusRate));
		OnRateChange(nullScrollEvent);
	}
	if (viewMenu->IsChecked(MNU_LOCKPAGE))
	{
		currentPane = PANE_LOCKS;
		cbRate->SetValue(rateToCboString(locksRate));
		OnRateChange(nullScrollEvent);
	}
	if (viewMenu->IsEnabled(MNU_XACTPAGE) && viewMenu->IsChecked(MNU_XACTPAGE))
	{
		currentPane = PANE_XACT;
		cbRate->SetValue(rateToCboString(xactRate));
		OnRateChange(nullScrollEvent);
	}
	if (viewMenu->IsEnabled(MNU_LOGPAGE) && viewMenu->IsChecked(MNU_LOGPAGE))
	{
		currentPane = PANE_LOG;
		cbRate->SetValue(rateToCboString(logRate));
		OnRateChange(nullScrollEvent);
	}

	// Refresh all pages
	wxCommandEvent nullEvent;
	OnRefresh(nullEvent);
}


void frmStatus::OnClose(wxCloseEvent &event)
{
	Destroy();
}


void frmStatus::OnExit(wxCommandEvent &event)
{
	Destroy();
}


void frmStatus::OnChangeDatabase(wxCommandEvent &ev)
{
	wxString initquery;

	if (locks_connection != connection)
	{
		delete locks_connection;
	}

	locks_connection = new pgConn(connection->GetHostName(), connection->GetService(), connection->GetHostAddr(), cbDatabase->GetValue(),
	                              connection->GetUser(), connection->GetPassword(), connection->GetPort(), connection->GetRole(), connection->GetSslMode(),
	                              0, connection->GetApplicationName(), connection->GetSSLCert(), connection->GetSSLKey(), connection->GetSSLRootCert(), connection->GetSSLCrl(),
	                              connection->GetSSLCompression());

	pgUser *user = new pgUser(locks_connection->GetUser());
	if (user)
	{
		if (user->GetSuperuser())
		{
			if (locks_connection->BackendMinimumVersion(8, 0))
				initquery = wxT("SET log_statement='none';SET log_duration='off';SET log_min_duration_statement=-1;");
			else
				initquery = wxT("SET log_statement='off';SET log_duration='off';SET log_min_duration_statement=-1;");
			locks_connection->ExecuteVoid(initquery, false);
		}
		delete user;
	}
}


void frmStatus::AddStatusPane()
{
	// Create panel
	wxPanel *pnlActivity = new wxPanel(this);

	// Create flex grid
	wxFlexGridSizer *grdActivity = new wxFlexGridSizer(1, 1, 5, 5);
	grdActivity->AddGrowableCol(0);
	grdActivity->AddGrowableRow(0);

	// Add the list control
#ifdef __WXMAC__
	// Switch to the generic list control.
	// Disable sort on Mac.
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), true);
#endif
	wxListCtrl *lstStatus = new wxListCtrl(pnlActivity, CTL_STATUSLIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER);
	// Now switch back
#ifdef __WXMAC__
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), false);
#endif
	grdActivity->Add(lstStatus, 0, wxGROW, 3);

	// Add the panel to the notebook
	manager.AddPane(pnlActivity,
	                wxAuiPaneInfo().
	                Name(wxT("Activity")).Caption(_("Activity")).
	                CaptionVisible(true).CloseButton(true).MaximizeButton(true).
	                Dockable(true).Movable(true));

	// Auto-sizing
	pnlActivity->SetSizer(grdActivity);
	grdActivity->Fit(pnlActivity);

	// Add each column to the list control
	statusList = (ctlListView *)lstStatus;
	statusList->AddColumn(_("PID"), 35);
	if (connection->BackendMinimumVersion(8, 5))
		statusList->AddColumn(_("Application name"), 70);
	statusList->AddColumn(_("Database"), 70);
	statusList->AddColumn(_("User"), 70);
	if (connection->BackendMinimumVersion(8, 1))
	{
		statusList->AddColumn(_("Client"), 70);
		statusList->AddColumn(_("Client start"), 80);
	}
	if (connection->BackendMinimumVersion(7, 4))
		statusList->AddColumn(_("Query start"), 50);
	if (connection->BackendMinimumVersion(8, 3))
		statusList->AddColumn(_("TX start"), 50);
	if (connection->BackendMinimumVersion(9, 2))
	{
		statusList->AddColumn(_("State"), 35);
		statusList->AddColumn(_("State change"), 35);
	}
	if (connection->BackendMinimumVersion(9, 4))
	{
		statusList->AddColumn(_("Backend XID"), 35);
		statusList->AddColumn(_("Backend XMin"), 35);
	}
	statusList->AddColumn(_("Blocked by"), 35);
	statusList->AddColumn(_("Query"), 500);

	// Get through the list of columns to build the popup menu
	// and reinitialize column's width if we find a saved width
	statusPopupMenu = new wxMenu();
	wxListItem item;
	item.SetMask(wxLIST_MASK_TEXT);
	int savedwidth;
	for (int col = 0; col < statusList->GetColumnCount(); col++)
	{
		// Get column
		statusList->GetColumn(col, item);

		// Reinitialize column's width
		settings->Read(wxT("frmStatus/StatusPane_") + item.GetText() + wxT("_Width"), &savedwidth, item.GetWidth());
		if (savedwidth > 0)
			statusList->SetColumnWidth(col, savedwidth);
		else
			statusList->SetColumnWidth(col, 0);
		statusColWidth[col] = savedwidth;

		// Add new check item on the popup menu
		statusPopupMenu->AppendCheckItem(1000 + col, item.GetText());
		statusPopupMenu->Check(1000 + col, statusList->GetColumnWidth(col) > 0);
		this->Connect(1000 + col, wxEVT_COMMAND_MENU_SELECTED,
		              wxCommandEventHandler(frmStatus::OnStatusMenu));
	}

	// Build image list
	statusList->SetImageList(listimages, wxIMAGE_LIST_SMALL);

	// Read statusRate configuration
	settings->Read(wxT("frmStatus/RefreshStatusRate"), &statusRate, 10);

	// Initialize sort order
	statusSortColumn = 1;
	statusSortOrder = wxT("ASC");

	// Create the timer
	statusTimer = new wxTimer(this, TIMER_STATUS_ID);
}


void frmStatus::AddLockPane()
{
	// Create panel
	wxPanel *pnlLock = new wxPanel(this);

	// Create flex grid
	wxFlexGridSizer *grdLock = new wxFlexGridSizer(1, 1, 5, 5);
	grdLock->AddGrowableCol(0);
	grdLock->AddGrowableRow(0);

	// Add the list control
#ifdef __WXMAC__
	// Switch to the generic list control.
	// Disable sort on Mac.
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), true);
#endif
	wxListCtrl *lstLocks = new wxListCtrl(pnlLock, CTL_LOCKLIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER);
	// Now switch back
#ifdef __WXMAC__
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), false);
#endif
	grdLock->Add(lstLocks, 0, wxGROW, 3);

	// Add the panel to the notebook
	manager.AddPane(pnlLock,
	                wxAuiPaneInfo().
	                Name(wxT("Locks")).Caption(_("Locks")).
	                CaptionVisible(true).CloseButton(true).MaximizeButton(true).
	                Dockable(true).Movable(true));

	// Auto-sizing
	pnlLock->SetSizer(grdLock);
	grdLock->Fit(pnlLock);

	// Add each column to the list control
	lockList = (ctlListView *)lstLocks;
	lockList->AddColumn(wxT("PID"), 35);
	lockList->AddColumn(_("Database"), 50);
	lockList->AddColumn(_("Relation"), 50);
	lockList->AddColumn(_("User"), 50);
	if (locks_connection->BackendMinimumVersion(8, 3))
		lockList->AddColumn(_("XID"), 50);
	lockList->AddColumn(_("TX"), 50);
	lockList->AddColumn(_("Mode"), 50);
	lockList->AddColumn(_("Granted"), 50);
	if (locks_connection->BackendMinimumVersion(7, 4))
		lockList->AddColumn(_("Start"), 50);
	lockList->AddColumn(_("Query"), 500);

	// Get through the list of columns to build the popup menu
	lockPopupMenu = new wxMenu();
	wxListItem item;
	item.SetMask(wxLIST_MASK_TEXT);
	int savedwidth;
	for (int col = 0; col < lockList->GetColumnCount(); col++)
	{
		// Get column
		lockList->GetColumn(col, item);

		// Reinitialize column's width
		settings->Read(wxT("frmStatus/LockPane_") + item.GetText() + wxT("_Width"), &savedwidth, item.GetWidth());
		if (savedwidth > 0)
			lockList->SetColumnWidth(col, savedwidth);
		else
			lockList->SetColumnWidth(col, 0);
		lockColWidth[col] = savedwidth;

		// Add new check item on the popup menu
		lockPopupMenu->AppendCheckItem(2000 + col, item.GetText());
		lockPopupMenu->Check(2000 + col, lockList->GetColumnWidth(col) > 0);
		this->Connect(2000 + col, wxEVT_COMMAND_MENU_SELECTED,
		              wxCommandEventHandler(frmStatus::OnLockMenu));
	}

	// Build image list
	lockList->SetImageList(listimages, wxIMAGE_LIST_SMALL);

	// Read locksRate configuration
	settings->Read(wxT("frmStatus/RefreshLockRate"), &locksRate, 10);

	// Initialize sort order
	lockSortColumn = 1;
	lockSortOrder = wxT("ASC");

	// Create the timer
	locksTimer = new wxTimer(this, TIMER_LOCKS_ID);
}


void frmStatus::AddXactPane()
{
	// Create panel
	wxPanel *pnlXacts = new wxPanel(this);

	// Create flex grid
	wxFlexGridSizer *grdXacts = new wxFlexGridSizer(1, 1, 5, 5);
	grdXacts->AddGrowableCol(0);
	grdXacts->AddGrowableRow(0);

	// Add the list control
#ifdef __WXMAC__
	// Switch to the generic list control.
	// Disable sort on Mac.
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), true);
#endif
	wxListCtrl *lstXacts = new wxListCtrl(pnlXacts, CTL_XACTLIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER);
	// Now switch back
#ifdef __WXMAC__
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), false);
#endif
	grdXacts->Add(lstXacts, 0, wxGROW, 3);

	// Add the panel to the notebook
	manager.AddPane(pnlXacts,
	                wxAuiPaneInfo().
	                Name(wxT("Transactions")).Caption(_("Transactions")).
	                CaptionVisible(true).CloseButton(true).MaximizeButton(true).
	                Dockable(true).Movable(true));

	// Auto-sizing
	pnlXacts->SetSizer(grdXacts);
	grdXacts->Fit(pnlXacts);

	// Add the xact list
	xactList = (ctlListView *)lstXacts;

	// We don't need this report if server release is less than 8.1
	// GPDB doesn't have external global transactions.
	// Perhaps we should use this display to show our
	// global xid to local xid mappings?
	if (!connection->BackendMinimumVersion(8, 1) || connection->GetIsGreenplum())
	{
		// manager.GetPane(wxT("Transactions")).Show(false);
		lstXacts->InsertColumn(lstXacts->GetColumnCount(), _("Message"), wxLIST_FORMAT_LEFT, 800);
		lstXacts->InsertItem(lstXacts->GetItemCount(), _("Prepared transactions not available on this server."), -1);
		lstXacts->Enable(false);
		xactTimer = NULL;

		// We're done
		return;
	}

	// Add each column to the list control
	xactList->AddColumn(wxT("XID"), 50);
	xactList->AddColumn(_("Global ID"), 200);
	xactList->AddColumn(_("Time"), 100);
	xactList->AddColumn(_("Owner"), 50);
	xactList->AddColumn(_("Database"), 50);

	// Get through the list of columns to build the popup menu
	xactPopupMenu = new wxMenu();
	wxListItem item;
	item.SetMask(wxLIST_MASK_TEXT);
	int savedwidth;
	for (int col = 0; col < xactList->GetColumnCount(); col++)
	{
		// Get column
		xactList->GetColumn(col, item);

		// Reinitialize column's width
		settings->Read(wxT("frmStatus/XactPane_") + item.GetText() + wxT("_Width"), &savedwidth, item.GetWidth());
		if (savedwidth > 0)
			xactList->SetColumnWidth(col, savedwidth);
		else
			xactList->SetColumnWidth(col, 0);
		xactColWidth[col] = savedwidth;

		// Add new check item on the popup menu
		xactPopupMenu->AppendCheckItem(3000 + col, item.GetText());
		xactPopupMenu->Check(3000 + col, xactList->GetColumnWidth(col) > 0);
		this->Connect(3000 + col, wxEVT_COMMAND_MENU_SELECTED,
		              wxCommandEventHandler(frmStatus::OnXactMenu));
	}

	// Build image list
	xactList->SetImageList(listimages, wxIMAGE_LIST_SMALL);

	// Read xactRate configuration
	settings->Read(wxT("frmStatus/RefreshXactRate"), &xactRate, 10);

	// Initialize sort order
	xactSortColumn = 2;
	xactSortOrder = wxT("ASC");

	// Create the timer
	xactTimer = new wxTimer(this, TIMER_XACT_ID);
}


void frmStatus::AddLogPane()
{
	int rc = -1;
	wxString hint = HINT_INSTRUMENTATION;

	// Create panel
	wxPanel *pnlLog = new wxPanel(this);

	// Create flex grid
	wxFlexGridSizer *grdLog = new wxFlexGridSizer(1, 1, 5, 5);
	grdLog->AddGrowableCol(0);
	grdLog->AddGrowableRow(0);

	// Add the list control
#ifdef __WXMAC__
	// Switch to the generic list control.
	// Disable sort on Mac.
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), true);
#endif
	wxListCtrl *lstLog = new wxListCtrl(pnlLog, CTL_LOGLIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER);
	// Now switch back
#ifdef __WXMAC__
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), false);
#endif
	grdLog->Add(lstLog, 0, wxGROW, 3);

	// Add the panel to the notebook
	manager.AddPane(pnlLog,
	                wxAuiPaneInfo().Center().
	                Name(wxT("Logfile")).Caption(_("Logfile")).
	                CaptionVisible(true).CloseButton(true).MaximizeButton(true).
	                Dockable(true).Movable(true));

	// Auto-sizing
	pnlLog->SetSizer(grdLog);
	grdLog->Fit(pnlLog);

	// Add the log list
	logList = (ctlListView *)lstLog;

	// We don't need this report (but we need the pane)
	// if server release is less than 8.0 or if server has no adminpack
	if (!(connection->BackendMinimumVersion(8, 0) &&
	        connection->HasFeature(FEATURE_FILEREAD)))
	{
		// if the server release is 9.1 or more and the server has no adminpack
		if (connection->BackendMinimumVersion(9, 1))
		{
			// Search the adminpack extension
			pgSet *set = connection->ExecuteSet(wxT("SELECT 1 FROM pg_available_extensions WHERE name='adminpack'"));
			if (set->NumRows() == 1)
				hint = HINT_INSTRUMENTATION_91_WITH;
			else
				hint = HINT_INSTRUMENTATION_91_WITHOUT;
			delete set;
		}

		if (connection->BackendMinimumVersion(8, 0))
			rc = frmHint::ShowHint(this, hint);

		if (rc == HINT_RC_FIX)
			connection->ExecuteVoid(wxT("CREATE EXTENSION adminpack"), true);

		if (!connection->HasFeature(FEATURE_FILEREAD, true))
		{
			logList->InsertColumn(logList->GetColumnCount(), _("Message"), wxLIST_FORMAT_LEFT, 800);
			logList->InsertItem(logList->GetItemCount(), _("Logs are not available for this server."), -1);
			logList->Enable(false);
			logTimer = NULL;
			// We're done
			return;
		}
	}

	// Add each column to the list control
	logFormat = connection->ExecuteScalar(wxT("SHOW log_line_prefix"));
	if (logFormat == wxT("unset"))
		logFormat = wxEmptyString;
	logFmtPos = logFormat.Find('%', true);

	if (logFmtPos < 0)
		logFormatKnown = true;  // log_line_prefix not specified.
	else if (!logFmtPos && logFormat.Mid(logFmtPos, 2) == wxT("%t") && logFormat.Length() > 2)  // Timestamp at end of log_line_prefix?
	{
		logFormatKnown = true;
		logHasTimestamp = true;
	}
	else if (connection->GetIsGreenplum())
	{
		// Always %m|%u|%d|%p|%I|%X|:- (timestamp w/ millisec) for 3.2.x
		// Usually CSV formatted for 3.3
		logFormatKnown = true;
		logHasTimestamp = true;
	}


	if (connection->GetIsGreenplum() && connection->BackendMinimumVersion(8, 2, 13))
	{
		// Be ready for GPDB CSV format log file
		logList->AddColumn(_("Timestamp"), 120);  // Room for millisecs
		logList->AddColumn(_("Level"), 35);
		logList->AddColumn(_("Log entry"), 400);
		logList->AddColumn(_("Connection"), 45);
		logList->AddColumn(_("Cmd number"), 48);
		logList->AddColumn(_("Dbname"), 48);
		logList->AddColumn(_("Segment"), 45);
	}
	else    // Non-GPDB or non-CSV format log
	{
		if (logHasTimestamp)
			logList->AddColumn(_("Timestamp"), 100);

		if (logFormatKnown)
			logList->AddColumn(_("Level"), 35);

		logList->AddColumn(_("Log entry"), 800);
	}

	if (!connection->HasFeature(FEATURE_ROTATELOG))
		btnRotateLog->Disable();

	// Re-initialize variables
	logfileLength = 0;

	// Read logRate configuration
	settings->Read(wxT("frmStatus/RefreshLogRate"), &logRate, 10);

	// Create the timer
	logTimer = new wxTimer(this, TIMER_LOG_ID);
}


void frmStatus::OnCopy(wxCommandEvent &ev)
{
	ctlListView *list;
	int row, col;
	wxString text;

	switch(currentPane)
	{
		case PANE_STATUS:
			list = statusList;
			break;
		case PANE_LOCKS:
			list = lockList;
			break;
		case PANE_XACT:
			list = xactList;
			break;
		case PANE_LOG:
			list = logList;
			break;
		default:
			// This shouldn't happen.
			// If it does, it's no big deal, we just need to get out.
			return;
			break;
	}

	row = list->GetFirstSelected();

	while (row >= 0)
	{
		for (col = 0; col < list->GetColumnCount(); col++)
		{
			text.Append(list->GetText(row, col) + wxT("\t"));
		}
#ifdef __WXMSW__
		text.Append(wxT("\r\n"));
#else
		text.Append(wxT("\n"));
#endif
		row = list->GetNextSelected(row);
	}

	if (text.Length() > 0 && wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(text));
		wxTheClipboard->Close();
	}
}


void frmStatus::OnCopyQuery(wxCommandEvent &ev)
{
	ctlListView *list;
	int row, col;
	wxString text = wxT("");
	wxString dbname = wxT("");
	unsigned int maxlength;

	// Only the status list shows the query
	list = statusList;

	// Get the database
	row = list->GetFirstSelected();
	col = connection->BackendMinimumVersion(9, 0) ? 2 : 1;
	dbname.Append(list->GetText(row, col));

	// Get the actual query
	row = list->GetFirstSelected();
	text.Append(queries.Item(row));

	// Check if we have a query whose length is maximum
	maxlength = 1024;
	if (connection->BackendMinimumVersion(8, 4))
	{
		pgSet *set;
		set = connection->ExecuteSet(wxT("SELECT setting FROM pg_settings\n")
		                             wxT("  WHERE name='track_activity_query_size'"));
		if (set)
		{
			maxlength = set->GetLong(0);
			delete set;
		}
	}

	if (text.Length() == maxlength)
	{
		wxLogError(_("The query you copied is at the maximum length.\nIt may have been truncated."));
	}

	// If we have some real query, launch the query tool
	if (text.Length() > 0 && dbname.Length() > 0
	        && text.Trim() != wxT("<IDLE>") && text.Trim() != wxT("<IDLE in transaction>"))
	{
		pgConn *conn = new pgConn(connection->GetHostName(), connection->GetService(), connection->GetHostAddr(), dbname,
		                          connection->GetUser(), connection->GetPassword(),
		                          connection->GetPort(), connection->GetRole(), connection->GetSslMode(), connection->GetDbOid(),
		                          connection->GetApplicationName(),
		                          connection->GetSSLCert(), connection->GetSSLKey(), connection->GetSSLRootCert(), connection->GetSSLCrl(),
		                          connection->GetSSLCompression());
		if (conn)
		{
			frmQuery *fq = new frmQuery(mainForm, wxEmptyString, conn, text);
			fq->Go();
			mainForm->AddFrame(fq);
		}
	}
}


void frmStatus::OnPaneClose(wxAuiManagerEvent &evt)
{
	if (evt.pane->name == wxT("Activity"))
	{
		viewMenu->Check(MNU_STATUSPAGE, false);
		statusTimer->Stop();
	}
	if (evt.pane->name == wxT("Locks"))
	{
		viewMenu->Check(MNU_LOCKPAGE, false);
		locksTimer->Stop();
	}
	if (evt.pane->name == wxT("Transactions"))
	{
		viewMenu->Check(MNU_XACTPAGE, false);
		if (xactTimer)
			xactTimer->Stop();
	}
	if (evt.pane->name == wxT("Logfile"))
	{
		viewMenu->Check(MNU_LOGPAGE, false);
		if (logTimer)
			logTimer->Stop();
	}
}


void frmStatus::OnToggleStatusPane(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_STATUSPAGE))
	{
		manager.GetPane(wxT("Activity")).Show(true);
		cbRate->SetValue(rateToCboString(statusRate));
		if (statusRate > 0)
			statusTimer->Start(statusRate * 1000L);
	}
	else
	{
		manager.GetPane(wxT("Activity")).Show(false);
		statusTimer->Stop();
	}

	// Tell the manager to "commit" all the changes just made
	manager.Update();
}


void frmStatus::OnToggleLockPane(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_LOCKPAGE))
	{
		manager.GetPane(wxT("Locks")).Show(true);
		cbRate->SetValue(rateToCboString(locksRate));
		if (locksRate > 0)
			locksTimer->Start(locksRate * 1000L);
	}
	else
	{
		manager.GetPane(wxT("Locks")).Show(false);
		locksTimer->Stop();
	}

	// Tell the manager to "commit" all the changes just made
	manager.Update();
}


void frmStatus::OnToggleXactPane(wxCommandEvent &event)
{
	if (viewMenu->IsEnabled(MNU_XACTPAGE) && viewMenu->IsChecked(MNU_XACTPAGE))
	{
		manager.GetPane(wxT("Transactions")).Show(true);
		cbRate->SetValue(rateToCboString(xactRate));
		if (xactRate > 0 && xactTimer)
			xactTimer->Start(xactRate * 1000L);
	}
	else
	{
		manager.GetPane(wxT("Transactions")).Show(false);
		if (xactTimer)
			xactTimer->Stop();
	}

	// Tell the manager to "commit" all the changes just made
	manager.Update();
}


void frmStatus::OnToggleLogPane(wxCommandEvent &event)
{
	if (viewMenu->IsEnabled(MNU_LOGPAGE) && viewMenu->IsChecked(MNU_LOGPAGE))
	{
		manager.GetPane(wxT("Logfile")).Show(true);
		cbRate->SetValue(rateToCboString(logRate));
		if (logRate > 0 && logTimer)
			logTimer->Start(logRate * 1000L);
	}
	else
	{
		manager.GetPane(wxT("Logfile")).Show(false);
		if (logTimer)
			logTimer->Stop();
	}

	// Tell the manager to "commit" all the changes just made
	manager.Update();
}


void frmStatus::OnToggleToolBar(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_TOOLBAR))
	{
		manager.GetPane(wxT("toolBar")).Show(true);
	}
	else
	{
		manager.GetPane(wxT("toolBar")).Show(false);
	}

	// Tell the manager to "commit" all the changes just made
	manager.Update();
}


void frmStatus::OnDefaultView(wxCommandEvent &event)
{
	manager.LoadPerspective(FRMSTATUS_DEFAULT_PERSPECTIVE, true);

	// Reset the captions for the current language
	manager.GetPane(wxT("toolBar")).Caption(_("Tool bar"));
	manager.GetPane(wxT("Activity")).Caption(_("Activity"));
	manager.GetPane(wxT("Locks")).Caption(_("Locks"));
	manager.GetPane(wxT("Transactions")).Caption(_("Prepared Transactions"));
	manager.GetPane(wxT("Logfile")).Caption(_("Logfile"));

	// tell the manager to "commit" all the changes just made
	manager.Update();

	// Sync the View menu options
	viewMenu->Check(MNU_TOOLBAR, manager.GetPane(wxT("toolBar")).IsShown());
	viewMenu->Check(MNU_STATUSPAGE, manager.GetPane(wxT("Activity")).IsShown());
	viewMenu->Check(MNU_LOCKPAGE, manager.GetPane(wxT("Locks")).IsShown());
	viewMenu->Check(MNU_XACTPAGE, manager.GetPane(wxT("Transactions")).IsShown());
	viewMenu->Check(MNU_LOGPAGE, manager.GetPane(wxT("Logfile")).IsShown());
}


void frmStatus::OnHighlightStatus(wxCommandEvent &event)
{
	wxTimerEvent evt;

	OnRefreshStatusTimer(evt);
}


void frmStatus::OnHelp(wxCommandEvent &ev)
{
	DisplayHelp(wxT("status"), HELP_PGADMIN);
}


void frmStatus::OnContents(wxCommandEvent &ev)
{
	DisplayHelp(wxT("index"), HELP_PGADMIN);
}

void frmStatus::OnRateChange(wxCommandEvent &event)
{
	wxTimer *timer;
	int rate;

	switch(currentPane)
	{
		case PANE_STATUS:
			timer = statusTimer;
			rate = cboToRate();
			statusRate = rate;
			break;
		case PANE_LOCKS:
			timer = locksTimer;
			rate = cboToRate();
			locksRate = rate;
			break;
		case PANE_XACT:
			timer = xactTimer;
			rate = cboToRate();
			xactRate = rate;
			break;
		case PANE_LOG:
			timer = logTimer;
			rate = cboToRate();
			logRate = rate;
			break;
		default:
			// This shouldn't happen.
			// If it does, it's no big deal, we just need to get out.
			return;
			break;
	}

	if (timer)
	{
		timer->Stop();
		if (rate > 0)
			timer->Start(rate * 1000L);
	}
	OnRefresh(event);
}


void frmStatus::OnRefreshUITimer(wxTimerEvent &event)
{
	wxListEvent evt;

	refreshUITimer->Stop();

	for (unsigned int i = 0; i < manager.GetAllPanes().GetCount(); i++)
	{
		wxAuiPaneInfo &pane = manager.GetAllPanes()[i];

		if (pane.HasFlag(wxAuiPaneInfo::optionActive))
		{
			if (pane.name == wxT("Activity") && currentPane != PANE_STATUS)
			{
				OnSelStatusItem(evt);
			}
			if (pane.name == wxT("Locks") && currentPane != PANE_LOCKS)
			{
				OnSelLockItem(evt);
			}
			if (pane.name == wxT("Transactions") && currentPane != PANE_XACT)
			{
				OnSelXactItem(evt);
			}
			if (pane.name == wxT("Logfile") && currentPane != PANE_LOG)
			{
				OnSelLogItem(evt);
			}
		}
	}

	refreshUITimer->Start(250);
}


void frmStatus::OnRefreshStatusTimer(wxTimerEvent &event)
{
	long pid = 0;
	wxString pidcol = connection->BackendMinimumVersion(9, 2) ? wxT("p.pid") : wxT("p.procpid");
	wxString querycol = connection->BackendMinimumVersion(9, 2) ? wxT("query") : wxT("current_query");

	if (! viewMenu->IsChecked(MNU_STATUSPAGE))
		return;

	checkConnection();
	if (!connection)
	{
		statusTimer->Stop();
		locksTimer->Stop();
		if (xactTimer)
			xactTimer->Stop();
		if (logTimer)
			logTimer->Stop();
		return;
	}

	wxCriticalSectionLocker lock(gs_critsect);

	long row = 0;
	wxString q = wxT("SELECT ");

	// PID
	q += pidcol + wxT(" AS pid, ");

	// Application name (when available)
	if (connection->BackendMinimumVersion(8, 5))
		q += wxT("application_name, ");

	// Database, and user name
	q += wxT("datname, usename,\n");

	// Client connection method
	if (connection->BackendMinimumVersion(8, 1))
	{
		q += wxT("CASE WHEN client_port=-1 THEN 'local pipe' ");
		if (connection->BackendMinimumVersion(9, 1))
			q += wxT("WHEN length(client_hostname)>0 THEN client_hostname||':'||client_port ");
		q += wxT("ELSE textin(inet_out(client_addr))||':'||client_port END AS client,\n");
	}

	// Backend start timestamp
	if (connection->BackendMinimumVersion(8, 1))
		q += wxT("date_trunc('second', backend_start) AS backend_start, ");

	// Query start timestamp (when available)
	if (connection->BackendMinimumVersion(9, 2))
	{
		q += wxT("CASE WHEN state='active' THEN date_trunc('second', query_start)::text ELSE '' END ");
	}
	else if (connection->BackendMinimumVersion(7, 4))
	{
		q += wxT("CASE WHEN ") + querycol + wxT("='' OR ") + querycol + wxT("='<IDLE>' THEN '' ")
		     wxT("     ELSE date_trunc('second', query_start)::text END ");
	}
	else
	{
		q += wxT("'' ");
	}
	q += wxT("AS query_start,\n");

	// Transaction start timestamp
	if (connection->BackendMinimumVersion(8, 3))
		q += wxT("date_trunc('second', xact_start) AS xact_start, ");

	// State
	if (connection->BackendMinimumVersion(9, 2))
		q += wxT("state, date_trunc('second', state_change) AS state_change, ");

	// Xmin and XID
	if (connection->BackendMinimumVersion(9, 4))
		q += wxT("backend_xid::text, backend_xmin::text, ");

	// Blocked by...
	q +=   wxT("(SELECT min(l1.pid) FROM pg_locks l1 WHERE GRANTED AND (")
	       wxT("relation IN (SELECT relation FROM pg_locks l2 WHERE l2.pid=") + pidcol + wxT(" AND NOT granted)")
	       wxT(" OR ")
	       wxT("transactionid IN (SELECT transactionid FROM pg_locks l3 WHERE l3.pid=") + pidcol + wxT(" AND NOT granted)")
	       wxT(")) AS blockedby,\n");

	// Query
	q += querycol + wxT(" AS query,\n");

	// Slow query?
	if (connection->BackendMinimumVersion(9, 2))
	{
		q += wxT("CASE WHEN query_start IS NULL OR state<>'active' THEN false ELSE query_start < now() - '10 seconds'::interval END ");
	}
	else if (connection->BackendMinimumVersion(7, 4))
	{
		q += wxT("CASE WHEN query_start IS NULL OR ") + querycol + wxT(" LIKE '<IDLE>%' THEN false ELSE query_start < now() - '10 seconds'::interval END ");
	}
	else
	{
		q += wxT("false");
	}
	q += wxT("AS slowquery\n");

	// And the rest of the query...
	q += wxT("FROM pg_stat_activity p ")
	     wxT("ORDER BY ") + NumToStr((long)statusSortColumn) + wxT(" ") + statusSortOrder;

	pgSet *dataSet1 = connection->ExecuteSet(q);
	if (dataSet1)
	{
		statusBar->SetStatusText(_("Refreshing status list."));
		statusList->Freeze();

		// Clear the queries array content
		queries.Clear();

		while (!dataSet1->Eof())
		{
			pid = dataSet1->GetLong(wxT("pid"));

			// Update the UI
			if (pid != backend_pid)
			{
				// Add the query content to the queries array
				queries.Add(dataSet1->GetVal(wxT("query")));

				if (row >= statusList->GetItemCount())
				{
					statusList->InsertItem(row, NumToStr(pid), -1);
					row = statusList->GetItemCount() - 1;
				}
				else
				{
					statusList->SetItem(row, 0, NumToStr(pid));
				}

				wxString qry = dataSet1->GetVal(wxT("query"));

				int colpos = 1;
				if (connection->BackendMinimumVersion(8, 5))
					statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("application_name")));
				statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("datname")));
				statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("usename")));

				if (connection->BackendMinimumVersion(8, 1))
				{
					statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("client")));
					statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("backend_start")));
				}
				if (connection->BackendMinimumVersion(7, 4))
				{
					statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("query_start")));
				}

				if (connection->BackendMinimumVersion(8, 3))
					statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("xact_start")));

				if (connection->BackendMinimumVersion(9, 2))
				{
					statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("state")));
					statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("state_change")));
				}

				if (connection->BackendMinimumVersion(9, 4))
				{
					statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("backend_xid")));
					statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("backend_xmin")));
				}

				statusList->SetItem(row, colpos++, dataSet1->GetVal(wxT("blockedby")));
				statusList->SetItem(row, colpos, qry);

				// Colorize the new line
				if (viewMenu->IsChecked(MNU_HIGHLIGHTSTATUS))
				{
					statusList->SetItemBackgroundColour(row,
					                                    wxColour(settings->GetActiveProcessColour()));
					if (qry == wxT("<IDLE>") || qry == wxT("<IDLE> in transaction0"))
						statusList->SetItemBackgroundColour(row,
						                                    wxColour(settings->GetIdleProcessColour()));
					if (connection->BackendMinimumVersion(9, 2))
					{
						if (dataSet1->GetVal(wxT("state")) != wxT("active"))
							statusList->SetItemBackgroundColour(row,
							                                    wxColour(settings->GetIdleProcessColour()));
					}

					if (dataSet1->GetVal(wxT("blockedby")).Length() > 0)
						statusList->SetItemBackgroundColour(row,
						                                    wxColour(settings->GetBlockedProcessColour()));
					if (dataSet1->GetBool(wxT("slowquery")))
						statusList->SetItemBackgroundColour(row,
						                                    wxColour(settings->GetSlowProcessColour()));
				}
				else
					statusList->SetItemBackgroundColour(row, *wxWHITE);

				row++;
			}
			dataSet1->MoveNext();
		}
		delete dataSet1;

		while (row < statusList->GetItemCount())
			statusList->DeleteItem(row);

		statusList->Thaw();
		wxListEvent ev;
		OnSelStatusItem(ev);
		statusBar->SetStatusText(_("Done."));
	}
	else
		checkConnection();
}


void frmStatus::OnRefreshLocksTimer(wxTimerEvent &event)
{
	long pid = 0;

	if (! viewMenu->IsChecked(MNU_LOCKPAGE))
		return;

	checkConnection();
	if (!locks_connection)
	{
		statusTimer->Stop();
		locksTimer->Stop();
		if (xactTimer)
			xactTimer->Stop();
		if (logTimer)
			logTimer->Stop();
		return;
	}

	wxCriticalSectionLocker lock(gs_critsect);

	// There are no sort operator for xid before 8.3
	if (!connection->BackendMinimumVersion(8, 3) && lockSortColumn == 5)
	{
		wxLogError(_("You cannot sort by transaction id on your PostgreSQL release. You need at least 8.3."));
		lockSortColumn = 1;
	}

	long row = 0;
	wxString sql;
	if (locks_connection->BackendMinimumVersion(8, 3))
	{
		sql = wxT("SELECT pg_stat_get_backend_pid(svrid) AS pid, ")
		      wxT("(SELECT datname FROM pg_database WHERE oid = pgl.database) AS dbname, ")
		      wxT("coalesce(pgc.relname, pgl.relation::text) AS class, ")
		      wxT("pg_get_userbyid(pg_stat_get_backend_userid(svrid)) as user, ")
		      wxT("pgl.virtualxid::text, pgl.virtualtransaction::text AS transaction, pgl.mode, pgl.granted, ")
		      wxT("date_trunc('second', pg_stat_get_backend_activity_start(svrid)) AS query_start, ")
		      wxT("pg_stat_get_backend_activity(svrid) AS query ")
		      wxT("FROM pg_stat_get_backend_idset() svrid, pg_locks pgl ")
		      wxT("LEFT JOIN pg_class pgc ON pgl.relation=pgc.oid ")
		      wxT("WHERE pgl.pid = pg_stat_get_backend_pid(svrid) ")
		      wxT("ORDER BY ") + NumToStr((long)lockSortColumn) + wxT(" ") + lockSortOrder;
	}
	else if (locks_connection->BackendMinimumVersion(7, 4))
	{
		sql = wxT("SELECT pg_stat_get_backend_pid(svrid) AS pid, ")
		      wxT("(SELECT datname FROM pg_database WHERE oid = pgl.database) AS dbname, ")
		      wxT("coalesce(pgc.relname, pgl.relation::text) AS class, ")
		      wxT("pg_get_userbyid(pg_stat_get_backend_userid(svrid)) as user, ")
		      wxT("pgl.transaction, pgl.mode, pgl.granted, ")
		      wxT("date_trunc('second', pg_stat_get_backend_activity_start(svrid)) AS query_start, ")
		      wxT("pg_stat_get_backend_activity(svrid) AS query ")
		      wxT("FROM pg_stat_get_backend_idset() svrid, pg_locks pgl ")
		      wxT("LEFT JOIN pg_class pgc ON pgl.relation=pgc.oid ")
		      wxT("WHERE pgl.pid = pg_stat_get_backend_pid(svrid) ")
		      wxT("ORDER BY ") + NumToStr((long)lockSortColumn) + wxT(" ") + lockSortOrder;
	}
	else
	{
		sql = wxT("SELECT pg_stat_get_backend_pid(svrid) AS pid, ")
		      wxT("(SELECT datname FROM pg_database WHERE oid = pgl.database) AS dbname, ")
		      wxT("coalesce(pgc.relname, pgl.relation::text) AS class, ")
		      wxT("pg_get_userbyid(pg_stat_get_backend_userid(svrid)) as user, ")
		      wxT("pgl.transaction, pgl.mode, pgl.granted, ")
		      wxT("pg_stat_get_backend_activity(svrid) AS query ")
		      wxT("FROM pg_stat_get_backend_idset() svrid, pg_locks pgl ")
		      wxT("LEFT JOIN pg_class pgc ON pgl.relation=pgc.oid ")
		      wxT("WHERE pgl.pid = pg_stat_get_backend_pid(svrid) ")
		      wxT("ORDER BY ") + NumToStr((long)lockSortColumn) + wxT(" ") + lockSortOrder;
	}

	pgSet *dataSet2 = locks_connection->ExecuteSet(sql);
	if (dataSet2)
	{
		statusBar->SetStatusText(_("Refreshing locks list."));
		lockList->Freeze();

		while (!dataSet2->Eof())
		{
			pid = dataSet2->GetLong(wxT("pid"));

			if (pid != backend_pid)
			{
				if (row >= lockList->GetItemCount())
				{
					lockList->InsertItem(row, NumToStr(pid), -1);
					row = lockList->GetItemCount() - 1;
				}
				else
				{
					lockList->SetItem(row, 0, NumToStr(pid));
				}

				int colpos = 1;
				lockList->SetItem(row, colpos++, dataSet2->GetVal(wxT("dbname")));
				lockList->SetItem(row, colpos++, dataSet2->GetVal(wxT("class")));
				lockList->SetItem(row, colpos++, dataSet2->GetVal(wxT("user")));
				if (locks_connection->BackendMinimumVersion(8, 3))
					lockList->SetItem(row, colpos++, dataSet2->GetVal(wxT("virtualxid")));
				lockList->SetItem(row, colpos++, dataSet2->GetVal(wxT("transaction")));
				lockList->SetItem(row, colpos++, dataSet2->GetVal(wxT("mode")));

				if (dataSet2->GetVal(wxT("granted")) == wxT("t"))
					lockList->SetItem(row, colpos++, _("Yes"));
				else
					lockList->SetItem(row, colpos++, _("No"));

				wxString qry = dataSet2->GetVal(wxT("query"));

				if (locks_connection->BackendMinimumVersion(7, 4))
				{
					if (qry.IsEmpty() || qry == wxT("<IDLE>"))
						lockList->SetItem(row, colpos++, wxEmptyString);
					else
						lockList->SetItem(row, colpos++, dataSet2->GetVal(wxT("query_start")));
				}
				lockList->SetItem(row, colpos++, qry.Left(250));

				row++;
			}
			dataSet2->MoveNext();
		}

		delete dataSet2;

		while (row < lockList->GetItemCount())
			lockList->DeleteItem(row);

		lockList->Thaw();
		wxListEvent ev;
		OnSelLockItem(ev);
		statusBar->SetStatusText(_("Done."));
	}
	else
		checkConnection();
}


void frmStatus::OnRefreshXactTimer(wxTimerEvent &event)
{
	if (! viewMenu->IsEnabled(MNU_XACTPAGE) || ! viewMenu->IsChecked(MNU_XACTPAGE) || !xactTimer)
		return;

	checkConnection();
	if (!connection)
	{
		statusTimer->Stop();
		locksTimer->Stop();
		xactTimer->Stop();
		if (logTimer)
			logTimer->Stop();
		return;
	}

	wxCriticalSectionLocker lock(gs_critsect);

	// There are no sort operator for xid before 8.3
	if (!connection->BackendMinimumVersion(8, 3) && xactSortColumn == 1)
	{
		wxLogError(_("You cannot sort by transaction id on your PostgreSQL release. You need at least 8.3."));
		xactSortColumn = 2;
	}

	long row = 0;
	wxString sql;
	if (connection->BackendMinimumVersion(8, 3))
		sql = wxT("SELECT transaction::text, gid, prepared, owner, database ")
		      wxT("FROM pg_prepared_xacts ")
		      wxT("ORDER BY ") + NumToStr((long)xactSortColumn) + wxT(" ") + xactSortOrder;
	else
		sql = wxT("SELECT transaction, gid, prepared, owner, database ")
		      wxT("FROM pg_prepared_xacts ")
		      wxT("ORDER BY ") + NumToStr((long)xactSortColumn) + wxT(" ") + xactSortOrder;

	pgSet *dataSet3 = connection->ExecuteSet(sql);
	if (dataSet3)
	{
		statusBar->SetStatusText(_("Refreshing transactions list."));
		xactList->Freeze();

		while (!dataSet3->Eof())
		{
			long xid = dataSet3->GetLong(wxT("transaction"));

			if (row >= xactList->GetItemCount())
			{
				xactList->InsertItem(row, NumToStr(xid), -1);
				row = xactList->GetItemCount() - 1;
			}
			else
			{
				xactList->SetItem(row, 0, NumToStr(xid));
			}

			int colpos = 1;
			xactList->SetItem(row, colpos++, dataSet3->GetVal(wxT("gid")));
			xactList->SetItem(row, colpos++, dataSet3->GetVal(wxT("prepared")));
			xactList->SetItem(row, colpos++, dataSet3->GetVal(wxT("owner")));
			xactList->SetItem(row, colpos++, dataSet3->GetVal(wxT("database")));

			row++;
			dataSet3->MoveNext();
		}
		delete dataSet3;

		while (row < xactList->GetItemCount())
			xactList->DeleteItem(row);

		xactList->Thaw();
		wxListEvent ev;
		OnSelXactItem(ev);
		statusBar->SetStatusText(_("Done."));
	}
	else
		checkConnection();
}


void frmStatus::OnRefreshLogTimer(wxTimerEvent &event)
{
	if (! viewMenu->IsEnabled(MNU_LOGPAGE) || ! viewMenu->IsChecked(MNU_LOGPAGE) || !logTimer)
		return;

	checkConnection();
	if (!connection)
	{
		statusTimer->Stop();
		locksTimer->Stop();
		if (xactTimer)
			xactTimer->Stop();
		logTimer->Stop();
		return;
	}

	wxCriticalSectionLocker lock(gs_critsect);

	if (connection->GetLastResultError().sql_state == wxT("42501"))
	{
		// Don't have superuser privileges, so can't do anything with the log display
		logTimer->Stop();
		cbLogfiles->Disable();
		btnRotateLog->Disable();
		manager.GetPane(wxT("Logfile")).Show(false);
		manager.Update();
		return;
	}

	long newlen = 0;

	if (logDirectory.IsEmpty())
	{
		// freshly started
		logDirectory = connection->ExecuteScalar(wxT("SHOW log_directory"));
		if (connection->GetLastResultError().sql_state == wxT("42501"))
		{
			// Don't have superuser privileges, so can't do anything with the log display
			logTimer->Stop();
			cbLogfiles->Disable();
			btnRotateLog->Disable();
			manager.GetPane(wxT("Logfile")).Show(false);
			manager.Update();
			return;
		}
		if (fillLogfileCombo())
		{
			savedPartialLine.Clear();
			cbLogfiles->SetSelection(0);
			wxCommandEvent ev;
			OnLoadLogfile(ev);
			return;
		}
		else
		{
			logDirectory = wxT("-");
			if (connection->BackendMinimumVersion(8, 3))
				logList->AppendItem(-1, wxString(_("logging_collector not enabled or log_filename misconfigured")));
			else
				logList->AppendItem(-1, wxString(_("redirect_stderr not enabled or log_filename misconfigured")));
			cbLogfiles->Disable();
			btnRotateLog->Disable();
		}
	}

	if (logDirectory == wxT("-"))
		return;

	if (isCurrent)
	{
		// check if the current logfile changed
		pgSet *set = connection->ExecuteSet(wxT("SELECT pg_file_length(") + connection->qtDbString(logfileName) + wxT(") AS len"));
		if (set)
		{
			newlen = set->GetLong(wxT("len"));
			delete set;
		}
		else
		{
			checkConnection();
			return;
		}
		if (newlen > logfileLength)
		{
			statusBar->SetStatusText(_("Refreshing log list."));
			addLogFile(logfileName, logfileTimestamp, newlen, logfileLength, false);
			statusBar->SetStatusText(_("Done."));

			// as long as there was new data, the logfile is probably the current
			// one so we don't need to check for rotation
			return;
		}
	}

	//
	wxString newDirectory = connection->ExecuteScalar(wxT("SHOW log_directory"));

	int newfiles = 0;
	if (newDirectory != logDirectory)
		cbLogfiles->Clear();

	newfiles = fillLogfileCombo();

	if (newfiles)
	{
		if (!showCurrent)
			isCurrent = false;

		if (isCurrent)
		{
			int pos = cbLogfiles->GetCount() - newfiles;
			bool skipFirst = true;

			while (newfiles--)
			{
				addLogLine(_("pgadmin:Logfile rotated."), false);
				wxDateTime *ts = (wxDateTime *)cbLogfiles->wxItemContainer::GetClientData(pos++);
				wxASSERT(ts != 0);

				addLogFile(ts, skipFirst);
				skipFirst = false;

				pos++;
			}
		}
	}
}


void frmStatus::OnRefresh(wxCommandEvent &event)
{
	wxTimerEvent evt;

	OnRefreshStatusTimer(evt);
	OnRefreshLocksTimer(evt);
	OnRefreshXactTimer(evt);
	OnRefreshLogTimer(evt);
}


void frmStatus::checkConnection()
{
	if (!locks_connection->IsAlive())
	{
		locks_connection = connection;
	}
	if (!connection->IsAlive())
	{
		delete connection;
		connection = 0;
		statusTimer->Stop();
		locksTimer->Stop();
		if (xactTimer)
			xactTimer->Stop();
		if (logTimer)
			logTimer->Stop();
		actionMenu->Enable(MNU_REFRESH, false);
		toolBar->EnableTool(MNU_REFRESH, false);
		statusBar->SetStatusText(_("Connection broken."));
	}
}


void frmStatus::addLogFile(wxDateTime *dt, bool skipFirst)
{
	pgSet *set = connection->ExecuteSet(
	                 wxT("SELECT filetime, filename, pg_file_length(filename) AS len ")
	                 wxT("  FROM pg_logdir_ls() AS A(filetime timestamp, filename text) ")
	                 wxT(" WHERE filetime = '") + DateToAnsiStr(*dt) + wxT("'::timestamp"));
	if (set)
	{
		logfileName = set->GetVal(wxT("filename"));
		logfileTimestamp = set->GetDateTime(wxT("filetime"));
		long len = set->GetLong(wxT("len"));

		logfileLength = 0;
		addLogFile(logfileName, logfileTimestamp, len, logfileLength, skipFirst);

		delete set;
	}
}


void frmStatus::addLogFile(const wxString &filename, const wxDateTime timestamp, long len, long &read, bool skipFirst)
{
	wxString line;

	if (skipFirst)
	{
		long maxServerLogSize = settings->GetMaxServerLogSize();

		if (!logfileLength && maxServerLogSize && logfileLength > maxServerLogSize)
		{
			long maxServerLogSize = settings->GetMaxServerLogSize();
			len = read - maxServerLogSize;
		}
		else
			skipFirst = false;
	}

	// If GPDB 3.3 and later, log is normally in CSV format.  Let's get a whole log line before calling addLogLine,
	// so we can do things smarter.

	// PostgreSQL can log in CSV format, as well as regular format.  Normally, we'd only see
	// the regular format logs here, because pg_logdir_ls only returns those.  But if pg_logdir_ls is
	// changed to return the csv format log files, we should handle it.

	bool csv_log_format = filename.Right(4) == wxT(".csv");

	if (csv_log_format && savedPartialLine.length() > 0)
	{
		if (read == 0)  // Starting at beginning of log file
			savedPartialLine.clear();
		else
			line = savedPartialLine;
	}

	while (len > read)
	{
		statusBar->SetStatusText(_("Reading log from server..."));
		pgSet *set = connection->ExecuteSet(wxT("SELECT pg_file_read(") +
		                                    connection->qtDbString(filename) + wxT(", ") + NumToStr(read) + wxT(", 50000)"));
		if (!set)
		{
			connection->IsAlive();
			return;
		}
		char *raw = set->GetCharPtr(0);

		if (!raw || !*raw)
		{
			delete set;
			break;
		}

		read += strlen(raw);

		wxString str;
		if (wxString(wxString(raw, wxConvLibc).wx_str(), wxConvUTF8).Len() > 0)
			str = line + wxString(wxString(raw, wxConvLibc).wx_str(), wxConvUTF8);
		else
			str = line + wxTextBuffer::Translate(wxString(raw, set->GetConversion()), wxTextFileType_Unix);

		delete set;

		if (str.Len() == 0)
		{
			wxString msgstr = _("The server log contains entries in multiple encodings and cannot be displayed by pgAdmin.");
			wxMessageBox(msgstr);
			return;
		}

		if (csv_log_format)
		{
			// This will work for any DB using CSV format logs

			if (logHasTimestamp)
			{
				// Right now, csv format logs from GPDB and PostgreSQL always start with a timestamp, so we count on that.

				// And the only reason we need to do that is to make sure we are in sync.

				// Bad things happen if we start in the middle of a
				// double-quoted string, as we would never find a correct line terminator!

				// In CSV logs, the first field must be a Timestamp, so must start with "2009" or "201" or "202" (at least for the next 20 years).
				if (str.length() > 4 && str.Left(4) != wxT("2009") && str.Left(3) != wxT("201") && str.Left(3) != wxT("202"))
				{
					wxLogNotice(wxT("Log line does not start with timestamp: %s \n"), str.Mid(0, 100).c_str());
					// Something isn't right, as we are not at the beginning of a csv log record.
					// We should never get here, but if we do, try to handle it in a smart way.
					str = str.Mid(str.Find(wxT("\n20")) + 1); // Try to re-sync.
				}
			}

			CSVLineTokenizer tk(str);

			logList->Freeze();

			while (tk.HasMoreLines())
			{
				line.Clear();

				bool partial;
				str = tk.GetNextLine(partial);
				if (partial)
				{
					line = str; // Start of a log line, but not complete.  Loop back, Read more data.
					break;
				}

				// Some extra debug checking, assuming csv logs line start with timestamps.
				// Not really necessary, but it is good for debugging if something isn't right.
				if (logHasTimestamp)
				{
					// The first field must be a Timestamp, so must start with "2009" or "201" or "202" (at least for the next 20 years).
					// This is just an extra check to make sure we haven't gotten out of sync with the log.
					if (str.length() > 5 && str.Left(4) != wxT("2009") && str.Left(3) != wxT("201") && str.Left(3) != wxT("202"))
					{
						// BUG:  We are out of sync on the log
						wxLogNotice(wxT("Log line does not start with timestamp: %s\n"), str.c_str());
					}
					else if (str.length() < 20)
					{
						// BUG:  We are out of sync on the log, or the log is garbled
						wxLogNotice(wxT("Log line too short: %s\n"), str.c_str());
					}
				}

				// Looks like we have a good complete CSV log record.
				addLogLine(str.Trim(), true, true);
			}

			logList->Thaw();
		}
		else
		{
			// Non-csv format log file

			bool hasCr = (str.Right(1) == wxT("\n"));

			wxStringTokenizer tk(str, wxT("\n"));

			logList->Freeze();

			while (tk.HasMoreTokens())
			{
				str = tk.GetNextToken();
				if (skipFirst)
				{
					// could be truncated
					skipFirst = false;
					continue;
				}

				if (tk.HasMoreTokens() || hasCr)
					addLogLine(str.Trim());
				else
					line = str;
			}

			logList->Thaw();
		}
	}

	savedPartialLine.clear();

	if (!line.IsEmpty())
	{
		// We finished reading to the end of the log file, but still have some data left
		if (csv_log_format)
		{
			savedPartialLine = line;    // Save partial log line for next read of the data file.
			line.Clear();
		}
		else
			addLogLine(line.Trim());
	}

}


void frmStatus::addLogLine(const wxString &str, bool formatted, bool csv_log_format)
{
	int row = logList->GetItemCount();

	int idxTimeStampCol = -1, idxLevelCol = -1;
	int idxLogEntryCol = 0;

	if (logFormatKnown)
	{
		// Known Format first will be level, then Log entry
		// idxLevelCol : 0, idxLogEntryCol : 1, idxTimeStampCol : -1
		idxLevelCol++;
		idxLogEntryCol++;
		if (logHasTimestamp)
		{
			// idxLevelCol : 1, idxLogEntryCol : 2, idxTimeStampCol : 0
			idxTimeStampCol++;
			idxLevelCol++;
			idxLogEntryCol++;
		}
	}

	if (!logFormatKnown)
		logList->AppendItem(-1, str);
	else if ((!csv_log_format) && str.Find(':') < 0)
	{
		// Must be a continuation of a previous line.
		logList->InsertItem(row, wxEmptyString, -1);
		logList->SetItem(row, idxLogEntryCol, str);
	}
	else if (!formatted)
	{
		// Not from a log, from pgAdmin itself.
		if (logHasTimestamp)
		{
			logList->InsertItem(row, wxEmptyString, -1);
			logList->SetItem(row, idxLevelCol, str.BeforeFirst(':'));
		}
		else
		{
			logList->InsertItem(row, str.BeforeFirst(':'), -1);
		}
		logList->SetItem(row, idxLogEntryCol, str.AfterFirst(':'));
	}
	else // formatted log
	{
		if (csv_log_format)
		{
			// Log is in CSV format (GPDB 3.3 and later, or Postgres if only csv log enabled)
			// In this case, we are always supposed to have a complete log line in csv format in str when called.

			if (logHasTimestamp && (str.Length() < 20 || (logHasTimestamp && (str[0] != wxT('2') || str[1] != wxT('0')))))
			{
				// Log line too short or does not start with an expected timestamp...
				// Must be a continuation of the previous line or garbage,
				// or we are out of sync in our CSV handling.
				// We shouldn't ever get here.
				logList->InsertItem(row, wxEmptyString, -1);
				logList->SetItem(row, 2, str);
			}
			else
			{
				CSVTokenizer tk(str);

				bool gpdb = connection->GetIsGreenplum();

				// Get the fields from the CSV log.
				wxString logTime = tk.GetNextToken();
				wxString logUser = tk.GetNextToken();
				wxString logDatabase = tk.GetNextToken();
				wxString logPid = tk.GetNextToken();

				wxString logSession;
				wxString logCmdcount;
				wxString logSegment;

				if (gpdb)
				{
					wxString logThread =  tk.GetNextToken();        // GPDB specific
					wxString logHost = tk.GetNextToken();
					wxString logPort = tk.GetNextToken();           // GPDB (Postgres puts port with Host)
					wxString logSessiontime = tk.GetNextToken();
					wxString logTransaction = tk.GetNextToken();
					logSession = tk.GetNextToken();
					logCmdcount = tk.GetNextToken();
					logSegment = tk.GetNextToken();
					wxString logSlice = tk.GetNextToken();
					wxString logDistxact = tk.GetNextToken();
					wxString logLocalxact = tk.GetNextToken();
					wxString logSubxact = tk.GetNextToken();
				}
				else
				{
					wxString logHost = tk.GetNextToken();       // Postgres puts port with Hostname
					logSession = tk.GetNextToken();
					wxString logLineNumber = tk.GetNextToken();
					wxString logPsDisplay = tk.GetNextToken();
					wxString logSessiontime = tk.GetNextToken();
					wxString logVXid = tk.GetNextToken();
					wxString logTransaction = tk.GetNextToken();
				}

				wxString logSeverity = tk.GetNextToken();
				wxString logState = tk.GetNextToken();
				wxString logMessage = tk.GetNextToken();
				wxString logDetail = tk.GetNextToken();
				wxString logHint = tk.GetNextToken();
				wxString logQuery = tk.GetNextToken();
				wxString logQuerypos = tk.GetNextToken();
				wxString logContext = tk.GetNextToken();
				wxString logDebug = tk.GetNextToken();
				wxString logCursorpos = tk.GetNextToken();

				wxString logStack;
				if (gpdb)
				{
					wxString logFunction = tk.GetNextToken();       // GPDB.  Postgres puts func, file, and line together
					wxString logFile = tk.GetNextToken();
					wxString logLine = tk.GetNextToken();
					logStack = tk.GetNextToken();                   // GPDB only.
				}
				else
					wxString logFuncFileLine = tk.GetNextToken();

				logList->InsertItem(row, logTime, -1);      // Insert timestamp (with time zone)

				logList->SetItem(row, 1, logSeverity);

				// Display the logMessage, breaking it into lines
				wxStringTokenizer lm(logMessage, wxT("\n"));
				logList->SetItem(row, 2, lm.GetNextToken());

				logList->SetItem(row, 3, logSession);
				logList->SetItem(row, 4, logCmdcount);
				logList->SetItem(row, 5, logDatabase);
				if ((!gpdb) || (logSegment.length() > 0 && logSegment != wxT("seg-1")))
				{
					logList->SetItem(row, 6, logSegment);
				}
				else
				{
					// If we are reading the masterDB log only, the logSegment won't
					// have anything useful in it.  Look in the logMessage, and see if the
					// segment info exists in there.  It will always be at the end.
					if (logMessage.length() > 0 && logMessage[logMessage.length() - 1] == wxT(')'))
					{
						int segpos = -1;
						segpos = logMessage.Find(wxT("(seg"));
						if (segpos <= 0)
							segpos = logMessage.Find(wxT("(mir"));
						if (segpos > 0)
						{
							logSegment = logMessage.Mid(segpos + 1);
							if (logSegment.Find(wxT(' ')) > 0)
								logSegment = logSegment.Mid(0, logSegment.Find(wxT(' ')));
							logList->SetItem(row, 6, logSegment);
						}
					}
				}

				// The rest of the lines from the logMessage
				while (lm.HasMoreTokens())
				{
					int controw = logList->GetItemCount();
					logList->InsertItem(controw, wxEmptyString, -1);
					logList->SetItem(controw, 2, lm.GetNextToken());
				}

				// Add the detail
				wxStringTokenizer ld(logDetail, wxT("\n"));
				while (ld.HasMoreTokens())
				{
					int controw = logList->GetItemCount();
					logList->InsertItem(controw, wxEmptyString, -1);
					logList->SetItem(controw, 2, ld.GetNextToken());
				}

				// And the hint
				wxStringTokenizer lh(logHint, wxT("\n"));
				while (lh.HasMoreTokens())
				{
					int controw = logList->GetItemCount();
					logList->InsertItem(controw, wxEmptyString, -1);
					logList->SetItem(controw, 2, lh.GetNextToken());
				}

				if (logDebug.length() > 0)
				{
					wxString logState3 = logState.Mid(0, 3);
					if (logState3 == wxT("426") || logState3 == wxT("22P") || logState3 == wxT("427")
					        || logState3 == wxT("42P") || logState3 == wxT("458")
					        || logMessage.Mid(0, 9) == wxT("duration:") || logSeverity == wxT("FATAL") || logSeverity == wxT("PANIC"))
					{
						// If not redundant, add the statement from the debug_string
						wxStringTokenizer lh(logDebug, wxT("\n"));
						if (lh.HasMoreTokens())
						{
							int controw = logList->GetItemCount();
							logList->InsertItem(controw, wxEmptyString, -1);
							logList->SetItem(controw, 2, wxT("statement: ") + lh.GetNextToken());
						}
						while (lh.HasMoreTokens())
						{
							int controw = logList->GetItemCount();
							logList->InsertItem(controw, wxEmptyString, -1);
							logList->SetItem(controw, 2, lh.GetNextToken());
						}
					}
				}

				if (gpdb)
					if (logSeverity == wxT("PANIC") ||
					        (logSeverity == wxT("FATAL") && logState != wxT("57P03") && logState != wxT("53300")))
					{
						// If this is a severe error, add the stack trace.
						wxStringTokenizer ls(logStack, wxT("\n"));
						if (ls.HasMoreTokens())
						{
							int controw = logList->GetItemCount();
							logList->InsertItem(controw, wxEmptyString, -1);
							logList->SetItem(controw, 1, wxT("STACK"));
							logList->SetItem(controw, 2, ls.GetNextToken());
						}
						while (ls.HasMoreTokens())
						{
							int controw = logList->GetItemCount();
							logList->InsertItem(controw, wxEmptyString, -1);
							logList->SetItem(controw, 2, ls.GetNextToken());
						}
					}
			}
		}
		else if (connection->GetIsGreenplum())
		{
			// Greenplum 3.2 and before.  log_line_prefix =  "%m|%u|%d|%p|%I|%X|:-"

			wxString logSeverity;
			// Skip prefix, get message.  In GPDB, always follows ":-".
			wxString rest = str.Mid(str.Find(wxT(":-")) + 1) ;
			if (rest.Length() > 0 && rest[0] == wxT('-'))
				rest = rest.Mid(1);

			// Separate loglevel from message

			if (rest.Length() > 1 && rest[0] != wxT(' ') && rest.Find(':') > 0)
			{
				logSeverity = rest.BeforeFirst(':');
				rest = rest.AfterFirst(':').Mid(2);
			}

			wxString ts = str.BeforeFirst(logFormat.c_str()[logFmtPos + 2]);
			if (ts.Length() < 20  || (logHasTimestamp && (ts.Left(2) != wxT("20") || str.Find(':') < 0)))
			{
				// No Timestamp?  Must be a continuation of a previous line?
				// Not sure if it is possible to get here.
				logList->InsertItem(row, wxEmptyString, -1);
				logList->SetItem(row, 2, rest);
			}
			else if (logSeverity.Length() > 1)
			{
				// Normal case:  Start of a new log record.
				logList->InsertItem(row, ts, -1);
				logList->SetItem(row, 1, logSeverity);
				logList->SetItem(row, 2, rest);
			}
			else
			{
				// Continuation of previous line
				logList->InsertItem(row, wxEmptyString, -1);
				logList->SetItem(row, 2, rest);
			}
		}
		else
		{
			// All Non-csv-format non-GPDB PostgreSQL systems.

			wxString rest;

			if (logHasTimestamp)
			{
				if (formatted)
				{
					rest = str.Mid(logFmtPos + 22).AfterFirst(':');
					wxString ts = str.Mid(logFmtPos, str.Length() - rest.Length() - logFmtPos - 1);

					int pos = ts.Find(logFormat.c_str()[logFmtPos + 2], true);
					logList->InsertItem(row, ts.Left(pos), -1);
					logList->SetItem(row, idxLevelCol, ts.Mid(pos + logFormat.Length() - logFmtPos - 2));
					logList->SetItem(row, idxLogEntryCol, rest.Mid(2));
				}
				else
				{
					logList->InsertItem(row, wxEmptyString, -1);
					logList->SetItem(row, idxLevelCol, str.BeforeFirst(':'));
					logList->SetItem(row, idxLogEntryCol, str.AfterFirst(':').Mid(2));
				}
			}
			else
			{
				if (formatted)
					rest = str.Mid(logFormat.Length());
				else
					rest = str;

				int pos = rest.Find(':');

				if (pos < 0)
					logList->InsertItem(row, rest, -1);
				else
				{
					logList->InsertItem(row, rest.BeforeFirst(':'), -1);
					logList->SetItem(row, idxLogEntryCol, rest.AfterFirst(':').Mid(2));
				}
			}
		}
	}
}


void frmStatus::emptyLogfileCombo()
{
	if (cbLogfiles->GetCount()) // first entry has no client data
		cbLogfiles->Delete(0);

	while (cbLogfiles->GetCount())
	{
		wxDateTime *dt = (wxDateTime *)cbLogfiles->wxItemContainer::GetClientData(0);
		if (dt)
			delete dt;
		cbLogfiles->Delete(0);
	}
}


int frmStatus::fillLogfileCombo()
{
	int count = cbLogfiles->GetCount();
	if (!count)
		cbLogfiles->Append(_("Current log"));
	else
		count--;

	pgSet *set = connection->ExecuteSet(
	                 wxT("SELECT filename, filetime\n")
	                 wxT("  FROM pg_logdir_ls() AS A(filetime timestamp, filename text)\n")
	                 wxT(" ORDER BY filetime DESC"));
	if (set)
	{
		if (set->NumRows() <= count)
		{
			delete set;
			return 0;
		}

		set->Locate(count + 1);
		count = 0;

		while (!set->Eof())
		{
			count++;
			wxString fn = set->GetVal(wxT("filename"));
			wxDateTime ts = set->GetDateTime(wxT("filetime"));

			cbLogfiles->Append(DateToAnsiStr(ts), (void *)new wxDateTime(ts));

			set->MoveNext();
		}

		delete set;
	}

	return count;
}


void frmStatus::OnLoadLogfile(wxCommandEvent &event)
{
	int pos = cbLogfiles->GetCurrentSelection();
	if (pos >= 0)
	{
		showCurrent = (pos == 0);
		isCurrent = showCurrent || (pos == 1);

		wxDateTime *ts = (wxDateTime *)cbLogfiles->wxItemContainer::GetClientData(showCurrent ? 1 : pos);
		wxASSERT(ts != 0);

		if (ts != NULL && (!logfileTimestamp.IsValid() || *ts != logfileTimestamp))
		{
			logList->DeleteAllItems();
			addLogFile(ts, true);
		}
	}
}


void frmStatus::OnRotateLogfile(wxCommandEvent &event)
{
	if (wxMessageBox(_("Are you sure the logfile should be rotated?"), _("Logfile rotation"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxYES)
		connection->ExecuteVoid(wxT("select pg_logfile_rotate()"));
}


void frmStatus::OnCancelBtn(wxCommandEvent &event)
{
	switch(currentPane)
	{
		case PANE_STATUS:
			OnStatusCancelBtn(event);
			break;
		case PANE_LOCKS:
			OnLocksCancelBtn(event);
			break;
		default:
			// This shouldn't happen. If it does, it's no big deal
			break;
	}
}


void frmStatus::OnStatusCancelBtn(wxCommandEvent &event)
{
	long item = statusList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item < 0)
		return;

	if (wxMessageBox(_("Are you sure you wish to cancel the selected query(s)?"), _("Cancel query?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES)
		return;

	while  (item >= 0)
	{
		wxString pid = statusList->GetItemText(item);
		wxString sql = wxT("SELECT pg_cancel_backend(") + pid + wxT(");");
		connection->ExecuteScalar(sql);

		item = statusList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	}

	wxMessageBox(_("A cancel signal was sent to the selected server process(es)."), _("Cancel query"), wxOK | wxICON_INFORMATION);
	OnRefresh(event);
	wxListEvent ev;
	OnSelStatusItem(ev);
}


void frmStatus::OnLocksCancelBtn(wxCommandEvent &event)
{
	long item = lockList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item < 0)
		return;

	if (wxMessageBox(_("Are you sure you wish to cancel the selected query(s)?"), _("Cancel query?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES)
		return;

	while  (item >= 0)
	{
		wxString pid = lockList->GetItemText(item);
		wxString sql = wxT("SELECT pg_cancel_backend(") + pid + wxT(");");
		connection->ExecuteScalar(sql);

		item = lockList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	}

	wxMessageBox(_("A cancel signal was sent to the selected server process(es)."), _("Cancel query"), wxOK | wxICON_INFORMATION);
	OnRefresh(event);
	wxListEvent ev;
	OnSelLockItem(ev);
}


void frmStatus::OnTerminateBtn(wxCommandEvent &event)
{
	switch(currentPane)
	{
		case PANE_STATUS:
			OnStatusTerminateBtn(event);
			break;
		case PANE_LOCKS:
			OnLocksTerminateBtn(event);
			break;
		default:
			// This shouldn't happen. If it does, it's no big deal
			break;
	}
}


void frmStatus::OnStatusTerminateBtn(wxCommandEvent &event)
{
	long item = statusList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item < 0)
		return;

	if (wxMessageBox(_("Are you sure you wish to terminate the selected server process(es)?"), _("Terminate process?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES)
		return;

	while  (item >= 0)
	{
		wxString pid = statusList->GetItemText(item);
		wxString sql = wxT("SELECT pg_terminate_backend(") + pid + wxT(");");
		connection->ExecuteScalar(sql);

		item = statusList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	}

	wxMessageBox(_("A terminate signal was sent to the selected server process(es)."), _("Terminate process"), wxOK | wxICON_INFORMATION);
	OnRefresh(event);
	wxListEvent ev;
	OnSelStatusItem(ev);
}


void frmStatus::OnLocksTerminateBtn(wxCommandEvent &event)
{
	long item = lockList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item < 0)
		return;

	if (wxMessageBox(_("Are you sure you wish to terminate the selected server process(es)?"), _("Terminate process?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES)
		return;

	while  (item >= 0)
	{
		wxString pid = lockList->GetItemText(item);
		wxString sql = wxT("SELECT pg_terminate_backend(") + pid + wxT(");");
		connection->ExecuteScalar(sql);

		item = lockList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	}

	wxMessageBox(_("A terminate signal was sent to the selected server process(es)."), _("Terminate process"), wxOK | wxICON_INFORMATION);
	OnRefresh(event);
	wxListEvent ev;
	OnSelLockItem(ev);
}


void frmStatus::OnStatusMenu(wxCommandEvent &event)
{
	wxListItem column;
	column.SetMask(wxLIST_MASK_TEXT);

	for (unsigned int i = 0; i < statusPopupMenu->GetMenuItemCount(); i++)
	{
		// Save column's width in a variable so that we can restore the old width
		// if we make this column "invisible"
		if (statusList->GetColumnWidth(i) > 0)
			statusColWidth[i] = statusList->GetColumnWidth(i);

		wxMenuItem *menu = statusPopupMenu->FindItemByPosition(i);
		if (menu && menu->IsChecked())
			statusList->SetColumnWidth(i, statusColWidth[i]);
		else if (statusList->GetColumnWidth(i) > 0)
			statusList->SetColumnWidth(i, 0);

		// Save current width to restore it at next launch
		statusList->GetColumn(i, column);
		if (column.GetWidth() > 0)
			settings->WriteInt(wxT("frmStatus/StatusPane_") + column.GetText() + wxT("_Width"),
			                   statusColWidth[i]);
		else
			settings->WriteInt(wxT("frmStatus/StatusPane_") + column.GetText() + wxT("_Width"),
			                   -statusColWidth[i]);
	}
}


void frmStatus::OnLockMenu(wxCommandEvent &event)
{
	wxListItem column;
	column.SetMask(wxLIST_MASK_TEXT);

	for (unsigned int i = 0; i < lockPopupMenu->GetMenuItemCount(); i++)
	{
		// Save column's width in a variable so that we can restore the old width
		// if we make this column "invisible"
		if (lockList->GetColumnWidth(i) > 0)
			lockColWidth[i] = lockList->GetColumnWidth(i);

		wxMenuItem *menu = lockPopupMenu->FindItemByPosition(i);
		if (menu && menu->IsChecked())
			lockList->SetColumnWidth(i, lockColWidth[i]);
		else if (lockList->GetColumnWidth(i) > 0)
			lockList->SetColumnWidth(i, 0);

		// Save current width to restore it at next launch
		lockList->GetColumn(i, column);
		if (column.GetWidth() > 0)
			settings->WriteInt(wxT("frmStatus/LockPane_") + column.GetText() + wxT("_Width"),
			                   lockColWidth[i]);
		else
			settings->WriteInt(wxT("frmStatus/LockPane_") + column.GetText() + wxT("_Width"),
			                   -lockColWidth[i]);
	}
}


void frmStatus::OnXactMenu(wxCommandEvent &event)
{
	wxListItem column;
	column.SetMask(wxLIST_MASK_TEXT);

	for (unsigned int i = 0; i < xactPopupMenu->GetMenuItemCount(); i++)
	{
		// Save column's width in a variable so that we can restore the old width
		// if we make this column "invisible"
		if (xactList->GetColumnWidth(i) > 0)
			xactColWidth[i] = xactList->GetColumnWidth(i);

		wxMenuItem *menu = xactPopupMenu->FindItemByPosition(i);
		if (menu && menu->IsChecked())
			xactList->SetColumnWidth(i, xactColWidth[i]);
		else if (xactList->GetColumnWidth(i) > 0)
			xactList->SetColumnWidth(i, 0);

		// Save current width to restore it at next launch
		xactList->GetColumn(i, column);
		if (column.GetWidth() > 0)
			settings->WriteInt(wxT("frmStatus/XactPane_") + column.GetText() + wxT("_Width"),
			                   xactColWidth[i]);
		else
			settings->WriteInt(wxT("frmStatus/XactPane_") + column.GetText() + wxT("_Width"),
			                   -xactColWidth[i]);
	}
}


void frmStatus::OnCommit(wxCommandEvent &event)
{
	long item = xactList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item < 0)
		return;

	if (wxMessageBox(_("Are you sure you wish to commit the selected prepared transactions?"), _("Commit transaction?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES)
		return;

	while  (item >= 0)
	{
		wxString xid = xactList->GetText(item, 1);
		wxString sql = wxT("COMMIT PREPARED ") + connection->qtDbString(xid);

		// We must execute this in the database in which the prepared transaction originated.
		if (connection->GetDbname() != xactList->GetText(item, 4))
		{
			pgConn *tmpConn = new pgConn(connection->GetHost(),
			                             connection->GetService(),
			                             connection->GetHostAddr(),
			                             xactList->GetText(item, 4),
			                             connection->GetUser(),
			                             connection->GetPassword(),
			                             connection->GetPort(),
			                             connection->GetRole(),
			                             connection->GetSslMode(),
			                             0,
			                             connection->GetApplicationName(),
			                             connection->GetSSLCert(),
			                             connection->GetSSLKey(),
			                             connection->GetSSLRootCert(),
			                             connection->GetSSLCrl(),
			                             connection->GetSSLCompression());
			if (tmpConn)
			{
				if (tmpConn->GetStatus() != PGCONN_OK)
				{
					wxMessageBox(wxT("Connection failed: ") + tmpConn->GetLastError());
					return ;
				}
				tmpConn->ExecuteScalar(sql);

				tmpConn->Close();
				delete tmpConn;
			}
		}
		else
			connection->ExecuteScalar(sql);

		item = xactList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	}

	OnRefresh(event);
	wxListEvent ev;
	OnSelXactItem(ev);
}


void frmStatus::OnRollback(wxCommandEvent &event)
{
	long item = xactList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (item < 0)
		return;

	if (wxMessageBox(_("Are you sure you wish to rollback the selected prepared transactions?"), _("Rollback transaction?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES)
		return;

	while  (item >= 0)
	{
		wxString xid = xactList->GetText(item, 1);
		wxString sql = wxT("ROLLBACK PREPARED ") + connection->qtDbString(xid);

		// We must execute this in the database in which the prepared transaction originated.
		if (connection->GetDbname() != xactList->GetText(item, 4))
		{
			pgConn *tmpConn = new pgConn(connection->GetHost(),
			                             connection->GetService(),
			                             connection->GetHostAddr(),
			                             xactList->GetText(item, 4),
			                             connection->GetUser(),
			                             connection->GetPassword(),
			                             connection->GetPort(),
			                             connection->GetRole(),
			                             connection->GetSslMode(),
			                             0,
			                             connection->GetApplicationName(),
			                             connection->GetSSLCert(),
			                             connection->GetSSLKey(),
			                             connection->GetSSLRootCert(),
			                             connection->GetSSLCrl(),
			                             connection->GetSSLCompression());
			if (tmpConn)
			{
				if (tmpConn->GetStatus() != PGCONN_OK)
				{
					wxMessageBox(wxT("Connection failed: ") + tmpConn->GetLastError());
					return ;
				}
				tmpConn->ExecuteScalar(sql);

				tmpConn->Close();
				delete tmpConn;
			}
		}
		else
			connection->ExecuteScalar(sql);

		item = xactList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	}

	OnRefresh(event);
	wxListEvent ev;
	OnSelXactItem(ev);
}


void frmStatus::OnSelStatusItem(wxListEvent &event)
{
#ifdef __WXGTK__
	manager.GetPane(wxT("Activity")).SetFlag(wxAuiPaneInfo::optionActive, true);
	manager.GetPane(wxT("Locks")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.GetPane(wxT("Transactions")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.GetPane(wxT("Logfile")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.Update();
#endif
	currentPane = PANE_STATUS;
	cbRate->SetValue(rateToCboString(statusRate));
	if (connection && connection->BackendMinimumVersion(8, 0))
	{
		if(statusList->GetSelectedItemCount() > 0)
		{
			toolBar->EnableTool(MNU_CANCEL, true);
			actionMenu->Enable(MNU_CANCEL, true);
			if (connection->HasFeature(FEATURE_TERMINATE_BACKEND))
			{
				toolBar->EnableTool(MNU_TERMINATE, true);
				actionMenu->Enable(MNU_TERMINATE, true);
			}
		}
		else
		{
			toolBar->EnableTool(MNU_CANCEL, false);
			actionMenu->Enable(MNU_CANCEL, false);
			toolBar->EnableTool(MNU_TERMINATE, false);
			actionMenu->Enable(MNU_TERMINATE, false);
		}
	}
	toolBar->EnableTool(MNU_COMMIT, false);
	actionMenu->Enable(MNU_COMMIT, false);
	toolBar->EnableTool(MNU_ROLLBACK, false);
	actionMenu->Enable(MNU_ROLLBACK, false);
	cbLogfiles->Enable(false);
	btnRotateLog->Enable(false);

	editMenu->Enable(MNU_COPY, statusList->GetFirstSelected() >= 0);
	actionMenu->Enable(MNU_COPY_QUERY, statusList->GetFirstSelected() >= 0);
	toolBar->EnableTool(MNU_COPY_QUERY, statusList->GetFirstSelected() >= 0);
}


void frmStatus::OnSelLockItem(wxListEvent &event)
{
#ifdef __WXGTK__
	manager.GetPane(wxT("Activity")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.GetPane(wxT("Locks")).SetFlag(wxAuiPaneInfo::optionActive, true);
	manager.GetPane(wxT("Transactions")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.GetPane(wxT("Logfile")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.Update();
#endif
	currentPane = PANE_LOCKS;
	cbRate->SetValue(rateToCboString(locksRate));
	if (connection && connection->BackendMinimumVersion(8, 0))
	{
		if(lockList->GetSelectedItemCount() > 0)
		{
			toolBar->EnableTool(MNU_CANCEL, true);
			actionMenu->Enable(MNU_CANCEL, true);
			if (connection->HasFeature(FEATURE_TERMINATE_BACKEND))
			{
				toolBar->EnableTool(MNU_TERMINATE, true);
				actionMenu->Enable(MNU_TERMINATE, true);
			}
		}
		else
		{
			toolBar->EnableTool(MNU_CANCEL, false);
			actionMenu->Enable(MNU_CANCEL, false);
			toolBar->EnableTool(MNU_TERMINATE, false);
			actionMenu->Enable(MNU_TERMINATE, false);
		}
	}
	toolBar->EnableTool(MNU_COMMIT, false);
	actionMenu->Enable(MNU_COMMIT, false);
	toolBar->EnableTool(MNU_ROLLBACK, false);
	actionMenu->Enable(MNU_ROLLBACK, false);
	cbLogfiles->Enable(false);
	btnRotateLog->Enable(false);

	editMenu->Enable(MNU_COPY, lockList->GetFirstSelected() >= 0);
	actionMenu->Enable(MNU_COPY_QUERY, false);
	toolBar->EnableTool(MNU_COPY_QUERY, false);
}


void frmStatus::OnSelXactItem(wxListEvent &event)
{
#ifdef __WXGTK__
	manager.GetPane(wxT("Activity")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.GetPane(wxT("Locks")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.GetPane(wxT("Transactions")).SetFlag(wxAuiPaneInfo::optionActive, true);
	manager.GetPane(wxT("Logfile")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.Update();
#endif
	currentPane = PANE_XACT;
	cbRate->SetValue(rateToCboString(xactRate));
	if(xactList->GetSelectedItemCount() > 0)
	{
		toolBar->EnableTool(MNU_COMMIT, true);
		actionMenu->Enable(MNU_COMMIT, true);
		toolBar->EnableTool(MNU_ROLLBACK, true);
		actionMenu->Enable(MNU_ROLLBACK, true);
	}
	else
	{
		toolBar->EnableTool(MNU_COMMIT, false);
		actionMenu->Enable(MNU_COMMIT, false);
		toolBar->EnableTool(MNU_ROLLBACK, false);
		actionMenu->Enable(MNU_ROLLBACK, false);
	}
	toolBar->EnableTool(MNU_CANCEL, false);
	actionMenu->Enable(MNU_CANCEL, false);
	toolBar->EnableTool(MNU_TERMINATE, false);
	actionMenu->Enable(MNU_TERMINATE, false);
	cbLogfiles->Enable(false);
	btnRotateLog->Enable(false);

	editMenu->Enable(MNU_COPY, xactList->GetFirstSelected() >= 0);
	actionMenu->Enable(MNU_COPY_QUERY, false);
	toolBar->EnableTool(MNU_COPY_QUERY, false);
}


void frmStatus::OnSelLogItem(wxListEvent &event)
{
#ifdef __WXGTK__
	manager.GetPane(wxT("Activity")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.GetPane(wxT("Locks")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.GetPane(wxT("Transactions")).SetFlag(wxAuiPaneInfo::optionActive, false);
	manager.GetPane(wxT("Logfile")).SetFlag(wxAuiPaneInfo::optionActive, true);
	manager.Update();
#endif
	currentPane = PANE_LOG;
	cbRate->SetValue(rateToCboString(logRate));

	// if there's no log, don't enable items
	if (logDirectory != wxT("-"))
	{
		cbLogfiles->Enable(true);
		btnRotateLog->Enable(true);
		toolBar->EnableTool(MNU_CANCEL, false);
		toolBar->EnableTool(MNU_TERMINATE, false);
		toolBar->EnableTool(MNU_COMMIT, false);
		toolBar->EnableTool(MNU_ROLLBACK, false);
		actionMenu->Enable(MNU_CANCEL, false);
		actionMenu->Enable(MNU_TERMINATE, false);
		actionMenu->Enable(MNU_COMMIT, false);
		actionMenu->Enable(MNU_ROLLBACK, false);
	}

	editMenu->Enable(MNU_COPY, logList->GetFirstSelected() >= 0);
	actionMenu->Enable(MNU_COPY_QUERY, false);
	toolBar->EnableTool(MNU_COPY_QUERY, false);
}


void frmStatus::SetColumnImage(ctlListView *list, int col, int image)
{
	wxListItem item;
	item.SetMask(wxLIST_MASK_IMAGE);
	item.SetImage(image);
	list->SetColumn(col, item);
}


void frmStatus::OnSortStatusGrid(wxListEvent &event)
{
	// Get the information for the SQL ORDER BY
	if (statusSortColumn == event.GetColumn() + 1)
	{
		if (statusSortOrder == wxT("ASC"))
			statusSortOrder = wxT("DESC");
		else
			statusSortOrder = wxT("ASC");
	}
	else
	{
		statusSortColumn = event.GetColumn() + 1;
		statusSortOrder = wxT("ASC");
	}


	// Re-initialize all columns' image
	for (int i = 0; i < statusList->GetColumnCount(); i++)
	{
		SetColumnImage(statusList, i, -1);
	}

	// Set the up/down image
	if (statusSortOrder == wxT("ASC"))
		SetColumnImage(statusList, statusSortColumn - 1, 0);
	else
		SetColumnImage(statusList, statusSortColumn - 1, 1);

	// Refresh grid
	wxTimerEvent evt;
	OnRefreshStatusTimer(evt);
}


void frmStatus::OnSortLockGrid(wxListEvent &event)
{
	// Get the information for the SQL ORDER BY
	if (lockSortColumn == event.GetColumn() + 1)
	{
		if (lockSortOrder == wxT("ASC"))
			lockSortOrder = wxT("DESC");
		else
			lockSortOrder = wxT("ASC");
	}
	else
	{
		lockSortColumn = event.GetColumn() + 1;
		lockSortOrder = wxT("ASC");
	}

	// There are no sort operator for xid before 8.3
	if (!connection->BackendMinimumVersion(8, 3) && lockSortColumn == 5)
	{
		wxLogError(_("You cannot sort by transaction id on your PostgreSQL release. You need at least 8.3."));
		lockSortColumn = 1;
	}

	// Re-initialize all columns' image
	for (int i = 0; i < lockList->GetColumnCount(); i++)
	{
		SetColumnImage(lockList, i, -1);
	}

	// Set the up/down image
	if (lockSortOrder == wxT("ASC"))
		SetColumnImage(lockList, lockSortColumn - 1, 0);
	else
		SetColumnImage(lockList, lockSortColumn - 1, 1);

	// Refresh grid
	wxTimerEvent evt;
	OnRefreshLocksTimer(evt);
}


void frmStatus::OnSortXactGrid(wxListEvent &event)
{
	// Get the information for the SQL ORDER BY
	if (xactSortColumn == event.GetColumn() + 1)
	{
		if (xactSortOrder == wxT("ASC"))
			xactSortOrder = wxT("DESC");
		else
			xactSortOrder = wxT("ASC");
	}
	else
	{
		xactSortColumn = event.GetColumn() + 1;
		xactSortOrder = wxT("ASC");
	}

	// There are no sort operator for xid before 8.3
	if (!connection->BackendMinimumVersion(8, 3) && xactSortColumn == 1)
	{
		wxLogError(_("You cannot sort by transaction id on your PostgreSQL release. You need at least 8.3."));
		xactSortColumn = 2;
	}

	// Re-initialize all columns' image
	for (int i = 0; i < xactList->GetColumnCount(); i++)
	{
		SetColumnImage(xactList, i, -1);
	}

	// Set the up/down image
	if (xactSortOrder == wxT("ASC"))
		SetColumnImage(xactList, xactSortColumn - 1, 0);
	else
		SetColumnImage(xactList, xactSortColumn - 1, 1);

	// Refresh grid
	wxTimerEvent evt;
	OnRefreshXactTimer(evt);
}


void frmStatus::OnRightClickStatusGrid(wxListEvent &event)
{
	statusList->PopupMenu(statusPopupMenu, event.GetPoint());
}

void frmStatus::OnRightClickLockGrid(wxListEvent &event)
{
	lockList->PopupMenu(lockPopupMenu, event.GetPoint());
}

void frmStatus::OnRightClickXactGrid(wxListEvent &event)
{
	xactList->PopupMenu(xactPopupMenu, event.GetPoint());
}


void frmStatus::OnChgColSizeStatusGrid(wxListEvent &event)
{
	wxCommandEvent ev;
	OnStatusMenu(ev);
}

void frmStatus::OnChgColSizeLockGrid(wxListEvent &event)
{
	wxCommandEvent ev;
	OnLockMenu(ev);
}

void frmStatus::OnChgColSizeXactGrid(wxListEvent &event)
{
	wxCommandEvent ev;
	OnXactMenu(ev);
}


serverStatusFactory::serverStatusFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
	mnu->Append(id, _("&Server Status"), _("Displays the current database status."));
}


wxWindow *serverStatusFactory::StartDialog(frmMain *form, pgObject *obj)
{

	pgServer *server = obj->GetServer();
	wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Server Status");

	pgConn *conn = server->CreateConn(wxEmptyString, 0, applicationname);
	if (conn)
	{
		wxString txt = _("Server Status - ") + server->GetDescription()
		               + wxT(" (") + server->GetName() + wxT(":") + NumToStr((long)server->GetPort()) + wxT(")");

		frmStatus *status = new frmStatus(form, txt, conn);
		status->Go();
		return status;
	}
	return 0;
}


bool serverStatusFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->GetServer() != 0;
}
