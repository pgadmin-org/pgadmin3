//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmOptions.cpp - The main options dialogue
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/fontdlg.h>
#include <wx/fontutil.h>

// App headers
#include "frmOptions.h"
#include "frmMain.h"
#include "frmHint.h"
#include "sysSettings.h"
#include "sysLogger.h"
#include "misc.h"
#include "menu.h"

#include "images/properties.xpm"

extern wxLocale *locale;
extern wxArrayInt existingLangs;
extern wxArrayString existingLangNames;

#define nbOptions                   CTRL_NOTEBOOK("nbOptions")
#define txtSqlHelpSite              CTRL_TEXT("txtSqlHelpSite")
#define txtProxy                    CTRL_TEXT("txtProxy")
#define txtSlonyPath                CTRL_TEXT("txtSlonyPath")
#define txtSystemSchemas            CTRL_TEXT("txtSystemSchemas")
#define txtLogfile                  CTRL_TEXT("txtLogfile")
#define radLoglevel                 CTRL_RADIOBOX("radLoglevel")
#define txtMaxRows                  CTRL_TEXT("txtMaxRows")
#define txtMaxColSize               CTRL_TEXT("txtMaxColSize")
#define txtFont                     CTRL_TEXT("txtFont")
#define chkUnicodeFile              CTRL_CHECKBOX("chkUnicodeFile")
#define chkAskSaveConfirm           CTRL_CHECKBOX("chkAskSaveConfirm")
#define chkAskDelete                CTRL_CHECKBOX("chkAskDelete")
#define chkShowUsersForPrivileges   CTRL_CHECKBOX("chkShowUsersForPrivileges")
#define txtAutoRowCount             CTRL_TEXT("txtAutoRowCount")
#define txtIndent                   CTRL_TEXT("txtIndent")
#define chkStickySql                CTRL_CHECKBOX("chkStickySql")
#define chkDoubleClickProperties    CTRL_CHECKBOX("chkDoubleClickProperties")
#define cbLanguage                  CTRL_COMBOBOX("cbLanguage")
#define txtSqlFont                  CTRL_TEXT("txtSqlFont")
#define chkSuppressHints            CTRL_CHECKBOX("chkSuppressHints")
#define chkResetHints               CTRL_CHECKBOX("chkResetHints")


BEGIN_EVENT_TABLE(frmOptions, pgDialog)
    EVT_MENU(MNU_HELP,                        frmOptions::OnHelp)
    EVT_BUTTON (XRCID("btnFont"),             frmOptions::OnFontSelect)
    EVT_BUTTON (XRCID("btnSqlFont"),          frmOptions::OnSqlFontSelect)
    EVT_BUTTON (XRCID("btnBrowseLogfile"),    frmOptions::OnBrowseLogFile)
    EVT_BUTTON (XRCID("btnSlonyPath"),        frmOptions::OnSlonyPathSelect)
    EVT_CHECKBOX(XRCID("chkSuppressHints"),   frmOptions::OnSuppressHints)
    EVT_CHECKBOX(XRCID("chkResetHints"),      frmOptions::OnResetHints)
    EVT_BUTTON (wxID_OK,                      frmOptions::OnOK)
    EVT_BUTTON (wxID_HELP,                    frmOptions::OnHelp)
    EVT_BUTTON (wxID_CANCEL,                  frmOptions::OnCancel)
END_EVENT_TABLE()

