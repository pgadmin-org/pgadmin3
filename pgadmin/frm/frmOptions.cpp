//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
#include <wx/file.h>
#include <wx/clrpicker.h>
#include <wx/filepicker.h>
#include <wx/fontpicker.h>

// App headers
#include "frm/frmOptions.h"
#include "frm/frmMain.h"
#include "frm/frmHint.h"
#include "utils/sysSettings.h"
#include "utils/sysLogger.h"
#include "utils/misc.h"
#include "frm/menu.h"
#include "ctl/ctlColourPicker.h"

// Must be after pgAdmin3.h or MSVC++ complains
#include <wx/colordlg.h>

#include "images/properties.xpm"

#define nbOptions                   CTRL_NOTEBOOK("nbOptions")
#define txtPgHelpPath               CTRL_TEXT("txtPgHelpPath")
#define txtEdbHelpPath              CTRL_TEXT("txtEdbHelpPath")
#define txtGpHelpPath               CTRL_TEXT("txtGpHelpPath")
#define txtSlonyHelpPath            CTRL_TEXT("txtSlonyHelpPath")
#define pickerSlonyPath             CTRL_DIRPICKER("pickerSlonyPath")
#define pickerPostgresqlPath        CTRL_DIRPICKER("pickerPostgresqlPath")
#define pickerEnterprisedbPath      CTRL_DIRPICKER("pickerEnterprisedbPath")
#define pickerGPDBPath              CTRL_DIRPICKER("pickerGPDBPath")
#define txtSystemSchemas            CTRL_TEXT("txtSystemSchemas")
#define pickerLogfile               CTRL_FILEPICKER("pickerLogfile")
#define radLoglevel                 CTRL_RADIOBOX("radLoglevel")
#define txtMaxRows                  CTRL_TEXT("txtMaxRows")
#define txtMaxColSize               CTRL_TEXT("txtMaxColSize")
#define pickerFont                  CTRL_FONTPICKER("pickerFont")
#define chkUnicodeFile              CTRL_CHECKBOX("chkUnicodeFile")
#define chkWriteBOM                 CTRL_CHECKBOX("chkWriteBOM")
#define chkAskSaveConfirm           CTRL_CHECKBOX("chkAskSaveConfirm")
#define chkAskDelete                CTRL_CHECKBOX("chkAskDelete")
#define chkShowUsersForPrivileges   CTRL_CHECKBOX("chkShowUsersForPrivileges")
#define txtAutoRowCount             CTRL_TEXT("txtAutoRowCount")
#define txtIndent                   CTRL_TEXT("txtIndent")
#define chkSpacesForTabs			CTRL_CHECKBOX("chkSpacesForTabs")
#define cbCopyQuote					CTRL_COMBOBOX("cbCopyQuote")
#define cbCopyQuoteChar				CTRL_COMBOBOX("cbCopyQuoteChar")
#define cbCopySeparator				CTRL_COMBOBOX("cbCopySeparator")
#define chkStickySql                CTRL_CHECKBOX("chkStickySql")
#define chkIndicateNull             CTRL_CHECKBOX("chkIndicateNull")
#define chkAutoRollback             CTRL_CHECKBOX("chkAutoRollback")
#define chkDoubleClickProperties    CTRL_CHECKBOX("chkDoubleClickProperties")
#define cbLanguage                  CTRL_COMBOBOX("cbLanguage")
#define pickerSqlFont               CTRL_FONTPICKER("pickerSqlFont")
#define chkSuppressHints            CTRL_CHECKBOX("chkSuppressHints")
#define chkResetHints               CTRL_CHECKBOX("chkResetHints")
#define lstDisplay					CTRL_CHECKLISTBOX("lstDisplay")
#define chkSystemObjects            CTRL_CHECKBOX("chkSystemObjects")
#define chkIgnoreVersion            CTRL_CHECKBOX("chkIgnoreVersion")
#define pickerIdleProcessColour     CTRL_COLOURPICKER("pickerIdleProcessColour")
#define pickerActiveProcessColour   CTRL_COLOURPICKER("pickerActiveProcessColour")
#define pickerSlowProcessColour     CTRL_COLOURPICKER("pickerSlowProcessColour")
#define pickerBlockedProcessColour  CTRL_COLOURPICKER("pickerBlockedProcessColour")
#define pickerFavouritesFile        CTRL_FILEPICKER("pickerFavouritesFile")
#define pickerMacrosFile               CTRL_FILEPICKER("pickerMacrosFile")
#define pickerHistoryFile              CTRL_FILEPICKER("pickerHistoryFile")
#define txtHistoryMaxQueries        CTRL_TEXT("txtHistoryMaxQueries")
#define txtHistoryMaxQuerySize      CTRL_TEXT("txtHistoryMaxQuerySize")
#define chkSQLUseSystemBackgroundColour  CTRL_CHECKBOX("chkSQLUseSystemBackgroundColour")
#define chkSQLUseSystemForegroundColour  CTRL_CHECKBOX("chkSQLUseSystemForegroundColour")
#define pickerSQLBackgroundColour        CTRL_COLOURPICKER("pickerSQLBackgroundColour")
#define pickerSQLForegroundColour        CTRL_COLOURPICKER("pickerSQLForegroundColour")
#define stSQLCustomBackgroundColour      CTRL_STATIC("stSQLCustomBackgroundColour")
#define stSQLCustomForegroundColour      CTRL_STATIC("stSQLCustomForegroundColour")
#define pickerSQLMarginBackgroundColour  CTRL_COLOURPICKER("pickerSQLMarginBackgroundColour")
#define pickerSQLColour1            CTRL_COLOURPICKER("pickerSQLColour1")
#define pickerSQLColour2            CTRL_COLOURPICKER("pickerSQLColour2")
#define pickerSQLColour3            CTRL_COLOURPICKER("pickerSQLColour3")
#define pickerSQLColour4            CTRL_COLOURPICKER("pickerSQLColour4")
#define pickerSQLColour5            CTRL_COLOURPICKER("pickerSQLColour5")
#define pickerSQLColour6            CTRL_COLOURPICKER("pickerSQLColour6")
#define pickerSQLColour7            CTRL_COLOURPICKER("pickerSQLColour7")
#define pickerSQLColour10           CTRL_COLOURPICKER("pickerSQLColour10")
#define pickerSQLColour11           CTRL_COLOURPICKER("pickerSQLColour11")
#define pickerSQLCaretColour               CTRL_COLOURPICKER("pickerSQLCaretColour")
#define chkKeywordsInUppercase      CTRL_CHECKBOX("chkKeywordsInUppercase")

