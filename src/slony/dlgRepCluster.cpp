//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepCluster.cpp - PostgreSQL Slony-I Cluster Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgDefs.h"
#include <wx/textbuf.h>
#include <wx/file.h>

#include "frmMain.h"
#include "dlgRepCluster.h"
#include "slCluster.h"
#include "slSet.h"
#include "slCluster.h"
#include "pgDatatype.h"
#include "sysProcess.h"


// Images
#include "images/slcluster.xpm"

extern wxString slony1BaseScript;
extern wxString slony1FunctionScript;
extern wxString slony1XxidScript;
extern wxString backupExecutable;


// pointer to controls
#define chkJoinCluster      CTRL_CHECKBOX("chkJoinCluster")
#define cbClusterName       CTRL_COMBOBOX("cbClusterName")
#define txtClusterName      CTRL_TEXT("txtClusterName")
#define cbServer            CTRL_COMBOBOX("cbServer")
#define cbDatabase          CTRL_COMBOBOX("cbDatabase")
#define txtNodeID           CTRL_TEXT("txtNodeID")
#define txtNodeName         CTRL_TEXT("txtNodeName")
#define txtAdminNodeID      CTRL_TEXT("txtAdminNodeID")
#define txtAdminNodeName    CTRL_TEXT("txtAdminNodeName")
#define cbAdminNode         CTRL_COMBOBOX("cbAdminNode")

BEGIN_EVENT_TABLE(dlgRepCluster, dlgProperty)
    EVT_BUTTON(wxID_OK,                     dlgRepCluster::OnOK)
    EVT_CHECKBOX(XRCID("chkJoinCluster"),   dlgRepCluster::OnChangeJoin)
    EVT_COMBOBOX(XRCID("cbServer"),         dlgRepCluster::OnChangeServer)
    EVT_COMBOBOX(XRCID("cbDatabase"),       dlgRepCluster::OnChangeDatabase)
    EVT_COMBOBOX(XRCID("cbClusterName"),    dlgRepCluster::OnChangeCluster)
    EVT_TEXT(XRCID("txtClusterName"),       dlgRepCluster::OnChange)
    EVT_TEXT(XRCID("txtNodeID"),            dlgRepCluster::OnChange)
    EVT_TEXT(XRCID("txtNodeName"),          dlgRepCluster::OnChange)
    EVT_COMBOBOX(XRCID("cbAdminNode"),      dlgRepCluster::OnChange)
    EVT_END_PROCESS(-1,                     dlgRepCluster::OnEndProcess)
END_EVENT_TABLE();


dlgRepCluster::dlgRepCluster(frmMain *frame, slCluster *node, pgDatabase *db)
: dlgProperty(frame, wxT("dlgRepCluster"))
{
    SetIcon(wxIcon(slcluster_xpm));
    cluster=node;
    remoteServer=0;
    remoteConn=0;
    process = 0;

    pgObject *obj=db;
    servers = obj->GetId();
    while (obj && obj->GetType() != PG_SERVERS)
    {
        servers = frame->GetBrowser()->GetItemParent(servers);
        if (servers)            
            obj = (pgObject*)frame->GetBrowser()->GetItemData(servers);
    }
}


dlgRepCluster::~dlgRepCluster()
{
   if (remoteConn)
    {
        delete remoteConn;
        remoteConn=0;
    }
}


pgObject *dlgRepCluster::GetObject()
{
    return cluster;
}


