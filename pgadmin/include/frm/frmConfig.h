//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmConfig.h - Configuration tool
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMCONFIG_H
#define FRMCONFIG_H

#include <wx/textbuf.h>

class pgConn;
class ctlListView;
class frmMain;


extern wxImageList *configImageList;

class frmConfig : public pgFrame
{
public:

	enum tryMode
	{
		NONE = 0,
		ANYFILE,
		HBAFILE,
		MAINFILE,
		PGPASSFILE
	};

	static frmConfig *Create(const wxString &title, const wxString &configFile, tryMode mode);
	void Go();
	void DoOpen(const wxString &fn = wxEmptyString);


protected:
	frmConfig(const wxString &title, const wxString &configFile);
	frmConfig(frmMain *parent, const wxString &title, pgConn *conn);
	~frmConfig();

	virtual void DisplayFile(const wxString &str) = 0;
	virtual void WriteFile(pgConn *conn = 0) = 0;
	virtual wxString GetHintString()
	{
		return wxEmptyString;
	}

	void OpenLastFile();
	void InitFrame(const wxChar *frameName);
	bool DoWriteFile(const wxChar *str, pgConn *conn);
	bool CheckChanged(bool canVeto);

private:

	virtual void OnOpen(wxCommandEvent &event);
	void OnSave(wxCommandEvent &event);
	void OnSaveAs(wxCommandEvent &event);

	void OnClose(wxCloseEvent &event);
	void OnExecute(wxCommandEvent &event);
	void OnHelp(wxCommandEvent &event);
	void OnHint(wxCommandEvent &event);
	void OnBugreport(wxCommandEvent &event);

	void DisplayHint(bool force);

protected:
	pgConn *conn;
	frmMain *mainForm;
	wxString serverFileName;

	wxTextFileType filetype;

	DECLARE_EVENT_TABLE()
};



#endif
