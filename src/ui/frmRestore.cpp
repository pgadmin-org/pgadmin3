//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmRestore.cpp - Restore database dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>


// App headers
#include "pgAdmin3.h"
#include "frmRestore.h"
#include "sysLogger.h"
#include "pgSchema.h"
#include "pgTable.h"
#include <wx/process.h>
#include <wx/textbuf.h>
#include <wx/file.h>

// Icons
#include "images/restore.xpm"


#define nbNotebook              CTRL_NOTEBOOK("nbNotebook")
#define txtFilename             CTRL_TEXT("txtFilename")
#define btnFilename             CTRL_BUTTON("btnFilename")
#define chkOnlyData             CTRL_CHECKBOX("chkOnlyData")
#define chkOnlySchema           CTRL_CHECKBOX("chkOnlySchema")
#define chkNoOwner              CTRL_CHECKBOX("chkNoOwner")
#define chkCreateDb             CTRL_CHECKBOX("chkCreateDb")
#define chkDropDb               CTRL_CHECKBOX("chkDropDb")
#define chkDisableTrigger       CTRL_CHECKBOX("chkDisableTrigger")
#define chkVerbose              CTRL_CHECKBOX("chkVerbose")

#define lstContents             CTRL_LISTVIEW("lstContents")
#define btnView                 CTRL_BUTTON("btnView")


BEGIN_EVENT_TABLE(frmRestore, ExternProcessDialog)
    EVT_TEXT(XRCID("txtFilename"),          frmRestore::OnChangeName)
    EVT_BUTTON(XRCID("btnFilename"),        frmRestore::OnSelectFilename)
    EVT_BUTTON(XRCID("btnOK"),              frmRestore::OnOK)
    EVT_BUTTON(XRCID("btnView"),            frmRestore::OnView)
    EVT_END_PROCESS(-1,                     frmRestore::OnEndProcess)
    EVT_CLOSE(                              ExternProcessDialog::OnClose)
END_EVENT_TABLE()



frmRestore::frmRestore(frmMain *_form, pgObject *obj) : ExternProcessDialog(form)
{
    object=obj;
    form=_form;
    wxLogInfo(wxT("Creating a restore dialogue for %s %s"), object->GetTypeName().c_str(), object->GetFullName().c_str());

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(wxT("frmRestore"));
    RestorePosition();

    SetTitle(wxString::Format(_("Restore %s %s"), object->GetTranslatedTypeName().c_str(), object->GetFullIdentifier().c_str()));


    // Icon
    SetIcon(wxIcon(restore_xpm));

    txtMessages = CTRL_TEXT("txtMessages");
    txtMessages->SetMaxLength(0L);
    btnOK->Disable();
    filenameValid=false;

    wxCommandEvent ev;
    OnChange(ev);
    CenterOnParent();
}


frmRestore::~frmRestore()
{
    wxLogInfo(wxT("Destroying a restore dialogue"));
    SavePosition();
}


wxString frmRestore::GetHelpPage() const
{
    wxString page;
    page = wxT("app-pgdump");
    return page;
}


void frmRestore::OnSelectFilename(wxCommandEvent &ev)
{
    wxFileDialog file(this, _("Select backup filename"), wxGetHomeDir(), txtFilename->GetValue(), 
        _("Backup files (*.backup)|*.backup|All files (*.*)|*.*"));

    if (file.ShowModal() == wxID_OK)
    {
        txtFilename->SetValue(file.GetPath());
        OnChange(ev);
    }
}


void frmRestore::OnChangeName(wxCommandEvent &ev)
{
    wxString name=txtFilename->GetValue();
    if (name.IsEmpty() || !wxFile::Exists(name))
        filenameValid=false;
    else
    {
        wxFile file(name, wxFile::read);
        if (file.IsOpened())
        {
            char buffer[8];
            off_t size=file.Read(buffer, 8);
            if (size == 8)
            {
                if (memcmp(buffer, "PGDMP", 5) && !memcmp(buffer, "toc.dat", 8))
                {
                    // tar format?
                    file.Seek(512);
                    size=file.Read(buffer, 8);
                }
                if (size == 8 && !memcmp(buffer, "PGDMP", 5))
                {
                    // check version here?
                    filenameValid=true;
                }
            }
        }
    }
    OnChange(ev);
}


void frmRestore::OnChange(wxCommandEvent &ev)
{
    btnOK->Enable(filenameValid);
    btnView->Enable(filenameValid);
}


