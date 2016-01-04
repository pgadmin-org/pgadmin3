//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slNode.cpp PostgreSQL Slony-I node
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgObject.h"
#include "slony/slNode.h"
#include "slony/slCluster.h"
#include "slony/slPath.h"
#include "slony/slListen.h"
#include "slony/slSet.h"
#include "frm/frmMain.h"


slNode::slNode(slCluster *cl, const wxString &newName)
	: slObject(cl, nodeFactory, newName)
{
	pid = -1;
}

int slNode::GetIconId()
{
	if (!GetActive())
		return nodeFactory.GetDisabledIconId();
	else if (GetSlId() == GetCluster()->GetLocalNodeID())
		return nodeFactory.GetLocalIconId();
	else
		return nodeFactory.GetIconId();
}


wxMenu *slNode::GetNewMenu()
{
	wxMenu *menu = pgObject::GetNewMenu();

//    if (GetCreatePrivilege())
	pathFactory.AppendMenu(menu);
	listenFactory.AppendMenu(menu);

	return menu;
}


bool slNode::CanDrop()
{
	return (GetSlId() != GetCluster()->GetLocalNodeID());
}


bool slNode::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetDatabase()->ExecuteVoid(
	           wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
	           + wxT("dropnode(") + NumToStr(GetSlId()) + wxT(");\n"));
}


wxString slNode::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony node");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony node");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony node \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony node \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop Slony node cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop Slony node?");
			break;
		case PROPERTIESREPORT:
			message = _("Slony node properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Slony node properties");
			break;
		case DDLREPORT:
			message = _("Slony node DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Slony node DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Slony node dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Slony node dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Slony node dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Slony node dependents");
			break;
	}

	return message;
}

bool slNode::WaitForEvent(long evNode)
{
	return true;
}


wxString slNode::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Create replication node ") + GetName() + wxT(".\n\n")
		      wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("storenode(")
		      + NumToStr(GetSlId()) + wxT(", ")
		      + qtDbString(GetComment());

		if (GetCluster()->ClusterMinimumVersion(1, 1) && !GetCluster()->ClusterMinimumVersion(2, 0))
			sql += wxT(", ") + BoolToStr(GetSpool());
		sql += wxT(");\n");
	}
	return sql;
}


long slNode::GetOutstandingAcks()
{
	long l = StrToLong(GetDatabase()->ExecuteScalar(
	                       wxT("SELECT SUM(st_lag_num_events) AS sumlagevents\n")
	                       wxT("  FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_status\n")
	                       wxT(" WHERE st_origin = ") + NumToStr(GetCluster()->GetLocalNodeID()) + wxT("\n")
	                       wxT("  GROUP BY st_last_event")));

	return l;
}


bool slNode::CheckAcksAndContinue(wxFrame *frame)
{
	long l = GetOutstandingAcks();
	if (!l)
		return true;

	wxMessageDialog dlg(frame, wxString::Format(wxPLURAL("There are %ld event acknowledgement outstanding.\nContinue anyway?",
	                    "There are %ld event acknowledgements outstanding.\nContinue anyway?", l), l),
	                    _("Events pending"), wxYES_NO | wxNO_DEFAULT);

	return dlg.ShowModal() == wxID_YES;
}


