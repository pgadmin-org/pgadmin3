//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlSeclabelPanel.cpp - Panel with security label information
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/imaglist.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/sysLogger.h"
#include "ctl/ctlSeclabelPanel.h"
#include "db/pgConn.h"
#include "schema/pgObject.h"
#include "utils/pgDefs.h"


BEGIN_EVENT_TABLE(ctlSeclabelPanel, wxPanel)
	EVT_LIST_ITEM_SELECTED(CTL_LBSECLABEL,  ctlSeclabelPanel::OnSeclabelSelChange)
	EVT_BUTTON(CTL_ADDSECLABEL,             ctlSeclabelPanel::OnAddSeclabel)
	EVT_BUTTON(CTL_DELSECLABEL,             ctlSeclabelPanel::OnDelSeclabel)
	EVT_TEXT(CTL_PROVIDER,                  ctlSeclabelPanel::OnChange)
	EVT_TEXT(CTL_SECLABEL,                  ctlSeclabelPanel::OnChange)
END_EVENT_TABLE();

DEFINE_LOCAL_EVENT_TYPE(EVT_SECLABELPANEL_CHANGE)


ctlSeclabelPanel::ctlSeclabelPanel(wxNotebook *nb)
	: wxPanel(nb, -1, wxDefaultPosition, wxDefaultSize)
{
	wxStaticText *label;

	nbNotebook = nb;
	nbNotebook->AddPage(this, _("Security Labels"));

	connection = NULL;

	// root sizer
	wxFlexGridSizer *sizer0 = new wxFlexGridSizer(4, 1, 5, 5);
	sizer0->AddGrowableCol(0);
	sizer0->AddGrowableRow(0);

	// grid sizer
	wxFlexGridSizer *sizer1 = new wxFlexGridSizer(1, 1, 5, 5);
	sizer1->AddGrowableCol(0);
	sizer1->AddGrowableRow(0);
	lbSeclabels = new ctlListView(this, CTL_LBSECLABEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT);
	lbSeclabels->AddColumn(_("Provider"), 70, wxLIST_FORMAT_LEFT);
	lbSeclabels->AddColumn(_("Security label"), 70, wxLIST_FORMAT_LEFT);
	sizer1->Add(lbSeclabels, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	sizer0->Add(sizer1, 0, wxEXPAND | wxALL, 5);

	// buttons sizer
	wxBoxSizer *sizer2 = new wxBoxSizer(wxHORIZONTAL);
	btnAddSeclabel = new wxButton(this, CTL_ADDSECLABEL, _("Add/Change"));
	sizer2->Add(btnAddSeclabel, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	btnDelSeclabel = new wxButton(this, CTL_DELSECLABEL, _("Remove"));
	sizer2->Add(btnDelSeclabel, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	sizer0->Add(sizer2, 0, wxEXPAND | wxALL, 0);


	// textboxes sizer
	wxFlexGridSizer *sizer3 = new wxFlexGridSizer(2, 2, 5, 5);
	sizer3->AddGrowableCol(1);
	label = new wxStaticText(this, 0, _("Provider"));
	sizer3->Add(label, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	txtProvider = new wxTextCtrl(this, CTL_PROVIDER);
	sizer3->Add(txtProvider, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	label = new wxStaticText(this, 0, _("Security label"));
	sizer3->Add(label, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	txtSeclabel = new wxTextCtrl(this, CTL_SECLABEL);
	sizer3->Add(txtSeclabel, 0, wxEXPAND | wxALIGN_CENTRE_VERTICAL | wxTOP | wxLEFT | wxRIGHT, 4);
	sizer0->Add(sizer3, 0, wxEXPAND | wxALL, 5);

	// compute sizes
	this->SetSizer(sizer0);
	sizer0->Fit(this);

	btnAddSeclabel->Enable(false);
}


ctlSeclabelPanel::~ctlSeclabelPanel()
{
}


void ctlSeclabelPanel::Disable()
{
	lbSeclabels->Disable();
	btnAddSeclabel->Disable();
	btnDelSeclabel->Disable();
	txtProvider->Disable();
	txtSeclabel->Disable();
}


void ctlSeclabelPanel::SetConnection(pgConn *conn)
{
	connection = conn;
}


void ctlSeclabelPanel::SetObject(pgObject *obj)
{
	object = obj;

	if (object && !object->GetLabels().IsEmpty())
	{
		wxArrayString seclabels = object->GetProviderLabelArray();
		if (seclabels.GetCount() > 0)
		{
			for (unsigned int index = 0 ; index < seclabels.GetCount() - 1 ; index += 2)
			{
				lbSeclabels->AppendItem(seclabels.Item(index),
				                        seclabels.Item(index + 1));
			}
		}
	}
}


void ctlSeclabelPanel::OnDelSeclabel(wxCommandEvent &ev)
{
	if (lbSeclabels->GetFirstSelected() == -1)
		return;

	lbSeclabels->DeleteCurrentItem();

	wxCommandEvent event( EVT_SECLABELPANEL_CHANGE, GetId() );
	event.SetEventObject( this );
	GetEventHandler()->ProcessEvent( event );

	ev.Skip();
}


void ctlSeclabelPanel::OnAddSeclabel(wxCommandEvent &ev)
{
	bool found = false;

	for (int indexList = 0; indexList < lbSeclabels->GetItemCount(); indexList++)
	{
		if (lbSeclabels->GetText(indexList) == txtProvider->GetValue())
		{
			found = true;
			lbSeclabels->SetItem(indexList, 1, txtSeclabel->GetValue());
			break;
		}
	}

	if (!found)
	{
		int pos = lbSeclabels->GetItemCount();
		lbSeclabels->InsertItem(pos, txtProvider->GetValue());
		lbSeclabels->SetItem(pos, 1, txtSeclabel->GetValue());
	}

	wxCommandEvent event( EVT_SECLABELPANEL_CHANGE, GetId() );
	event.SetEventObject( this );
	GetEventHandler()->ProcessEvent( event );

	ev.Skip();
}

void ctlSeclabelPanel::OnChange(wxCommandEvent &event)
{
	wxString provider = txtProvider->GetValue().Trim(true).Trim(false);
	wxString label = txtSeclabel->GetValue().Trim(true).Trim(false);
	btnAddSeclabel->Enable(!provider.IsEmpty() && !label.IsEmpty());
}

void ctlSeclabelPanel::OnSeclabelSelChange(wxListEvent &ev)
{
	if (lbSeclabels->GetFirstSelected() == -1)
		return;

	txtProvider->SetValue(lbSeclabels->GetText(lbSeclabels->GetSelection()));
	txtSeclabel->SetValue(lbSeclabels->GetText(lbSeclabels->GetSelection(), 1));
}

void ctlSeclabelPanel::GetCurrentProviderLabelArray(wxArrayString &secLabels)
{
	for(int indexList = 0; indexList < lbSeclabels->GetItemCount(); indexList++)
	{
		secLabels.Add(lbSeclabels->GetText(indexList));
		secLabels.Add(lbSeclabels->GetText(indexList, 1));
	}
}

wxString ctlSeclabelPanel::GetSqlForSecLabels(wxString objecttype, wxString objectname)
{
	wxASSERT(connection != NULL);

	wxString sql;
	wxArrayString seclabels;
	int indexList;
	unsigned int indexArray;
	wxString oldprovider, newprovider, oldlabel, newlabel;
	bool found;

	if (object)
	{
		seclabels = object->GetProviderLabelArray();

		// find new or changed seclabels
		for (indexList = 0; indexList < lbSeclabels->GetItemCount(); indexList++)
		{
			found = false;
			newprovider = lbSeclabels->GetText(indexList);
			newlabel = lbSeclabels->GetText(indexList, 1);

			// try to find the provider in the current providers list
			if (object && seclabels.GetCount() > 0)
			{
				for (indexArray = 0 ; indexArray < seclabels.GetCount() - 1 ; indexArray += 2)
				{
					oldprovider = seclabels.Item(indexArray);
					oldlabel = seclabels.Item(indexArray + 1);

					if (oldprovider == newprovider)
					{
						found = true;
						// provider is available on the old and new list
						// we should check is the label has changed
						if (oldlabel != newlabel)
							sql += wxT("SECURITY LABEL FOR ") + newprovider
							       + wxT("\n  ON ") + objecttype + wxT(" ") + objectname
							       + wxT("\n  IS ") + object->qtDbString(newlabel) + wxT(";\n");
					}
				}
			}

			if (!found)
				sql += wxT("SECURITY LABEL FOR ") + newprovider
				       + wxT("\n  ON ") + objecttype + wxT(" ") + objectname
				       + wxT("\n  IS ") + object->qtDbString(newlabel) + wxT(";\n");
		}

		// find old seclabels
		if (seclabels.GetCount() > 0)
		{
			for (indexArray = 0 ; indexArray < seclabels.GetCount() - 1 ; indexArray += 2)
			{
				found = false;
				oldprovider = seclabels.Item(indexArray);

				for (indexList = 0; indexList < lbSeclabels->GetItemCount(); indexList++)
				{
					newprovider = lbSeclabels->GetText(indexList);

					if (oldprovider == newprovider)
					{
						found = true;
					}
				}

				if (!found)
					sql += wxT("SECURITY LABEL FOR ") + oldprovider
					       + wxT("\n  ON ") + objecttype + wxT(" ") + objectname
					       + wxT("\n  IS NULL;\n");
			}
		}
	}
	else
	{
		// find new or changed seclabels
		for (indexList = 0; indexList < lbSeclabels->GetItemCount(); indexList++)
		{
			newprovider = lbSeclabels->GetText(indexList);
			newlabel = lbSeclabels->GetText(indexList, 1);

			sql += wxT("SECURITY LABEL FOR ") + newprovider
			       + wxT("\n  ON ") + objecttype + wxT(" ") + objectname
			       + wxT("\n  IS ") + connection->qtDbString(newlabel) + wxT(";\n");
		}
	}

	return sql;
}