BEGIN_EVENT_TABLE(frmOptions, pgDialog)
	EVT_MENU(MNU_HELP,                                            frmOptions::OnHelp)
	EVT_BUTTON (XRCID("btnDefault"),                              frmOptions::OnDefault)
	EVT_CHECKBOX(XRCID("chkSuppressHints"),                       frmOptions::OnSuppressHints)
	EVT_CHECKBOX(XRCID("chkResetHints"),                          frmOptions::OnResetHints)
	EVT_CHECKBOX(XRCID("chkSQLUseSystemBackgroundColour"),        frmOptions::OnChangeSQLUseCustomColour)
	EVT_CHECKBOX(XRCID("chkSQLUseSystemForegroundColour"),        frmOptions::OnChangeSQLUseCustomColour)
	EVT_BUTTON (wxID_OK,                                          frmOptions::OnOK)
	EVT_BUTTON (wxID_HELP,                                        frmOptions::OnHelp)
	EVT_BUTTON (wxID_CANCEL,                                      frmOptions::OnCancel)
	EVT_COMBOBOX(XRCID("cbCopyQuote"),		                      frmOptions::OnChangeCopyQuote)
END_EVENT_TABLE()


//----------------------------------------------------------------------------
// wxRichTextFontDialog: a substitute for wxFontDialog, which is broken on
// Mac OS X Snow Leopard
//----------------------------------------------------------------------------

#ifdef __WXMAC__

#include <wx/fontdlg.h>
#include <wx/richtext/richtextbuffer.h>
#include <wx/richtext/richtextformatdlg.h>

class wxRichTextFontDialog: public wxFontDialogBase
{
public:
	wxRichTextFontDialog() : wxFontDialogBase()
	{
		Init(); /* must be
														   Create()d later */
	}
	wxRichTextFontDialog(wxWindow *parent)
		: wxFontDialogBase(parent)
	{
		Init();
		Create(parent);
	}
	wxRichTextFontDialog(wxWindow *parent, const wxFontData &data)
		: wxFontDialogBase(parent, data)
	{
		Init();
		Create(parent, data);
	}

	void Init()
	{
		m_title = _("Font");
	}

	virtual int ShowModal();

	virtual void SetTitle( const wxString &title)
	{
		m_title = title;
	}
	virtual wxString GetTitle() const
	{
		return m_title;
	}

protected:
	wxString    m_title;

	DECLARE_DYNAMIC_CLASS_NO_COPY(wxRichTextFontDialog)

};

