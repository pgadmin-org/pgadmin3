//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
#define chkSingleObject         CTRL_CHECKBOX("chkSingleObject")
#define chkNoOwner              CTRL_CHECKBOX("chkNoOwner")
#define chkDisableTrigger       CTRL_CHECKBOX("chkDisableTrigger")
#define chkVerbose              CTRL_CHECKBOX("chkVerbose")
#define stSingleObject          CTRL_STATIC("stSingleObject")

#define lstContents             CTRL_LISTVIEW("lstContents")
#define btnView                 CTRL_BUTTON("btnView")


BEGIN_EVENT_TABLE(frmRestore, ExternProcessDialog)
    EVT_TEXT(XRCID("txtFilename"),          frmRestore::OnChangeName)
    EVT_CHECKBOX(XRCID("chkOnlyData"),      frmRestore::OnChangeData)
    EVT_CHECKBOX(XRCID("chkOnlySchema"),    frmRestore::OnChangeSchema)
    EVT_CHECKBOX(XRCID("chkSingleObject"),  frmRestore::OnChange)
    EVT_BUTTON(XRCID("btnFilename"),        frmRestore::OnSelectFilename)
    EVT_BUTTON(wxID_OK,                     frmRestore::OnOK)
    EVT_BUTTON(XRCID("btnView"),            frmRestore::OnView)
    EVT_END_PROCESS(-1,                     frmRestore::OnEndProcess)
    EVT_LIST_ITEM_SELECTED(XRCID("lstContents"), frmRestore::OnChangeList)
    EVT_CLOSE(                              ExternProcessDialog::OnClose)
END_EVENT_TABLE()



frmRestore::frmRestore(frmMain *_form, pgObject *obj) : ExternProcessDialog(form)
{
    object=obj;

    if (object->GetType() == PG_SERVER)
        server = (pgServer*)object;
    else
        server=object->GetDatabase()->GetServer();

    form=_form;
    wxLogInfo(wxT("Creating a restore dialogue for %s %s"), object->GetTypeName().c_str(), object->GetFullName().c_str());

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(_form, wxT("frmRestore"));
    RestorePosition();

    SetTitle(wxString::Format(_("Restore %s %s"), object->GetTranslatedTypeName().c_str(), object->GetFullIdentifier().c_str()));


    if (object->GetType() != PG_DATABASE)
    {
        if (object->GetType() == PG_TABLE)
        {
            chkOnlySchema->SetValue(false);
            chkOnlyData->SetValue(true);
        }
        else
        {
            chkOnlySchema->SetValue(true);
            chkOnlyData->SetValue(false);
        }
        chkSingleObject->SetValue(true);
        chkOnlyData->Disable();
        chkOnlySchema->Disable();
        chkSingleObject->Disable();
    }

    // Icon
    SetIcon(wxIcon(restore_xpm));

    txtMessages = CTRL_TEXT("txtMessages");
    txtMessages->SetMaxLength(0L);
    btnOK->Disable();
    filenameValid=false;
    environment.Add(wxT("PGPASSWORD=") + server->GetPassword());

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
    page = wxT("app-pgrestore");
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


void frmRestore::OnChangeData(wxCommandEvent &ev)
{
    if (chkOnlyData->GetValue())
    {
        chkOnlySchema->SetValue(false);
        chkOnlySchema->Disable();
    }
    else
        chkOnlySchema->Enable();

    OnChange(ev);
}


void frmRestore::OnChangeSchema(wxCommandEvent &ev)
{
    if (chkOnlySchema->GetValue())
    {
        chkOnlyData->SetValue(false);
        chkOnlyData->Disable();
    }
    else
        chkOnlyData->Enable();

    OnChange(ev);
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


void frmRestore::OnChangeList(wxListEvent &ev)
{
    OnChange(ev);
}


void frmRestore::OnChange(wxCommandEvent &ev)
{
    bool singleValid = !chkSingleObject->GetValue();
    if (!singleValid)
    {
        switch(object->GetType())
        {
            case PG_DATABASE:
            {
                int sel=lstContents->GetSelection();
                if (sel >= 0)
                {
                    wxString type=lstContents->GetText(sel, 0);
                    if ((type == _("Function") && !chkOnlyData->GetValue()) || 
                        (type == _("Table") && !chkOnlySchema->GetValue()))
                    {
                        singleValid = true;
                        stSingleObject->SetLabel(type + wxT(" ") + lstContents->GetText(sel, 1));
                    }
                }
                break;
            }
            case PG_TABLE:
            case PG_FUNCTION:
            {
                singleValid=true;
                stSingleObject->SetLabel(object->GetTranslatedTypeName() + wxT(" ") + object->GetName());

                break;
            }
        }
    }
    btnOK->Enable(filenameValid && singleValid);
    btnView->Enable(filenameValid);
}


wxString frmRestore::GetCmd(int step)
{
    wxString cmd = getCmdPart1();

    return cmd + getCmdPart2(step);
}


wxString frmRestore::GetDisplayCmd(int step)
{
    wxString cmd = getCmdPart1();

    return cmd + getCmdPart2(step);
}


wxString frmRestore::getCmdPart1()
{
    extern wxString restoreExecutable;

    wxString cmd=restoreExecutable;

    cmd += wxT(" -i")
           wxT(" -h ") + server->GetName()
         + wxT(" -p ") + NumToStr((long)server->GetPort())
         + wxT(" -U ") + qtIdent(server->GetUsername())
         + wxT(" -d ") + qtIdent(object->GetDatabase()->GetName());
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
        {
            cmd.Append(wxT(" -a"));
        }
        else
        {
            if (chkNoOwner->GetValue())
                cmd.Append(wxT(" -O"));
        }

        if (chkOnlySchema->GetValue())
        {
            cmd.Append(wxT(" -s"));
        }
        else
        {
            if (chkDisableTrigger->GetValue())
            cmd.Append(wxT(" --disable-triggers"));
        }

        if (chkSingleObject->GetValue())
        {
            switch (object->GetType())
            {
                case PG_DATABASE:
                {
                    int sel=lstContents->GetSelection();
                    if (lstContents->GetText(sel, 0) == _("Function"))
                        cmd.Append(wxT(" -P ") + qtIdent(lstContents->GetText(sel, 1).BeforeLast('(')));
                    else if (lstContents->GetText(sel, 0) == _("Table"))
                        cmd.Append(wxT(" -t ") + qtIdent(lstContents->GetText(sel, 1)));
                    else
                        return wxT("restore: internal pgadmin error.");   // shouldn't happen!

                    break;
                }
                case PG_TABLE:
                    cmd.Append(wxT(" -t ") + object->GetQuotedIdentifier());
                    break;
                case PG_FUNCTION:
                    cmd.Append(wxT(" -P ") + object->GetQuotedIdentifier());
                    break;
            }
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
    lstContents->DeleteAllItems();
    Execute(1, false);
    btnOK->SetLabel(_("OK"));
    done=0;
}


void frmRestore::OnOK(wxCommandEvent &ev)
{
    viewRunning = false;
    btnView->Disable();

#if 1
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
        done = false;

        lstContents->CreateColumns(0, _("Type"), _("Name"));

        wxString str=wxTextBuffer::Translate(txtMessages->GetValue(), wxTextFileType_Unix);

        wxStringTokenizer line(str, wxT("\n"));
        line.GetNextToken();
        
        lstContents->Freeze();
        wxBeginBusyCursor();

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

        lstContents->Thaw();
        wxEndBusyCursor();
        nbNotebook->SetSelection(1);
    }
}


void frmRestore::Go()
{
    txtFilename->SetFocus();
    Show(true);
}
