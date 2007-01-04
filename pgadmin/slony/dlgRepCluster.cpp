//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepCluster.cpp - PostgreSQL Slony-I Cluster Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/pgDefs.h"
#include <wx/textbuf.h>
#include <wx/file.h>

#include "frm/frmMain.h"
#include "slony/dlgRepCluster.h"
#include "slony/slCluster.h"
#include "slony/slSet.h"
#include "slony/slCluster.h"
#include "schema/pgDatatype.h"
#include "utils/sysProcess.h"


extern wxString backupExecutable;

#define cbServer            CTRL_COMBOBOX("cbServer")
#define cbDatabase          CTRL_COMBOBOX("cbDatabase")
#define cbClusterName       CTRL_COMBOBOX("cbClusterName")

BEGIN_EVENT_TABLE(dlgRepClusterBase, dlgProperty)
    EVT_COMBOBOX(XRCID("cbServer"),         dlgRepClusterBase::OnChangeServer)
    EVT_COMBOBOX(XRCID("cbDatabase"),       dlgRepClusterBase::OnChangeDatabase)
END_EVENT_TABLE();



dlgRepClusterBase::dlgRepClusterBase(pgaFactory *f, frmMain *frame, const wxString &dlgName, slCluster *node, pgDatabase *db)
: dlgProperty(f, frame, dlgName)
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
            obj = frame->GetBrowser()->GetObject(servers);
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


bool dlgRepClusterBase::AddScript(wxString &sql, const wxString &fn)
{
    wxFileName filename;
    filename.Assign(settings->GetSlonyPath(), fn);

    if (!wxFile::Exists(filename.GetFullPath()))
        return false;

    wxFile file(filename.GetFullPath(), wxFile::read);
    if (!file.IsOpened())
        return false;

    char *buffer;
    size_t done;

    buffer = new char[file.Length()+1];
    done=file.Read(buffer, file.Length());
    buffer[done] = 0;
    sql += wxTextBuffer::Translate(wxString::FromAscii(buffer), wxTextFileType_Unix);
    delete[] buffer;

    return done > 0;
}


int dlgRepClusterBase::Go(bool modal)
{
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
    int sel=cbServer->GetCurrentSelection();
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

    int sel=cbDatabase->GetCurrentSelection();
    if (remoteServer && sel >= 0)
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
    return new dlgRepCluster(this, frame, (slCluster*)node, (pgDatabase*)parent);
}


dlgRepCluster::dlgRepCluster(pgaFactory *f, frmMain *frame, slCluster *node, pgDatabase *db)
: dlgRepClusterBase(f, frame, wxT("dlgRepCluster"), node, db)
{
    process = 0;
}



wxString dlgRepCluster::GetHelpPage() const
{
    wxString page=wxT("slony-install");
    if (chkJoinCluster->GetValue())
        page += wxT("#join");

    return page;
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
            wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%host=") + cluster->GetServer()->GetName() + wxT("%")) +
            wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%dbname=") + cluster->GetDatabase()->GetName() + wxT("%"));

        if (cluster->GetServer()->GetPort() != 5432)
            sql += wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%port=") + NumToStr((long)cluster->GetServer()->GetPort()) + wxT("%"));

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

        cbServer->Append(cluster->GetServer()->GetName());
        cbServer->SetSelection(0);
        cbDatabase->Append(cluster->GetDatabase()->GetName());
        cbDatabase->SetSelection(0);
        cbClusterName->Append(cluster->GetName());
        cbClusterName->SetSelection(0);
    }
    else
    {
        // create mode
        cbAdminNode->Hide();

        txtNodeID->SetValidator(numericValidator);
        txtAdminNodeID->SetValidator(numericValidator);
        txtClusterName->Hide();

        if (!AddScript(createScript, wxT("xxid.v74.sql")) || 
            !AddScript(createScript, wxT("slony1_base.sql")) || 
            !AddScript(createScript, wxT("slony1_funcs.sql")) || 
            !AddScript(createScript, wxT("slony1_funcs.v74.sql")))
            createScript = wxEmptyString;

        treeObjectIterator it(mainForm->GetBrowser(), mainForm->GetServerCollection());
        pgServer *s;
        int sel=-1;
        while ((s=(pgServer*)it.GetNextObject()) != 0)
        {
            if (s->IsCreatedBy(serverFactory))
            {
                if (s == database->GetServer())
                    sel = cbServer->GetCount();
                cbServer->Append(mainForm->GetBrowser()->GetItemText(s->GetId()), (void*)s);
            }
        }
        if (sel >= 0)
            cbServer->SetSelection(sel);
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

    if (joinCluster && !cbDatabase->GetCount())
    {
        OnChangeServer(ev);
        return;
    }

    OnChange(ev);
}


