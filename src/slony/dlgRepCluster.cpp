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


extern wxString slony1BaseScript;
extern wxString slony1FunctionScript;
extern wxString slony1XxidScript;
extern wxString backupExecutable;


#define cbServer            CTRL_COMBOBOX("cbServer")
#define cbDatabase          CTRL_COMBOBOX("cbDatabase")
#define cbClusterName       CTRL_COMBOBOX("cbClusterName")

BEGIN_EVENT_TABLE(dlgRepClusterBase, dlgProperty)
    EVT_COMBOBOX(XRCID("cbServer"),         dlgRepClusterBase::OnChangeServer)
    EVT_COMBOBOX(XRCID("cbDatabase"),       dlgRepClusterBase::OnChangeDatabase)
END_EVENT_TABLE();



dlgRepClusterBase::dlgRepClusterBase(frmMain *frame, const wxString &dlgName, slCluster *node, pgDatabase *db)
: dlgProperty(frame, dlgName)
{
    cluster=node;
    remoteServer=0;
    remoteConn=0;

    pgObject *obj=db;
    servers = obj->GetId();
    while (obj && obj != frame->GetServerCollection())
    {
        servers = frame->GetBrowser()->GetItemParent(servers);
        if (servers)            
            obj = (pgObject*)frame->GetBrowser()->GetItemData(servers);
    }
}


dlgRepClusterBase::~dlgRepClusterBase()
{
   if (remoteConn)
    {
        delete remoteConn;
        remoteConn=0;
    }
}


pgObject *dlgRepClusterBase::GetObject()
{
    return cluster;
}

int dlgRepClusterBase::Go(bool modal)
{
    if (cluster)
    {
        cbServer->Append(cluster->GetServer()->GetName());
        cbServer->SetSelection(0);
        cbDatabase->Append(cluster->GetDatabase()->GetName());
        cbDatabase->SetSelection(0);
        cbClusterName->Append(cluster->GetName());
        cbClusterName->SetSelection(0);
    }
    else
    {
        wxCookieType cookie;
        wxTreeItemId serverItem=mainForm->GetBrowser()->GetFirstChild(servers, cookie);        
        while (serverItem)
        {
            pgServer *s = (pgServer*)mainForm->GetBrowser()->GetItemData(serverItem);
            if (s && s->IsCreatedBy(serverFactory))
                cbServer->Append(mainForm->GetBrowser()->GetItemText(serverItem), (void*)s);

            serverItem = mainForm->GetBrowser()->GetNextChild(servers, cookie);
        }
        if (cbServer->GetCount())
            cbServer->SetSelection(0);
    }
    return dlgProperty::Go(modal);
}


void dlgRepClusterBase::OnChangeServer(wxCommandEvent &ev)
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
            remoteServer->Connect(mainForm, remoteServer->GetStorePwd());
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



void dlgRepClusterBase::OnChangeDatabase(wxCommandEvent &ev)
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

////////////////////////////////////////////////////////////////////////////////7

// pointer to controls
#define chkJoinCluster      CTRL_CHECKBOX("chkJoinCluster")
#define txtClusterName      CTRL_TEXT("txtClusterName")
#define txtNodeID           CTRL_TEXT("txtNodeID")
#define txtNodeName         CTRL_TEXT("txtNodeName")
#define txtAdminNodeID      CTRL_TEXT("txtAdminNodeID")
#define txtAdminNodeName    CTRL_TEXT("txtAdminNodeName")
#define cbAdminNode         CTRL_COMBOBOX("cbAdminNode")


BEGIN_EVENT_TABLE(dlgRepCluster, dlgRepClusterBase)
    EVT_BUTTON(wxID_OK,                     dlgRepCluster::OnOK)
    EVT_CHECKBOX(XRCID("chkJoinCluster"),   dlgRepCluster::OnChangeJoin)
    EVT_COMBOBOX(XRCID("cbClusterName"),    dlgRepCluster::OnChangeCluster)
    EVT_TEXT(XRCID("txtClusterName"),       dlgRepCluster::OnChange)
    EVT_TEXT(XRCID("txtNodeID"),            dlgRepCluster::OnChange)
    EVT_TEXT(XRCID("txtNodeName"),          dlgRepCluster::OnChange)
    EVT_COMBOBOX(XRCID("cbAdminNode"),      dlgRepCluster::OnChange)
    EVT_END_PROCESS(-1,                     dlgRepCluster::OnEndProcess)
END_EVENT_TABLE();


dlgProperty *pgaSlClusterFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgRepCluster(frame, (slCluster*)node, (pgDatabase*)parent);
}