int dlgRepCluster::Go(bool modal)
{
    chkJoinCluster->SetValue(false);

    if (cluster)
    {
        // edit mode
        txtClusterName->SetValue(cluster->GetName());
        txtNodeID->SetValue(NumToStr(cluster->GetLocalNodeID()));
        txtClusterName->Disable();
        txtNodeID->Disable();
        txtNodeName->SetValue(cluster->GetLocalNodeName());
        txtNodeName->Disable();
        chkJoinCluster->Disable();

        txtAdminNodeID->Hide();
        txtAdminNodeName->Hide();

        wxString sql=
            wxT("SELECT no_id, no_comment\n")
            wxT("  FROM ") + cluster->GetSchemaPrefix() + wxT("sl_node\n")
            wxT("  JOIN ") + cluster->GetSchemaPrefix() + wxT("sl_path ON no_id = pa_client\n")
            wxT(" WHERE pa_server = ") + NumToStr(cluster->GetLocalNodeID()) + 
            wxT("   AND pa_conninfo LIKE ") + qtString(wxT("%host=") + cluster->GetServer()->GetName() + wxT("%")) +
            wxT("   AND pa_conninfo LIKE ") + qtString(wxT("%dbname=") + cluster->GetDatabase()->GetName() + wxT("%"));

        if (cluster->GetServer()->GetPort() != 5432)
            sql += wxT("   AND pa_conninfo LIKE ") + qtString(wxT("%port=") + NumToStr((long)cluster->GetServer()->GetPort()) + wxT("%"));

        sql += wxT(" ORDER BY no_id");

        pgSet *set=connection->ExecuteSet(sql);
        if (set)
        {
            while (!set->Eof())
            {
                long id=set->GetLong(wxT("no_id"));
                cbAdminNode->Append(IdAndName(id, set->GetVal(wxT("no_comment"))), (void*)id);
                if (id == cluster->GetAdminNodeID())
                    cbAdminNode->SetSelection(cbAdminNode->GetCount()-1);

                set->MoveNext();
            }
            delete set;
        }
        if (!cbAdminNode->GetCount())
        {
            cbAdminNode->Append(_("<none>"), (void*)-1);
            cbAdminNode->SetSelection(0);
        }
    }
    else
    {
        // create mode
        cbAdminNode->Hide();

        txtNodeID->SetValidator(numericValidator);
        txtAdminNodeID->SetValidator(numericValidator);
        txtClusterName->Hide();

        wxCookieType cookie;
        wxTreeItemId serverItem=mainForm->GetBrowser()->GetFirstChild(servers, cookie);        
        while (serverItem)
        {
            pgServer *s = (pgServer*)mainForm->GetBrowser()->GetItemData(serverItem);
            if (s && s->GetType() == PG_SERVER)
                cbServer->Append(mainForm->GetBrowser()->GetItemText(serverItem), (void*)s);

            serverItem = mainForm->GetBrowser()->GetNextChild(servers, cookie);
        }
    }

    wxCommandEvent ev;
    OnChangeJoin(ev);

    return dlgProperty::Go(modal);
}


void dlgRepCluster::OnChangeJoin(wxCommandEvent &ev)
{
    bool joinCluster = chkJoinCluster->GetValue();
    txtClusterName->Show(!joinCluster);
    cbClusterName->Show(joinCluster);

    cbServer->Enable(joinCluster);
    cbDatabase->Enable(joinCluster);

    txtAdminNodeID->Show(!joinCluster && !cluster);
    txtAdminNodeName->Show(!joinCluster && !cluster);
    cbAdminNode->Show(joinCluster || cluster);

    OnChange(ev);
}


void dlgRepCluster::OnChangeServer(wxCommandEvent &ev)
{
    cbDatabase->Clear();
    if (remoteConn)
    {
        delete remoteConn;
        remoteConn=0;
    }
    int sel=cbServer->GetSelection();
    if (sel >= 0)
    {
        remoteServer = (pgServer*)cbServer->GetClientData(sel);

        if (!remoteServer->GetConnected())
        {
            remoteServer->Connect(mainForm, remoteServer->GetNeedPwd());
            if (!remoteServer->GetConnected())
            {
                wxLogError(remoteServer->GetLastError());
                return;
            }
        }
        if (remoteServer->GetConnected())
        {
            pgSet *set=remoteServer->ExecuteSet(
                wxT("SELECT DISTINCT datname\n")
                wxT("  FROM pg_database db\n")
                wxT(" WHERE datallowconn ORDER BY datname"));
            if (set)
            {
                while (!set->Eof())
                {
                    cbDatabase->Append(set->GetVal(wxT("datname")));
                    set->MoveNext();
                }
                delete set;

                cbDatabase->SetSelection(0);
            }
        }

    }
    OnChangeDatabase(ev);
}


