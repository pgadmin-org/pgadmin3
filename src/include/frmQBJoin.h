//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmQBJoin.h - Connect to a database
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMQBJOIN_H
#define FRMQBJOIN_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgConn.h"
#include "pgServer.h"

// Class declarations
class frmQBJoin : public wxDialog
{
public:
    frmQBJoin(wxFrame *frame, wxString txt);
    ~frmQBJoin();

	// Methods
	wxString GetLeftTable() { return m_leftname; }
	wxString GetRightTable() { return m_rightname; }
	int GetLeftColumn();
	int GetRightColumn();
	wxString GetJoinType();
	int GetConditions(wxArrayString& cond);
	void PopulateData(wxFrame *leftframe, wxFrame *rightframe);
    
private:

	// Methods
    bool TransferDataFromWindow();

	// Events
	void OnAdd();
	void OnDelete();

	// Data
	wxString m_leftname;
	wxString m_rightname;

    DECLARE_EVENT_TABLE()
};

#endif // FRMQBJOIN_H
