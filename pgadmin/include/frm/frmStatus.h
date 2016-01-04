//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmStatus.h - Status Screen
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRMSTATUS_H
#define __FRMSTATUS_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/notebook.h>

// wxAUI
#include <wx/aui/aui.h>

#include "dlg/dlgClasses.h"
#include "utils/factory.h"
#include "ctl/ctlAuiNotebook.h"

enum
{
	CTL_RATECBO = 250,
	CTL_REFRESHBTN,
	CTL_CANCELBTN,
	CTL_TERMINATEBTN,
	CTL_COMMITBTN,
	CTL_ROLLBACKBTN,
	CTL_LOGCBO,
	CTL_ROTATEBTN,
	CTL_STATUSLIST,
	CTL_LOCKLIST,
	CTL_XACTLIST,
	CTL_LOGLIST,
	MNU_STATUSPAGE,
	MNU_LOCKPAGE,
	MNU_XACTPAGE,
	MNU_LOGPAGE,
	MNU_TERMINATE,
	MNU_COMMIT,
	MNU_ROLLBACK,
	MNU_COPY_QUERY,
	MNU_HIGHLIGHTSTATUS,
	TIMER_REFRESHUI_ID,
	TIMER_STATUS_ID,
	TIMER_LOCKS_ID,
	TIMER_XACT_ID,
	TIMER_LOG_ID
};


enum
{
	PANE_STATUS = 1,
	PANE_LOCKS,
	PANE_XACT,
	PANE_LOG
};


//
// This number MUST be incremented if changing any of the default perspectives
//
#define FRMSTATUS_PERSPECTIVE_VER wxT("8274")

#ifdef __WXMAC__
#define FRMSTATUS_DEFAULT_PERSPECTIVE wxT("layout2|name=Activity;caption=Activity;state=6293500;dir=4;layer=0;row=0;pos=0;prop=100000;bestw=321;besth=244;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=462;floaty=165;floatw=595;floath=282|name=Locks;caption=Locks;state=6293500;dir=4;layer=0;row=0;pos=1;prop=100000;bestw=321;besth=244;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-231;floaty=235;floatw=595;floath=282|name=Transactions;caption=Transactions;state=6293500;dir=4;layer=0;row=0;pos=2;prop=100000;bestw=0;besth=0;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=461;floaty=527;floatw=595;floath=282|name=Logfile;caption=Logfile;state=6293500;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=0;besth=0;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-103;floaty=351;floatw=595;floath=282|name=toolBar;caption=Tool bar;state=2124528;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=808;besth=33;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=888;floaty=829;floatw=558;floath=49|dock_size(4,0,0)=583|dock_size(5,0,0)=10|dock_size(1,10,0)=35|")
#else
#ifdef __WXGTK__
#define FRMSTATUS_DEFAULT_PERSPECTIVE wxT("layout2|name=Activity;caption=Activity;state=6293500;dir=4;layer=0;row=0;pos=2;prop=100000;bestw=20;besth=20;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=Locks;caption=Locks;state=6293500;dir=4;layer=0;row=0;pos=1;prop=100000;bestw=20;besth=20;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=Transactions;caption=Transactions;state=6293500;dir=4;layer=0;row=0;pos=0;prop=100000;bestw=20;besth=20;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=Logfile;caption=Logfile;state=6293500;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=20;besth=20;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=toolBar;caption=Tool bar;state=2108144;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=1020;besth=31;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|dock_size(4,0,0)=549|dock_size(5,0,0)=22|dock_size(1,10,0)=33|")
#else
#define FRMSTATUS_DEFAULT_PERSPECTIVE wxT("layout2|name=Activity;caption=Activity;state=6309884;dir=4;layer=0;row=1;pos=0;prop=100000;bestw=20;besth=20;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=174;floaty=216;floatw=578;floath=282|name=Locks;caption=Locks;state=6293500;dir=4;layer=0;row=1;pos=1;prop=100000;bestw=20;besth=20;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=136;floaty=339;floatw=576;floath=283|name=Transactions;caption=Transactions;state=6293500;dir=4;layer=0;row=1;pos=2;prop=100000;bestw=20;besth=20;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=133;floaty=645;floatw=577;floath=283|name=Logfile;caption=Logfile;state=6293500;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=20;besth=20;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=toolBar;caption=Tool bar;state=2108144;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=716;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=586;floaty=525;floatw=483;floath=49|dock_size(5,0,0)=22|dock_size(1,10,0)=25|dock_size(4,0,1)=627|")
#endif
#endif


static wxCriticalSection gs_critsect;


// Class declarations

class frmStatus : public pgFrame
{
public:
	frmStatus(frmMain *form, const wxString &_title, pgConn *conn);
	~frmStatus();
	void Go();

private:
	wxAuiManager manager;

	frmMain *mainForm;
	pgConn *connection, *locks_connection;