void slNode::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	CreateListColumns(statistics, _("Statistic"), _("Value"));

	if (GetActive())
	{
		if (GetCluster()->GetLocalNodeID() == GetSlId())
		{
			pgSet *stats = GetDatabase()->ExecuteSet(
			                   wxT("SELECT st_last_event,\n")
			                   wxT("       MAX(st_last_received_ts - st_last_received_event_ts) AS roundtrip,\n")
			                   wxT("       SUM(st_lag_num_events) AS sumlagevents, st_last_event - MAX(st_lag_num_events) as oldestlagevent,\n")
			                   wxT("       MAX(st_last_event_ts - st_last_received_ts) AS maxeventlag\n")
			                   wxT("  FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_status\n")
			                   wxT(" WHERE st_origin = ") + NumToStr(GetCluster()->GetLocalNodeID()) + wxT("\n")
			                   wxT("  GROUP BY st_last_event"));

			if (stats)
			{
				wxString roundtrip = stats->GetVal(wxT("roundtrip"));
				long lags = stats->GetLong(wxT("sumlagevents"));
				if (roundtrip.Left(6) == wxT("00:00:"))
					roundtrip = NumToStr(StrToLong(roundtrip.Mid(6))) + roundtrip.Mid(8) + wxT(" s");
				else if (roundtrip.Left(3) == wxT("00:"))
					roundtrip =  NumToStr(StrToLong(roundtrip.Mid(3))) + roundtrip.Mid(5);

				statistics->AppendItem(_("Last event"), stats->GetLong(wxT("st_last_event")));
				statistics->AppendItem(_("Max response time"), roundtrip);
				statistics->AppendItem(_("Acks outstanding"), lags);


				if (lags > 0)
				{
					long lagEvent = stats->GetLong(wxT("oldestlagevent"));
					statistics->AppendItem(_("Oldest outstanding"), lagEvent);
					statistics->AppendItem(_("Outstanding for"), stats->GetVal(wxT("maxeventlag")));
				}

				delete stats;
			}
		}
		else
		{
			pgSet *stats = GetDatabase()->ExecuteSet(
			                   wxT("SELECT st_last_event, st_last_event_ts, st_last_received, st_last_received_ts,\n")
			                   wxT("       st_last_received_ts - st_last_received_event_ts AS roundtrip,\n")
			                   wxT("       st_last_event_ts - st_last_received_ts AS eventlag,")
			                   wxT("       ev_seqno, ev_type || ' ' || COALESCE(ev_data1, '') AS hanging\n")
			                   wxT("  FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_status\n")
			                   wxT("  LEFT JOIN ") + GetCluster()->GetSchemaPrefix() + wxT("sl_event ON ev_origin=st_origin AND ev_seqno=\n")
			                   wxT("         (SELECT MIN(ev_seqno) FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_event WHERE ev_seqno > st_last_received)\n")
			                   wxT(" WHERE st_origin = ") + NumToStr(GetCluster()->GetLocalNodeID()) + wxT("\n")
			                   wxT("   AND st_received = ") + NumToStr(GetSlId()));

			if (stats)
			{
				long evno = stats->GetLong(wxT("st_last_event"));
				long ack = stats->GetLong(wxT("st_last_received"));
				wxString roundtrip = stats->GetVal(wxT("roundtrip"));
				if (roundtrip.Left(6) == wxT("00:00:"))
					roundtrip = NumToStr(StrToLong(roundtrip.Mid(6))) + roundtrip.Mid(8) + wxT(" s");
				else if (roundtrip.Left(3) == wxT("00:"))
					roundtrip =  NumToStr(StrToLong(roundtrip.Mid(3))) + roundtrip.Mid(5);

				statistics->AppendItem(_("Last event"), evno);
				statistics->AppendItem(_("Last event timestamp"), stats->GetDateTime(wxT("st_last_event_ts")));
				statistics->AppendItem(_("Last acknowledged"), ack);
				statistics->AppendItem(_("Last ack timestamp"), stats->GetDateTime(wxT("st_last_received_ts")));
				statistics->AppendItem(_("Last response time"), roundtrip);

				if (evno > ack)
				{
					statistics->AppendItem(_("Outstanding acks"), evno - ack);
					statistics->AppendItem(_("No ack for"), stats->GetVal(wxT("eventlag")));
					statistics->AppendItem(_("Hanging event"), stats->GetVal(wxT("ev_seqno")));
					statistics->AppendItem(_("Command"), stats->GetVal(wxT("hanging")));
				}

				delete stats;
			}
		}
	}
	else
		statistics->AppendItem(_("not active"), wxEmptyString);
}


