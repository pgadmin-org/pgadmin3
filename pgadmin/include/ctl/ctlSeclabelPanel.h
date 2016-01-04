//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlSeclabelPanel.h - Panel with security label information
//
//////////////////////////////////////////////////////////////////////////


#ifndef CTL_SECLBLPANEL_H
#define CTL_SECLBLPANEL_H

#include <wx/wx.h>
#include <wx/notebook.h>

enum
{
	CTL_LBSECLABEL = 250,
	CTL_ADDSECLABEL,
	CTL_DELSECLABEL,
	CTL_PROVIDER,
	CTL_SECLABEL
};

DECLARE_LOCAL_EVENT_TYPE(EVT_SECLABELPANEL_CHANGE, -1)

class pgConn;

class ctlSeclabelPanel : public wxPanel
{

public:

	ctlSeclabelPanel(wxNotebook *nb);
	~ctlSeclabelPanel();

	ctlListView *lbSeclabels;
	void SetConnection(pgConn *conn);
	void SetObject(pgObject *obj);
	void Disable();
	wxString GetSqlForSecLabels(wxString objecttype = wxEmptyString, wxString objectname = wxEmptyString);
	void GetCurrentProviderLabelArray(wxArrayString &secLabels);
protected:
	wxNotebook *nbNotebook;
	pgConn *connection;
	pgObject *object;

	wxButton *btnAddSeclabel, *btnDelSeclabel;
	wxTextCtrl *txtProvider, *txtSeclabel;

	void OnSeclabelSelChange(wxListEvent &ev);
	void OnAddSeclabel(wxCommandEvent &ev);
	void OnDelSeclabel(wxCommandEvent &ev);
	void OnProviderChange(wxCommandEvent &ev);
	void OnSeclabelChange(wxCommandEvent &ev);
	void OnChange(wxCommandEvent &ev);
	DECLARE_EVENT_TABLE()
};

#endif
