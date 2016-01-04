//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSynonym.h - EnterpriseDB Synonym property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_SYNONYMPROP
#define __DLG_SYNONYMPROP

#include "dlg/dlgProperty.h"

class edbSynonym;
class edbPrivateSynonymFactory;
class edbPrivateSynonym;
class pgSchema;

class dlgSynonym : public dlgProperty
{
public:
	dlgSynonym(pgaFactory *factory, frmMain *frame, edbSynonym *sy);
	dlgSynonym(edbPrivateSynonymFactory *factory, frmMain *frame, edbPrivateSynonym *syn, pgSchema *schema);
	int Go(bool modal);

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createpubsynonym");
	}

private:
	void OnChangeTargetType(wxCommandEvent &ev)
	{
		ProcessTypeChange();
	};
	void OnChangeTargetSchema(wxCommandEvent &ev)
	{
		ProcessSchemaChange();
	};
	void ProcessTypeChange();
	void ProcessSchemaChange();

	edbSynonym        *synonym;
	edbPrivateSynonym *privSynonym;
	pgSchema          *synonymSchema;

	DECLARE_EVENT_TABLE()
};


#endif