dlgRepCluster::dlgRepCluster(frmMain *frame, slCluster *node, pgDatabase *db)
: dlgRepClusterBase(frame, wxT("dlgRepCluster"), node, db)
{
    process = 0;
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
    }

    wxCommandEvent ev;
    OnChangeJoin(ev);

    return dlgRepClusterBase::Go(modal);
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
    pgObject *obj=slClusterFactory.CreateObjects(collection, 0,
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

        if (chkJoinCluster->GetValue())
            CheckValid(enable, !cbClusterName->GetValue().IsEmpty(), _("Please select a cluster name."));
        else
            CheckValid(enable, !txtClusterName->GetValue().IsEmpty(), _("Please specify name."));

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
            if (!remoteServer->GetPasswordIsStored())
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



////////////////////////////////////////////////////////////////////////////////7


#define txtCurrentVersion   CTRL_TEXT("txtCurrentVersion")
#define txtVersion          CTRL_TEXT("txtVersion")

BEGIN_EVENT_TABLE(dlgRepClusterUpgrade, dlgRepClusterBase)
END_EVENT_TABLE();

// no factory needed; called by slFunction

dlgRepClusterUpgrade::dlgRepClusterUpgrade(frmMain *frame, slCluster *cl)
: dlgRepClusterBase(frame, wxT("dlgRepClusterUpgrade"), cl, cl->GetDatabase())
{
}


int dlgRepClusterUpgrade::Go(bool modal)
{
    txtCurrentVersion->SetValue(cluster->GetClusterVersion());
    txtCurrentVersion->Disable();
    txtVersion->Disable();
    return dlgRepClusterBase::Go(modal);
}


void dlgRepClusterUpgrade::CheckChange()
{
    bool enable=true;
    CheckValid(enable, cbDatabase->GetCount() > 0, _("Select server with Slony-I cluster installed."));
    CheckValid(enable, cbClusterName->GetCount() > 0, _("Select database with Slony-I cluster installed."));
    CheckValid(enable, cbClusterName->GetSelection() >= 0, _("Select Slony-I cluster."));
    CheckValid(enable, version > cluster->GetClusterVersion(), _("Selected cluster doesn't contain newer software."));
    EnableOK(enable);
}


wxString dlgRepClusterUpgrade::GetSql()
{
    if (sql.IsEmpty() && !version.IsEmpty() && remoteConn)
    {
        sql = wxT("SET SEARCH_PATH = ") + qtIdent(wxT("_") + cluster->GetName()) + wxT(", pg_catalog;\n\n");

        pgSet *set=remoteConn->ExecuteSet(
            wxT("SELECT proname, proisagg, prosecdef, proisstrict, proretset, provolatile, pronargs, prosrc, probin,\n")
            wxT("       lanname, tr.typname as rettype,\n")
            wxT("       t0.typname AS arg0, t1.typname AS arg1, t2.typname AS arg2, t3.typname AS arg3, t4.typname AS arg4,\n")
            wxT("       t5.typname AS arg5, t6.typname AS arg6, t7.typname AS arg7, t8.typname AS arg8, t9.typname AS arg9, \n")
            wxT("       proargnames[0] AS an0, proargnames[1] AS an1, proargnames[2] AS an2, proargnames[3] AS an3, proargnames[4] AS an4,\n")
            wxT("       proargnames[5] AS an5, proargnames[6] AS an6, proargnames[7] AS an7, proargnames[8] AS an8, proargnames[9] AS an9\n")
            wxT("  FROM pg_proc\n")
            wxT("  JOIN pg_namespace nsp ON nsp.oid=pronamespace\n")
            wxT("  JOIN pg_language l ON l.oid=prolang\n")
            wxT("  JOIN pg_type tr ON tr.oid=prorettype\n")
            wxT("  LEFT JOIN pg_type t0 ON t0.oid=proargtypes[0]\n")
            wxT("  LEFT JOIN pg_type t1 ON t1.oid=proargtypes[1]\n")
            wxT("  LEFT JOIN pg_type t2 ON t2.oid=proargtypes[2]\n")
            wxT("  LEFT JOIN pg_type t3 ON t3.oid=proargtypes[3]\n")
            wxT("  LEFT JOIN pg_type t4 ON t4.oid=proargtypes[4]\n")
            wxT("  LEFT JOIN pg_type t5 ON t5.oid=proargtypes[5]\n")
            wxT("  LEFT JOIN pg_type t6 ON t6.oid=proargtypes[6]\n")
            wxT("  LEFT JOIN pg_type t7 ON t7.oid=proargtypes[7]\n")
            wxT("  LEFT JOIN pg_type t8 ON t8.oid=proargtypes[8]\n")
            wxT("  LEFT JOIN pg_type t9 ON t9.oid=proargtypes[9]\n")
            wxT(" WHERE nspname = ") + qtString(wxT("_") + cluster->GetName())
            );

        if (set)
        {
            while (!set->Eof())
            {
                sql += wxT("CREATE OR REPLACE FUNCTION " + qtIdent(set->GetVal(wxT("proname"))) + wxT("(");

                wxString language = set->GetVal(wxT("lanname"));
                wxString volat = set->GetVal(wxT("provolatile"));
                long numArgs=set->GetLong(wxT("pronargs"));

                long i;

                for (i=0 ; i < numArgs ; i++)
                {
                    if (i)
                        sql += wxT(", ");
                    wxString argname=set->GetVal(wxT("an") + NumToStr(i));
                    if (!argname.IsEmpty())
                        sql += qtIdent(argname) + wxT(" ");

                    sql += qtIdent(set->GetVal(wxT("arg") + NumToStr(i)));
                }
                sql += wxT(")\n")
                       wxT("  RETURNS ");
                if (set->GetBool(wxT("proretset")))
                    sql += wxT("SETOF "));
                sql += qtIdent(set->GetVal(wxT("rettype")));

                if (language == wxT("c"))
                    sql += wxT("\n")
                           wxT("AS '" + set->GetVal(wxT("probin")) + wxT("', '") + 
                                set->GetVal(wxT("prosrc")) + wxT("'");
                else
                    sql += wxT(" AS\n")
                           wxT("$BODY$") + set->GetVal(wxT("prosrc")) + wxT("$BODY$");

                sql += wxT(" LANGUAGE ") 
                    +  language;

                if (volat == wxT("v"))
                    sql += wxT(" VOLATILE");
                else if (volat == wxT("i"))
                    sql += wxT(" IMMUTABLE"));
                else
                    sql += wxT(" STABLE");

                if (set->GetBool(wxT("proisstrict")))
                    sql += wxT(" STRICT");

                if (set->GetBool(wxT("prosecdef")))
                    sql += wxT(" SECURITY DEFINER");

                sql += wxT(";\n\n");

                set->MoveNext();
            }
            delete set;
        }
    }
    return sql;
}


pgObject *dlgRepClusterUpgrade::CreateObject(pgCollection *collection)
{
    return 0;
}


void dlgRepClusterUpgrade::OnChangeCluster(wxCommandEvent &ev)
{
    version = wxEmptyString;
    sql = wxEmptyString;

    int sel=cbClusterName->GetSelection();
    if (remoteConn && sel >= 0)
    {
        wxString schemaPrefix = qtIdent(wxT("_") + cbClusterName->GetValue()) + wxT(".");

        version = remoteConn->ExecuteScalar(wxT("SELECT ") + schemaPrefix + wxT("slonyversion();"));
    }
    OnChange(ev);


    txtVersion->SetValue(version);
    OnChange(ev);
}


//////////////////////////////////////////////7

bool clusterActionFactory::CheckEnable(pgObject *obj)
{
    return obj && obj->IsCreatedBy(slClusterFactory);
}


slonyRestartFactory::slonyRestartFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : clusterActionFactory(list)
{
    mnu->Append(id, _("Restart node"), _("Restart node."));
}


wxWindow *slonyRestartFactory::StartDialog(frmMain *form, pgObject *obj)
{
    slCluster *cluster=(slCluster*)obj;

    wxString notifyName=cluster->GetDatabase()->ExecuteScalar(
        wxT("SELECT relname FROM pg_listener")
        wxT(" WHERE relname=") + qtString(wxT("_") + cluster->GetName() + wxT("_Restart")));

    if (notifyName.IsEmpty())
    {
        wxMessageDialog dlg(form, wxString::Format(_("Node \"%s\" not running"), cluster->GetLocalNodeName().c_str()),
              _("Can't restart node"), wxICON_EXCLAMATION|wxOK);
        dlg.ShowModal();
        form->CheckAlive();

        return 0;
    }

    wxMessageDialog dlg(form, wxString::Format(_("Restart node \"%s\"?"), 
        cluster->GetLocalNodeName().c_str()), _("Restart node"), wxICON_EXCLAMATION|wxYES_NO|wxNO_DEFAULT);

    if (dlg.ShowModal() != wxID_YES)
        return 0;

    if (!cluster->GetDatabase()->ExecuteVoid(
        wxT("NOTIFY ") + qtIdent(notifyName)))
        form->CheckAlive();

    return 0;
}


slonyUpgradeFactory::slonyUpgradeFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : clusterActionFactory(list)
{
    mnu->Append(id, _("Upgrade node"), _("Upgrade node to newest function version."));
}


wxWindow *slonyUpgradeFactory::StartDialog(frmMain *form, pgObject *obj)
{
    dlgProperty *dlg=new dlgRepClusterUpgrade(form, (slCluster*)obj);
    dlg->InitDialog(form, obj);
    dlg->CreateAdditionalPages();
    dlg->Go(false);
    dlg->CheckChange();
    return dlg;
}


slonyFailoverFactory::slonyFailoverFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : clusterActionFactory(list)
{
    mnu->Append(id, _("Failover"), _("Failover to backup node."));
}


wxWindow *slonyFailoverFactory::StartDialog(frmMain *form, pgObject *obj)
{
    return 0;
}
