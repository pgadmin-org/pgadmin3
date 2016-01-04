//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgExtTable.h - Greenplum External Table property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_EXTTABLEPROP
#define __DLG_EXTTABLEPROP

#include "dlg/dlgProperty.h"

class pgSchema;
class gpExtTable;
class ctlSQLBox;

class dlgExtTable : public dlgSecurityProperty
{
public:
	dlgExtTable(pgaFactory *factory, frmMain *frame, gpExtTable *v, pgSchema *sch);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

	void SetObject(pgObject *obj)
	{
		extTable = (gpExtTable *)obj;
	}

private:

	virtual bool IsUpToDate();

	pgSchema *schema;
	gpExtTable *extTable;
	wxString oldDefinition;

	DECLARE_EVENT_TABLE()
};


#endif
