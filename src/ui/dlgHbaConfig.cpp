//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgHbaConfig.cpp - Configure setting
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlgHbaConfig.h"
#include "pgConn.h"
#include "pgSet.h"

// Icons
#include "images/property.xpm"





BEGIN_EVENT_TABLE(dlgHbaConfig, DialogWithHelp)
    EVT_BUTTON (wxID_OK,                dlgHbaConfig::OnOK)
    EVT_BUTTON (wxID_CANCEL,            dlgHbaConfig::OnCancel)
    EVT_BUTTON(wxID_REFRESH,	     	dlgHbaConfig::OnAddValue)
    EVT_CHECKBOX(XRCID("chkEnabled"),   dlgHbaConfig::OnChange)
    EVT_COMBOBOX(XRCID("cbType"),       dlgHbaConfig::OnChange)
    EVT_TEXT(XRCID("cbDatabase"),       dlgHbaConfig::OnChange)
    EVT_TEXT(XRCID("cbUser"),           dlgHbaConfig::OnChange)
    EVT_COMBOBOX(XRCID("cbDatabase"),   dlgHbaConfig::OnAddDatabase)
    EVT_COMBOBOX(XRCID("cbUser"),       dlgHbaConfig::OnAddUser)
    EVT_COMBOBOX(XRCID("cbMethod"),     dlgHbaConfig::OnChange)
    EVT_TEXT(XRCID("txtIPaddress"),     dlgHbaConfig::OnChange)
    EVT_TEXT(XRCID("txtOption"),        dlgHbaConfig::OnChange)
END_EVENT_TABLE()


#define chkEnabled          CTRL_CHECKBOX("chkEnabled")
#define cbType              CTRL_COMBOBOX("cbType")
#define cbDatabase          CTRL_COMBOBOX("cbDatabase")
#define cbUser              CTRL_COMBOBOX("cbUser")
#define cbMethod            CTRL_COMBOBOX("cbMethod")
#define txtIPaddress        CTRL_TEXT("txtIPaddress")
#define txtOption           CTRL_TEXT("txtOption")
#define stIPaddress         CTRL_STATIC("stIPaddress")
#define stOption            CTRL_STATIC("stOption")


