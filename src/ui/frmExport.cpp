//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmExport.cpp - The export file dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/file.h>


// App headers
#include "pgAdmin3.h"
#include "frmExport.h"
#include "ctlSQLResult.h"
#include "sysSettings.h"
#include "misc.h"

// Icons
#include "images/pgAdmin3.xpm"


#define txtFilename     CTRL("txtFilename",     wxTextCtrl)
#define btnOK           CTRL("btnOK",           wxButton)
#define rbUnicode       CTRL("rbUnicode",       wxRadioButton)
#define rbLocal         CTRL("rbLocal",         wxRadioButton)
#define rbCRLF          CTRL("rbCRLF",          wxRadioButton)
#define rbLF            CTRL("rbLF",            wxRadioButton)
#define rbQuoteStrings  CTRL("rbQuoteStrings",  wxRadioButton)
#define rbQuoteAll      CTRL("rbQuoteAll",      wxRadioButton)
#define rbQuoteNone     CTRL("rbQuoteNone",     wxRadioButton)
#define chkColnames     CTRL("chkColnames",     wxCheckBox)
#define cbColSeparator  CTRL("cbColSeparator",  wxComboBox)
#define cbQuoteChar     CTRL("cbQuoteChar",     wxComboBox)


BEGIN_EVENT_TABLE(frmExport, wxDialog)
    EVT_TEXT(XRCID("txtFilename"),          frmExport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbQuoteNone"),   frmExport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbQuoteStrings"),frmExport::OnChange)
    EVT_RADIOBUTTON(XRCID("rbQuoteAll"),    frmExport::OnChange)
    EVT_BUTTON(XRCID("btnFilename"),        frmExport::OnBrowseFile)
    EVT_BUTTON(XRCID("btnHelp"),            frmExport::OnHelp)
    EVT_BUTTON(XRCID("btnOK"),              frmExport::OnOK)
    EVT_BUTTON(XRCID("btnCancel"),          frmExport::OnCancel)
END_EVENT_TABLE()



frmExport::frmExport(ctlSQLResult *parent)
{
    data=parent;
    wxLogInfo(wxT("Creating the export dialogue"));

    wxXmlResource::Get()->LoadDialog(this, parent, wxT("frmExport")); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();
    cbQuoteChar->Disable();
    btnOK->Disable();

    wxString val;

    settings->Read(wxT("Export/Unicode"), &val, wxT("No"));
    rbUnicode->SetValue(StrToBool(val));
    rbLocal->SetValue(!StrToBool(val));

#ifdef __WXMSW__
    settings->Read(wxT("Export/RowSeparator"), &val, wxT("CR/LF"));
#else
    settings->Read(wxT("Export/RowSeparator"), &val, wxT("LF"));
#endif

    rbCRLF->SetValue(val == wxT("CR/LF"));
    rbLF->SetValue(val == wxT("LF"));

    settings->Read(wxT("Export/Quote"), &val, wxT("Strings"));

    rbQuoteNone->SetValue(val == wxT("None"));
    rbQuoteStrings->SetValue(val == wxT("Strings"));
    rbQuoteAll->SetValue(val == wxT("All"));

    settings->Read(wxT("Export/ColSeparator"), &val, wxT(";"));
    cbColSeparator->SetValue(val);

    settings->Read(wxT("Export/QuoteChar"), &val, wxT("\""));
    cbQuoteChar->SetValue(val);

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
    wxFile file(txtFilename->GetValue(), wxFile::write);
    if (!file.IsOpened())
    {
        wxLogError(__("Failed to open file %s."), txtFilename->GetValue().c_str());
        return;
    }

    wxString line;
    wxListItem item;
    item.m_mask=wxLIST_MASK_TEXT;
    int startCol=0;

    if (data->GetColumnCount() > 1) // first col contains row count
        startCol=1;


    if (chkColnames->GetValue())
    {
        for (item.m_col=startCol ; item.m_col < data->GetColumnCount() ; item.m_col++)
        {
            if (item.m_col == startCol)
                line=wxEmptyString;
            else
                line += cbColSeparator->GetValue();
            line += data->colNames.Item(item.m_col);
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

    for (item.m_itemId=0 ; item.m_itemId < data->GetItemCount() ; item.m_itemId++)
    {
        for (item.m_col=startCol ; item.m_col < data->GetColumnCount() ; item.m_col++)
        {
            if (item.m_col == startCol)
                line=wxEmptyString;
            else
                line += cbColSeparator->GetValue();
            data->GetItem(item);
            bool needQuote=rbQuoteAll->GetValue();
            if (!needQuote && rbQuoteStrings->GetValue())
            {
                // find out if string
                switch (data->colTypClasses.Item(item.m_col))
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
                line += cbQuoteChar->GetValue() + item.GetText() + cbQuoteChar->GetValue();
            else
                line += item.GetText();
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
    file.Close();

    settings->Write(wxT("Export/Unicode"), BoolToStr(rbUnicode->GetValue()));
    settings->Write(wxT("Export/RowSeparator"), rbCRLF->GetValue() ? wxT("CR/LF") : wxT("LF"));
    if (rbQuoteAll->GetValue())
        settings->Write(wxT("Export/Quote"), wxT("All"));
    else if (rbQuoteStrings->GetValue())
        settings->Write(wxT("Export/Quote"), wxT("Strings"));
    else
        settings->Write(wxT("Export/Quote"), wxT("None"));

    settings->Write(wxT("Export/ColSeparator"), cbColSeparator->GetValue());
    settings->Write(wxT("Export/QuoteChar"), cbQuoteChar->GetValue());

    settings->Write(wxT("Export/LastFile"), txtFilename->GetValue());

    if (IsModal())
        EndModal(1);
    else
        Destroy();
}


void frmExport::OnCancel(wxCommandEvent &ev)
{
    if (IsModal())
        EndModal(-1);
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
