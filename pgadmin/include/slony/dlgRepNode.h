//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepNode.h - Slony-I Node property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPNODEPROP
#define __DLG_REPNODEPROP

#include "slony/dlgRepProperty.h"


class slCluster;
class slNode;

class dlgRepNode : public dlgRepProperty
{
public:
	dlgRepNode(pgaFactory *factory, frmMain *frame, slNode *node, slCluster *c);
	int Go(bool modal);
	wxString GetHelpPage() const
	{
		return wxT("slony-install#node");
	}

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);
	pgObject *GetObject();

private:
	slNode *node;

	DECLARE_EVENT_TABLE()
};

#endif