void slNode::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	wxString pidcol = wxEmptyString;
	pgConn *conn = GetCluster()->GetNodeConn(form, GetSlId(), pid < 0);

	if (!expandedKids)
	{
		expandedKids = true;

		browser->RemoveDummyChild(this);
		// Log
		wxLogInfo(wxT("Adding child object to node %s"), GetIdentifier().c_str());

		browser->AppendCollection(this, pathFactory);
		browser->AppendCollection(this, listenFactory);
	}


	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("ID"), GetSlId());
		if (GetCluster()->GetLocalNodeID() == GetSlId())
			properties->AppendYesNoItem(_("Local node"), true);
		properties->AppendYesNoItem(_("Active"), GetActive());
		if (GetCluster()->ClusterMinimumVersion(1, 1))
			properties->AppendYesNoItem(_("Log spooler"), GetSpool());
		properties->AppendYesNoItem(_("Connected"), conn != NULL);
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));

		if (conn && pid < 0)
		{
			if (conn->BackendMinimumVersion(9, 0))
			{
				if (conn->BackendMinimumVersion(9, 2))
					pidcol = wxT("pid");
				else
					pidcol = wxT("procpid");

				pid = StrToLong(conn->ExecuteScalar(
				                    wxT("SELECT nl_backendpid FROM ") + qtIdent(wxT("_") + GetCluster()->GetName()) + wxT(".sl_nodelock nl, ")
				                    wxT("pg_stat_activity sa WHERE nl.nl_backendpid = sa.") + pidcol + wxT(" AND nl_nodeid = ")
				                    + NumToStr(GetCluster()->GetLocalNodeID())));
			}
			else
			{
				pid = StrToLong(conn->ExecuteScalar(
				                    wxT("SELECT listenerpid FROM pg_listener WHERE relname=") + qtDbString(wxT("_") + GetCluster()->GetName() + wxT("_Restart"))));
			}
		}

		if (conn)
		{
			if (GetPid())
				properties->AppendItem(_("Running PID"), GetPid());
			else
				properties->AppendItem(_("Running PID"), _("not running"));
		}
		else
		{
			if (GetSlId() != GetCluster()->GetAdminNodeID())
				properties->AppendItem(_("Running PID"), _("administrative node"));
		}
	}
}



pgObject *slNode::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *node = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		node = nodeFactory.CreateObjects(coll, 0, wxT(" WHERE no_id=") + NumToStr(GetSlId()) + wxT("\n"));

	return node;
}



pgObject *slNodeFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
	slObjCollection *collection = (slObjCollection *)coll;
	slNode *node = 0;

	pgSet *nodes = collection->GetDatabase()->ExecuteSet(
	                   wxT("SELECT * FROM ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_node\n")
	                   + restriction +
	                   wxT(" ORDER BY no_id"));

	if (nodes)
	{
		while (!nodes->Eof())
		{
			node = new slNode(collection->GetCluster(), nodes->GetVal(wxT("no_comment")).BeforeFirst('\n'));
			node->iSetSlId(nodes->GetLong(wxT("no_id")));
			node->iSetActive(nodes->GetBool(wxT("no_active")));
			node->iSetComment(nodes->GetVal(wxT("no_comment")));

			if (collection->GetCluster()->ClusterMinimumVersion(1, 1))
			{
				if (nodes->HasColumn(wxT("no_spool")))
					node->iSetSpool(nodes->GetBool(wxT("no_spool")));
			}

			if (browser)
			{
				browser->AppendObject(collection, node);
				nodes->MoveNext();
			}
			else
				break;
		}

		delete nodes;
	}
	return node;
}


wxString slNodeCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony nodes");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony nodes");
			break;
		case OBJECTSLISTREPORT:
			message = _("Slony nodes list report");
			break;
	}

	return message;
}


