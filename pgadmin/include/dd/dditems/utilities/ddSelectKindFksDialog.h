//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTableNameDialog.h - Utility dialog class to allow user input of table name and short name
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDSELECTKINDFKSDIALOG_H
#define DDSELECTKINDFKSDIALOG_H

#include <wx/statline.h>
#include "dd/dditems/figures/ddRelationshipFigure.h"

class hdDrawingView;

enum
{
	DDSELECTKINDFKSDIALOG = 10000,
	DDSELECTKINDFK = 10001,
	DDCHOICESELECTBASE = 30000
};

class ddSelectFkKindLine : public hdObject
{
public:
	wxStaticText *sourceCtrl;
	wxChoice *destinationCtrl;

	ddSelectFkKindLine(wxWindow *parent, wxString sourceColumn, wxArrayString possibleTargets, wxWindowID eventId);
	ddSelectFkKindLine();
	~ddSelectFkKindLine();
};


WX_DECLARE_STRING_HASH_MAP( ddSelectFkKindLine *, choicesControlsHashMap );

class ddSelectKindFksDialog : public wxDialog
{
	DECLARE_CLASS( ddSelectKindFksDialog )
	DECLARE_EVENT_TABLE()
public:
	ddSelectKindFksDialog();
	~ddSelectKindFksDialog();
	ddSelectKindFksDialog(	wxWindow *parent,
	                        ddRelationshipFigure *relation,
	                        wxWindowID id = DDSELECTKINDFKSDIALOG,
	                        const wxPoint &pos = wxDefaultPosition,
	                        const wxSize &size = wxDefaultSize,
	                        long style = wxCAPTION
	                     );

	// Member initialization
	void Init();
	// Creation
	bool Create(	wxWindow *parent,
	                wxWindowID id,
	                const wxPoint &pos,
	                const wxSize &size,
	                long style);


	// Creates the controls and sizers
	void CreateControls();

	// Sets the validators for the dialog controls
	bool TransferDataToWindow();
	bool TransferDataFromWindow();

	// Sets the help text for the dialog controls
	void SetDialogHelp();

	//wxEVT_COMMAND_TEXT_ENTER event_handle for DDVALUE1
	void OnEnterPressed( wxCommandEvent &event );
	void OnOkButtonClicked( wxCommandEvent &event );
	void OnCancelButtonClicked( wxCommandEvent &event );


protected:
	//methods
	void populateColumnsControls(bool primaryKey, int useUkIndex);
	void deleteColsControls();
	void OnChoiceFkKind(wxCommandEvent &event);

	// Data members
	ddRelationshipFigure *tablesRelation;

	// Dialog controls
	choicesControlsHashMap choices;
	wxBoxSizer *topSizer, *linesSizer, *okCancelBox, *colsTopSizer;
	wxChoice *kindFkCtrl;
	wxButton *ok, *cancel;
	wxStaticLine *line;

private:

};
#endif

