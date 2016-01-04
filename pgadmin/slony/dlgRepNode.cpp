// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepNode.cpp - PostgreSQL Slony-I Node Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"


#include "slony/dlgRepNode.h"
#include "slony/slCluster.h"
#include "slony/slNode.h"


// pointer to controls
#define txtID               CTRL_TEXT("txtID")



BEGIN_EVENT_TABLE(dlgRepNode, dlgProperty)
END_EVENT_TABLE();


dlgProperty *slNodeFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgRepNode(this, frame, (slNode *)node, (slCluster *)parent);
}

dlgRepNode::dlgRepNode(pgaFactory *f, frmMain *frame, slNode *s, slCluster *c)
	: dlgRepProperty(f, frame, c, wxT("dlgRepNode"))
{
	node = s;
}


pgObject *dlgRepNode::GetObject()
{
	return node;
}


int dlgRepNode::Go(bool modal)
{
	txtID->SetValidator(numericValidator);

	if (node)
	{
		// edit mode
		txtID->SetValue(NumToStr(node->GetSlId()));
		txtID->Disable();
	}
	else
	{
		// create mode
		EnableOK(true);
	}

	return dlgProperty::Go(modal);
}


pgObject *dlgRepNode::CreateObject(pgCollection *collection)
{
	wxString restriction;
	if (StrToLong(txtID->GetValue()) > 0)
		restriction = txtID->GetValue();
	else
		restriction = wxT("(SELECT MAX(no_id) FROM ") + cluster->GetSchemaPrefix() + wxT("sl_node)");

	pgObject *obj = nodeFactory.CreateObjects(collection, 0,
	                wxT(" WHERE no_id = ") + restriction);

	return obj;
}


void dlgRepNode::CheckChange()
{
	if (node)
	{
		EnableOK(txtComment->GetValue() != node->GetComment());
	}
	else
	{
		bool enable = true;

		EnableOK(enable);
	}
}



wxString dlgRepNode::GetSql()
{
	wxString sql;

	sql = wxT("-- Create replication node\n\n")
	      wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("storenode(");

	if (StrToLong(txtID->GetValue()) > 0)
		sql += txtID->GetValue();
	else
		sql += wxT("(SELECT COALESCE(MAX(no_id), 0) + 1 FROM ")
		       +  cluster->GetSchemaPrefix() + wxT("sl_node)");

	sql += wxT(", ") + qtDbString(txtComment->GetValue());

	// The spool parameter was removed for Slony 2.0
	if (!cluster->ClusterMinimumVersion(2, 0))
		sql +=  wxT(", false");

	sql += wxT(");\n");

	return sql;
}