frmOptions::frmOptions(frmMain *parent)
{
    wxLogInfo(wxT("Creating an options dialogue"));
    mainForm=parent;
    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(parent, wxT("frmOptions")); 

    // Icon
    SetIcon(wxIcon(properties_xpm));
    CenterOnParent();

    wxAcceleratorEntry entries[1];

    entries[0].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);

    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);


    wxTextValidator numval(wxFILTER_NUMERIC);
    txtMaxRows->SetValidator(numval);
    txtMaxColSize->SetValidator(numval);
    txtAutoRowCount->SetValidator(numval);
    txtIndent->SetValidator(numval);
    
    txtLogfile->SetValue(settings->GetLogFile());
    radLoglevel->SetSelection(settings->GetLogLevel());
    txtMaxRows->SetValue(NumToStr(settings->GetMaxRows()));
    txtMaxColSize->SetValue(NumToStr(settings->GetMaxColSize()));
    chkAskSaveConfirm->SetValue(!settings->GetAskSaveConfirmation());
    chkAskDelete->SetValue(settings->GetConfirmDelete());
    chkShowUsersForPrivileges->SetValue(settings->GetShowUsersForPrivileges());
    txtAutoRowCount->SetValue(NumToStr(settings->GetAutoRowCountThreshold()));
    txtIndent->SetValue(NumToStr(settings->GetIndentSpaces()));
    chkStickySql->SetValue(settings->GetStickySql());
    chkDoubleClickProperties->SetValue(settings->GetDoubleClickProperties());
    txtSqlHelpSite->SetValue(settings->GetSqlHelpSite());
    txtProxy->SetValue(settings->GetProxy());
    txtSystemSchemas->SetValue(settings->GetSystemSchemas());
    chkUnicodeFile->SetValue(settings->GetUnicodeFile());
    chkSuppressHints->SetValue(settings->GetSuppressGuruHints());
    txtSlonyPath->SetValue(settings->GetSlonyPath());


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

    currentFont=settings->GetSystemFont();
    txtFont->SetValue(currentFont.GetNativeFontInfoUserDesc());
    currentSqlFont=settings->GetSQLFont();
    txtSqlFont->SetValue(currentSqlFont.GetNativeFontInfoUserDesc());
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


void frmOptions::OnSlonyPathSelect(wxCommandEvent &ev)
{
    wxDirDialog dlg(this, _("Select directory with Slony-I creation scripts"), txtSlonyPath->GetValue());
    if (dlg.ShowModal() == wxID_OK)
        txtSlonyPath->SetValue(dlg.GetPath());
}


void frmOptions::OnSuppressHints(wxCommandEvent &ev)
{
    if (chkSuppressHints->GetValue())
        chkResetHints->SetValue(false);
}


void frmOptions::OnResetHints(wxCommandEvent &ev)
{
    if (chkResetHints->GetValue())
        chkSuppressHints->SetValue(false);
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

    settings->SetAskSaveConfirmation(!chkAskSaveConfirm->GetValue());
    settings->SetConfirmDelete(chkAskDelete->GetValue());
    settings->SetShowUsersForPrivileges(chkShowUsersForPrivileges->GetValue());
    settings->SetAutoRowCountThreshold(StrToLong(txtAutoRowCount->GetValue()));
    settings->SetIndentSpaces(StrToLong(txtIndent->GetValue()));
    settings->SetStickySql(chkStickySql->GetValue());
    settings->SetDoubleClickProperties(chkDoubleClickProperties->GetValue());
    settings->SetUnicodeFile(chkUnicodeFile->GetValue());
    settings->SetFont(currentFont);
    settings->SetSQLFont(currentSqlFont);
    settings->SetSuppressGuruHints(chkSuppressHints->GetValue());
    settings->SetSlonyPath(txtSlonyPath->GetValue());

    if (chkResetHints->GetValue())
        frmHint::ResetHints();


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
    settings->SetSystemSchemas(txtSystemSchemas->GetValue());

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
            delete locale;
            locale = new wxLocale();
            if (locale->Init(langId))
            {
#ifdef __LINUX__
                {
                    wxLogNull noLog;
                    locale->AddCatalog(wxT("fileutils"));
                }
#endif
                locale->AddCatalog(wxT("pgadmin3"));
                settings->Write(wxT("LanguageId"), (long)langId);
            }
        }

    }

    settings->Save();
    Destroy();
}



void frmOptions::OnSqlFontSelect(wxCommandEvent &ev)
{
    wxFontData fd;
    fd.SetInitialFont(settings->GetSQLFont());
    wxFontDialog dlg(this, fd);

    if (dlg.ShowModal() == wxID_OK)
    {
        currentSqlFont=dlg.GetFontData().GetChosenFont();
        txtSqlFont->SetValue(currentSqlFont.GetNativeFontInfoUserDesc());
    }
}


void frmOptions::OnFontSelect(wxCommandEvent &ev)
{
    wxFontData fd;
    fd.SetInitialFont(settings->GetSystemFont());
    wxFontDialog dlg(this, fd);

    if (dlg.ShowModal() == wxID_OK)
    {
        currentFont=dlg.GetFontData().GetChosenFont();
        txtFont->SetValue(currentFont.GetNativeFontInfoUserDesc());
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


optionsFactory::optionsFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : actionFactory(list)
{
    mnu->Append(id, _("&Options..."), _("Show options dialog."));
}


wxWindow *optionsFactory::StartDialog(frmMain *form, pgObject *obj)
{
    frmOptions *frm=new frmOptions(form);
    frm->Show();
    return 0;
}
