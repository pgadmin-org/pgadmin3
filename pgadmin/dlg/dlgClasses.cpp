//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgClasses.cpp - Some dialogue base classes
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "db/pgConn.h"
#include "schema/pgObject.h"
#include "schema/pgDatabase.h"
#include "utils/sysProcess.h"
#include "frm/menu.h"
#include "db/pgQueryThread.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(windowList);

#define STATUSBAR_XRCNAME   wxT("unkStatusBar")
#define STATUSBAR_CONTAINER STATUSBAR_XRCNAME wxT("_container")


BEGIN_EVENT_TABLE(pgDialog, wxDialog)
	EVT_BUTTON (wxID_CANCEL,            pgDialog::OnCancel)
	EVT_CLOSE(                          pgDialog::OnClose)
END_EVENT_TABLE()



void pgDialog::AddStatusBar()
{
	if (!statusBar)
	{
		long flags = 0;
		if (GetWindowStyle() & wxRESIZE_BORDER)
			flags = wxST_SIZEGRIP;
		statusBar = new wxStatusBar(this, -1, flags);

		wxWindow *statusBarContainer = FindWindow(STATUSBAR_CONTAINER);

		if (statusBarContainer)
		{
			wxXmlResource::Get()->AttachUnknownControl(STATUSBAR_XRCNAME, statusBar);
		}
		else
		{
			int sbHeight = statusBar->GetSize().y;
			wxSize size = GetSize();
			size.y += sbHeight;
			SetSize(size);

			size = GetClientSize();
			statusBar->Move(0, size.y - sbHeight);
		}
	}
}


void pgDialog::PostCreation()
{
	if (!statusBar && FindWindow(STATUSBAR_CONTAINER))
		AddStatusBar();

	if (GetWindowStyle() & wxRESIZE_BORDER)   // is designed with sizers; don't change
		return;

	if (!btnCancel)
		return;

	wxSize  size = btnCancel->GetSize();
	wxPoint pos = btnCancel->GetPosition();
	int height = pos.y + size.GetHeight() + ConvertDialogToPixels(wxSize(0, 3)).y;
	if (statusBar)
		height += statusBar->GetSize().GetHeight();

	int right = pos.x + ConvertDialogToPixels(wxSize(50, 0)).x - size.GetWidth();
	btnCancel->Move(right, pos.y);

	if (btnOK)
	{
		size = btnOK->GetSize();
		right -= size.GetWidth() + ConvertDialogToPixels(wxSize(3, 0)).x;
		btnOK->Move(right, pos.y);
	}

// On OS X, reverse the buttons for UI consistency
#ifdef __WXMAC__
	wxPoint pos2;
	pos = btnCancel->GetPosition();

	if (btnOK)
	{
		pos2 = btnOK->GetPosition();
		btnOK->Move(pos.x, pos.y);
	}

	btnCancel->Move(pos2.x, pos2.y);
#endif

	int w, h;
	size = GetSize();
	GetClientSize(&w, &h);

	SetSize(size.GetWidth(), size.GetHeight() + height - h);
}


void pgDialog::RestorePosition(int defaultX, int defaultY, int defaultW, int defaultH, int minW, int minH)
{
	if (minW == -1)
		minW = GetSize().GetX();

	if (minH == -1)
		minH = GetSize().GetY();

	wxPoint pos(settings->Read(dlgName, wxPoint(defaultX, defaultY)));
	wxSize size;

	if ((this->GetWindowStyle() & wxRESIZE_BORDER) == wxRESIZE_BORDER)
	{
		if (defaultW < 0)
			size = settings->Read(dlgName, GetSize());
		else
			size = settings->Read(dlgName, wxSize(defaultW, defaultH));
	}
	else
	{
		size = GetSize();
	}

	bool posDefault = (pos.x == -1 && pos.y == -1);

	CheckOnScreen(this, pos, size, minW, minH);
	SetSize(pos.x, pos.y, size.x, size.y);
	if (posDefault)
		CenterOnParent();
}

