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
#include <wx/xrc/xmlres.h>

// App headers
#include "frmQBJoin.h"
#include "frmQueryBuilder.h"
#include "frmChildTableViewFrame.h"

// Icons
#include "images/pgAdmin3.xpm"

// Event Table
BEGIN_EVENT_TABLE(frmQBJoin, wxDialog)

    EVT_BUTTON (XRCID("btnOK"), wxDialog::OnOK)
    EVT_BUTTON (XRCID("btnCancel"), wxDialog::OnCancel)
    EVT_BUTTON (XRCID("btnAdd"), frmQBJoin::OnAdd)
    EVT_BUTTON (XRCID("btnDel"), frmQBJoin::OnDelete)

END_EVENT_TABLE()

// Globals
extern sysSettings *settings;

////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////
frmQBJoin::frmQBJoin(wxFrame *frame, wxString txt)
{
	// Indicate we built this dialog in the log
    wxLogInfo(wxT("Creating a Query Builder Join dialogue"));

	// Load the XML resource for this dialog
    wxXmlResource::Get()->LoadDialog(this, frame, wxT("frmQBJoin")); 

    // Set the Icon
    SetIcon(wxIcon(pgAdmin3_xpm));

	// Get a pointer to the parent frame 
	frmChildTableViewFrame *tmpparent = 
			(frmChildTableViewFrame*)frame;

	// **********
	// Note: temporary solution until an object cache is built, and it
	// might need to be namespaced as well?

	// Get the operator list
	pgSet *columns = tmpparent->m_database->ExecuteSet(
		wxT("SELECT DISTINCT ")
			wxT("	a.oprname ")
			wxT("FROM ")
			wxT("	pg_operator a ")
			wxT("JOIN ")
			wxT("	pg_type b on ( a.oprresult = b.oid ) ")
			wxT("WHERE ")
			wxT("	a.oprkind = 'b' and b.typname = 'bool' ")
			wxT("ORDER BY ")
			wxT("	a.oprname; " ));

	// Get the column count
	int rowct = columns->NumRows();

	// Fill the operator list
	for (int si = 0; si < rowct; si++ )
	{
		XRCCTRL(*this, "choiceOperator", wxChoice)->
			Append(columns->GetVal(wxT("oprname")));
		columns->MoveNext();
	}

	// Cleanup the pgSet
	delete columns;

	// **********

	// Set the default operator to "="
	XRCCTRL(*this, "choiceOperator", wxChoice)->
		SetStringSelection(wxT("="));
}

////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////
frmQBJoin::~frmQBJoin()
{
	// Indicate we're done with this dialog
    wxLogInfo(wxT("Destroying a Query Builder Join dialogue"));
}

////////////////////////////////////////////////////////////////////////////////
//
// PopulateData
//
// We have to fill in the resource dialog controls with data
//
////////////////////////////////////////////////////////////////////////////////
void frmQBJoin::PopulateData(wxFrame *leftframe, wxFrame *rightframe)
{
	int si;

	// Get pointers to the left and right frames
	frmChildTableViewFrame *tmpleftframe = 
		(frmChildTableViewFrame*)leftframe;
	frmChildTableViewFrame *tmprightframe = 
		(frmChildTableViewFrame*)rightframe;

	// Get the table aliases
	wxString tmpleftname = tmpleftframe->m_title;
	wxString tmprightname = tmprightframe->m_title;

	// Get pointers to the column lists
	wxListBox *tmpleftlist = XRCCTRL(*this, "listLeft", wxListBox);
	wxListBox *tmprightlist = XRCCTRL(*this, "listRight", wxListBox);

	// Get the selected column from the frames
	wxString tmpleftcolumn = 
		tmpleftframe->m_columnlist->GetStringSelection();
	wxString tmprightcolumn = 
		tmprightframe->m_columnlist->GetStringSelection();

	// How many columns are in the lists?
	int leftcount = tmpleftframe->m_columnlist->GetCount();
	int rightcount = tmprightframe->m_columnlist->GetCount();

	// Iterate through the left frame column list (skip the asterisk)
	for (si = 1; si < leftcount; si++)
	{
		wxString tmpstr = tmpleftframe->m_columnlist->GetString(si);
		tmpleftlist->Append(tmpstr);
	}

	// Iterate through the right frame column list (skip the asterisk)
	for (si = 1; si < rightcount; si++)
	{
		wxString tmpstr = tmprightframe->m_columnlist->GetString(si);
		tmprightlist->Append(tmpstr);
	}

	// Set the defaults for the column lists
	if (!tmpleftcolumn.IsEmpty())
		tmpleftlist->SetStringSelection(tmpleftcolumn);

	if (!tmprightcolumn.IsEmpty())
		tmprightlist->SetStringSelection(tmprightcolumn);

	// Set the table names
	XRCCTRL(*this, "lblLeft", wxStaticText)->
		SetLabel(wxT("Left: ") + tmpleftname);
	XRCCTRL(*this, "lblRight", wxStaticText)->
		SetLabel(wxT("Right: ") + tmprightname);

	// Store the names for later use
	m_leftname = tmpleftname;
	m_rightname = tmprightname;
}

