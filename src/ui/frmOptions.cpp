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
#include <wx/fontdlg.h>

// App headers
#include "pgAdmin3.h"
#include "frmOptions.h"
#include "frmMain.h"
#include "sysSettings.h"
#include "sysLogger.h"
#include "misc.h"
#include "menu.h"

// Icons
#include "images/pgAdmin3.xpm"

extern wxLocale locale;
extern wxArrayInt existingLangs;
extern wxArrayString existingLangNames;

#define nbOptions                   CTRL("nbOptions", wxNotebook)
#define txtSqlHelpSite              CTRL("txtSqlHelpSite", wxTextCtrl)
#define txtProxy                    CTRL("txtProxy", wxTextCtrl)
#define txtLogfile                  CTRL("txtLogfile", wxTextCtrl)
#define radLoglevel                 CTRL("radLoglevel", wxRadioBox)
#define txtMaxRows                  CTRL("txtMaxRows", wxTextCtrl)
#define txtMaxColSize               CTRL("txtMaxColSize", wxTextCtrl)
#define chkUnicodeFile              CTRL("chkUnicodeFile", wxCheckBox)
#define chkAskSaveConfirm           CTRL("chkAskSaveConfirm", wxCheckBox)
#define chkAskDelete                CTRL("chkAskDelete", wxCheckBox)
#define chkShowUsersForPrivileges   CTRL("chkShowUsersForPrivileges", wxCheckBox)
#define txtAutoRowCount             CTRL("txtAutoRowCount", wxTextCtrl)
#define chkStickySql                CTRL("chkStickySql", wxCheckBox)
#define chkDoubleClickProperties    CTRL("chkDoubleClickProperties", wxCheckBox)
#define cbLanguage                  CTRL("cbLanguage", wxComboBox)
#define txtFont                     CTRL("txtFont", wxTextCtrl)


BEGIN_EVENT_TABLE(frmOptions, wxDialog)
    EVT_MENU(MNU_HELP,                        frmOptions::OnHelp)
    EVT_BUTTON (XRCID("btnFont"),             frmOptions::OnFontSelect)
    EVT_BUTTON (XRCID("btnBrowseLogfile"),    frmOptions::OnBrowseLogFile)
    EVT_BUTTON (XRCID("btnOK"),               frmOptions::OnOK)
    EVT_BUTTON (XRCID("btnHelp"),             frmOptions::OnHelp)
    EVT_BUTTON (XRCID("btnCancel"),           frmOptions::OnCancel)
END_EVENT_TABLE()

frmOptions::frmOptions(frmMain *parent)
{
    wxLogInfo(wxT("Creating an options dialogue"));
    mainForm=parent;
    wxWindowBase::SetFont(settings->GetSystemFont());
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("frmOptions")); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();

    wxAcceleratorEntry entries[1];

    entries[0].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);

    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);


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
    txtSqlHelpSite->SetValue(settings->GetSqlHelpSite());
    txtProxy->SetValue(settings->GetProxy());
    chkUnicodeFile->SetValue(settings->GetUnicodeFile());


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
            cbLanguage->Append(wxT("(") + langInfo->CanonicalName + wxT(") ")
                + existingLangNames.Item(langNo));
            if (langId == langInfo->Language)
                sel=langNo+1;
        }
    }
    cbLanguage->SetSelection(sel);

    currentFont=settings->GetSQLFont();
    txtFont->SetValue(currentFont.GetFaceName() + wxT(".") + NumToStr((long)currentFont.GetPointSize()));
}


frmOptions::~frmOptions()
{
    wxLogInfo(wxT("Destroying an options dialogue"));
}


void frmOptions::OnHelp(wxCommandEvent &ev)
{
    long page=nbOptions->GetSelection();
    DisplayHelp(this, wxT("options-tab") + NumToStr(page+1L));
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
          settings->SetLogLevel(LOG_NOTICE);
          break;
        case(3):
          settings->SetLogLevel(LOG_SQL);
          break;
        case(4):
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
    settings->SetUnicodeFile(chkUnicodeFile->IsChecked());
    settings->SetSQLFont(currentFont);

    // Make sure there's a slash on the end of the path

    if (txtSqlHelpSite->GetValue().length() != 0) {
        if (txtSqlHelpSite->GetValue().Last() == '/' || txtSqlHelpSite->GetValue().Last() == '\\')
            settings->SetSqlHelpSite(txtSqlHelpSite->GetValue());
        else
            settings->SetSqlHelpSite(txtSqlHelpSite->GetValue() + wxT("/"));
    }
    else
        settings->SetSqlHelpSite(wxT(""));
    
    settings->SetProxy(txtProxy->GetValue());

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
            if (locale.Init(langId))
            {
#ifdef __LINUX__
                {
                    wxLogNull noLog;
                    locale.AddCatalog(wxT("fileutils"));
                }
#endif
                locale.AddCatalog(wxT("pgadmin3"));
                settings->Write(wxT("LanguageId"), (long)langId);
            }
        }

    }
    
    Destroy();
}



void frmOptions::OnFontSelect(wxCommandEvent &ev)
{
    wxFontData fd;
    fd.SetInitialFont(settings->GetSQLFont());
    wxFontDialog dlg(this, fd);

    if (dlg.ShowModal() == wxID_OK)
    {
        currentFont=dlg.GetFontData().GetChosenFont();
        txtFont->SetValue(currentFont.GetFaceName() + wxT(".") + NumToStr((long)currentFont.GetPointSize()));
    }
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