void pgDialog::SavePosition()
{
#ifndef __WXGTK__
	if (!IsIconized())
	{
#endif
		wxPoint pos = GetPosition();
		if (pos.x < 0)
			pos.x = 0;
		if (pos.y < 0)
			pos.y = 0;
		settings->WriteSizePoint(dlgName, GetSize(), pos);
#ifndef __WXGTK__
	}
#endif
}

void pgDialog::LoadResource(wxWindow *parent, const wxChar *name)
{
	if (name)
		dlgName = name;
	wxXmlResource::Get()->LoadDialog(this, parent, dlgName);
	PostCreation();
}



void pgDialog::OnClose(wxCloseEvent &event)
{
	if (IsModal())
		EndModal(wxID_CANCEL);
	else
		Destroy();
}


void pgDialog::OnCancel(wxCommandEvent &ev)
{
	if (IsModal())
		EndModal(wxID_CANCEL);
	else
		Destroy();
}


///////////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(pgFrame, wxFrame)
	EVT_MENU(MNU_EXIT,                  pgFrame::OnExit)
	EVT_MENU(MNU_RECENT + 1,              pgFrame::OnRecent)
	EVT_MENU(MNU_RECENT + 2,              pgFrame::OnRecent)
	EVT_MENU(MNU_RECENT + 3,              pgFrame::OnRecent)
	EVT_MENU(MNU_RECENT + 4,              pgFrame::OnRecent)
	EVT_MENU(MNU_RECENT + 5,              pgFrame::OnRecent)
	EVT_MENU(MNU_RECENT + 6,              pgFrame::OnRecent)
	EVT_MENU(MNU_RECENT + 7,              pgFrame::OnRecent)
	EVT_MENU(MNU_RECENT + 8,              pgFrame::OnRecent)
	EVT_MENU(MNU_RECENT + 9,              pgFrame::OnRecent)
#ifdef __WXGTK__
	EVT_KEY_DOWN(                       pgFrame::OnKeyDown)
#endif
END_EVENT_TABLE()


pgFrame::pgFrame(wxFrame *parent, const wxString &title, const wxPoint &pos, const wxSize &size, long flags)
	: wxFrame(parent, -1, title, pos, size, flags)
{
	changed = false;
	recentFileMenu = 0;
	menuFactories = 0;
}


pgFrame::~pgFrame()
{
	if (!dlgName.IsEmpty())
		SavePosition();
	if (menuFactories)
		delete menuFactories;

}


void pgFrame::RemoveFrame(wxWindow *frame)
{
	frames.DeleteObject(frame);
}


void pgFrame::OnAction(wxCommandEvent &ev)
{
	actionFactory *af = menuFactories->GetFactory(ev.GetId());
	if (af)
	{
		wxWindow *wnd = af->StartDialog((frmMain *)this, 0);
		if (wnd)
			AddFrame(wnd);
	}
}


// Event handlers
void pgFrame::OnKeyDown(wxKeyEvent &event)
{
	event.m_metaDown = false;
	event.Skip();
}


void pgFrame::OnExit(wxCommandEvent &event)
{
	Close();
}


void pgFrame::OnHelp(wxCommandEvent &WXUNUSED(event))
{
	wxString page = GetHelpPage();
	DisplayHelp(page, HELP_PGADMIN);
}


void pgFrame::OnRecent(wxCommandEvent &event)
{
	int fileNo = event.GetId() - MNU_RECENT;
	wxString newPath = settings->Read(recentKey + wxString::Format(wxT("/%d"), fileNo), wxT(""));

	if (!newPath.IsNull())
	{
		if (CheckChanged(true))
			return;

		lastPath = newPath;
		int dirsep;
		dirsep = lastPath.Find(wxFILE_SEP_PATH, true);
		lastDir = lastPath.Mid(0, dirsep);
		lastFilename = lastPath.Mid(dirsep + 1);
		OpenLastFile();
	}
}



