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
#include "misc.h"
#include "pgDefs.h"
#include <wx/textbuf.h>
#include <wx/file.h>

#include "frmMain.h"
#include "dlgRepCluster.h"
#include "slCluster.h"
#include "slSet.h"
#include "slCluster.h"
#include "pgDatatype.h"


// Images
#include "images/slcluster.xpm"


// pointer to controls
#define chkJoinCluster  CTRL_CHECKBOX("chkJoinCluster")
#define cbClusterName   CTRL_COMBOBOX("cbClusterName")
#define txtClusterName  CTRL_TEXT("txtClusterName")
#define cbServer        CTRL_COMBOBOX("cbServer")
#define cbDatabase      CTRL_COMBOBOX("cbDatabase")
#define txtNodeID       CTRL_TEXT("txtNodeID")
#define txtNodeName     CTRL_TEXT("txtNodeName")

BEGIN_EVENT_TABLE(dlgRepCluster, dlgProperty)
    EVT_BUTTON(wxID_OK,                     dlgRepCluster::OnOK)
    EVT_CHECKBOX(XRCID("chkJoinCluster"),   dlgRepCluster::OnChangeJoin)
    EVT_COMBOBOX(XRCID("cbServer"),         dlgRepCluster::OnChangeServer)
    EVT_COMBOBOX(XRCID("cbDatabase"),       dlgRepCluster::OnChangeDatabase)
    EVT_COMBOBOX(XRCID("cbClusterName"),    dlgRepCluster::OnChangeCluster)
    EVT_TEXT(XRCID("txtClusterName"),       dlgRepCluster::OnChange)
    EVT_TEXT(XRCID("txtNodeID"),            dlgRepCluster::OnChange)
    EVT_TEXT(XRCID("txtNodeName"),          dlgRepCluster::OnChange)
END_EVENT_TABLE();


dlgRepCluster::dlgRepCluster(frmMain *frame, slCluster *node, pgObject *obj)
: dlgProperty(frame, wxT("dlgRepCluster"))
{
    SetIcon(wxIcon(slcluster_xpm));
    cluster=node;
    server=0;
    remoteConn=0;

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
    txtNodeID->SetValidator(numericValidator);
    chkJoinCluster->SetValue(false);

    if (cluster)
    {
        // edit mode
        txtClusterName->SetValue(cluster->GetName());
        txtNodeID->SetValue(NumToStr(cluster->GetLocalNodeID()));
        txtNodeID->Disable();
        txtNodeName->SetValue(cluster->GetLocalNodeName());
        txtNodeName->Disable();
        chkJoinCluster->Disable();
    }
    else
    {
        // create mode
        txtClusterName->Hide();

        wxCookieType cookie;
        wxTreeItemId serverItem=mainForm->GetBrowser()->GetFirstChild(servers, cookie);        
        while (serverItem)
        {
            pgServer *server = (pgServer*)mainForm->GetBrowser()->GetItemData(serverItem);
            if (server && server->GetType() == PG_SERVER)
                cbServer->Append(mainForm->GetBrowser()->GetItemText(serverItem), (void*)server);

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
        server = (pgServer*)cbServer->GetClientData(sel);

        if (!server->GetConnected())
        {
            server->Connect(mainForm, server->GetNeedPwd());
            if (!server->GetConnected())
            {
                wxLogError(server->GetLastError());
                return;
            }
        }
        if (server->GetConnected())
        {
            pgSet *set=server->ExecuteSet(
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
        remoteConn = server->CreateConn(cbDatabase->GetValue());
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
    int sel=cbClusterName->GetSelection();
    if (remoteConn && sel >= 0)
    {
        usedNodes.Clear();
        pgSet *set=remoteConn->ExecuteSet(
            wxT("SELECT no_id FROM ") + qtIdent(wxT("_") + cbClusterName->GetValue()) + wxT(".sl_node"));

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
    connection->ExecuteVoid(wxT("BEGIN TRANSACTION;"));

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

        // create new node on existing cluster
        if (done)
            done = remoteConn->ExecuteVoid(
                wxT("SELECT ") + schemaPrefix + wxT("storenode(") 
                    + txtNodeID->GetValue() + wxT(", ")
                    + qtString(txtNodeName->GetValue()) + wxT(");\n")
                wxT("SELECT ") + schemaPrefix + wxT("enablenode(") 
                    + txtNodeID->GetValue() + wxT(");\n"));
    }

    if (!done)
    {
        done = connection->ExecuteVoid(wxT("ROLLBACK TRANSACTION;"));
        EnableOK(true);
        return;
    }

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
        EnableOK(txtComment->GetValue() != cluster->GetComment());
    }
    else
    {
        bool enable=true;
        long nodeId = StrToLong(txtNodeID->GetValue());
        CheckValid(enable, !cbClusterName->GetValue().IsEmpty(), _("Please specify name."));
        CheckValid(enable, nodeId > 0, _("Please specify local node ID."));
        size_t i;
        for (i=0 ; i < usedNodes.GetCount() && enable; i++)
        {
            CheckValid(enable, nodeId != usedNodes[i], _("Node ID is already in use."));
        }
        CheckValid(enable, !txtNodeName->GetValue().IsEmpty(), _("Please specify local node name."));

        EnableOK(enable);
    }
}



wxString dlgRepCluster::GetSql()
{
    wxString sql;
    wxString name;
    if (chkJoinCluster->GetValue())
        name = qtIdent(wxT("_") + cbClusterName->GetValue());
    else
        name = qtIdent(wxT("_") + txtClusterName->GetValue());

    extern wxString slony1BaseScript;
    extern wxString slony1FunctionScript;
    extern wxString slony1XxidScript;

    if (cluster)
    {
        // edit mode
    }
    else
    {
        // create mode
        wxFile base(slony1BaseScript, wxFile::read);
        if (!base.IsOpened())
            return sql;

        wxFile func(slony1FunctionScript, wxFile::read);
        if (!func.IsOpened())
            return sql;

        wxFile xxid(slony1XxidScript, wxFile::read);
        if (!xxid.IsOpened())
            return sql;

        sql = wxT("CREATE SCHEMA ") + name + wxT(";\n\n");

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

        sql.Replace(wxT("@NAMESPACE@"), name);
        sql.Replace(wxT("@CLUSTERNAME@"), GetName());

        sql += wxT("\n")
               wxT("SELECT ") + name + wxT(".initializelocalnode(") +
                txtNodeID->GetValue() + wxT(", ") + qtString(txtNodeName->GetValue())
                + wxT(");\n");
    }

    if (!txtComment->GetValue().IsEmpty())
        sql += wxT("\n")
               wxT("COMMENT ON SCHEMA ") + name + wxT(" IS ") 
               + qtString(txtComment->GetValue()) + wxT(";\n");
    

    if (chkJoinCluster->GetValue())
        sql += wxT("\n\n-- In addition, the configuration is copied from the existing cluster.\n");

    return sql;
}
