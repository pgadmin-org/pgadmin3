//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmOptions.cpp - The main options dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

// App headers
#include "frmQBJoin.h"
#include "frmQueryBuilder.h"
#include "frmChildTableViewFrame.h"

// Icons
#include "images/pgAdmin3.xpm"

// Event Table
BEGIN_EVENT_TABLE(frmQBJoin, wxDialog)

    EVT_BUTTON (XRCID("btnOK"),     wxDialog::OnOK)
    EVT_BUTTON (XRCID("btnCancel"), wxDialog::OnCancel)
    EVT_BUTTON (XRCID("btnAdd"),    frmQBJoin::OnAdd)
    EVT_BUTTON (XRCID("btnDel"),    frmQBJoin::OnDelete)

END_EVENT_TABLE()

// Globals
extern sysSettings *settings;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
frmQBJoin::frmQBJoin(wxFrame *frame, wxString txt)
{
    wxLogInfo(wxT("Creating a Query Builder Join dialogue"));

    wxXmlResource::Get()->LoadDialog(this, frame, "frmQBJoin"); 

    // Icon
    SetIcon(wxIcon(pgAdmin3_xpm));
    CenterOnParent();

	// Get a pointer to the parent frame 
	frmChildTableViewFrame *tmpparent = 
			(frmChildTableViewFrame*)frame;

	// Get the operator list
	pgSet *columns = tmpparent->m_database->ExecuteSet(
		wxT("SELECT DISTINCT "
			"	a.oprname "
			"FROM "
			"	pg_operator a "
			"JOIN "
			"	pg_type b on ( a.oprresult = b.oid ) "
			"WHERE "
			"	a.oprkind = 'b' and b.typname = 'bool' "
			"ORDER BY "
			"	a.oprname; " ));

	// Get the column count
	int rowct = columns->NumRows();

	// Fill the operator list
	for (int si = 0; si < rowct; si++ )
	{
		XRCCTRL(*this, "choiceOperator", wxChoice)->
			Append(columns->GetVal(wxT("oprname")));
		columns->MoveNext();
	}

	XRCCTRL(*this, "choiceOperator", wxChoice)->
		SetStringSelection(wxT("="));

	delete columns;

	// Set the Join Type list
	XRCCTRL(*this, "choiceJoinType", wxChoice)->
		Append(wxT("INNER JOIN"));
	XRCCTRL(*this, "choiceJoinType", wxChoice)->
		Append(wxT("LEFT JOIN"));
	XRCCTRL(*this, "choiceJoinType", wxChoice)->
		Append(wxT("RIGHT JOIN"));
	XRCCTRL(*this, "choiceJoinType", wxChoice)->
		Append(wxT("FULL JOIN"));
	XRCCTRL(*this, "choiceJoinType", wxChoice)->
		Append(wxT("CROSS JOIN"));

	XRCCTRL(*this, "choiceJoinType", wxChoice)->
		SetStringSelection(wxT("INNER JOIN"));

}

frmQBJoin::~frmQBJoin()
{
    wxLogInfo(wxT("Destroying a Query Builder Join dialogue"));
}

bool frmQBJoin::TransferDataFromWindow()
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQBJoin::PopulateData(wxFrame *leftframe, wxFrame *rightframe)
{
	// Get pointers to the frames
	frmChildTableViewFrame *tmpleftframe = 
		(frmChildTableViewFrame*)leftframe;
	frmChildTableViewFrame *tmprightframe = 
		(frmChildTableViewFrame*)rightframe;

	// Get the table aliases
	wxString tmpleftname = tmpleftframe->m_title;
	wxString tmprightname = tmprightframe->m_title;

	// Get pointers to the lists
	wxListBox *tmpleftlist = XRCCTRL(*this, "listLeft", wxListBox);
	wxListBox *tmprightlist = XRCCTRL(*this, "listRight", wxListBox);

	// Get the selected column from the frames
	wxString tmpleftcolumn = 
		tmpleftframe->m_columnlist->GetStringSelection();
	wxString tmprightcolumn = 
		tmprightframe->m_columnlist->GetStringSelection();

	// How many columns are in the frames?
	int leftcount = tmpleftframe->m_columnlist->GetCount();
	int rightcount = tmprightframe->m_columnlist->GetCount();

	// Iterate through the left frame column list
	// skipping the asterisk
	int si;
	for (si = 1; si < leftcount; si++)
	{
		wxString tmpstr = tmpleftframe->m_columnlist->GetString(si);
		tmpleftlist->Append(tmpstr);
	}

	// Iterate through the right frame column list,
	// skipping the asterisk
	for (si = 1; si < leftcount; si++)
	{
		wxString tmpstr = tmprightframe->m_columnlist->GetString(si);
		tmprightlist->Append(tmpstr);
	}

	// Set the defaults
	if (!tmpleftcolumn.IsEmpty())
		tmpleftlist->SetStringSelection(tmpleftcolumn);

	if (!tmprightcolumn.IsEmpty())
		tmprightlist->SetStringSelection(tmprightcolumn);

	// Set the table names
	XRCCTRL(*this, "lblLeft", wxStaticText)->
		SetLabel(wxT("Left: ") + tmpleftname);
	XRCCTRL(*this, "lblRight", wxStaticText)->
		SetLabel(wxT("Right: ") + tmprightname);

	m_leftname = tmpleftname;
	m_rightname = tmprightname;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQBJoin::OnAdd()
{
	wxString tmpleftcolumn = 
		XRCCTRL(*this, "listLeft", wxListBox)->GetStringSelection();

	wxString tmprightcolumn = 
		XRCCTRL(*this, "listRight", wxListBox)->GetStringSelection();

	if (tmpleftcolumn.IsEmpty() || tmprightcolumn.IsEmpty())
	{
		wxLogError(wxT("You must select one column from the\n"
			"left table and one column from the\n"
			"right table."));
		return;
	}

	wxString tmpoperator =
		XRCCTRL(*this, "choiceOperator", wxChoice)->GetStringSelection();

	wxString tmpcondition =
		m_leftname + wxT(".") + 
		tmpleftcolumn + wxT(" ") + 
		tmpoperator + wxT(" ") + 
		m_rightname + wxT(".") + 
		tmprightcolumn;

	XRCCTRL(*this, "listCondition", wxListBox)->Append(tmpcondition);

	XRCCTRL(*this, "btnOK", wxButton)->Enable();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmQBJoin::OnDelete()
{
	int n = XRCCTRL(*this, "listCondition", wxListBox)->GetSelection();

	if (n>=0)
		XRCCTRL(*this, "listCondition", wxListBox)->Delete(n);

	int count = XRCCTRL(*this, "listCondition", wxListBox)->GetCount();

	if (!count)
		XRCCTRL(*this, "btnOK", wxButton)->Disable();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int frmQBJoin::GetLeftColumn() 
{ 
	return XRCCTRL(*this, "listLeft", wxListBox)->
		GetSelection() + 1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int frmQBJoin::GetRightColumn() 
{ 
	return XRCCTRL(*this, "listRight", wxListBox)->
		GetSelection() + 1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
wxString frmQBJoin::GetJoinType() 
{ 
	return XRCCTRL(*this, "choiceJoinType", wxChoice)->
		GetStringSelection();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int frmQBJoin::GetConditions(wxArrayString& cond)
{
	wxListBox *tmplist = XRCCTRL(*this, "listCondition", wxListBox);

	int count = tmplist->GetCount();

	for (int si = 0; si < count; si++ )
	{
		cond.Add(tmplist->GetString(si));
	}

	return count;
}
