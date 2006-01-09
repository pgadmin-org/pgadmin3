//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmExport.cpp - The export file dialogue
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"
#include <wx/file.h>
#include "frmExport.h"
#include "ctl/ctlSQLResult.h"
#include "sysSettings.h"
#include "misc.h"


#define txtFilename     CTRL_TEXT("txtFilename")
#define btnOK           CTRL_BUTTON("wxID_OK")
#define rbUnicode       CTRL_RADIOBUTTON("rbUnicode")
#define rbLocal         CTRL_RADIOBUTTON("rbLocal")
#define rbCRLF          CTRL_RADIOBUTTON("rbCRLF")
#define rbLF            CTRL_RADIOBUTTON("rbLF")
#define rbQuoteStrings  CTRL_RADIOBUTTON("rbQuoteStrings")
#define rbQuoteAll      CTRL_RADIOBUTTON("rbQuoteAll")
#define rbQuoteNone     CTRL_RADIOBUTTON("rbQuoteNone")
#define chkColnames     CTRL_CHECKBOX("chkColnames")
#define cbColSeparator  CTRL_COMBOBOX("cbColSeparator")
#define cbQuoteChar     CTRL_COMBOBOX("cbQuoteChar")


BEGIN_EVENT_TABLE(frmExport, pgDialog)
    EVT_TEXT(XRCID("txtFilename"),          frmExport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbQuoteNone"),   frmExport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbQuoteStrings"),frmExport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbQuoteAll"),    frmExport::OnChange)
    EVT_BUTTON(XRCID("btnFilename"),        frmExport::OnBrowseFile)
    EVT_BUTTON(wxID_HELP,                   frmExport::OnHelp)
    EVT_BUTTON(wxID_OK,                     frmExport::OnOK)
    EVT_BUTTON(wxID_CANCEL,                 frmExport::OnCancel)
END_EVENT_TABLE()



frmExport::frmExport(wxWindow *p)
{
    parent=p;
    wxLogInfo(wxT("Creating the export dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(p, wxT("frmExport")); 

    // Icon
    appearanceFactory->SetIcons(this);
    CenterOnParent();
    cbQuoteChar->Disable();
    btnOK->Disable();


    bool uc=settings->GetExportUnicode();
    rbUnicode->SetValue(uc);
    rbLocal->SetValue(!uc);

    bool isCrLf=settings->GetExportRowSeparator() == wxT("\r\n");
    rbCRLF->SetValue(isCrLf);
    rbLF->SetValue(!isCrLf);

    int qt=settings->GetExportQuoting();
    
    rbQuoteNone->SetValue(qt == 0);
    rbQuoteStrings->SetValue(qt == 1);
    rbQuoteAll->SetValue(qt == 2);

    cbColSeparator->SetValue(settings->GetExportColSeparator());


    cbQuoteChar->SetValue(settings->GetExportQuoteChar());

    wxString val;
    settings->Read(wxT("Export/LastFile"), &val, wxEmptyString);
    txtFilename->SetValue(val);

    wxCommandEvent ev;
    OnChange(ev);
}


frmExport::~frmExport()
{
    wxLogInfo(wxT("Destroying the export dialogue"));
}


void frmExport::OnHelp(wxCommandEvent &ev)
{
    DisplayHelp(this, wxT("export"));
}


void frmExport::OnChange(wxCommandEvent &ev)
{
    cbQuoteChar->Enable(rbQuoteStrings->GetValue() || rbQuoteAll->GetValue());
    btnOK->Enable(!txtFilename->GetValue().IsEmpty() && !cbColSeparator->GetValue().IsEmpty());
}


void frmExport::OnOK(wxCommandEvent &ev)
{
    settings->SetExportUnicode(rbUnicode->GetValue());
    settings->SetExportRowSeparator(rbCRLF->GetValue() ? wxT("\r\n") : wxT("\n"));
    settings->SetExportColSeparator(cbColSeparator->GetValue());

    if (rbQuoteAll->GetValue())
        settings->SetExportQuoting(2);
    else if (rbQuoteStrings->GetValue())
        settings->SetExportQuoting(1);
    else
        settings->SetExportQuoting(0);

    settings->SetExportQuoteChar(cbQuoteChar->GetValue());

    settings->Write(wxT("Export/LastFile"), txtFilename->GetValue());


    if (IsModal())
        EndModal(wxID_OK);
    else
        Destroy();
}

    

bool frmExport::Export(ctlSQLResult *data, pgSet *set)
{
    wxFile file(txtFilename->GetValue(), wxFile::write);
    if (!file.IsOpened())
    {
        wxLogError(__("Failed to open file %s."), txtFilename->GetValue().c_str());
        return false;
    }

    wxString line;

    int startCol=0;
    int colCount, rowCount;

    if (set)
    {
        colCount = set->NumCols();
        rowCount = set->NumRows();
    }
    else
    {
        colCount = data->GetColumnCount();
        rowCount = data->GetItemCount();
        if (colCount > 1) // first col contains row count
            startCol=1;
    }

    int col;
    if (chkColnames->GetValue())
    {
        for (col=startCol ; col < colCount ; col++)
        {
            if (col == startCol)
                line=wxEmptyString;
            else
                line += cbColSeparator->GetValue();
            if (set)
                line += set->ColName(col);
            else
                line += data->colNames.Item(col);
        }
        if (rbCRLF->GetValue())
            line += wxT("\r\n");
        else
            line += wxT("\n");

        if (rbUnicode->GetValue())
            file.Write(line, wxConvUTF8);
        else
            file.Write(line, wxConvLibc);
    }


    wxString text;
    OID typOid;

    int row;
    for (row=0 ; row < rowCount ; row++)
    {
        for (col=startCol ; col < colCount ; col++)
        {
            if (col == startCol)
                line=wxEmptyString;
            else
                line += cbColSeparator->GetValue();

            bool needQuote=rbQuoteAll->GetValue();

            if (set)
            {
                text = set->GetVal(col);
                typOid = set->ColTypeOid(col);
            }
            else
            {
                text = data->GetItemText(row, col);
                typOid = data->colTypClasses.Item(col);
            }
            if (!needQuote && rbQuoteStrings->GetValue())
            {
                // find out if string
                switch (typOid)
                {
                    case PGTYPCLASS_NUMERIC:
                    case PGTYPCLASS_BOOL:
                        break;
                    default:
                        needQuote=true;
                        break;
                }
            }
            if (needQuote)
            {
                wxString qc = cbQuoteChar->GetValue();
                text.Replace(qc, qc+qc);
                line += qc + text + qc;
            }
            else
                line += text;
        }
        if (rbCRLF->GetValue())
            line += wxT("\r\n");
        else
            line += wxT("\n");

        if (rbUnicode->GetValue())
            file.Write(line, wxConvUTF8);
        else
            file.Write(line, wxConvLibc);

        if (set)
            set->MoveNext();
    }
    file.Close();

    return true;
}


void frmExport::OnCancel(wxCommandEvent &ev)
{
    if (IsModal())
        EndModal(wxID_CANCEL);
    else
        Destroy();
}

void frmExport::OnBrowseFile(wxCommandEvent &ev)
{
    wxFileDialog file(this, _("Select export filename"), wxGetHomeDir(), txtFilename->GetValue(), 
        _("CSV files (*.csv)|*.csv|Data files (*.dat)|*.dat|All files (*.*)|*.*"));

    if (file.ShowModal() == wxID_OK)
    {
        txtFilename->SetValue(file.GetPath());
        OnChange(ev);
    }
}