IMPLEMENT_DYNAMIC_CLASS(wxRichTextFontDialog, wxDialog)

int wxRichTextFontDialog::ShowModal()
{
	wxTextAttrEx attr;
	if (m_fontData.GetInitialFont().Ok())
		attr.SetFont(m_fontData.GetInitialFont());

	if (m_fontData.GetColour().Ok())
		attr.SetTextColour(m_fontData.GetColour());

	wxRichTextFormattingDialog formatDlg(wxRICHTEXT_FORMAT_FONT,
	                                     GetParent(), GetTitle());
	formatDlg.SetAttributes(attr);

	if (formatDlg.ShowModal() == wxID_OK)
	{
		wxTextAttrEx attr(formatDlg.GetAttributes());

		m_fontData.SetChosenFont(attr.GetFont());
		m_fontData.SetColour(attr.GetTextColour());

		return wxID_OK;
	}
	else
		return wxID_CANCEL;

}

#endif

//----------------------------------------------------------------------------

frmOptions::frmOptions(frmMain *parent)
{
	mainForm = parent;
	wxWindowBase::SetFont(settings->GetSystemFont());
	LoadResource(parent, wxT("frmOptions"));

	// Icon
	SetIcon(wxIcon(properties_xpm));
	RestorePosition();

	wxAcceleratorEntry entries[1];

	entries[0].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);

	wxAcceleratorTable accel(1, entries);
	SetAcceleratorTable(accel);

	wxTextValidator numval(wxFILTER_NUMERIC);
	txtMaxRows->SetValidator(numval);
	txtMaxColSize->SetValidator(numval);
	txtAutoRowCount->SetValidator(numval);
	txtIndent->SetValidator(numval);
	txtHistoryMaxQueries->SetValidator(numval);
	txtHistoryMaxQuerySize->SetValidator(numval);

	pickerLogfile->SetPath(settings->GetLogFile());
	radLoglevel->SetSelection(settings->GetLogLevel());
	txtMaxRows->SetValue(NumToStr(settings->GetMaxRows()));
	txtMaxColSize->SetValue(NumToStr(settings->GetMaxColSize()));
	chkAskSaveConfirm->SetValue(!settings->GetAskSaveConfirmation());
	chkAskDelete->SetValue(settings->GetConfirmDelete());
	chkShowUsersForPrivileges->SetValue(settings->GetShowUsersForPrivileges());
	txtAutoRowCount->SetValue(NumToStr(settings->GetAutoRowCountThreshold()));
	txtIndent->SetValue(NumToStr(settings->GetIndentSpaces()));
	chkSpacesForTabs->SetValue(settings->GetSpacesForTabs());
	cbCopyQuote->SetSelection(settings->GetCopyQuoting());
	cbCopyQuoteChar->SetValue(settings->GetCopyQuoteChar());

	wxString copySeparator = settings->GetCopyColSeparator();
	if (copySeparator == wxT("\t"))
		copySeparator = _("Tab");
	cbCopySeparator->SetValue(copySeparator);

	chkStickySql->SetValue(settings->GetStickySql());
	chkIndicateNull->SetValue(settings->GetIndicateNull());
	chkAutoRollback->SetValue(settings->GetAutoRollback());
	chkDoubleClickProperties->SetValue(settings->GetDoubleClickProperties());

	txtPgHelpPath->SetValue(settings->GetPgHelpPath());
	txtEdbHelpPath->SetValue(settings->GetEdbHelpPath());
	txtGpHelpPath->SetValue(settings->GetGpHelpPath());
	txtSlonyHelpPath->SetValue(settings->GetSlonyHelpPath());

	txtSystemSchemas->SetValue(settings->GetSystemSchemas());
	chkUnicodeFile->SetValue(settings->GetUnicodeFile());
	chkWriteBOM->SetValue(settings->GetWriteBOM());
	chkSuppressHints->SetValue(settings->GetSuppressGuruHints());
	pickerSlonyPath->SetPath(settings->GetSlonyPath());
	pickerPostgresqlPath->SetPath(settings->GetPostgresqlPath());
	pickerEnterprisedbPath->SetPath(settings->GetEnterprisedbPath());
	pickerGPDBPath->SetPath(settings->GetGPDBPath());
	chkIgnoreVersion->SetValue(settings->GetIgnoreVersion());

	// Get back the colours
	pickerIdleProcessColour->SetColour(settings->GetIdleProcessColour());
	pickerActiveProcessColour->SetColour(settings->GetActiveProcessColour());
	pickerSlowProcessColour->SetColour(settings->GetSlowProcessColour());
	pickerBlockedProcessColour->SetColour(settings->GetBlockedProcessColour());

	pickerFavouritesFile->SetPath(settings->GetFavouritesFile());
	pickerMacrosFile->SetPath(settings->GetMacrosFile());
	pickerHistoryFile->SetPath(settings->GetHistoryFile());

	txtHistoryMaxQueries->SetValue(NumToStr(settings->GetHistoryMaxQueries()));
	txtHistoryMaxQuerySize->SetValue(NumToStr(settings->GetHistoryMaxQuerySize()));

	chkSQLUseSystemBackgroundColour->SetValue(settings->GetSQLBoxUseSystemBackground());
	chkSQLUseSystemForegroundColour->SetValue(settings->GetSQLBoxUseSystemForeground());
	UpdateColourControls();

	pickerSQLColour1->SetColour(settings->GetSQLBoxColour(1));
	pickerSQLColour2->SetColour(settings->GetSQLBoxColour(2));
	pickerSQLColour3->SetColour(settings->GetSQLBoxColour(3));
	pickerSQLColour4->SetColour(settings->GetSQLBoxColour(4));
	pickerSQLColour5->SetColour(settings->GetSQLBoxColour(5));
	pickerSQLColour6->SetColour(settings->GetSQLBoxColour(6));
	pickerSQLColour7->SetColour(settings->GetSQLBoxColour(7));
	pickerSQLColour10->SetColour(settings->GetSQLBoxColour(10));
	pickerSQLColour11->SetColour(settings->GetSQLBoxColour(11));

	chkKeywordsInUppercase->SetValue(settings->GetSQLKeywordsInUppercase());

	cbLanguage->Append(_("Default"));
	int sel = 0;
	wxLanguage langId = settings->GetCanonicalLanguage();

	int langCount = existingLangs.GetCount();
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
				sel = langNo + 1;
		}
	}
	cbLanguage->SetSelection(sel);

	pickerFont->SetSelectedFont(settings->GetSystemFont());
	pickerSqlFont->SetSelectedFont(settings->GetSQLFont());

	// Load the display options
	lstDisplay->Append(_("Databases"));
	lstDisplay->Append(_("Tablespaces"));
	lstDisplay->Append(_("pgAgent Jobs"));
	lstDisplay->Append(_("Groups/group Roles"));
	lstDisplay->Append(_("Users/login Roles"));
	lstDisplay->Append(_("Resource Queues"));
	lstDisplay->Append(_("Catalogs"));
	lstDisplay->Append(_("Casts"));
    lstDisplay->Append(_("Foreign Data Wrappers"));
	lstDisplay->Append(_("Languages"));
	lstDisplay->Append(_("Synonyms"));
	lstDisplay->Append(_("Schemas"));
	lstDisplay->Append(_("Slony-I Clusters"));
	lstDisplay->Append(_("Aggregates"));
	lstDisplay->Append(_("Conversions"));
	lstDisplay->Append(_("Domains"));
	lstDisplay->Append(_("Functions"));
	lstDisplay->Append(_("Trigger Functions"));
	lstDisplay->Append(_("Packages"));
	lstDisplay->Append(_("Procedures"));
	lstDisplay->Append(_("Operators"));
	lstDisplay->Append(_("Operator Classes"));
	lstDisplay->Append(_("Operator Families"));
	lstDisplay->Append(_("Sequences"));
	lstDisplay->Append(_("Tables"));
	lstDisplay->Append(_("External Tables"));
	lstDisplay->Append(_("FTS Configurations"));
	lstDisplay->Append(_("FTS Dictionaries"));
	lstDisplay->Append(_("FTS Parsers"));
	lstDisplay->Append(_("FTS Templates"));
	lstDisplay->Append(_("Types"));
	lstDisplay->Append(_("Views"));

	for (unsigned int x = 0; x < lstDisplay->GetCount(); x++)
		lstDisplay->Check(x, settings->GetDisplayOption(lstDisplay->GetString(x)));

	chkSystemObjects->SetValue(settings->GetShowSystemObjects());

	wxCommandEvent e;
	OnChangeCopyQuote(e);
}