void dlgRepCluster::OnChangeDatabase(wxCommandEvent &ev)
{
    cbClusterName->Clear();

    int sel=cbDatabase->GetSelection();
    if (sel >= 0)
    {
        if (remoteConn)
        {
            delete remoteConn;
            remoteConn=0;
        }
        remoteConn = remoteServer->CreateConn(cbDatabase->GetValue());
        if (remoteConn)
        {
            pgSet *set=remoteConn->ExecuteSet(
                wxT("SELECT substr(nspname, 2) as clustername\n")
                wxT("  FROM pg_namespace nsp\n")
                wxT("  JOIN pg_proc pro ON pronamespace=nsp.oid AND proname = 'slonyversion'\n")
                wxT(" ORDER BY nspname"));

            if (set)
            {
                while (!set->Eof())
                {
                    cbClusterName->Append(set->GetVal(wxT("clustername")));
                    set->MoveNext();
                }
                delete set;
            }
            cbClusterName->SetSelection(0);
        }
    }
    OnChangeCluster(ev);
}


void dlgRepCluster::OnChangeCluster(wxCommandEvent &ev)
{
    clusterBackup = wxEmptyString;

    cbAdminNode->Clear();
    cbAdminNode->Append(_("<none>"), (void*)-1);

    int sel=cbClusterName->GetSelection();
    if (remoteConn && sel >= 0)
    {
        wxString schemaPrefix = qtIdent(wxT("_") + cbClusterName->GetValue()) + wxT(".");
        long adminNodeID = settings->Read(wxT("Replication/") + cbClusterName->GetValue() + wxT("/AdminNode"), -1L);

        wxString sql=
            wxT("SELECT no_id, no_comment\n")
            wxT("  FROM ") + schemaPrefix + wxT("sl_node\n")
            wxT("  JOIN ") + schemaPrefix + wxT("sl_path ON no_id = pa_client\n")
            wxT(" WHERE pa_server = (SELECT last_value FROM ") + schemaPrefix + wxT("sl_local_node_id)")
            wxT("   AND pa_conninfo LIKE ") + qtString(wxT("%host=") + remoteServer->GetName() + wxT("%")) +
            wxT("   AND pa_conninfo LIKE ") + qtString(wxT("%dbname=") + cbDatabase->GetValue() + wxT("%"));

        if (remoteServer->GetPort() != 5432)
            sql += wxT("   AND pa_conninfo LIKE ") + qtString(wxT("%port=") + NumToStr((long)remoteServer->GetPort()) + wxT("%"));

        pgSet *set = remoteConn->ExecuteSet(sql);
        if (set)
        {
            if (!set->Eof())
            {
                long id = set->GetLong(wxT("no_id"));
                cbAdminNode->Append(IdAndName(id, set->GetVal(wxT("no_comment"))), (void*)id);
                if (adminNodeID == id)
                    cbAdminNode->SetSelection(cbAdminNode->GetCount()-1);
            }
            delete set;
        }


        usedNodes.Clear();
        set=remoteConn->ExecuteSet(
            wxT("SELECT no_id FROM ") + schemaPrefix + wxT("sl_node"));

        if (set)
        {
            while (!set->Eof())
            {
                usedNodes.Add(set->GetLong(wxT("no_id")));
                set->MoveNext();
            }
            delete set;
        }
    }
    OnChange(ev);
}