dlgHbaConfig::dlgHbaConfig(pgFrame *parent, pgHbaConfigLine *_line, pgConn *conn) : 
DialogWithHelp((frmMain*)parent)
{
    wxLogInfo(wxT("Creating a hba config dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource((wxWindow*)parent, wxT("dlgHbaConfig"));

    userAdding = databaseAdding = false;

    // Icon
    SetIcon(wxIcon(property_xpm));
    CenterOnParent();

    line = _line;

    cbType->Append(wxT("local"));
    cbType->Append(wxT("host"));
    cbType->Append(wxT("hostssl"));
    cbType->Append(wxT("hostnossl"));

    cbDatabase->Append(wxT("all"));
    cbDatabase->Append(wxT("sameuser"));
    cbDatabase->Append(wxT("samegroup"));
    cbDatabase->Append(wxT("@<filename>"));

    cbUser->Append(wxT("all"));

    cbMethod->Append(wxT("trust"));
    cbMethod->Append(wxT("reject"));
    cbMethod->Append(wxT("md5"));
    cbMethod->Append(wxT("crypt"));
    cbMethod->Append(wxT("password"));
    cbMethod->Append(wxT("krb4"));
    cbMethod->Append(wxT("krb5"));
    cbMethod->Append(wxT("ident"));
    cbMethod->Append(wxT("pam"));

    if (conn)
    {
        pgSet *set=conn->ExecuteSet(wxT("SELECT datname FROM pg_database"));
        if (set)
        {
            while (!set->Eof())
            {
                cbDatabase->Append(set->GetVal(0));
                set->MoveNext();
            }
            delete set;
        }
        wxString sql = wxT("SELECT 'group ' || groname FROM pg_group");
        if (settings->GetShowUsersForPrivileges())
            sql += wxT("\nUNION\nSELECT usename FROM pg_user");

        set=conn->ExecuteSet(sql);
        if (set)
        {
            while (!set->Eof())
            {
                cbUser->Append(set->GetVal(0));
                set->MoveNext();
            }
            delete set;
        }
    }

    // Setup the default values

    chkEnabled->SetValue(!line->isComment);
    if(line->connectType != pgHbaConfigLine::PGC_INVALIDCONF)
    {
        database = line->database;
        user = line->user;

        cbType->SetSelection(line->connectType);
        cbMethod->SetSelection(line->method);
        cbDatabase->SetValue(database);
        cbUser->SetValue(user);
        txtIPaddress->SetValue(line->ipaddress);
        txtOption->SetValue(line->option);

        wxCommandEvent noEvent;
        OnChange(noEvent);
    }
    else
        btnOK->Disable();
}


dlgHbaConfig::~dlgHbaConfig()
{
    wxLogInfo(wxT("Destroying a hba config dialogue"));
}


wxString dlgHbaConfig::GetHelpPage() const
{
    return wxT("pg/client-authentication");
    // auth-methods#auth-trust #auth-password #kerberos-auth #auth-ident #auth-pam
}


void dlgHbaConfig::OnAddDatabase(wxCommandEvent& ev)
{
    if (cbDatabase->GetSelection() < 3)
        return;

    wxString newDatabase;
    if (database == wxT("all") || database == wxT("sameuser") || database == wxT("samegroup") || database.Left(1) == wxT("@"))
        database = wxEmptyString;

    if (cbDatabase->GetSelection() == 3)    // file
        newDatabase = wxT("@");
    else
    {
        wxString str=cbDatabase->GetValue();
        if (str.Find(' ') >= 0)
            str = wxT("\"") + str + wxT("\"");

        int pos=database.Find(str);
        if (pos >= 0)
        {
            if (pos >0  && database.Mid(pos-1, 1) != wxT(","))
                pos = -1;
            if (database.Length() > str.Length() + pos && database.Mid(pos+str.Length(), 1) != wxT(","))
                pos = -1;
        }
        if (pos < 0)
        {
            if (!database.IsEmpty())
                database += wxT(",");

            newDatabase = database + str;
        }
        else 
            newDatabase = database;
    }

    wxYield();

    database = newDatabase;
    databaseAdding = true;
    wxCommandEvent buttonEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_REFRESH);
    AddPendingEvent(buttonEvent);
}


void dlgHbaConfig::OnAddUser(wxCommandEvent& ev)
{
    if (cbUser->GetSelection() < 1)
        return;

    wxString newUser;

    wxString str=cbUser->GetValue();
    if (str.Left(6) == wxT("group "))
    {
        if (str.Find(' ', true) > 5)
            str = wxT("+\"") + str.Mid(6) + wxT("\"");
        else
            str = wxT("+") + str.Mid(6);
    }
    else
    {
        if (str.Find(' ') >= 0)
            str = wxT("\"") + str + wxT("\"");
    }

    if (user == wxT("all"))
        newUser = str;
    else
    {
        int pos=user.Find(str);
        if (pos >= 0)
        {
            if (pos >0  && user.Mid(pos-1, 1) != wxT(","))
                pos = -1;
            if (user.Length() > str.Length() + pos && user.Mid(pos+str.Length(), 1) != wxT(","))
                pos = -1;
        }

        if (pos < 0)
        {
            if (!user.IsEmpty())
               user += wxT(",");
            newUser = user + str;
        }
        else
            newUser = user;
    }

    wxYield();

    user = newUser;
    userAdding = true;
	wxCommandEvent buttonEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_REFRESH);
    AddPendingEvent(buttonEvent);
}



void dlgHbaConfig::OnAddValue(wxCommandEvent& ev)
{
    if (databaseAdding)
    {
        cbDatabase->SetSelection(-1);
        cbDatabase->SetValue(database);
        cbDatabase->SetInsertionPointEnd();
        databaseAdding = false;
    }

    if (userAdding)
    {
        cbUser->SetSelection(-1);
        cbUser->SetValue(user);
        cbUser->SetInsertionPointEnd();
        userAdding = false;
    }
    OnChange(ev);
}


void dlgHbaConfig::OnChange(wxCommandEvent& ev)
{
    if (databaseAdding || userAdding)
        return;

    database = cbDatabase->GetValue();
    user = cbUser->GetValue();

    bool needIp= (cbType->GetSelection() != 0);

    stIPaddress->Enable(needIp);
    txtIPaddress->Enable(needIp);

    bool needOption = (cbMethod->GetSelection() >= pgHbaConfigLine::PGC_IDENT);
    stOption->Enable(needOption);
    txtOption->Enable(needOption);

    bool ipValid=chkEnabled->GetValue() || cbType->GetSelection() == pgHbaConfigLine::PGC_LOCAL;
    if (!ipValid)
    {

    }
    btnOK->Enable(cbType->GetSelection() >= 0 && !database.IsEmpty() && !user.IsEmpty() && 
            cbMethod->GetSelection() >= 0 && ipValid);
}


void dlgHbaConfig::OnOK(wxCommandEvent& ev)
{
    line->isComment = !chkEnabled->GetValue();
    line->connectType = (pgHbaConfigLine::pgHbaConnectType)cbType->GetSelection();
    line->database = database;
    line->user = user;
    line->ipaddress = txtIPaddress->GetValue();
    line->method = (pgHbaConfigLine::pgHbaMethod)cbMethod->GetSelection();
    line->option = txtOption->GetValue();
    line->changed = true;

    EndModal(wxID_OK);
}


void dlgHbaConfig::OnCancel(wxCommandEvent& ev)
{
    EndModal(wxID_CANCEL);
}


int dlgHbaConfig::Go()
{
    // Set focus on the Password textbox and show modal
    return ShowModal();
}