////////////////////////////////////////////////////////////////////////////////
//
// OnAdd
//
// When the user clicks add, a join condition must be constructed
// and added to the conditions list. Enable the OK button.
//
////////////////////////////////////////////////////////////////////////////////
void frmQBJoin::OnAdd(wxCommandEvent &event)
{
	// Get the column from the left column list
	wxString tmpleftcolumn = 
		XRCCTRL(*this, "listLeft", wxListBox)->GetStringSelection();

	// Get the column from the right column list
	wxString tmprightcolumn = 
		XRCCTRL(*this, "listRight", wxListBox)->GetStringSelection();

	// If either column is empty
	if (tmpleftcolumn.IsEmpty() || tmprightcolumn.IsEmpty())
	{
		// Fail with an error 
		wxLogError(__("You must select one column from te left table and one column from the right table."));
		return;
	}

	// Get the operator for the condition
	wxString tmpoperator =
		XRCCTRL(*this, "choiceOperator", wxChoice)->GetStringSelection();

	// Build the condition
	wxString tmpcondition =
		m_leftname + wxT(".") + 
		tmpleftcolumn + wxT(" ") + 
		tmpoperator + wxT(" ") + 
		m_rightname + wxT(".") + 
		tmprightcolumn;

	// Append the condition to the condition list
	XRCCTRL(*this, "listCondition", wxListBox)->Append(tmpcondition);

	// Enable the OK button
	XRCCTRL(*this, "btnOK", wxButton)->Enable();
}

////////////////////////////////////////////////////////////////////////////////
//
// OnDelete
//
// When the user clicks the delete button, it will remove the highlighted
// condition from the condition list. If there are no conditions in the 
// condition list, the OK button will be disabled. 
// 
////////////////////////////////////////////////////////////////////////////////
void frmQBJoin::OnDelete(wxCommandEvent &event)
{
	// Get the item that is currently selected
	int n = XRCCTRL(*this, "listCondition", wxListBox)->GetSelection();

	// If the item is valid then delete it
	if (n >= 0)
		XRCCTRL(*this, "listCondition", wxListBox)->Delete(n);

	// How many conditions are left in the list?
	int count = XRCCTRL(*this, "listCondition", wxListBox)->GetCount();

	// If there are none in the list, disable the OK button 
	if (!count)
		XRCCTRL(*this, "btnOK", wxButton)->Disable();
}

////////////////////////////////////////////////////////////////////////////////
//
// GetLeftColumn
//
// Grab the current column from the left column list.
//
////////////////////////////////////////////////////////////////////////////////
int frmQBJoin::GetLeftColumn() 
{ 
	return XRCCTRL(*this, "listLeft", wxListBox)->
		GetSelection() + 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// GetRightColumn
//
// Grab the current column from the right column list.
//
////////////////////////////////////////////////////////////////////////////////
int frmQBJoin::GetRightColumn() 
{ 
	return XRCCTRL(*this, "listRight", wxListBox)->
		GetSelection() + 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// GetJoinType
// 
// Grab the join type.
//
////////////////////////////////////////////////////////////////////////////////
wxString frmQBJoin::GetJoinType() 
{ 
	return XRCCTRL(*this, "choiceJoinType", wxChoice)->
		GetStringSelection();
}

////////////////////////////////////////////////////////////////////////////////
// 
// GetConditions
// 
// Grab a list of conditions, and how many conditions are in the list.
//
////////////////////////////////////////////////////////////////////////////////
int frmQBJoin::GetConditions(wxArrayString& cond)
{
	// Get a pointer to the condition listbox
	wxListBox *tmplist = XRCCTRL(*this, "listCondition", wxListBox);

	// How many conditions are in the condition list?
	int count = tmplist->GetCount();

	// Add all the conditions into our string array
	for (int si = 0; si < count; si++ )
	{
		cond.Add(tmplist->GetString(si));
	}

	// Return the number of conditions
	return count;
}

////////////////////////////////////////////////////////////////////////////////
//
// GetJoinOperator
// 
// Grab the operator (AND/OR) for combining the join conditions on multiple
// column joins. 
//
////////////////////////////////////////////////////////////////////////////////
wxString frmQBJoin::GetJoinOperator() 
{ 
	return XRCCTRL(*this, "choiceJoinOp", wxChoice)->
		GetStringSelection();
}
