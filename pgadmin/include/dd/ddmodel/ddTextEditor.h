//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTextEditor.cpp - class used to show generate SQL from model
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDTEXTEDITOR_H
#define DDTEXTEDITOR_H


class ddTextEditor : public wxFrame
{
public:
	ddTextEditor(wxString initialText);

private:
	wxMenuBar *menu;
	wxMenu *file;
	wxTextCtrl *text;
	void OnSave(wxCommandEvent &event);
	void OnOpen(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);

	enum MenuControls
	{
		idSave = 7590,
		idOpen,
		idExit
	};
	DECLARE_EVENT_TABLE()
};
#endif