bool dlgRepCluster::CopyTable(pgConn *from, pgConn *to, const wxString &table)
{
    bool ok=true;

    pgSet *set=from->ExecuteSet(wxT("SELECT * FROM ") + table);
    if (!set)
        return false;
    
    while (ok && !set->Eof())
    {
        wxString sql=wxT("INSERT INTO ") + table + wxT("(");
        wxString vals;
        int i;

        for (i=0 ; i < set->NumCols() ; i++)
        {
            if (i)
            {
                sql += wxT(", ");;
                vals += wxT(", ");
            }

            sql += set->ColName(i);

            if (set->IsNull(i))
                vals += wxT("NULL");
            else
            {
                switch (set->ColTypeOid(i))
                {
                    case PGOID_TYPE_BOOL:
                    case PGOID_TYPE_BYTEA:
                    case PGOID_TYPE_CHAR:
                    case PGOID_TYPE_NAME:
                    case PGOID_TYPE_TEXT:
                    case PGOID_TYPE_VARCHAR:
                    case PGOID_TYPE_TIME:
                    case PGOID_TYPE_TIMESTAMP:
                    case PGOID_TYPE_TIME_ARRAY:
                    case PGOID_TYPE_TIMESTAMPTZ:
                    case PGOID_TYPE_INTERVAL:
                    case PGOID_TYPE_TIMETZ:
                        vals += qtString(set->GetVal(i));
                        break;
                    default:
                        vals += set->GetVal(i);
                }
            }
        }

        ok = to->ExecuteVoid(
            sql + wxT(")\n VALUES (") + vals + wxT(");"));
                   

        set->MoveNext();
    }
    delete set;
    return ok;
}


