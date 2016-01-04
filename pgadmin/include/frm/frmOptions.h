//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmOptions.h - The main options dialogue
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMOPTIONS_H
#define FRMOPTIONS_H

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class frmMain;

class ItemWithString : public wxClientData
{
public:
	wxString data;
};

// Class declarations
class frmOptions : public pgDialog
{
public:
	frmOptions(frmMain *parent);
	~frmOptions();

private:
	frmMain *mainForm;
	wxString menuSelection;

	void OnOK(wxCommandEvent &ev);
	void OnCancel(wxCommandEvent &ev);
	void OnHelp(wxCommandEvent &ev);
	void OnDefault(wxCommandEvent &ev);
	void OnSuppressHints(wxCommandEvent &ev);
	void OnResetHints(wxCommandEvent &ev);
	void OnChangeCopyQuote(wxCommandEvent &ev);
	void OnChangeSQLUseCustomColour(wxCommandEvent &ev);
	void OnTreeSelChanged(wxTreeEvent &event);
	wxString CheckColour(wxString colour);
	void UpdateColourControls();
	wxTreeItemId GetTreeItemByLabel(const wxTreeItemId &root, const wxString &label);
	void ShowPanel(const wxTreeItemId &menuItem);
	DECLARE_EVENT_TABLE()
};


class optionsFactory : public actionFactory
{
public:
	optionsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


#endif
