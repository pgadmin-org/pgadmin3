//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSequence.h - Sequence property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_SEQUENCEPROP
#define __DLG_SEQUENCEPROP

#include "dlg/dlgProperty.h"
#include "ctl/ctlSeclabelPanel.h"

class pgSchema;
class pgSequence;

class dlgSequence : public dlgSecurityProperty
{
public:
	dlgSequence(pgaFactory *factory, frmMain *frame, pgSequence *seq, pgSchema *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	bool doesOverflowBigInt(const wxString &str, bool emptyAllowed);

	pgSchema *schema;
	pgSequence *sequence;
	ctlSeclabelPanel *seclabelPage;

	void OnChange(wxCommandEvent &event);

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	DECLARE_EVENT_TABLE()
};


#endif
