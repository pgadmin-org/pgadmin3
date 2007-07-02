//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id:$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgManageMacros.h - Manage macros
//
//////////////////////////////////////////////////////////////////////////

#ifndef dlgManageMacros_H
#define dlgManageMacros_H

#include "dlg/dlgClasses.h"
#include "utils/macros.h"

class dlgManageMacros : public DialogWithHelp
{
public:
	dlgManageMacros(wxWindow *parent, frmMain *form, queryMacroList *macros);
	~dlgManageMacros();
	int ManageMacros(); // returns: 0=no changes, 1=changes to save, -1=changes to rollback
    

private:
	queryMacroList *macros;
	wxString GetHelpPage() const;
	void OnOK(wxCommandEvent& ev);
	void OnCancel(wxCommandEvent& ev);
	void OnClear(wxCommandEvent& ev);
	void OnSave (wxCommandEvent& ev);
	void OnKeySelect(wxListEvent& ev);
	void OnNameChange(wxCommandEvent& ev);
	void OnQueryChange(wxStyledTextEvent &ev);

	// Helper methods
	void AddKeyToList(int position, const wxString& key);
	void DeleteMacro(int listItem);
	void SetMacro(bool silent);

	bool anythingChanged;
	bool thisMacroChanged;
	bool appQueryModify; // Marker to ignore modify events while setting query text

	DECLARE_EVENT_TABLE()
};

#endif /*dlgManageMacros_H*/