void pgFrame::UpdateRecentFiles(bool updatefile)
{
	if (!recentFileMenu)
		return;

	if (recentKey.IsEmpty())
		recentKey = dlgName + wxT("/RecentFiles");

	wxString lastFiles[10]; // 0 will be unused for convenience
	int i, maxFiles = 9;
	int recentIndex = maxFiles;

	for (i = 1 ; i <= maxFiles ; i++)
	{
		lastFiles[i] = settings->Read(recentKey + wxString::Format(wxT("/%d"), i), wxT(""));
		if (!lastPath.IsNull() && lastPath.IsSameAs(lastFiles[i], wxARE_FILENAMES_CASE_SENSITIVE))
			recentIndex = i;
	}
	while (i <= maxFiles)
		lastFiles[i++] = wxT("");

	if (recentIndex > 1 && !lastPath.IsNull() && updatefile)
	{
		for (i = recentIndex ; i > 1 ; i--)
			lastFiles[i] = lastFiles[i - 1];
		lastFiles[1] = lastPath;
	}

	i = recentFileMenu->GetMenuItemCount();
	while (i)
	{
		wxMenuItem *item = recentFileMenu->Remove(MNU_RECENT + i);
		if (item)
			delete item;
		i--;
	}

	for (i = 1 ; i <= maxFiles ; i++)
	{
		if (updatefile)
			settings->Write(recentKey + wxString::Format(wxT("/%d"), i), lastFiles[i]);

		if (!lastFiles[i].IsNull())
			recentFileMenu->Append(MNU_RECENT + i, wxT("&") + wxString::Format(wxT("%d"), i) + wxT("  ") + lastFiles[i]);
	}
}


void pgFrame::RestorePosition(int defaultX, int defaultY, int defaultW, int defaultH, int minW, int minH)
{
	bool maximized = false;
	wxPoint pos(settings->Read(dlgName, wxPoint(defaultX, defaultY)));
	wxSize size;
	if (defaultW < 0)
		size = GetSize();
	else
		size = settings->Read(dlgName, wxSize(defaultW, defaultH));

	bool posDefault = (pos.x == -1 && pos.y == -1);

	CheckOnScreen(this, pos, size, minW, minH);
	SetSize(pos.x, pos.y, size.x, size.y);
	if (posDefault)
		CenterOnParent();

	settings->Read(dlgName + wxT("/Maximized"), &maximized, false);
	if (maximized)
		Maximize();
}


void pgFrame::SavePosition()
{
#ifndef __WXGTK__
	if (!IsIconized())
	{
#endif
		wxPoint pos = GetPosition();
		if (pos.x < 0)
			pos.x = 0;
		if (pos.y < 0)
			pos.y = 0;
		settings->WriteSizePoint(dlgName, GetSize(), pos);
		settings->WriteBool(dlgName + wxT("/Maximized"), IsMaximized());
#ifndef __WXGTK__
	}
#endif
}



//////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(DialogWithHelp, pgDialog)
	EVT_MENU(MNU_HELP,                  DialogWithHelp::OnHelp)
	EVT_BUTTON(wxID_HELP,               DialogWithHelp::OnHelp)
END_EVENT_TABLE();


DialogWithHelp::DialogWithHelp(frmMain *frame) : pgDialog()
{
	mainForm = frame;

	wxAcceleratorEntry entries[2];
	entries[0].Set(wxACCEL_NORMAL, WXK_F1, MNU_HELP);
// this is for GTK because Meta (usually Numlock) is interpreted like Alt
// there are too many controls to reset m_Meta in all of them
	entries[1].Set(wxACCEL_ALT, WXK_F1, MNU_HELP);
	wxAcceleratorTable accel(2, entries);

	SetAcceleratorTable(accel);
}


void DialogWithHelp::OnHelp(wxCommandEvent &ev)
{
	wxString page = GetHelpPage();

	if (!page.IsEmpty())
	{
		if (page.StartsWith(wxT("pg/")))
			DisplayHelp(page.Mid(3), HELP_POSTGRESQL);
		else if (page.StartsWith(wxT("slony/")))
			DisplayHelp(page.Mid(6), HELP_SLONY);
		else
			DisplayHelp(page, HELP_PGADMIN);
	}
}



