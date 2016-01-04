//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepPath.cpp - PostgreSQL Slony-I Path Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "slony/dlgRepPath.h"
#include "slony/slCluster.h"
#include "slony/slNode.h"
#include "slony/slPath.h"
#include "schema/pgDatatype.h"


// pointer to controls
#define cbServer      CTRL_COMBOBOX("cbServer")
#define txtConnInfo     CTRL_TEXT("txtConnInfo")
#define txtConnRetry    CTRL_TEXT("txtConnRetry")


BEGIN_EVENT_TABLE(dlgRepPath, dlgProperty)
	EVT_TEXT(XRCID("txtConnInfo"),          dlgRepPath::OnChange)
	EVT_COMBOBOX(XRCID("cbServer"),       dlgRepPath::OnChange)
END_EVENT_TABLE();

dlgProperty *slPathFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgRepPath(this, frame, (slPath *)node, (slNode *)parent);
}


dlgRepPath::dlgRepPath(pgaFactory *f, frmMain *frame, slPath *p, slNode *n)
	: dlgRepProperty(f, frame, n->GetCluster(), wxT("dlgRepPath"))
{
	path = p;
	node = n;
}


pgObject *dlgRepPath::GetObject()
{
	return path;
}


int dlgRepPath::Go(bool modal)
{

	if (path)
	{
		// edit mode
		cbServer->Append(IdAndName(path->GetSlId(), path->GetName()), (void *)path->GetSlId());
		cbServer->SetSelection(0);
		cbServer->Disable();

		txtConnInfo->SetValue(path->GetConnInfo());
		txtConnRetry->SetValue(NumToStr(path->GetConnRetry()));
	}
	else
	{
		// create mode

		txtConnRetry->SetValue(wxT("10"));

		pgSet *nodes = connection->ExecuteSet(
		                   wxT("SELECT no_id, no_comment\n")
		                   wxT("  FROM ") + cluster->GetSchemaPrefix() + wxT("sl_node\n")
		                   wxT("  LEFT JOIN ") + cluster->GetSchemaPrefix() + wxT("sl_path ON pa_client=\n") + NumToStr(node->GetSlId()) +
		                   wxT(" AND pa_server=no_id\n")
		                   wxT(" WHERE no_active AND pa_client IS NULL\n")
		                   wxT("   AND no_id <> ") + NumToStr(node->GetSlId()) + wxT("\n")
		                   wxT(" ORDER BY no_id")
		               );

		if (nodes)
		{
			while (!nodes->Eof())
			{
				cbServer->Append(IdAndName(nodes->GetLong(wxT("no_id")), nodes->GetVal(wxT("no_comment"))),
				                 (void *)nodes->GetLong(wxT("no_id")));
				nodes->MoveNext();
			}
			delete nodes;
		}
		if (cbServer->GetCount() > 0)
			cbServer->SetSelection(0);
	}

	return dlgProperty::Go(modal);
}


pgObject *dlgRepPath::CreateObject(pgCollection *collection)
{
	pgObject *obj = pathFactory.CreateObjects(collection, 0,
	                wxT(" WHERE pa_server = ") + NumToStr((OID)cbServer->wxItemContainer::GetClientData(cbServer->GetCurrentSelection())) +
	                wxT("   AND pa_client = ") + NumToStr(node->GetSlId()));

	return obj;
}




void dlgRepPath::CheckChange()
{
	if (path)
	{
		EnableOK(txtConnInfo->GetValue() != path->GetConnInfo()
		         ||   StrToLong(txtConnRetry->GetValue()) != path->GetConnRetry());
	}
	else
	{
		bool enable = true;
		CheckValid(enable, cbServer->GetCount() > 0, _("No provider node without path definition left."));
		CheckValid(enable, cbServer->GetCurrentSelection() >= 0, _("Please select provider node."));

		wxString connInfo = txtConnInfo->GetValue();
		CheckValid(enable, connInfo.Find(wxT("host=")) >= 0, _("Please provide host in connect info."));
		CheckValid(enable, connInfo.Find(wxT("dbname=")) >= 0, _("Please provide dbname in connect info."));
		CheckValid(enable, connInfo.Find(wxT("user=")) >= 0, _("Please provide user in connect info."));

		EnableOK(enable);
	}
}



wxString dlgRepPath::GetSql()
{
	wxString sql;

	int sel = cbServer->GetCurrentSelection();

	if (sel >= 0)
	{
		sql = wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("storepath(")
		      + NumToStr((OID)cbServer->wxItemContainer::GetClientData(sel)) + wxT(", ")
		      + NumToStr(node->GetSlId()) + wxT(", ")
		      + qtDbString(txtConnInfo->GetValue()) + wxT(", ")
		      + NumToStr(StrToLong(txtConnRetry->GetValue())) + wxT(");");
	}
	return sql;
}
