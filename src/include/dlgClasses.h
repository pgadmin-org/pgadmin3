//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgClasses.h - Some dialogue base classes 
//
//////////////////////////////////////////////////////////////////////////

#ifndef DLGCLASSES_H
#define DLGCLASSES_H

#include <wx/wx.h>


#define btnOK                   CTRL_BUTTON("btnOK")
#define btnApply                CTRL_BUTTON("btnApply")
#define btnCancel               CTRL_BUTTON("btnCancel")


class frmMain;
class pgObject;
class pgQueryThread;

class pgDialog : public wxDialog
{
public:
    void RestorePosition(int defaultX=-1, int defaultY=-1, int defaultW=-1, int defaultH=-1, int minW=100, int minH=70);
    void SavePosition();
    void LoadResource(const wxChar *name=0);

protected:
    void OnCancel(wxCommandEvent& ev);
    void OnClose(wxCloseEvent& event);
    wxString dlgName;

    DECLARE_EVENT_TABLE();
};


class pgFrame : public wxFrame
{
public:
    pgFrame(wxFrame *parent, const wxString &title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long flags=wxDEFAULT_FRAME_STYLE) 
        : wxFrame(parent, -1, title, pos, size, flags) {}
    void RestorePosition(int defaultX=-1, int defaultY=-1, int defaultW=-1, int defaultH=-1, int minW=100, int minH=70);
    void SavePosition();

protected:

    wxString dlgName;
};


class DialogWithHelp : public pgDialog
{
public:
    DialogWithHelp(frmMain *frame);

protected:
    frmMain *mainForm;
    void OnHelp(wxCommandEvent& ev);

private:
    virtual wxString GetHelpPage() const = 0;
    DECLARE_EVENT_TABLE();
};

class ExecutionDialog : public DialogWithHelp
{
public:
    ExecutionDialog(frmMain *frame, pgObject *_object);
    virtual wxString GetSql()=0;

    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
    void OnClose(wxCloseEvent& event);

    void Abort();

protected:
    pgObject *object;
    pgQueryThread *thread;
    wxTextCtrl *txtMessages;

private:
    DECLARE_EVENT_TABLE();
};



class wxProcess;
class wxProcessEvent;
class wxTimer;
class wxTimerEvent;
class ExternProcessDialog : public DialogWithHelp
{
public:
    ExternProcessDialog(frmMain *frame);
    ~ExternProcessDialog();
    virtual wxString GetDisplayCmd(int version)=0;
    virtual wxString GetCmd(int step)=0;
    bool Execute(int step=0);
    void Abort();

protected:
    wxTextCtrl *txtMessages;
    wxProcess *process;
    int pid;
    bool done;

    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
    void OnClose(wxCloseEvent& event);
    void OnEndProcess(wxProcessEvent& event);

    void OnPollProcess(wxTimerEvent& event);
    void checkStreams();
    void readStream(wxInputStream *input);

    wxTimer *timer;
    DECLARE_EVENT_TABLE();
};

#endif