frmOptions::~frmOptions()
{
	SavePosition();
}

void frmOptions::OnHelp(wxCommandEvent &ev)
{
	long page = nbOptions->GetSelection();
	DisplayHelp(wxT("options-tab") + NumToStr(page + 1L), HELP_PGADMIN);
}

void frmOptions::OnDefault(wxCommandEvent &ev)
{
	// Reset the display options to the defaults.
	// Clear them all first
	for (unsigned int x = 0; x < lstDisplay->GetCount(); x++)
		lstDisplay->Check(x, settings->GetDisplayOption(lstDisplay->GetString(x), true));
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

void frmOptions::UpdateColourControls()
{
	if (chkSQLUseSystemBackgroundColour->GetValue())
	{
		pickerSQLBackgroundColour->Enable(false);
		pickerSQLBackgroundColour->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		stSQLCustomBackgroundColour->Enable(false);
	}
	else
	{
		pickerSQLBackgroundColour->Enable(true);
		pickerSQLBackgroundColour->SetColour(settings->GetSQLBoxColourBackground());
		stSQLCustomBackgroundColour->Enable(true);
	}

	if (chkSQLUseSystemForegroundColour->GetValue())
	{
		pickerSQLForegroundColour->Enable(false);
		pickerSQLForegroundColour->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
		stSQLCustomForegroundColour->Enable(false);
	}
	else
	{
		pickerSQLForegroundColour->Enable(true);
		pickerSQLForegroundColour->SetColour(settings->GetSQLBoxColourForeground());
		stSQLCustomForegroundColour->Enable(true);
	}

	pickerSQLCaretColour->SetColour(settings->GetSQLColourCaret());

	pickerSQLMarginBackgroundColour->SetColour(settings->GetSQLMarginBackgroundColour());
}

void frmOptions::OnChangeSQLUseCustomColour(wxCommandEvent &ev)
{
	UpdateColourControls();
}

void frmOptions::OnOK(wxCommandEvent &ev)
{
	// Check the PostgreSQL and EnterpriseDB paths
#ifdef __WXMSW__
	if (!pickerPostgresqlPath->GetPath().IsEmpty() && !isPgApp(pickerPostgresqlPath->GetPath() + wxT("\\pg_dump.exe")))
#else
	if (!pickerPostgresqlPath->GetPath().IsEmpty() && !isPgApp(pickerPostgresqlPath->GetPath() + wxT("/pg_dump")))
#endif
	{
		wxMessageBox(_("The PostgreSQL bin path specified is not valid or does not contain a PostgreSQL pg_dump executable.\n\nPlease select another directory, or leave the path blank."), _("Error"), wxICON_ERROR);
		return;
	}

#ifdef __WXMSW__
	if (!pickerEnterprisedbPath->GetPath().IsEmpty() && !isEdbApp(pickerEnterprisedbPath->GetPath() + wxT("\\pg_dump.exe")))
#else
	if (!pickerEnterprisedbPath->GetPath().IsEmpty() && !isEdbApp(pickerEnterprisedbPath->GetPath() + wxT("/pg_dump")))
#endif
	{
		wxMessageBox(_("The EnterpriseDB bin path specified is not valid or does not contain an EnterpriseDB pg_dump executable.\n\nPlease select another directory, or leave the path blank."), _("Error"), wxICON_ERROR);
		return;
	}

#ifdef __WXMSW__
	if (!pickerGPDBPath->GetPath().IsEmpty() && !isGpApp(pickerGPDBPath->GetPath() + wxT("\\pg_dump.exe")))
#else
	if (!pickerGPDBPath->GetPath().IsEmpty() && !isGpApp(pickerGPDBPath->GetPath() + wxT("/pg_dump")))
#endif
	{
		wxMessageBox(_("The Greenplum bin path specified is not valid or does not contain a Greenplum pg_dump executable.\n\nPlease select another directory, or leave the path blank."), _("Error"), wxICON_ERROR);
		return;
	}

	// Clean and check the help paths
	txtPgHelpPath->SetValue(CleanHelpPath(txtPgHelpPath->GetValue()));
	if (!HelpPathValid(txtPgHelpPath->GetValue()))
	{
		wxMessageBox(_("An invalid PostgreSQL help path was specified.\n\nPlease enter another filename, directory or URL, or leave the path blank."), _("Error"), wxICON_ERROR);
		txtPgHelpPath->SetFocus();
		return;
	}

	txtEdbHelpPath->SetValue(CleanHelpPath(txtEdbHelpPath->GetValue()));
	if (!HelpPathValid(txtEdbHelpPath->GetValue()))
	{
		wxMessageBox(_("An invalid EnterpriseDB help path was specified.\n\nPlease enter another filename, directory or URL, or leave the path blank."), _("Error"), wxICON_ERROR);
		txtEdbHelpPath->SetFocus();
		return;
	}

	txtGpHelpPath->SetValue(CleanHelpPath(txtGpHelpPath->GetValue()));
	if (!HelpPathValid(txtGpHelpPath->GetValue()))
	{
		wxMessageBox(_("An invalid GreenplumDB help path was specified.\n\nPlease enter another filename, directory or URL, or leave the path blank."), _("Error"), wxICON_ERROR);
		txtGpHelpPath->SetFocus();
		return;
	}

	txtSlonyHelpPath->SetValue(CleanHelpPath(txtSlonyHelpPath->GetValue()));
	if (!HelpPathValid(txtSlonyHelpPath->GetValue()))
	{
		wxMessageBox(_("An invalid Slony help path was specified.\n\nPlease enter another filename, directory or URL, or leave the path blank."), _("Error"), wxICON_ERROR);
		txtSlonyHelpPath->SetFocus();
		return;
	}

	// Logfile
	wxString logFile = pickerLogfile->GetPath();
	wxLogInfo(wxT("Setting logfile to: %s"), logFile.c_str());
	settings->SetLogFile(logFile);

	// Loglevel
	wxString logInfo = radLoglevel->GetStringSelection();
	wxLogInfo(wxT("Setting loglevel to: %s"), logInfo.c_str());
	int sel = radLoglevel->GetSelection();

	switch(sel)
	{
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
	settings->SetSpacesForTabs(chkSpacesForTabs->GetValue());
	settings->SetCopyQuoting(cbCopyQuote->GetCurrentSelection());
	settings->SetCopyQuoteChar(cbCopyQuoteChar->GetValue());
	settings->SetHistoryMaxQueries(StrToLong(txtHistoryMaxQueries->GetValue()));
	settings->SetHistoryMaxQuerySize(StrToLong(txtHistoryMaxQuerySize->GetValue()));

	wxString copySeparator = cbCopySeparator->GetValue();
	if (copySeparator == _("Tab"))
		copySeparator = wxT("\t");
	settings->SetCopyColSeparator(copySeparator);

	settings->SetStickySql(chkStickySql->GetValue());
	settings->SetIndicateNull(chkIndicateNull->GetValue());
	settings->SetAutoRollback(chkAutoRollback->GetValue());
	settings->SetDoubleClickProperties(chkDoubleClickProperties->GetValue());
	settings->SetUnicodeFile(chkUnicodeFile->GetValue());
	settings->SetWriteBOM(chkWriteBOM->GetValue());
	settings->SetSystemFont(pickerFont->GetSelectedFont());
	settings->SetSQLFont(pickerSqlFont->GetSelectedFont());
	settings->SetSuppressGuruHints(chkSuppressHints->GetValue());
	settings->SetSlonyPath(pickerSlonyPath->GetPath());
	settings->SetPostgresqlPath(pickerPostgresqlPath->GetPath());
	settings->SetEnterprisedbPath(pickerEnterprisedbPath->GetPath());
	settings->SetGPDBPath(pickerGPDBPath->GetPath());

	// Setup PostgreSQL/EnterpriseDB working paths
#if defined(__WXMSW__)
	pgBackupExecutable  = settings->GetPostgresqlPath() + wxT("\\pg_dump.exe");
	pgBackupAllExecutable  = settings->GetPostgresqlPath() + wxT("\\pg_dumpall.exe");
	pgRestoreExecutable = settings->GetPostgresqlPath() + wxT("\\pg_restore.exe");

	edbBackupExecutable  = settings->GetEnterprisedbPath() + wxT("\\pg_dump.exe");
	edbBackupAllExecutable  = settings->GetEnterprisedbPath() + wxT("\\pg_dumpall.exe");
	edbRestoreExecutable = settings->GetEnterprisedbPath() + wxT("\\pg_restore.exe");

	gpBackupExecutable  = settings->GetGPDBPath() + wxT("\\pg_dump.exe");
	gpBackupAllExecutable  = settings->GetGPDBPath() + wxT("\\pg_dumpall.exe");
	gpRestoreExecutable = settings->GetGPDBPath() + wxT("\\pg_restore.exe");
#else
	pgBackupExecutable  = settings->GetPostgresqlPath() + wxT("/pg_dump");
	pgBackupAllExecutable  = settings->GetPostgresqlPath() + wxT("/pg_dumpall");
	pgRestoreExecutable = settings->GetPostgresqlPath() + wxT("/pg_restore");

	edbBackupExecutable  = settings->GetEnterprisedbPath() + wxT("/pg_dump");
	edbBackupAllExecutable  = settings->GetEnterprisedbPath() + wxT("/pg_dumpall");
	edbRestoreExecutable = settings->GetEnterprisedbPath() + wxT("/pg_restore");

	gpBackupExecutable  = settings->GetGPDBPath() + wxT("/pg_dump");
	gpBackupAllExecutable  = settings->GetGPDBPath() + wxT("/pg_dumpall");
	gpRestoreExecutable = settings->GetGPDBPath() + wxT("/pg_restore");
#endif

	if (!wxFile::Exists(pgBackupExecutable))
		pgBackupExecutable = wxEmptyString;
	if (!wxFile::Exists(pgBackupAllExecutable))
		pgBackupAllExecutable = wxEmptyString;
	if (!wxFile::Exists(pgRestoreExecutable))
		pgRestoreExecutable = wxEmptyString;

	if (!wxFile::Exists(edbBackupExecutable))
		edbBackupExecutable = wxEmptyString;
	if (!wxFile::Exists(edbBackupAllExecutable))
		edbBackupAllExecutable = wxEmptyString;
	if (!wxFile::Exists(edbRestoreExecutable))
		edbRestoreExecutable = wxEmptyString;

	if (!wxFile::Exists(gpBackupExecutable))
		gpBackupExecutable = wxEmptyString;
	if (!wxFile::Exists(gpBackupAllExecutable))
		gpBackupAllExecutable = wxEmptyString;
	if (!wxFile::Exists(gpRestoreExecutable))
		gpRestoreExecutable = wxEmptyString;


	settings->SetIgnoreVersion(chkIgnoreVersion->GetValue());

	if (chkResetHints->GetValue())
		frmHint::ResetHints();

	// Set the help paths
	settings->SetPgHelpPath(txtPgHelpPath->GetValue());
	settings->SetEdbHelpPath(txtEdbHelpPath->GetValue());
	settings->SetGpHelpPath(txtGpHelpPath->GetValue());
	settings->SetSlonyHelpPath(txtSlonyHelpPath->GetValue());

	settings->SetSystemSchemas(txtSystemSchemas->GetValue());

	// Save the display options
	int changed = false;
	for (unsigned int x = 0; x < lstDisplay->GetCount(); x++)
	{
		if (lstDisplay->IsChecked(x) != settings->GetDisplayOption(lstDisplay->GetString(x)))
		{
			changed = true;
			settings->SetDisplayOption(lstDisplay->GetString(x), lstDisplay->IsChecked(x));
		}
	}

	if (chkSystemObjects->GetValue() != settings->GetShowSystemObjects())
	{
		changed = true;
		settings->SetShowSystemObjects(chkSystemObjects->GetValue());
	}

	// Change the status colours
	if (pickerIdleProcessColour->GetColourString() != settings->GetIdleProcessColour())
		changed = true;
	settings->SetIdleProcessColour(pickerIdleProcessColour->GetColourString());

	if (pickerActiveProcessColour->GetColourString() != settings->GetActiveProcessColour())
		changed = true;
	settings->SetActiveProcessColour(pickerActiveProcessColour->GetColourString());

	if (pickerSlowProcessColour->GetColourString() != settings->GetSlowProcessColour())
		changed = true;
	settings->SetSlowProcessColour(pickerSlowProcessColour->GetColourString());

	if (pickerBlockedProcessColour->GetColourString() != settings->GetBlockedProcessColour())
		changed = true;
	settings->SetBlockedProcessColour(pickerBlockedProcessColour->GetColourString());

	// Change files' location
	settings->SetFavouritesFile(pickerFavouritesFile->GetPath());
	settings->SetMacrosFile(pickerMacrosFile->GetPath());
	settings->SetHistoryFile(pickerHistoryFile->GetPath());

	// Change SQL Syntax colours
	if (settings->GetSQLBoxUseSystemBackground() != chkSQLUseSystemBackgroundColour->GetValue())
	{
		changed = true;
		settings->SetSQLBoxUseSystemBackground(chkSQLUseSystemBackgroundColour->GetValue());
	}

	if (settings->GetSQLBoxUseSystemForeground() != chkSQLUseSystemForegroundColour->GetValue())
	{
		changed = true;
		settings->SetSQLBoxUseSystemForeground(chkSQLUseSystemForegroundColour->GetValue());
	}

	if (!settings->GetSQLBoxUseSystemBackground())
	{
		if (pickerSQLBackgroundColour->GetColourString() != settings->GetSQLBoxColourBackground())
			changed = true;
		settings->SetSQLBoxColourBackground(pickerSQLBackgroundColour->GetColourString());
	}

	if (!settings->GetSQLBoxUseSystemForeground())
	{
		if (pickerSQLForegroundColour->GetColourString() != settings->GetSQLBoxColourForeground())
			changed = true;
		settings->SetSQLBoxColourForeground(pickerSQLForegroundColour->GetColourString());
	}

	if (pickerSQLMarginBackgroundColour->GetColourString() != settings->GetSQLMarginBackgroundColour())
		changed = true;
	settings->SetSQLMarginBackgroundColour(pickerSQLMarginBackgroundColour->GetColourString());

	if (pickerSQLCaretColour->GetColourString() != settings->GetSQLColourCaret())
		changed = true;
	settings->SetSQLColourCaret(pickerSQLCaretColour->GetColourString());

	if (pickerSQLColour1->GetColourString() != settings->GetSQLBoxColour(1))
		changed = true;
	settings->SetSQLBoxColour(1, pickerSQLColour1->GetColourString());
	if (pickerSQLColour2->GetColourString() != settings->GetSQLBoxColour(2))
		changed = true;
	settings->SetSQLBoxColour(2, pickerSQLColour2->GetColourString());
	if (pickerSQLColour3->GetColourString() != settings->GetSQLBoxColour(3))
		changed = true;
	settings->SetSQLBoxColour(3, pickerSQLColour3->GetColourString());
	if (pickerSQLColour4->GetColourString() != settings->GetSQLBoxColour(4))
		changed = true;
	settings->SetSQLBoxColour(4, pickerSQLColour4->GetColourString());
	if (pickerSQLColour5->GetColourString() != settings->GetSQLBoxColour(5))
		changed = true;
	settings->SetSQLBoxColour(5, pickerSQLColour5->GetColourString());
	if (pickerSQLColour6->GetColourString() != settings->GetSQLBoxColour(6))
		changed = true;
	settings->SetSQLBoxColour(6, pickerSQLColour6->GetColourString());
	if (pickerSQLColour7->GetColourString() != settings->GetSQLBoxColour(7))
		changed = true;
	settings->SetSQLBoxColour(7, pickerSQLColour7->GetColourString());
	if (pickerSQLColour10->GetColourString() != settings->GetSQLBoxColour(10))
		changed = true;
	settings->SetSQLBoxColour(10, pickerSQLColour10->GetColourString());
	if (pickerSQLColour11->GetColourString() != settings->GetSQLBoxColour(11))
		changed = true;
	settings->SetSQLBoxColour(11, pickerSQLColour11->GetColourString());

	if (settings->GetSQLKeywordsInUppercase() != chkKeywordsInUppercase->GetValue())
	{
		changed = true;
		settings->SetSQLKeywordsInUppercase(chkKeywordsInUppercase->GetValue());
	}

	// Change the language last, as it will affect our tests for changes
	// in the display object types.
	int langNo = cbLanguage->GetCurrentSelection();
	if (langNo >= 0)
	{
		wxLanguage langId;
		if (langNo == 0)
			langId = wxLANGUAGE_DEFAULT;
		else
		{
			const wxLanguageInfo *langInfo = wxLocale::GetLanguageInfo(existingLangs.Item(langNo - 1));
			langId = (wxLanguage) langInfo->Language;
		}

		settings->SetCanonicalLanguage(langId);
	}

	// Did any display options change? Display this message last, so it's
	// in the selected language.
	if (changed)
		wxMessageBox(_("Changes to the display options may not be visible until the browser tree is refreshed."), _("Display options"), wxICON_INFORMATION);

	Destroy();
}


void frmOptions::OnCancel(wxCommandEvent &ev)
{
	Destroy();
}


optionsFactory::optionsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
	mnu->Append(id, _("&Options..."), _("Show options dialog."));
}


wxWindow *optionsFactory::StartDialog(frmMain *form, pgObject *obj)
{
	frmOptions *frm = new frmOptions(form);
	frm->Show();
	return 0;
}

// Enable/disable the copy quote option as required.
void frmOptions::OnChangeCopyQuote(wxCommandEvent &WXUNUSED(ev))
{
	if (cbCopyQuote->GetValue() == _("None"))
		cbCopyQuoteChar->Disable();
	else
		cbCopyQuoteChar->Enable();
}


wxString frmOptions::CheckColour(wxString oldColour)
{
	wxString newColour = wxEmptyString;

	if (oldColour != wxEmptyString)
	{
		wxColour colour;

		if (colour.Set(oldColour))
			newColour = colour.GetAsString(wxC2S_HTML_SYNTAX);
		else
			wxLogError(_("The colour specified is not valid."));
	}

	return newColour;
}

