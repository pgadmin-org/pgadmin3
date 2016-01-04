//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddSelectKindFksDialog.cpp - Utility dialog class to allow user select destination of fk: automatically generated or existing column
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/statline.h>
#include "wx/stattext.h"


// App headers
#include "dd/dditems/utilities/ddSelectKindFksDialog.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "hotdraw/main/hdDrawingView.h"
#include "hotdraw/main/hdDrawingEditor.h"


IMPLEMENT_CLASS( ddSelectKindFksDialog, wxDialog )

BEGIN_EVENT_TABLE( ddSelectKindFksDialog, wxDialog )
	EVT_BUTTON(wxID_OK, ddSelectKindFksDialog::OnOkButtonClicked)
	EVT_BUTTON(wxID_CANCEL, ddSelectKindFksDialog::OnCancelButtonClicked)
	EVT_CHOICE(DDSELECTKINDFK, ddSelectKindFksDialog::OnChoiceFkKind)
END_EVENT_TABLE()

ddSelectKindFksDialog::ddSelectKindFksDialog(	wxWindow *parent,
        ddRelationshipFigure *relation,
        wxWindowID id,
        const wxPoint &pos,
        const wxSize &size,
        long style
                                            )
{
	tablesRelation = relation;
	Init();
	Create(parent, id, pos, size, style);
}

ddSelectKindFksDialog::~ddSelectKindFksDialog()
{
	//before delete all items inside to free memory
	deleteColsControls();
	delete ok;
	delete line;
}

ddSelectKindFksDialog::ddSelectKindFksDialog()
{
	Init();
}

void ddSelectKindFksDialog::Init( )
{
	//	choices.clear(); and delete all items inside hash map

}

bool ddSelectKindFksDialog::Create(	wxWindow *parent,
                                    wxWindowID id,
                                    const wxPoint &pos = wxDefaultPosition,
                                    const wxSize &size = wxDefaultSize,
                                    long style = wxCAPTION )
{
	if (!wxDialog::Create( parent, id, wxT("Select Foreign Key Mapping for each Column"), pos, size, style ))
		return false;

	CreateControls();

	// This fits the dialog to the minimum size dictated by
	// the sizers
	GetSizer()->Fit(this);

	// This ensures that the dialog cannot be sized smaller
	// than the minimum size

	GetSizer()->SetSizeHints(this);

	// Centre the dialog on the parent or (if none) screen

	Centre();

	return true;
}