void dlgRepCluster::OnOK(wxCommandEvent &ev)
{
    EnableOK(false);

    bool done=true;
    done = connection->ExecuteVoid(wxT("BEGIN TRANSACTION;"));

    if (remoteConn)
        done = remoteConn->ExecuteVoid(wxT("BEGIN TRANSACTION;"));

    // initialize cluster on local node
    done = connection->ExecuteVoid(GetSql());

    if (done && chkJoinCluster->GetValue())
    {
        // we're joining an existing cluster

        wxString schemaPrefix = qtIdent(wxT("_") + cbClusterName->GetValue()) + wxT(".");

        wxString clusterVersion = remoteConn->ExecuteScalar(
            wxT("SELECT ") + schemaPrefix + wxT("slonyversion()"));

        wxString newVersion = connection->ExecuteScalar(
            wxT("SELECT ") + schemaPrefix + wxT("slonyversion()"));
        
        if (clusterVersion != newVersion)
        {
            wxMessageDialog msg(this, 
                    wxString::Format(_("The newly created cluster version (%s)\n doesn't match the existing cluster's version (%s)"), 
                                        newVersion.c_str(), clusterVersion.c_str()),
                    _("Error while joining replication cluster"), wxICON_ERROR);
            msg.ShowModal();
            done = false;
        }

        if (done)
            done = CopyTable(remoteConn, connection, schemaPrefix + wxT("sl_node"));
        if (done)
            done = CopyTable(remoteConn, connection, schemaPrefix + wxT("sl_path"));
        if (done)
            done = CopyTable(remoteConn, connection, schemaPrefix + wxT("sl_listen"));
        if (done)
            done = CopyTable(remoteConn, connection, schemaPrefix + wxT("sl_set"));
        if (done)
            done = CopyTable(remoteConn, connection, schemaPrefix + wxT("sl_subscribe"));


        // make sure event seqno starts correctly after node reusage
        if (done)
        {
            pgSet *set=connection->ExecuteSet(
                wxT("SELECT ev_origin, MAX(ev_seqno) as seqno\n")
                wxT("  FROM ") + schemaPrefix + wxT("sl_event\n")
                wxT(" GROUP BY ev_origin"));
            if (set)
            {
                while (done && !set->Eof())
                {
                    if (set->GetVal(wxT("ev_origin")) == txtNodeID->GetValue())
                    {
                        done = connection->ExecuteVoid(
                            wxT("SELECT pg_catalog.setval(") + 
                                qtString(wxT("_") + cbClusterName->GetValue() + wxT(".sl_event_seq")) + 
                            wxT(", ") + set->GetVal(wxT("seqno")) + wxT("::int8 +1)"));
                    }
                    else
                    {
                        done = connection->ExecuteVoid(
                            wxT("INSERT INTO ") + schemaPrefix + wxT("sl_confirm(con_origin, con_received, con_seqno, con_timestamp\n")
                            wxT(" VALUES (") + set->GetVal(wxT("ev_origin")) +
                            wxT(", ") + txtNodeID->GetValue() +
                            wxT(", ") + set->GetVal(wxT("seqno")) +
                            wxT(", current_timestamp")); 

                    }
                    set->MoveNext();
                }
                delete set;
            }
        }


        // make sure rowid seq starts correctly
        if (done)
        {
            wxString seqno = connection->ExecuteScalar(
                wxT("SELECT MAX(seql_last_value)\n")
                wxT("  FROM ") + schemaPrefix + wxT("sl_seqlog\n")
                wxT(" WHERE seql_seqid = 0 AND seql_origin = ") + txtNodeID->GetValue());

            if (!seqno.IsEmpty())
            {
                done = connection->ExecuteVoid(
                    wxT("SELECT pg_catalog.setval(") + 
                    qtString(wxT("_") + cbClusterName->GetValue() + wxT(".sl_rowid_seq")) + 
                    wxT(", ") + seqno + wxT(")"));
            }
        }

        // create new node on existing cluster
        if (done)
            done = remoteConn->ExecuteVoid(
                wxT("SELECT ") + schemaPrefix + wxT("storenode(") 
                    + txtNodeID->GetValue() + wxT(", ")
                    + qtString(txtNodeName->GetValue()) + wxT(");\n")
                wxT("SELECT ") + schemaPrefix + wxT("enablenode(") 
                    + txtNodeID->GetValue() + wxT(");\n"));

        // add admin info to cluster

        if (done && cbAdminNode->GetSelection() > 0)
        {
            done = remoteConn->ExecuteVoid(
                wxT("SELECT ") + schemaPrefix + wxT("storepath(") +
                txtNodeID->GetValue() + wxT(", ") +
                    NumToStr((long)cbAdminNode->GetClientData(cbAdminNode->GetSelection())) + wxT(", ") +
                    qtString(wxT("host=") + database->GetServer()->GetName() + 
                            wxT(" port=") + NumToStr((long)database->GetServer()->GetPort()) +
                            wxT(" dbname=") + database->GetName()) + wxT(", ")
                    wxT("0);\n"));
        }
    }

    if (done && !cluster && !chkJoinCluster->GetValue())
    {
        wxString schemaPrefix = qtIdent(wxT("_") + txtClusterName->GetValue()) + wxT(".");
        long adminNode = StrToLong(txtAdminNodeID->GetValue());
        if (adminNode > 0 && adminNode != StrToLong(txtNodeID->GetValue()))
        {
            done = connection->ExecuteVoid(
                wxT("SELECT ") + schemaPrefix + wxT("storeNode(") +
                    NumToStr(adminNode) + wxT(", ") +
                    qtString(txtAdminNodeName->GetValue()) + wxT(");\n")
                wxT("SELECT ") + schemaPrefix + wxT("storepath(") +
                    NumToStr(adminNode) + wxT(", ") +
                    txtNodeID->GetValue() + wxT(", ") +
                    qtString(wxT("host=") + database->GetServer()->GetName() + 
                            wxT(" port=") + NumToStr((long)database->GetServer()->GetPort()) +
                            wxT(" dbname=") + database->GetName()) + wxT(", ")
                    wxT("0);\n"));
            
        }
    }

    if (!done)
    {
        if (remoteConn)
            done = remoteConn->ExecuteVoid(wxT("ROLLBACK TRANSACTION;"));
        done = connection->ExecuteVoid(wxT("ROLLBACK TRANSACTION;"));
        EnableOK(true);
        return;
    }

    if (remoteConn)
        done = remoteConn->ExecuteVoid(wxT("COMMIT TRANSACTION;"));
    done = connection->ExecuteVoid(wxT("COMMIT TRANSACTION;"));
    
    ShowObject();
    Destroy();
}


