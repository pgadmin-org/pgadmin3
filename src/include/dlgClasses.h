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
#define btnCancel               CTRL_BUTTON("btnCancel")



class frmMain;

class DialogWithHelp : public wxDialog
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
#endif
