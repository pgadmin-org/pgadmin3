//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmOptions.cpp - The main options dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frmOptions.h"
#include "sysSettings.h"
#include "sysLogger.h"
#include "misc.h"

// Icons
#include "images/pgAdmin3.xpm"


extern wxLocale locale;
extern wxArrayInt existingLangs;


#define txtHelpSite                 CTRL("txtHelpSite", wxTextCtrl)
#define txtLogfile                  CTRL("txtLogfile", wxTextCtrl)
#define radLoglevel                 CTRL("radLoglevel", wxRadioBox)
#define txtMaxRows                  CTRL("txtMaxRows", wxTextCtrl)
#define txtMaxColSize               CTRL("txtMaxColSize", wxTextCtrl)
#define chkAskSaveConfirm           CTRL("chkAskSaveConfirm", wxCheckBox)
#define chkAskDelete                CTRL("chkAskDelete", wxCheckBox)
#define chkShowUsersForPrivileges   CTRL("chkShowUsersForPrivileges", wxCheckBox)
#define txtAutoRowCount             CTRL("txtAutoRowCount", wxTextCtrl)
#define chkStickySql                CTRL("chkStickySql", wxCheckBox)
#define chkDoubleClickProperties    CTRL("chkDoubleClickProperties", wxCheckBox)
#define cbLanguage                  CTRL("cbLanguage", wxComboBox)


BEGIN_EVENT_TABLE(frmOptions, wxDialog)
    EVT_BUTTON (XRCID("btnOK"),               frmOptions::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),           frmOptions::OnCancel)
    EVT_BUTTON (XRCID("btnBrowseLogfile"),    frmOptions::OnBrowseLogFile)
END_EVENT_TABLE()

frmOptions::frmOptions(wxFrame *parent)
{

    wxLogInfo(wxT("Creating an options dialogue"));

    wxXmlResource::Get()->LoadDialog(this, parent, wxT("frmOptions")); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();

    wxTextValidator numval(wxFILTER_NUMERIC);
    txtMaxRows->SetValidator(numval);
    txtMaxColSize->SetValidator(numval);
    txtAutoRowCount->SetValidator(numval);
    
    txtLogfile->SetValue(settings->GetLogFile());
    radLoglevel->SetSelection(settings->GetLogLevel());
    txtMaxRows->SetValue(NumToStr(settings->GetMaxRows()));
    txtMaxColSize->SetValue(NumToStr(settings->GetMaxColSize()));
    chkAskSaveConfirm->SetValue(!settings->GetAskSaveConfirmation());
    chkAskDelete->SetValue(settings->GetConfirmDelete());
    chkShowUsersForPrivileges->SetValue(settings->GetShowUsersForPrivileges());
    txtAutoRowCount->SetValue(NumToStr(settings->GetAutoRowCountThreshold()));
    chkStickySql->SetValue(settings->GetStickySql());
    chkDoubleClickProperties->SetValue(settings->GetDoubleClickProperties());
    txtHelpSite->SetValue(settings->GetHelpSite());


    cbLanguage->Append(_("Default"));
    int sel=0;
    int langId=settings->Read(wxT("LanguageId"), wxLANGUAGE_UNKNOWN);

    int langCount=existingLangs.GetCount();
    if (langCount)
    {
        int langNo;
        const wxLanguageInfo *langInfo;
        for (langNo = 0; langNo < langCount ; langNo++)
        {
            langInfo = wxLocale::GetLanguageInfo(existingLangs.Item(langNo));
            cbLanguage->Append(wxString(wxGetTranslation(langInfo->Description)) + wxT(" (")
                + langInfo->CanonicalName + wxT(")"));
            if (langId == langInfo->Language)
                sel=langNo+1;
        }
    }
    cbLanguage->SetSelection(sel);
}


frmOptions::~frmOptions()
{
    wxLogInfo(wxT("Destroying an options dialogue"));
}



void frmOptions::OnOK(wxCommandEvent &ev)
{

    // Logfile
    wxString logFile = txtLogfile->GetValue();
    wxLogInfo(wxT("Setting logfile to: %s"), logFile.c_str());
    settings->SetLogFile(logFile);

    // Loglevel
    wxString logInfo = radLoglevel->GetStringSelection();
    wxLogInfo(wxT("Setting loglevel to: %s"),logInfo.c_str());
    int sel = radLoglevel->GetSelection();

    switch(sel) {
        case(0):
          settings->SetLogLevel(LOG_NONE);
          break;
        case(1):
          settings->SetLogLevel(LOG_ERRORS);
          break;
        case(2):
          settings->SetLogLevel(LOG_SQL);
          break;
        case(3):
          settings->SetLogLevel(LOG_DEBUG);
          break;
        default:
          settings->SetLogLevel(LOG_ERRORS);
          break;
    }

    // Query parameter
    settings->SetMaxRows(StrToLong(txtMaxRows->GetValue()));
    settings->SetMaxColSize(StrToLong(txtMaxColSize->GetValue()));

    settings->SetAskSaveConfirmation(!chkAskSaveConfirm->IsChecked());
    settings->SetConfirmDelete(chkAskDelete->IsChecked());
    settings->SetShowUsersForPrivileges(chkShowUsersForPrivileges->IsChecked());
    settings->SetAutoRowCountThreshold(StrToLong(txtAutoRowCount->GetValue()));
    settings->SetStickySql(chkStickySql->IsChecked());
    settings->SetDoubleClickProperties(chkDoubleClickProperties->IsChecked());
    settings->SetHelpSite(txtHelpSite->GetValue());

    int langNo=cbLanguage->GetSelection();
    if (langNo >= 0)
    {
        wxLanguage langId;
        if (langNo == 0)
            langId = wxLANGUAGE_DEFAULT;
        else
        {
            const wxLanguageInfo *langInfo=wxLocale::GetLanguageInfo(existingLangs.Item(langNo-1));
            langId = (wxLanguage) langInfo->Language;
        }
        if (langId != (wxLanguage)settings->Read(wxT("LanguageId"), wxLANGUAGE_UNKNOWN))
        {
            locale.Init(langId);
            locale.AddCatalog(wxT("pgadmin3"));
#ifdef __LINUX__
            {
                wxLogNull noLog;
                locale.AddCatalog(wxT("fileutils"));
            }
#endif
            settings->Write(wxT("LanguageId"), (long)langId);
        }

    }
    Destroy();
}


void frmOptions::OnCancel(wxCommandEvent &ev)
{
    Destroy();
}

void frmOptions::OnBrowseLogFile(wxCommandEvent &ev)
{
    wxFileDialog logFile(this, _("Select log file"), wxT(""), wxT(""), _("Log files (*.log)|*.log|All files (*.*)|*.*"));
    logFile.SetDirectory(wxGetHomeDir());

    if (logFile.ShowModal() == wxID_OK)
        txtLogfile->SetValue(logFile.GetPath());
}