void dlgRepCluster::OnChangeCluster(wxCommandEvent &ev)
{
    clusterBackup = wxEmptyString;
    remoteVersion = wxEmptyString;

    cbAdminNode->Clear();
    cbAdminNode->Append(_("<none>"), (void*)-1);

    int sel=cbClusterName->GetCurrentSelection();
    if (remoteConn && sel >= 0)
    {
        wxString schemaPrefix = qtIdent(wxT("_") + cbClusterName->GetValue()) + wxT(".");
        long adminNodeID = settings->Read(wxT("Replication/") + cbClusterName->GetValue() + wxT("/AdminNode"), -1L);

        remoteVersion = remoteConn->ExecuteScalar(wxT("SELECT ") + schemaPrefix + wxT("slonyVersion();"));

        wxString sql=
            wxT("SELECT no_id, no_comment\n")
            wxT("  FROM ") + schemaPrefix + wxT("sl_node\n")
            wxT("  JOIN ") + schemaPrefix + wxT("sl_path ON no_id = pa_client\n")
            wxT(" WHERE pa_server = (SELECT last_value FROM ") + schemaPrefix + wxT("sl_local_node_id)\n")
            wxT("   AND pa_conninfo ILIKE ") + qtDbString(wxT("%host=") + remoteServer->GetName() + wxT("%")) + wxT("\n")
            wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%dbname=") + cbDatabase->GetValue() + wxT("%")) + wxT("\n");

        if (remoteServer->GetPort() != 5432)
            sql += wxT("   AND pa_conninfo LIKE ") + qtDbString(wxT("%port=") + NumToStr((long)remoteServer->GetPort()) + wxT("%"));

        pgSet *set=remoteConn->ExecuteSet(sql);
        if (set)
        {
            if (!set->Eof())
            {
                long id = set->GetLong(wxT("no_id"));
                cbAdminNode->Append(IdAndName(id, set->GetVal(wxT("no_comment"))), (void*)id);
                if (adminNodeID == id)
                    cbAdminNode->SetSelection(cbAdminNode->GetCount()-1);
            }
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
                        vals += qtDbString(set->GetVal(i));
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
                                qtDbString(wxT("_") + cbClusterName->GetValue() + wxT(".sl_event_seq")) + 
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
                    qtDbString(wxT("_") + cbClusterName->GetValue() + wxT(".sl_rowid_seq")) + 
                    wxT(", ") + seqno + wxT(")"));
            }
        }

        // create new node on the existing cluster
        if (done)
        {
            wxString sql=
                wxT("SELECT ") + schemaPrefix + wxT("storenode(") 
                    + txtNodeID->GetValue() + wxT(", ")
                    + qtDbString(txtNodeName->GetValue());

            if (StrToDouble(remoteVersion) >= 1.1)
                sql += wxT(", false");

            sql += wxT(");\n")
            wxT("SELECT ") + schemaPrefix + wxT("enablenode(") 
                + txtNodeID->GetValue() + wxT(");\n");

            done = remoteConn->ExecuteVoid(sql);
        }

        // add admin info to cluster

        if (done && cbAdminNode->GetCurrentSelection() > 0)
        {
            done = remoteConn->ExecuteVoid(
                wxT("SELECT ") + schemaPrefix + wxT("storepath(") +
                txtNodeID->GetValue() + wxT(", ") +
                    NumToStr((long)cbAdminNode->GetClientData(cbAdminNode->GetCurrentSelection())) + wxT(", ") +
                    qtDbString(wxT("host=") + database->GetServer()->GetName() + 
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
         wxT(" WHERE nspname = ") + qtDbString(wxT("_") + GetName()));

    return obj;
}


void dlgRepCluster::CheckChange()
{
    if (cluster)
    {
        int sel=cbAdminNode->GetCurrentSelection();
        bool changed = (sel >= 0 && (long)cbAdminNode->GetClientData() != cluster->GetAdminNodeID());

        EnableOK(changed || txtComment->GetValue() != cluster->GetComment());
    }
    else
    {
        size_t i;
        bool enable=true;

        CheckValid(enable, chkJoinCluster->GetValue() || (!createScript.IsEmpty()),
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


// this is necessary because wxString::Replace is ridiculously slow on large strings.

void AppendBuf(wxChar* &buf, int &buflen, int &len, const wxChar *str, int slen=-1)
{
    if (slen < 0)
        slen = wxStrlen(str);
    if (!slen)
        return;
    if (buflen < len+slen)
    {
        buflen = (len+slen) *6 /5;
        wxChar *tmp=new wxChar[buflen+1];
        memcpy(tmp, buf, len*sizeof(wxChar));
        delete[] buf;
        buf=tmp;
    }
    memcpy(buf+len, str, slen*sizeof(wxChar));
    len += slen;
}


wxString ReplaceString(const wxString &str, const wxString &oldStr, const wxString &newStr)
{
    int buflen=str.Length() + 100;
    int len=0;

    wxChar *buf=new wxChar[buflen+1];

    const wxChar *ptrIn=str.c_str();
    const wxChar *ptrFound = wxStrstr(ptrIn, oldStr.c_str());
    
    while (ptrFound)
    {
        AppendBuf(buf, buflen, len, ptrIn, ptrFound-ptrIn);
        AppendBuf(buf, buflen, len, newStr.c_str());
        ptrIn = ptrFound + oldStr.Length();
        ptrFound = wxStrstr(ptrIn, oldStr.c_str());
    }

    AppendBuf(buf, buflen, len, ptrIn);
    buf[len]=0;
    wxString tmpstr(buf);
    delete[] buf;

    return tmpstr;
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
        int sel=cbAdminNode->GetCurrentSelection();
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

        if (remoteServer && clusterBackup.IsEmpty() && !backupExecutable.IsEmpty())
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
                if (process)
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
            sql = wxT("CREATE SCHEMA ") + quotedName + wxT(";\n\n")
                + ReplaceString(createScript, wxT("@NAMESPACE@"), quotedName);

            sql = ReplaceString(sql, wxT("@CLUSTERNAME@"), txtClusterName->GetValue());

            // From Slony 1.2 onwards, the scripts include the module version.
            // To figure it out, temporarily load and use _Slony_I_getModuleVersion.
            // We'll cache the result to save doing it again.
            if (sql.Contains(wxT("@MODULEVERSION@")) && slonyVersion.IsEmpty())
            {
                this->database->ExecuteVoid(wxT("CREATE OR REPLACE FUNCTION pgadmin_slony_version() returns text as '$libdir/slony1_funcs', '_Slony_I_getModuleVersion' LANGUAGE C"));
                slonyVersion = this->database->ExecuteScalar(wxT("SELECT pgadmin_slony_version();"));
                this->database->ExecuteVoid(wxT("DROP FUNCTION pgadmin_slony_version()"));

                if (slonyVersion.IsEmpty())
                {
                    wxLogError(_("Couldn't test for the Slony version. Assuming 1.2.0"));
                    slonyVersion = wxT("1.2.0");
                }
            }
            sql = ReplaceString(sql, wxT("@MODULEVERSION@"), slonyVersion);
        }

        sql += wxT("\n")
               wxT("SELECT ") + quotedName + wxT(".initializelocalnode(") +
               txtNodeID->GetValue() + wxT(", ") + qtDbString(txtNodeName->GetValue()) +
               wxT(");\n")
               wxT("SELECT ") + quotedName;

        if (chkJoinCluster->GetValue())
            sql += wxT(".enablenode_int(");
        else
            sql += wxT(".enablenode(");

        sql += txtNodeID->GetValue() +
               wxT(");\n");
    }

    if (!txtComment->GetValue().IsEmpty())
        sql += wxT("\n")
               wxT("COMMENT ON SCHEMA ") + quotedName + wxT(" IS ") 
               + qtDbString(txtComment->GetValue()) + wxT(";\n");
    

    if (chkJoinCluster->GetValue())
        sql += wxT("\n\n-- In addition, the configuration is copied from the existing cluster.\n");
    else
    {
        wxString schemaPrefix = qtIdent(wxT("_") + txtClusterName->GetValue()) + wxT(".");
        long adminNode = StrToLong(txtAdminNodeID->GetValue());
        if (adminNode > 0 && adminNode != StrToLong(txtNodeID->GetValue()))
        {
            sql +=
                wxT("\n-- Create admin node\n")
                wxT("SELECT ") + schemaPrefix + wxT("storeNode(") +
                    NumToStr(adminNode) + wxT(", ") +
                    qtDbString(txtAdminNodeName->GetValue());

            if (chkJoinCluster->GetValue())
            {
                if (StrToDouble(remoteVersion) >= 1.1)
                    sql += wxT(", false");
            }
            else
            {
                if (createScript.Find(wxT("storeNode (int4, text)")) < 0)
                    sql += wxT(", false");
            }
            
            sql += wxT(");\n")
                wxT("SELECT ") + schemaPrefix + wxT("storepath(") +
                    txtNodeID->GetValue() + wxT(", ") +
                    NumToStr(adminNode) + wxT(", ") +
                    qtDbString(wxT("host=") + database->GetServer()->GetName() + 
                            wxT(" port=") + NumToStr((long)database->GetServer()->GetPort()) +
                            wxT(" dbname=") + database->GetName()) + wxT(", ")
                    wxT("0);\n");
        }
    }
    return sql;
}



////////////////////////////////////////////////////////////////////////////////7


#define txtCurrentVersion   CTRL_TEXT("txtCurrentVersion")
#define txtVersion          CTRL_TEXT("txtVersion")

BEGIN_EVENT_TABLE(dlgRepClusterUpgrade, dlgRepClusterBase)
    EVT_COMBOBOX(XRCID("cbClusterName"),    dlgRepClusterUpgrade::OnChangeCluster)
END_EVENT_TABLE();

// no factory needed; called by slFunction

dlgRepClusterUpgrade::dlgRepClusterUpgrade(pgaFactory *f, frmMain *frame, slCluster *cl)
: dlgRepClusterBase(f, frame, wxT("dlgRepClusterUpgrade"), cl, cl->GetDatabase())
{
}


int dlgRepClusterUpgrade::Go(bool modal)
{
    txtCurrentVersion->SetValue(cluster->GetClusterVersion());
    txtCurrentVersion->Disable();
    txtVersion->Disable();

    treeObjectIterator it(mainForm->GetBrowser(), mainForm->GetServerCollection());
    pgServer *s;
    while ((s=(pgServer*)it.GetNextObject()) != 0)
    {
        if (s->IsCreatedBy(serverFactory))
            cbServer->Append(mainForm->GetBrowser()->GetItemText(s->GetId()), (void*)s);
    }
    if (cbServer->GetCount())
        cbServer->SetSelection(0);

    wxCommandEvent ev;
    OnChangeServer(ev);

    return dlgRepClusterBase::Go(modal);
}


void dlgRepClusterUpgrade::CheckChange()
{
    bool enable=true;
    CheckValid(enable, cluster->GetSlonPid() == 0, _("Slon process running on node; stop it before upgrading."));
    CheckValid(enable, cbDatabase->GetCount() > 0, _("Select server with Slony-I cluster installed."));
    CheckValid(enable, cbClusterName->GetCount() > 0, _("Select database with Slony-I cluster installed."));
    CheckValid(enable, cbClusterName->GetCurrentSelection() >= 0, _("Select Slony-I cluster."));
    CheckValid(enable, version > cluster->GetClusterVersion(), _("Selected cluster doesn't contain newer software."));
    EnableOK(enable);
}


wxString dlgRepClusterUpgrade::GetSql()
{
    if (sql.IsEmpty() && !version.IsEmpty() && remoteConn)
    {
        wxString remoteCluster = wxT("_") + cbClusterName->GetValue();
        sql = wxT("SET SEARCH_PATH = ") + qtIdent(wxT("_") + cluster->GetName()) + wxT(", pg_catalog;\n\n");

        bool upgradeSchemaAvailable=false;

        {
            // update functions
            pgSetIterator func(remoteConn,
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
                wxT(" WHERE nspname = ") + qtDbString(remoteCluster)
                );

            while (func.RowsLeft())
            {
                wxString proname=func.GetVal(wxT("proname"));
                if (proname == wxT("upgradeschema"))
                    upgradeSchemaAvailable=true;

                sql += wxT("CREATE OR REPLACE FUNCTION " + qtIdent(proname) + wxT("(");

                wxString language = func.GetVal(wxT("lanname"));
                wxString volat = func.GetVal(wxT("provolatile"));
                long numArgs=func.GetLong(wxT("pronargs"));

                long i;

                for (i=0 ; i < numArgs ; i++)
                {
                    if (i)
                        sql += wxT(", ");
                    wxString argname=func.GetVal(wxT("an") + NumToStr(i));
                    if (!argname.IsEmpty())
                        sql += qtIdent(argname) + wxT(" ");

                    sql += qtIdent(func.GetVal(wxT("arg") + NumToStr(i)));
                }
                sql += wxT(")\n")
                       wxT("  RETURNS ");
                if (func.GetBool(wxT("proretset")))
                    sql += wxT("SETOF "));
                sql += qtIdent(func.GetVal(wxT("rettype")));

                if (language == wxT("c"))
                    sql += wxT("\n")
                           wxT("AS '" + func.GetVal(wxT("probin")) + wxT("', '") + 
                                func.GetVal(wxT("prosrc")) + wxT("'");
                else
                    sql += wxT(" AS\n")
                           wxT("$BODY$") + func.GetVal(wxT("prosrc")) + wxT("$BODY$");

                sql += wxT(" LANGUAGE ") 
                    +  language;

                if (volat == wxT("v"))
                    sql += wxT(" VOLATILE");
                else if (volat == wxT("i"))
                    sql += wxT(" IMMUTABLE"));
                else
                    sql += wxT(" STABLE");

                if (func.GetBool(wxT("proisstrict")))
                    sql += wxT(" STRICT");

                if (func.GetBool(wxT("prosecdef")))
                    sql += wxT(" SECURITY DEFINER");

                sql += wxT(";\n\n");
            }
        }

        if (upgradeSchemaAvailable)
            sql += wxT("SELECT upgradeSchema(") + qtDbString(cluster->GetClusterVersion()) + wxT(");\n\n");

        {
            // Create missing tables and columns
            // we don't expect column names and types to change

            pgSetIterator srcCols(remoteConn,
                wxT("SELECT relname, attname, attndims, atttypmod, attnotnull, adsrc, ty.typname, tn.nspname as typnspname,\n")
                wxT("  (SELECT count(1) FROM pg_type t2 WHERE t2.typname=ty.typname) > 1 AS isdup\n")
                wxT("  FROM pg_attribute\n")
                wxT("  JOIN pg_class c ON c.oid=attrelid\n")
                wxT("  JOIN pg_namespace n ON n.oid=relnamespace")
                wxT("  LEFT JOIN pg_attrdef d ON adrelid=attrelid and adnum=attnum\n")
                wxT("  JOIN pg_type ty ON ty.oid=atttypid\n")
                wxT("  JOIN pg_namespace tn ON tn.oid=ty.typnamespace\n")
                wxT(" WHERE n.nspname = ") + qtDbString(remoteCluster) +
                wxT("   AND attnum>0 and relkind='r'\n")
                wxT(" ORDER BY (relname != 'sl_confirm'), relname, attname")
                );

            pgSetIterator destCols(connection,
                wxT("SELECT relname, attname, adsrc\n")
                wxT("  FROM pg_attribute\n")
                wxT("  JOIN pg_class c ON c.oid=attrelid\n")
                wxT("  JOIN pg_namespace n ON n.oid=relnamespace")
                wxT("  LEFT JOIN pg_attrdef d ON adrelid=attrelid and adnum=attnum\n")
                wxT(" WHERE n.nspname = ") + qtDbString(wxT("_") + cluster->GetName()) +
                wxT("   AND attnum>0 and relkind='r'\n")
                wxT(" ORDER BY (relname != 'sl_confirm'), relname, attname")
                );

            if (!destCols.RowsLeft())
                return wxT("error");

            wxString lastTable;
            while (srcCols.RowsLeft())
            {
                wxString table=srcCols.GetVal(wxT("relname"));
                wxString column=srcCols.GetVal(wxT("attname"));
                wxString defVal=srcCols.GetVal(wxT("adsrc"));

                if (table == wxT("sl_node"))
                {
                    table=wxT("sl_node");
                }
                pgDatatype dt(srcCols.GetVal(wxT("typnspname")), srcCols.GetVal(wxT("typname")), 
                    srcCols.GetBool(wxT("isdup")), 
                    srcCols.GetLong(wxT("attndims")), srcCols.GetLong(wxT("atttypmod")));


                if (destCols.Set()->Eof() || 
                    destCols.GetVal(wxT("relname")) != table || 
                    destCols.GetVal(wxT("attname")) != column)
                {
                    if (table == lastTable || table == destCols.GetVal(wxT("relname")))
                    {
                        // just an additional column
                        sql += wxT("ALTER TABLE ") + qtIdent(table)
                            +  wxT(" ADD COLUMN ") + qtIdent(column)
                            + wxT(" ") + dt.GetQuotedSchemaPrefix(0) + dt.QuotedFullName();
                    
                        if (!defVal.IsEmpty())
                            sql += wxT(" DEFAULT ") + defVal;
                        if (srcCols.GetBool(wxT("attnotnull")))
                            sql += wxT(" NOT NULL");

                        sql += wxT(";\n");
                    }
                    else
                    {
                        // new table
                        // sl_confirm will always exist and be the first so no need for special 
                        // precautions in case a new table is the very first in the set

                        sql += wxT("CREATE TABLE ") + qtIdent(table)
                            +  wxT(" (") + qtIdent(column)
                            + wxT(" ") + dt.GetQuotedSchemaPrefix(0) + dt.QuotedFullName();
                    
                        if (!defVal.IsEmpty())
                            sql += wxT(" DEFAULT ") + defVal;

                        sql += wxT(");\n");
                    }
                }
                else
                {
                    // column is found
                    if (destCols.GetVal(wxT("adsrc")) != defVal)
                    {
                        sql += wxT("ALTER TABLE ") + qtIdent(table)
                            +  wxT(" ALTER COLUMN ") + qtIdent(column);
                        if (defVal.IsEmpty())
                            sql += wxT(" DROP DEFAULT;\n");
                        else
                            sql += wxT(" SET DEFAULT ") + defVal + wxT(";\n");
                    }
                    destCols.RowsLeft();
                }
                lastTable = table;
            }
        }

        {
            // check missing indexes
            pgSetIterator srcIndexes(remoteConn,
                wxT("SELECT t.relname, indkey, ti.relname as indname, pg_get_indexdef(indexrelid) AS inddef\n")
                wxT("  FROM pg_index i\n")
                wxT("  JOIN pg_class ti ON indexrelid=ti.oid\n")
                wxT("  JOIN pg_class t ON indrelid=t.oid\n")
                wxT("  JOIN pg_namespace n ON n.oid=t.relnamespace\n")
                wxT(" WHERE nspname = ") + qtDbString(remoteCluster) +
                wxT(" ORDER BY t.relname, ti.relname, indkey"));

            pgSetIterator destIndexes(remoteConn,
                wxT("SELECT t.relname, indkey, ti.relname as indnamen")
                wxT("  FROM pg_index i\n")
                wxT("  JOIN pg_class ti ON indexrelid=ti.oid\n")
                wxT("  JOIN pg_class t ON indrelid=t.oid\n")
                wxT("  JOIN pg_namespace n ON n.oid=t.relnamespace\n")
                wxT(" WHERE nspname = ") + qtDbString(wxT("_") + cluster->GetName()) +
                wxT(" ORDER BY t.relname, ti.relname, indkey"));

            if (!destIndexes.RowsLeft())
                return wxT("error");

            while (srcIndexes.RowsLeft())
            {
                wxString table=srcIndexes.GetVal(wxT("relname"));

                bool needUpdate = destIndexes.Set()->Eof() ||
                                  destIndexes.GetVal(wxT("relname")) != table;
                                    
                if (!needUpdate && destIndexes.GetVal(wxT("indkey")) != srcIndexes.GetVal(wxT("indkey")))
                {
                    // better ignore index name and check column names here 
                    needUpdate = destIndexes.GetVal(wxT("indname")) != srcIndexes.GetVal(wxT("indname"));
                }
                if (needUpdate)
                {
                    wxString inddef=srcIndexes.GetVal(wxT("inddef"));
                    inddef.Replace(qtIdent(remoteCluster)+wxT("."), qtIdent(wxT("_") +cluster->GetName())+wxT("."));
                    sql += inddef + wxT(";\n");
                }
                else
                    destIndexes.RowsLeft();
            }
        }

        {
            // check missing constraints
            // we don't expect constraint definitions to change

            pgSetIterator srcConstraints(remoteConn,
                wxT("SELECT t.relname, contype, conkey, conname,\n")
                wxT("       pg_get_constraintdef(c.oid) AS condef\n")
                wxT("  FROM pg_constraint c\n")
                wxT("  JOIN pg_class t ON c.conrelid=t.oid\n")
                wxT("  JOIN pg_namespace n ON n.oid=relnamespace\n")
                wxT(" WHERE nspname = ") + qtDbString(remoteCluster) + wxT("\n")
                wxT(" ORDER BY (contype != 'p'), relname, contype, conname, conkey")
                );

            pgSetIterator destConstraints(connection,
                wxT("SELECT t.relname, contype, conkey, conname\n")
                wxT("  FROM pg_constraint c\n")
                wxT("  JOIN pg_class t ON c.conrelid=t.oid\n")
                wxT("  JOIN pg_namespace n ON n.oid=relnamespace\n")
                wxT(" WHERE nspname = ") + qtDbString(wxT("_") + cluster->GetName()) + wxT("\n")
                wxT(" ORDER BY (contype != 'p'), relname, contype, conname, conkey")
                );

            if (!destConstraints.RowsLeft())
                return wxT("error");

            while (srcConstraints.RowsLeft())
            {
                wxString table=srcConstraints.GetVal(wxT("relname"));
                wxString contype=srcConstraints.GetVal(wxT("contype"));

                bool needUpdate = destConstraints.Set()->Eof() || 
                                  destConstraints.GetVal(wxT("relname")) != table ||
                                  destConstraints.GetVal(wxT("contype")) != contype;
                if (!needUpdate && destConstraints.GetVal(wxT("conkey"))  != srcConstraints.GetVal(wxT("conkey")))
                {
                    // better ignore constraint name and compare column names here
                    needUpdate = destConstraints.GetVal(wxT("conname")) != srcConstraints.GetVal(wxT("conname"));
                }
                if (needUpdate)
                {
                    wxString condef=srcConstraints.GetVal(wxT("condef"));
                    condef.Replace(qtIdent(remoteCluster)+wxT("."), qtIdent(wxT("_") +cluster->GetName())+wxT("."));

                    sql += wxT("ALTER TABLE ") + qtIdent(table)
                        +  wxT(" ADD CONSTRAINT ") + qtIdent(srcConstraints.GetVal(wxT("conname")))
                        + wxT(" ") + condef
                        + wxT(";\n");
                }
                else
                    destConstraints.RowsLeft();
            }

            sql += wxT("\nNOTIFY ") + qtIdent(wxT("_") + cluster->GetName() + wxT("_Restart")) 
                +  wxT(";\n\n");
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

    int sel=cbClusterName->GetCurrentSelection();
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
    mnu->Append(id, _("Restart Node"), _("Restart node."));
}


wxWindow *slonyRestartFactory::StartDialog(frmMain *form, pgObject *obj)
{
    slCluster *cluster=(slCluster*)obj;

    wxString notifyName=cluster->GetDatabase()->ExecuteScalar(
        wxT("SELECT relname FROM pg_listener")
        wxT(" WHERE relname=") + cluster->GetDatabase()->GetConnection()->qtDbString(wxT("_") + cluster->GetName() + wxT("_Restart")));

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
    mnu->Append(id, _("Upgrade Node"), _("Upgrade node to newest replication software version."));
}


wxWindow *slonyUpgradeFactory::StartDialog(frmMain *form, pgObject *obj)
{
    dlgProperty *dlg=new dlgRepClusterUpgrade(&slClusterFactory, form, (slCluster*)obj);
    dlg->InitDialog(form, obj);
    dlg->CreateAdditionalPages();
    dlg->Go(false);
    dlg->CheckChange();
    return 0;
}


slonyFailoverFactory::slonyFailoverFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : clusterActionFactory(list)
{
    mnu->Append(id, _("Failover"), _("Failover to backup node."));
}


wxWindow *slonyFailoverFactory::StartDialog(frmMain *form, pgObject *obj)
{
    return 0;
}


bool slonyFailoverFactory::CheckEnable(pgObject *obj)
{
    return false;
}