////////////////////////////////////////////////////////////////77


BEGIN_EVENT_TABLE(ExecutionDialog, DialogWithHelp)
	EVT_BUTTON (wxID_OK,                ExecutionDialog::OnOK)
	EVT_BUTTON (wxID_CANCEL,            ExecutionDialog::OnCancel)
	EVT_CLOSE(                          ExecutionDialog::OnClose)
END_EVENT_TABLE()


ExecutionDialog::ExecutionDialog(frmMain *frame, pgObject *_object) : DialogWithHelp(frame)
{
	thread = 0;
	object = _object;
	txtMessages = 0;
	bIsAborted = false;
	bIsExecutionStarted = false;
	bIsExecutionCompleted = false;

	pgDatabase *db = object->GetDatabase();
	wxString applicationname = appearanceFactory->GetLongAppName() + _(" - Execution Tool");
	conn = db->CreateConn(applicationname);
}

void ExecutionDialog::EnableOK(const bool enable)
{
	btnOK->Enable(enable);
}


void ExecutionDialog::OnClose(wxCloseEvent &event)
{
	Abort();
	// If execution is started and not yet complete it means thread is still
	// running, so we should not delete the connection object and destroy the dialog.
	if(bIsExecutionStarted && !bIsExecutionCompleted)
		return;

	delete conn;
	if (IsModal())
		EndModal(-1);
	else
		Destroy();
}


void ExecutionDialog::OnCancel(wxCommandEvent &ev)
{
	if (thread)
	{
		btnCancel->Disable();
		Abort();
		btnCancel->Enable();
		btnOK->Enable();
	}
	else
	{
		delete conn;
		if (IsModal())
			EndModal(-1);
		else
			Destroy();
	}
}


void ExecutionDialog::Abort()
{
	// If bIsAborted is true it means abort is still in progress
	if (thread && !bIsAborted)
	{
		bIsAborted = true;
		if (thread->IsRunning())
		{
			thread->CancelExecution();
			thread->Wait();
		}

		delete thread;
		thread = 0;
	}
}


void ExecutionDialog::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	if (!thread)
	{
		wxString sql = GetSql();
		if (sql.IsEmpty())
			return;

		btnOK->Disable();
		// Reset the variables
		bIsAborted = false;
		bIsExecutionStarted = false;
		bIsExecutionCompleted = false;

		thread = new pgQueryThread(conn, sql);
		if (thread->Create() != wxTHREAD_NO_ERROR)
		{
			Abort();
			return;
		}

		wxLongLong startTime = wxGetLocalTimeMillis();
		thread->Run();
		// When execution is started then set the variable
		bIsExecutionStarted = true;
		wxNotebook *nb = CTRL_NOTEBOOK("nbNotebook");
		if (nb)
			nb->SetSelection(nb->GetPageCount() - 1);

		while (thread && thread->IsRunning())
		{
			wxMilliSleep(10);
			// here could be the animation
			if (txtMessages)
			{
				wxString msg = thread->GetMessagesAndClear();
				if (!msg.IsEmpty())
					txtMessages->AppendText(msg + wxT("\n"));
			}

			wxTheApp->Yield(true);
		}

		if (thread)
		{
			bool isOk = (thread->ReturnCode() == PGRES_COMMAND_OK || thread->ReturnCode() == PGRES_TUPLES_OK);

			if (txtMessages)
			{
				txtMessages->AppendText(thread->GetMessagesAndClear());
			}

			if (thread->DataSet() != NULL)
			{
				wxLogInfo(wxString::Format(wxT("%d rows."), thread->DataSet()->NumRows()));
			}

			if (isOk)
			{
				if (txtMessages)
					txtMessages->AppendText(
					    _("Total query runtime: ") +
					    ElapsedTimeToStr(
					        wxGetLocalTimeMillis() - startTime
					    )
					);

				btnOK->SetLabel(_("Done"));
				btnCancel->Disable();
			}
			else
			{
				if (txtMessages)
					txtMessages->AppendText(conn->GetLastError());
				Abort();
			}
		}
		else if (txtMessages)
			txtMessages->AppendText(_("\nCancelled.\n"));

		btnOK->Enable();
		bIsExecutionCompleted = true;
	}
	else
	{
		Abort();
		delete conn;
		Destroy();
	}
}