wxString frmRestore::GetCmd(int step)
{
    wxString cmd = getCmdPart1();
    pgServer *server=object->GetDatabase()->GetServer();

    if (!server->GetTrusted())
        cmd += wxT(" -W ") + server->GetPassword();

    return cmd + getCmdPart2(step);
}


wxString frmRestore::GetDisplayCmd(int step)
{
    wxString cmd = getCmdPart1();
    pgServer *server=object->GetDatabase()->GetServer();

    if (!server->GetTrusted())
        cmd += wxT(" -W ****");

    return cmd + getCmdPart2(step);
}


wxString frmRestore::getCmdPart1()
{
    extern wxString restoreExecutable;

    wxString cmd=restoreExecutable;

    pgServer *server=object->GetDatabase()->GetServer();
    cmd += wxT(" -h ") + server->GetName()
         +  wxT(" -p ") + NumToStr((long)server->GetPort())
         +  wxT(" -U ") + server->GetUsername();
    return cmd;
}


wxString frmRestore::getCmdPart2(int step)
{
    wxString cmd;
    // if (server->GetSSL())
    // pg_dump doesn't support ssl

    if (step)
    {
        cmd.Append(wxT(" -l"));
    }
    else
    {
        if (chkOnlyData->GetValue())
            cmd.Append(wxT(" -a"));
        else
        {
            if (chkOnlySchema->GetValue())
                cmd.Append(wxT(" -s"));
            if (chkOnlySchema->GetValue())
                cmd.Append(wxT(" -s"));
            if (chkNoOwner->GetValue())
                cmd.Append(wxT(" -O"));
            if (chkCreateDb->GetValue())
                cmd.Append(wxT(" -C"));
            if (chkDropDb->GetValue())
                cmd.Append(wxT(" -c"));
            if (chkDisableTrigger->GetValue())
                cmd.Append(wxT(" --disable-triggers"));
        }
        if (chkVerbose->GetValue())
            cmd.Append(wxT(" -v"));
    }


    cmd.Append(wxT(" \"") + txtFilename->GetValue() + wxT("\""));

    return cmd;
}


void frmRestore::OnView(wxCommandEvent &ev)
{
    btnView->Disable();
    btnOK->Disable();
    viewRunning = true;
    Execute(1);
}


void frmRestore::OnOK(wxCommandEvent &ev)
{
    viewRunning = false;
    btnView->Disable();

#if 0
    ExternProcessDialog::OnOK(ev);
#else   // demo mode
    if (!done)
    {
        done=true;
        btnOK->SetLabel(_("Done"));
    }
    else
    {
        pgDialog::OnCancel(ev);
    }
#endif
}


void frmRestore::OnEndProcess(wxProcessEvent& ev)
{
    ExternProcessDialog::OnEndProcess(ev);

    if (done && viewRunning && !ev.GetExitCode())
    {
        lstContents->CreateColumns(form, _("Type"), _("Name"));

        wxString str=wxTextBuffer::Translate(txtMessages->GetValue(), wxTextFileType_Unix);

        wxStringTokenizer line(str, wxT("\n"));
        line.GetNextToken();
        while (line.HasMoreTokens())
        {
            str=line.GetNextToken();
            if (str.Left(2) == wxT(";"))
                continue;

            wxStringTokenizer col(str, wxT(" "));
            col.GetNextToken();
            if (!StrToLong(col.GetNextToken().c_str()))
                continue;
            col.GetNextToken();
            wxString type=col.GetNextToken();

            int id=-1;
            int icon = -1;
            wxString typname;

            if (type == wxT("PROCEDURAL"))
            {
                id=PG_LANGUAGE;
                type = col.GetNextToken();
            }
            else if (type == wxT("FK"))
            {
                id=PG_FOREIGNKEY;
                type = col.GetNextToken();
            }
            else if (type == wxT("CONSTRAINT"))
            {
                typname = _("Constraint");
                icon = PGICON_CONSTRAINT;
            }
            else
                id = pgObject::GetTypeId(type);

            wxString name = str.Mid(str.Find(type)+type.Length()+1).BeforeLast(' ');
            if (id >= 0)
            {
                typname = wxGetTranslation(typesList[id].typName);
                icon = typesList[id].typeIcon;
            }
            else if (typname.IsEmpty())
                typname = type;

            lstContents->AppendItem(icon, typname, name);
        }

        nbNotebook->SetSelection(1);
    }
}


void frmRestore::Go()
{
    txtFilename->SetFocus();
    Show(true);
}
