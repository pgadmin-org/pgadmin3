//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgEventTrigger.h - Event trigger property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_EVENTTRIGGERPROP
#define __DLG_EVENTTRIGGERPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgEventTrigger;

class dlgEventTrigger : public dlgProperty
{
public:
	dlgEventTrigger(pgaFactory *factory, frmMain *frame, pgEventTrigger *evntrig, pgObject *parent);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();
	bool CanDropCascaded()
	{
		return true;
	}

private:
	pgEventTrigger *eventTrigger;
	ctlSeclabelPanel *seclabelPage;
	void OnChange(wxCommandEvent &ev);
	void OnChangeEnable(wxCommandEvent &ev);
	virtual bool IsUpToDate();

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif
	DECLARE_EVENT_TABLE()
};


#endif