#define TIMER_ID 4442

BEGIN_EVENT_TABLE(ExternProcessDialog, DialogWithHelp)
	EVT_BUTTON(wxID_OK,                     ExternProcessDialog::OnOK)
	EVT_BUTTON(wxID_CANCEL,                 ExternProcessDialog::OnCancel)
	EVT_CLOSE(                              ExternProcessDialog::OnClose)
	EVT_END_PROCESS(-1,                     ExternProcessDialog::OnEndProcess)
	EVT_TIMER(TIMER_ID,                     ExternProcessDialog::OnPollProcess)
END_EVENT_TABLE()



ExternProcessDialog::ExternProcessDialog(frmMain *frame) : DialogWithHelp(frame)
{
	txtMessages = 0;
	process = 0;
	done = false;
	processID = 0;

	timer = new wxTimer(this, TIMER_ID);
}



ExternProcessDialog::~ExternProcessDialog()
{
	Abort();
	delete timer;
}


void ExternProcessDialog::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif
	if (!done)
		Execute(0);
	else
	{
		Abort();
		pgDialog::OnCancel(ev);
	}
}


bool ExternProcessDialog::Execute(int step, bool finalStep)
{
	btnOK->Disable();
	final = finalStep;

	if (txtMessages)
		txtMessages->AppendText(GetDisplayCmd(step) + END_OF_LINE);

	if (process)
	{
		delete process;
		process = NULL;
		processID = 0;
	}

	process = new sysProcess(this);
	process->SetEnvironment(environment);

	processID = wxExecute(GetCmd(step), wxEXEC_ASYNC, process);

	if (processID)
	{
		wxNotebook *nb = CTRL_NOTEBOOK("nbNotebook");
		if (nb)
			nb->SetSelection(nb->GetPageCount() - 1);
		if (txtMessages)
		{
			checkStreams();
			timer->Start(100L);
		}
		return true;
	}
	else
	{
		delete process;
		process = NULL;
		processID = 0;
		return false;
	}
}


void ExternProcessDialog::OnCancel(wxCommandEvent &ev)
{
	if (process)
	{
		btnCancel->Disable();
		Abort();
		wxMilliSleep(100);
		if (txtMessages)
			txtMessages->AppendText(END_OF_LINE + wxString(_("Cancelled on user request.")) + END_OF_LINE + END_OF_LINE);
	}
	else
		pgDialog::OnCancel(ev);
}


void ExternProcessDialog::OnClose(wxCloseEvent &ev)
{
	btnCancel->Disable();
	Abort();
	pgDialog::OnClose(ev);
}


void ExternProcessDialog::OnPollProcess(wxTimerEvent &event)
{
	checkStreams();
}


void ExternProcessDialog::checkStreams()
{
	if (txtMessages && process)
	{
		txtMessages->AppendText(process->ReadErrorStream());
		txtMessages->AppendText(process->ReadInputStream());
	}
}


void ExternProcessDialog::OnEndProcess(wxProcessEvent &ev)
{
	if (process)
	{
		if (!ev.GetExitCode())
		{
			if (final)
				btnOK->SetLabel(_("Done"));
			done = true;
		}
	}
	timer->Stop();

	if (txtMessages)
	{
		checkStreams();
		txtMessages->AppendText(END_OF_LINE
		                        + wxString::Format(_("Process returned exit code %d."), ev.GetExitCode())
		                        + END_OF_LINE);
	}

	if (process)
	{
		delete process;
		processID = 0;
		process = 0;
	}
	btnOK->Enable();
	btnCancel->Enable();
}


void ExternProcessDialog::Abort()
{
	timer->Stop();
	if (process)
	{
		done = false;
		// Kill the child process started with wxExecute in Execute method
		wxKill(processID);
		processID = 0;
	}
}