pgObject *dlgRepCluster::CreateObject(pgCollection *collection)
{
    pgObject *obj=slCluster::ReadObjects(collection, 0,
         wxT(" WHERE nspname = ") + qtString(wxT("_") + GetName()));

    return obj;
}


void dlgRepCluster::CheckChange()
{
    if (cluster)
    {
        int sel=cbAdminNode->GetSelection();
        bool changed = (sel >= 0 && (long)cbAdminNode->GetClientData() != cluster->GetAdminNodeID());

        EnableOK(changed || txtComment->GetValue() != cluster->GetComment());
    }
    else
    {
        size_t i;
        bool enable=true;

        CheckValid(enable, chkJoinCluster->GetValue() || 
            (!slony1BaseScript.IsEmpty() && !slony1FunctionScript.IsEmpty() && !slony1XxidScript.IsEmpty()),
            _("Slony-I creation scripts not available; only joining possible."));

        CheckValid(enable, !cbClusterName->GetValue().IsEmpty(), _("Please specify name."));

        long nodeId = StrToLong(txtNodeID->GetValue());
        CheckValid(enable, nodeId > 0, _("Please specify local node ID."));
        for (i=0 ; i < usedNodes.GetCount() && enable; i++)
            CheckValid(enable, nodeId != usedNodes[i], _("Node ID is already in use."));

        CheckValid(enable, !txtNodeName->GetValue().IsEmpty(), _("Please specify local node name."));

        txtAdminNodeName->Enable(nodeId != StrToLong(txtAdminNodeID->GetValue()));

        EnableOK(enable);
    }
}



void dlgRepCluster::OnEndProcess(wxProcessEvent &ev)
{
    if (process)
    {
        wxString error = process->ReadErrorStream();
        clusterBackup += process->ReadInputStream();
        delete process;
        process=0;
    }
}