void slNodeCollection::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	wxLogInfo(wxT("Displaying statistics for nodes on Cluster %s"), GetCluster()->GetIdentifier().c_str());

	// Add the statistics view columns
	statistics->ClearAll();
	statistics->AddColumn(_("Node"), 50);
	statistics->AddColumn(_("Roundtrip"), 50);
	statistics->AddColumn(_("Acks outstanding"), 50);
	statistics->AddColumn(_("Outstanding time"), 50);
	statistics->AddColumn(_("Event No"), 50);
	statistics->AddColumn(_("Command"), 250);

	pgSet *stats = GetDatabase()->ExecuteSet(
	                   wxT("SELECT st_received, st_last_event, st_lag_num_events, st_last_event_ts, st_last_received, st_last_received_ts,\n")
	                   wxT("       st_last_received_ts - st_last_received_event_ts AS roundtrip,\n")
	                   wxT("       CASE WHEN st_lag_num_events > 0 THEN st_last_event_ts - st_last_received_ts END AS eventlag,")
	                   wxT("       ev_seqno, ev_type || ' ' || COALESCE(ev_data1, '') AS hanging\n")
	                   wxT("  FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_status\n")
	                   wxT("  LEFT JOIN ") + GetCluster()->GetSchemaPrefix() + wxT("sl_event ON ev_origin=st_origin AND ev_seqno=\n")
	                   wxT("         (SELECT MIN(ev_seqno) FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_event WHERE ev_seqno > st_last_received)\n")
	                   wxT(" WHERE st_origin = ") + NumToStr(GetCluster()->GetLocalNodeID()));

	if (stats)
	{
		long pos = 0;
		while (!stats->Eof())
		{
			long lagEvents = stats->GetLong(wxT("st_lag_num_events"));
			statistics->InsertItem(pos, NumToStr(stats->GetLong(wxT("st_received"))), nodeFactory.GetIconId());
			statistics->SetItem(pos, 1, stats->GetVal(wxT("roundtrip")));
			statistics->SetItem(pos, 2, NumToStr(lagEvents));
			statistics->SetItem(pos, 3, stats->GetVal(wxT("eventlag")));
			if (lagEvents)
			{
				statistics->SetItem(pos, 4, stats->GetVal(wxT("ev_seqno")));
				statistics->SetItem(pos, 5, stats->GetVal(wxT("hanging")));
			}
			stats->MoveNext();
			pos++;
		}

		delete stats;
	}
}


///////////////////////////////////////////////////

#include "images/slnode.pngc"
#include "images/slnode-local.pngc"
#include "images/slnode-disabled.pngc"
#include "images/slnodes.pngc"

slNodeFactory::slNodeFactory()
	: slObjFactory(__("Node"), __("New Node"), __("Create a new Node."), slnode_png_img)
{
	localIconId = addIcon(slnode_local_png_img);
	disabledIconId = addIcon(slnode_disabled_png_img);
	metaType = SLM_NODE;
}

pgCollection *slNodeFactory::CreateCollection(pgObject *obj)
{
	return new slNodeCollection(GetCollectionFactory(), (slCluster *)obj);
}


slNodeObject::slNodeObject(slNode *n, pgaFactory &factory, const wxString &newName)
	: slObject(n->GetCluster(), factory, newName)
{
	node = n;
}

slNodeObjCollection::slNodeObjCollection(pgaFactory *factory, slNode *n)
	: slObjCollection(factory, n->GetCluster())
{
	node = n;
	iSetSlId(n->GetSlId());
}


pgCollection *slNodeObjFactory::CreateCollection(pgObject *obj)
{
	return new slNodeObjCollection(GetCollectionFactory(), (slNode *)obj);
}


slNodeFactory nodeFactory;
static pgaCollectionFactory cf(&nodeFactory, __("Nodes"), slnodes_png_img);