void ddSelectKindFksDialog::CreateControls()
{
	// A top-level sizer

	topSizer = new wxBoxSizer(wxVERTICAL );
	this->SetSizer(topSizer);

	// A wxChoice to choice kind of foreign key: from Uk or from pk
	wxArrayString kindFks;
	kindFks.Add(_("From Primary"));

	int i, last = tablesRelation->getStartTable()->getUkConstraintsNames().Count();
	wxString tmp;

	for(i = 0; i < last; i++)
	{
		tmp = _("Unique Key: ");
		tmp += tablesRelation->getStartTable()->getUkConstraintsNames()[i];
		kindFks.Add(tmp);
	}

	kindFkCtrl = new wxChoice(this, DDSELECTKINDFK, wxDefaultPosition, wxDefaultSize, kindFks);
	kindFkCtrl->SetStringSelection(_("From Primary"));
	topSizer->Add(kindFkCtrl, 0, wxALIGN_CENTER, 5);

	// A dividing line before the mapping items and kind of mapping
	line = new wxStaticLine ( this, wxID_STATIC,
	                          wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	topSizer->Add(line, 0, wxGROW | wxALL, 5);


	//Hack to allow multiple pairs of wxStaticText wxchoice at dialog
	colsTopSizer = new wxBoxSizer(wxVERTICAL );
	topSizer->Add(colsTopSizer);
	populateColumnsControls(true, -1);

	// A dividing line before the OK and Cancel buttons

	line = new wxStaticLine ( this, wxID_STATIC,
	                          wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	topSizer->Add(line, 0, wxGROW | wxALL, 5);

	// A horizontal box sizer to contain Reset, OK, Cancel and Help

	okCancelBox = new wxBoxSizer(wxHORIZONTAL);
	topSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// The OK button

	ok = new wxButton ( this, wxID_OK, wxT("&OK"),
	                    wxDefaultPosition, wxDefaultSize, 0 );
	okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// The Cancel button

	cancel = new wxButton ( this, wxID_CANCEL, wxT("&Cancel"),
	                        wxDefaultPosition, wxDefaultSize, 0 );
	okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
}

void ddSelectKindFksDialog::OnChoiceFkKind(wxCommandEvent &event)
{
	int selectedUk = kindFkCtrl->GetSelection() == 0 ? -1 : kindFkCtrl->GetSelection() - 1;
	bool frompk = kindFkCtrl->GetSelection() == 0;

	populateColumnsControls(frompk, selectedUk);
}

void ddSelectKindFksDialog::deleteColsControls()
{
	choicesControlsHashMap::iterator it;
	bool repeat;
	ddSelectFkKindLine *columnLine;
	do
	{
		repeat = false;
		for (it = choices.begin(); it != choices.end(); ++it)
		{
			wxString key = it->first;
			columnLine = it->second;
			choices.erase(it);
			delete columnLine;
			repeat = true;
			if (repeat)
				break;
		}
	}
	while(repeat);
	colsTopSizer->Clear();
	choices.clear();
}
void ddSelectKindFksDialog::populateColumnsControls(bool primaryKey, int useUkIndex)
{
	// Adding all columns mapping items
	ddTableFigure *source = (ddTableFigure *) tablesRelation->getStartFigure();
	ddTableFigure *destination = (ddTableFigure *) tablesRelation->getEndFigure();

	//Delete existing controllers
	deleteColsControls();

	//Populate controllers
	if(primaryKey)
	{
		wxArrayString sourceCols = source->getAllFkSourceColsNames(true);
		wxArrayString destCols = destination->getAllColumnsNames();
		destCols.Insert(_("Automatically Generated"), 0);
		int i, last = sourceCols.Count();
		int eventID;
		for(i = 0; i < last; i++)
		{
			// A sizer to allow lines of controls
			linesSizer = new wxBoxSizer(wxHORIZONTAL );
			colsTopSizer->Add(linesSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
			//Create controls and put inside linesizer
			eventID =  DDCHOICESELECTBASE + i;
			ddSelectFkKindLine *newLineControls = new ddSelectFkKindLine(this, sourceCols[i], destCols, eventID);
			choices[ sourceCols[i] ] = newLineControls;
			linesSizer->Add(newLineControls->sourceCtrl, 0, wxALIGN_LEFT | wxALL, 5);
			linesSizer->Add(newLineControls->destinationCtrl, 0, wxGROW | wxALL, 5);
		}
	}
	else
	{
		wxArrayString sourceCols = source->getAllFkSourceColsNames(false, useUkIndex);
		wxArrayString destCols = destination->getAllColumnsNames();
		destCols.Insert(_("Automatically Generated"), 0);
		int i, last = sourceCols.Count();
		int eventID;
		for(i = 0; i < last; i++)
		{
			// A sizer to allow lines of controls
			linesSizer = new wxBoxSizer(wxHORIZONTAL );
			colsTopSizer->Add(linesSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
			//Create controls and put inside linesizer
			eventID =  DDCHOICESELECTBASE + i;
			ddSelectFkKindLine *newLineControls = new ddSelectFkKindLine(this, sourceCols[i], destCols, eventID);
			choices[ sourceCols[i] ] = newLineControls;
			linesSizer->Add(newLineControls->sourceCtrl, 0, wxALIGN_LEFT | wxALL, 5);
			linesSizer->Add(newLineControls->destinationCtrl, 0, wxGROW | wxALL, 5);
		}
	}
	this->Layout();
	this->InvalidateBestSize();
	this->Fit();
	topSizer->RecalcSizes();
}

//Transfer data to the window
bool ddSelectKindFksDialog::TransferDataToWindow()
{
	return true;
}

//Transfer data from the window
bool ddSelectKindFksDialog::TransferDataFromWindow()
{
	return true;
}

void ddSelectKindFksDialog::OnEnterPressed( wxCommandEvent &event )
{
	if (event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER)
	{
		if ( Validate() && TransferDataFromWindow() )
		{
			if ( IsModal() )
				EndModal(wxID_OK); // If modal
			else
			{
				SetReturnCode(wxID_OK);
				this->Show(false); // If modeless
			}
		}

	}
}

void ddSelectKindFksDialog::OnCancelButtonClicked( wxCommandEvent &event )
{
	//Do nothing, just return wxID_CANCEL to don't allow connection
	event.Skip();
}

void ddSelectKindFksDialog::OnOkButtonClicked( wxCommandEvent &event )
{
	choicesControlsHashMap::iterator it;
	ddSelectFkKindLine *lineOfCtrls;

	int selectedUk = kindFkCtrl->GetSelection() == 0 ? -1 : kindFkCtrl->GetSelection() - 1;
	bool fromPk = kindFkCtrl->GetSelection() == 0;
	tablesRelation->setFkFrom(fromPk, selectedUk);  //true or bigger from zero both are mutually exclusive

	for( it = choices.begin(); it != choices.end(); ++it )
	{
		wxString key = it->first;
		lineOfCtrls = it->second;
		if(lineOfCtrls->destinationCtrl->GetSelection() != 0) //No automatic Generated
		{
			ddColumnFigure *col = tablesRelation->getStartTable()->getColumnByName(lineOfCtrls->sourceCtrl->GetLabel());
			tablesRelation->addExistingColumnFk(col, lineOfCtrls->destinationCtrl->GetString(lineOfCtrls->destinationCtrl->GetSelection()));
		}
	}
	event.Skip();
}

ddSelectFkKindLine::ddSelectFkKindLine(wxWindow *parent, wxString sourceColumn, wxArrayString possibleTargets, wxWindowID eventId)
{
	sourceCtrl = new wxStaticText(parent, wxID_STATIC, sourceColumn, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	destinationCtrl = new wxChoice(parent, eventId, wxDefaultPosition, wxDefaultSize, possibleTargets);
	destinationCtrl->SetStringSelection(_("Automatically Generated"));
}

ddSelectFkKindLine::ddSelectFkKindLine()
{
	sourceCtrl = NULL;
	destinationCtrl = NULL;
}

ddSelectFkKindLine::~ddSelectFkKindLine()
{
	delete sourceCtrl;
	delete destinationCtrl;
}