wxString dlgRepCluster::GetSql()
{
    wxString sql;
    wxString name;
    if (chkJoinCluster->GetValue())
        name = wxT("_") + cbClusterName->GetValue();
    else
        name = wxT("_") + txtClusterName->GetValue();

    wxString quotedName=qtIdent(name);


    if (cluster)
    {
        // edit mode
        int sel=cbAdminNode->GetSelection();
        if (sel >= 0)
        {
            long id=(long)cbAdminNode->GetClientData(sel);
            if (id != cluster->GetAdminNodeID())
                settings->Write(wxT("Replication/") + cluster->GetName() + wxT("/AdminNode"), id);
        }
    }
    else
    {
        // create mode

        if (clusterBackup.IsEmpty() && !backupExecutable.IsEmpty())
        {
            wxArrayString environment;
            environment.Add(wxT("PGPASSWORD=") + remoteServer->GetPassword());

            process=sysProcess::Create(backupExecutable + 
                                    wxT(" -i -F p -h ") + remoteServer->GetName() +
                                    wxT(" -p ") + NumToStr((long)remoteServer->GetPort()) +
                                    wxT(" -U ") + remoteServer->GetUsername() +
                                    wxT(" -s -O -n ") + name +
                                    wxT(" ") + cbDatabase->GetValue(), 
                                    this, &environment);

            wxBusyCursor wait;
            while (process)
            {
                wxSafeYield();
                clusterBackup += process->ReadInputStream();
                wxSafeYield();
                wxMilliSleep(10);
            }
        }

        if (!clusterBackup.IsEmpty())
        {
            int opclassPos = clusterBackup.Find(wxT("CREATE OPERATOR CLASS"));
            sql = wxT("-- Extracted schema from existing cluster\n\n") +
                clusterBackup.Left(opclassPos > 0 ? opclassPos : 99999999);
            if (opclassPos > 0)
            {
                sql +=  wxT("----------- inserted by pgadmin: add public operators\n")
                        wxT("CREATE OPERATOR public.< (PROCEDURE = xxidlt,")
                        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
                        wxT("    COMMUTATOR = public.\">\", NEGATOR = public.\">=\",")
                        wxT("    RESTRICT = scalarltsel, JOIN = scalarltjoinsel);\n")
                        wxT("CREATE OPERATOR public.= (PROCEDURE = xxideq,")
                        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
                        wxT("    COMMUTATOR = public.\"=\", NEGATOR = public.\"<>\",")
                        wxT("    RESTRICT = eqsel, JOIN = eqjoinsel,")
                        wxT("    SORT1 = public.\"<\", SORT2 = public.\"<\", HASHES);\n")
                        wxT("CREATE OPERATOR public.<> (PROCEDURE = xxidne,")
                        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
                        wxT("    COMMUTATOR = public.\"<>\", NEGATOR = public.\"=\",")
                        wxT("    RESTRICT = neqsel, JOIN = neqjoinsel);\n")
                        wxT("CREATE OPERATOR public.> (PROCEDURE = xxidgt,")
                        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
                        wxT("    COMMUTATOR = public.\"<\", NEGATOR = public.\"<=\",")
                        wxT("    RESTRICT = scalargtsel, JOIN = scalargtjoinsel);\n")
                        wxT("CREATE OPERATOR public.<= (PROCEDURE = xxidle,")
                        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
                        wxT("    COMMUTATOR = public.\">=\", NEGATOR = public.\">\",")
                        wxT("    RESTRICT = scalarltsel, JOIN = scalarltjoinsel);\n")
                        wxT("CREATE OPERATOR public.>= (PROCEDURE = xxidge,")
                        wxT("    LEFTARG = xxid, RIGHTARG = xxid,")
                        wxT("    COMMUTATOR = public.\"<=\", NEGATOR = public.\"<\",")
                        wxT("    RESTRICT = scalargtsel, JOIN = scalargtjoinsel);\n")
                        wxT("------------- continue with backup script\n")
                    + clusterBackup.Mid(opclassPos);
            }
        }
        else
        {
            wxFile base(slony1BaseScript, wxFile::read);
            if (!base.IsOpened())
                return sql;

            wxFile func(slony1FunctionScript, wxFile::read);
            if (!func.IsOpened())
                return sql;

            wxFile xxid(slony1XxidScript, wxFile::read);
            if (!xxid.IsOpened())
                return sql;

            sql = wxT("CREATE SCHEMA ") + quotedName + wxT(";\n\n");

            char *buffer;
            size_t done;

            buffer = new char[xxid.Length()+1];
            done=xxid.Read(buffer, xxid.Length());
            buffer[done] = 0;
            sql += wxTextBuffer::Translate(wxString::FromAscii(buffer), wxTextFileType_Unix);
            delete[] buffer;

            buffer = new char[base.Length()+1];
            done=base.Read(buffer, base.Length());
            buffer[done] = 0;
            sql += wxTextBuffer::Translate(wxString::FromAscii(buffer), wxTextFileType_Unix);
            delete[] buffer;

            buffer = new char[func.Length()+1];
            done=func.Read(buffer, func.Length());
            buffer[done] = 0;
            sql += wxTextBuffer::Translate(wxString::FromAscii(buffer), wxTextFileType_Unix);
            delete[] buffer;

            sql.Replace(wxT("@NAMESPACE@"), quotedName);
            sql.Replace(wxT("@CLUSTERNAME@"), GetName());
        }

        sql += wxT("\n")
               wxT("SELECT ") + quotedName + wxT(".initializelocalnode(") +
               txtNodeID->GetValue() + wxT(", ") + qtString(txtNodeName->GetValue()) +
               wxT(");\n");

    }

    if (!txtComment->GetValue().IsEmpty())
        sql += wxT("\n")
               wxT("COMMENT ON SCHEMA ") + quotedName + wxT(" IS ") 
               + qtString(txtComment->GetValue()) + wxT(";\n");
    

    if (chkJoinCluster->GetValue())
        sql += wxT("\n\n-- In addition, the configuration is copied from the existing cluster.\n");

    return sql;
}
