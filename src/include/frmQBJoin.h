//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmQBJoin.h - Connect to a database
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMQBJOIN_H
#define FRMQBJOIN_H

// wxWindows headers
#include <wx/wx.h>

// Class declarations
class frmQBJoin : public wxDialog
{
public:

	// Construction/Deconstruction
    frmQBJoin(wxFrame *frame, wxString txt);
    ~frmQBJoin();

	// Methods
	wxString GetLeftTable() { return m_leftname; }
	wxString GetRightTable() { return m_rightname; }
	int GetLeftColumn();
	int GetRightColumn();
	wxString GetJoinType();
	int GetConditions(wxArrayString& cond);
	wxString GetJoinOperator();
	void PopulateData(wxFrame *leftframe, wxFrame *rightframe);
    
private:

	// Events
	void OnAdd(wxCommandEvent &event);
	void OnDelete(wxCommandEvent &event);

	// Data
	wxString m_leftname;
	wxString m_rightname;

	// Event Table
    DECLARE_EVENT_TABLE()
};

#endif // FRMQBJOIN_H