	wxString logFormat;
	bool logHasTimestamp, logFormatKnown;
	int logFmtPos;

	wxDateTime logfileTimestamp, latestTimestamp;
	wxString logDirectory, logfileName;

	wxString savedPartialLine;

	bool showCurrent, isCurrent;

	long backend_pid;

	bool loaded;
	long logfileLength;

	int currentPane;

	int statusSortColumn;
	wxString statusSortOrder;
	int lockSortColumn;
	wxString lockSortOrder;
	int xactSortColumn;
	wxString xactSortOrder;

	wxComboBox    *cbRate;
	wxComboBox    *cbLogfiles;
	wxButton      *btnRotateLog;
	ctlComboBoxFix *cbDatabase;

	wxTimer *refreshUITimer;
	wxTimer *statusTimer, *locksTimer, *xactTimer, *logTimer;
	int statusRate, locksRate, xactRate, logRate;

	ctlListView   *statusList;
	ctlListView   *lockList;
	ctlListView   *xactList;
	ctlListView   *logList;

	wxMenu        *actionMenu;
	wxMenu        *statusPopupMenu;
	wxMenu        *lockPopupMenu;
	wxMenu        *xactPopupMenu;

	wxArrayString queries;

	int statusColWidth[12], lockColWidth[10], xactColWidth[5];

	int cboToRate();
	wxString rateToCboString(int rate);

	wxImageList *listimages;

	void AddStatusPane();
	void AddLockPane();
	void AddXactPane();
	void AddLogPane();

	void OnHelp(wxCommandEvent &ev);
	void OnContents(wxCommandEvent &ev);
	void OnExit(wxCommandEvent &event);

	void OnCopy(wxCommandEvent &ev);
	void OnCopyQuery(wxCommandEvent &ev);

	void OnToggleStatusPane(wxCommandEvent &event);
	void OnToggleLockPane(wxCommandEvent &event);
	void OnToggleXactPane(wxCommandEvent &event);
	void OnToggleLogPane(wxCommandEvent &event);
	void OnToggleToolBar(wxCommandEvent &event);
	void OnDefaultView(wxCommandEvent &event);
	void OnHighlightStatus(wxCommandEvent &event);

	void OnRefreshUITimer(wxTimerEvent &event);
	void OnRefreshStatusTimer(wxTimerEvent &event);
	void OnRefreshLocksTimer(wxTimerEvent &event);
	void OnRefreshXactTimer(wxTimerEvent &event);
	void OnRefreshLogTimer(wxTimerEvent &event);

	void SetColumnImage(ctlListView *list, int col, int image);
	void OnSortStatusGrid(wxListEvent &event);
	void OnSortLockGrid(wxListEvent &event);
	void OnSortXactGrid(wxListEvent &event);

	void OnRightClickStatusGrid(wxListEvent &event);
	void OnRightClickLockGrid(wxListEvent &event);
	void OnRightClickXactGrid(wxListEvent &event);

	void OnStatusMenu(wxCommandEvent &event);
	void OnLockMenu(wxCommandEvent &event);
	void OnXactMenu(wxCommandEvent &event);

	void OnChgColSizeStatusGrid(wxListEvent &event);
	void OnChgColSizeLockGrid(wxListEvent &event);
	void OnChgColSizeXactGrid(wxListEvent &event);

	void OnRateChange(wxCommandEvent &event);

	void OnPaneClose(wxAuiManagerEvent &evt);

	void OnClose(wxCloseEvent &event);
	void OnRefresh(wxCommandEvent &event);
	void OnCancelBtn(wxCommandEvent &event);
	void OnStatusCancelBtn(wxCommandEvent &event);
	void OnLocksCancelBtn(wxCommandEvent &event);
	void OnTerminateBtn(wxCommandEvent &event);
	void OnStatusTerminateBtn(wxCommandEvent &event);
	void OnLocksTerminateBtn(wxCommandEvent &event);
	void OnSelStatusItem(wxListEvent &event);
	void OnSelLockItem(wxListEvent &event);
	void OnSelXactItem(wxListEvent &event);
	void OnSelLogItem(wxListEvent &event);
	void OnLoadLogfile(wxCommandEvent &event);
	void OnRotateLogfile(wxCommandEvent &event);
	void OnCommit(wxCommandEvent &event);
	void OnRollback(wxCommandEvent &event);

	void OnChangeDatabase(wxCommandEvent &ev);

	int fillLogfileCombo();
	void emptyLogfileCombo();

	void addLogFile(wxDateTime *dt, bool skipFirst);
	void addLogFile(const wxString &filename, const wxDateTime timestamp, long len, long &read, bool skipFirst);
	void addLogLine(const wxString &str, bool formatted = true, bool csv_log_format = false);

	void checkConnection();

	DECLARE_EVENT_TABLE()
};


class serverStatusFactory : public actionFactory
{
public:
	serverStatusFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif
