//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgServer.cpp - PostgreSQL Database Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "dlgServer.h"
#include "pgDatabase.h"
#include <wx/busyinfo.h>


// pointer to controls
#define txtDescription  CTRL_TEXT("txtDescription")
#define txtService      CTRL_TEXT("txtService")
#define cbDatabase      CTRL_COMBOBOX("cbDatabase")
#define txtPort         CTRL_TEXT("txtPort")
#define cbSSL           CTRL_COMBOBOX("cbSSL")
#define txtUsername     CTRL_TEXT("txtUsername")
#define stTryConnect    CTRL_STATIC("stTryConnect")
#define chkTryConnect   CTRL_CHECKBOX("chkTryConnect")
#define stStorePwd      CTRL_STATIC("stStorePwd")
#define chkStorePwd     CTRL_CHECKBOX("chkStorePwd")
#define stPassword      CTRL_STATIC("stPassword")
#define txtPassword     CTRL_TEXT("txtPassword")
#define txtDbRestriction CTRL_TEXT("txtDbRestriction")



BEGIN_EVENT_TABLE(dlgServer, dlgProperty)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),  dlgServer::OnPageSelect)  
    EVT_TEXT(XRCID("txtDescription"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtService"),                   dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbDatabase"),                   dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbDatabase"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtPort")  ,                    dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtUsername"),                  dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtDbRestriction"),             dlgServer::OnChangeRestr)
    EVT_COMBOBOX(XRCID("cbSSL"),                    dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkStorePwd"),              dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkTryConnect"),            dlgServer::OnChangeTryConnect)
    EVT_BUTTON(wxID_OK,                             dlgServer::OnOK)
END_EVENT_TABLE();


dlgProperty *pgServerFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgServer(this, frame, (pgServer*)node);
}


dlgServer::dlgServer(pgaFactory *f, frmMain *frame, pgServer *node)
: dlgProperty(f, frame, wxT("dlgServer"))
{
    server=node;
    dbRestrictionOk=true;

    cbDatabase->Append(wxT("postgres"));
    cbDatabase->Append(wxT("template1"));
    wxString lastDB = settings->GetLastDatabase();
    if (lastDB != wxT("postgres") && lastDB != wxT("template1"))
        cbDatabase->Append(lastDB);
    cbDatabase->SetSelection(0);

    txtPort->SetValue(NumToStr((long)settings->GetLastPort()));    
    cbSSL->SetSelection(settings->GetLastSSL());
    txtUsername->SetValue(settings->GetLastUsername());
 
    chkTryConnect->SetValue(true);
    chkStorePwd->SetValue(true);
    if (node)
    {
        chkTryConnect->SetValue(false);
        chkTryConnect->Disable();
    }
}


dlgServer::~dlgServer()
{
    if (!server)
    {
        settings->SetLastDatabase(cbDatabase->GetValue());
        settings->SetLastPort(StrToLong(txtPort->GetValue()));
        settings->SetLastSSL(cbSSL->GetCurrentSelection());
        settings->SetLastUsername(txtUsername->GetValue());
    }
}


pgObject *dlgServer::GetObject()
{
    return server;
}


void dlgServer::OnOK(wxCommandEvent &ev)
{
    // notice: changes active after reconnect

    EnableOK(false);


    if (server)
    {
        server->iSetName(GetName());
        server->iSetDescription(txtDescription->GetValue());
        if (txtService->GetValue() != server->GetServiceID())
        {
            mainForm->StartMsg(_("Checking server status"));
            server->iSetServiceID(txtService->GetValue());
            mainForm->EndMsg();
        }
        server->iSetPort(StrToLong(txtPort->GetValue()));
        server->iSetSSL(cbSSL->GetCurrentSelection());
        server->iSetDatabase(cbDatabase->GetValue());
        server->iSetUsername(txtUsername->GetValue());
        server->iSetStorePwd(chkStorePwd->GetValue());
        server->iSetDbRestriction(txtDbRestriction->GetValue());
        mainForm->execSelChange(server->GetId(), true);
        mainForm->GetBrowser()->SetItemText(item, server->GetFullName());
    }

    if (IsModal())
    {
        EndModal(wxID_OK);
        return;
    }
    else
        Destroy();
}


void dlgServer::OnChangeRestr(wxCommandEvent &ev)
{
    if (!connection || txtDbRestriction->GetValue().IsEmpty())
        dbRestrictionOk = true;
    else
    {
        wxString sql=wxT("EXPLAIN SELECT 1 FROM pg_database DB\n");
        if (connection->BackendMinimumVersion(8, 0))
            sql += wxT(" JOIN pg_tablespace ta ON db.dattablespace=ta.OID\n");
        sql += wxT(" WHERE (") + txtDbRestriction->GetValue() + wxT(")");


        wxLogNull nix;
        wxString result=connection->ExecuteScalar(sql);

        dbRestrictionOk = !result.IsEmpty();
    }
    dlgProperty::OnChange(ev);
}


void dlgServer::OnPageSelect(wxNotebookEvent &event)
{
    // to prevent dlgProperty from catching it
}


wxString dlgServer::GetHelpPage() const
{
    return wxT("connect");
}


int dlgServer::GoNew()
{
    if (cbSSL->IsEmpty())
        return Go(true);
    else
    {
        CheckChange();
        return ShowModal();
    }
}


int dlgServer::Go(bool modal)
{
    cbSSL->Append(wxT(" "));

#ifdef SSL
    cbSSL->Append(_("require"));
    cbSSL->Append(_("prefer"));

    if (pgConnBase::GetLibpqVersion() > 7.3)
    {
        cbSSL->Append(_("allow"));
        cbSSL->Append(_("disable"));
    }
#endif

    if (server)
    {
        if (cbDatabase->FindString(server->GetDatabaseName()) < 0)
            cbDatabase->Append(server->GetDatabaseName());
        txtDescription->SetValue(server->GetDescription());
        txtService->SetValue(server->GetServiceID());
        txtPort->SetValue(NumToStr((long)server->GetPort()));
        cbSSL->SetSelection(server->GetSSL());
        cbDatabase->SetValue(server->GetDatabaseName());
        txtUsername->SetValue(server->GetUsername());
        chkStorePwd->SetValue(server->GetStorePwd());
        txtDbRestriction->SetValue(server->GetDbRestriction());

        stPassword->Disable();
        txtPassword->Disable();
        if (connection)
        {
            txtName->Disable();
            cbDatabase->Disable();
            txtPort->Disable();
            cbSSL->Disable();
            txtUsername->Disable();
            chkStorePwd->Disable();
        }
    }
    else
    {
        SetTitle(_("Add server"));
    }

    int rc=dlgProperty::Go(modal);

    return rc;
}


bool dlgServer::GetTryConnect()
{
    return chkTryConnect->GetValue();
}


wxString dlgServer::GetPassword()
{
    return txtPassword->GetValue();
}


pgObject *dlgServer::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=new pgServer(GetName(), txtDescription->GetValue(), cbDatabase->GetValue(), 
        txtUsername->GetValue(), StrToLong(txtPort->GetValue()), chkTryConnect->GetValue() && chkStorePwd->GetValue(), cbSSL->GetCurrentSelection());

    return obj;
}


void dlgServer::OnChangeTryConnect(wxCommandEvent &ev)
{
    chkStorePwd->Enable(chkTryConnect->GetValue());
    txtPassword->Enable(chkTryConnect->GetValue());
    OnChange(ev);
}


void dlgServer::CheckChange()
{
    wxString name=GetName();
    bool enable=true;

    if (server)
    {
        enable =  name != server->GetName()
               || txtDescription->GetValue() != server->GetDescription()
               || txtService->GetValue() != server->GetServiceID()
               || StrToLong(txtPort->GetValue()) != server->GetPort()
               || cbDatabase->GetValue() != server->GetDatabaseName()
               || txtUsername->GetValue() != server->GetUsername()
               || cbSSL->GetCurrentSelection() != server->GetSSL()
               || chkStorePwd->GetValue() != server->GetStorePwd()
               || txtDbRestriction->GetValue() != server->GetDbRestriction();
    }


#ifdef __WXMSW__
    CheckValid(enable, !name.IsEmpty(), _("Please specify address."));
#else
    bool isPipe = (name.IsEmpty() || name.StartsWith(wxT("/")));
    cbSSL->Enable(!isPipe);
#endif
    CheckValid(enable, !txtDescription->GetValue().IsEmpty(), _("Please specify description."));
    CheckValid(enable, StrToLong(txtPort->GetValue()) > 0, _("Please specify port."));
    CheckValid(enable, !txtUsername->GetValue().IsEmpty(), _("Please specify user name"));
    CheckValid(enable, dbRestrictionOk, _("Restriction not valid."));

    EnableOK(enable);
}


wxString dlgServer::GetSql()
{
    return wxEmptyString;
}



#include "images/connect.xpm"
addServerFactory::addServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : actionFactory(list)
{
    mnu->Append(id, _("&Add Server..."), _("Add a connection to a server."));
    toolbar->AddTool(id, _("Add Server"), wxBitmap(connect_xpm), _("Add a connection to a server."), wxITEM_NORMAL);
}


wxWindow *addServerFactory::StartDialog(frmMain *form, pgObject *obj)
{
    int rc = PGCONN_BAD;
    
    dlgServer dlg(&serverFactory, form, 0);
    dlg.CenterOnParent();

    ctlTree *browser=form->GetBrowser();

    while (rc != PGCONN_OK)
    {
        if (dlg.GoNew() != wxID_OK)
            return 0;

        pgServer *server=(pgServer*)dlg.CreateObject(0);

        if (dlg.GetTryConnect())
        {
            wxBusyInfo waiting(wxString::Format(_("Connecting to server %s (%s:%d)"),
                server->GetDescription().c_str(), server->GetName().c_str(), server->GetPort()), form);
            rc = server->Connect(form, false, dlg.GetPassword());
        }
        else
        {
            rc = PGCONN_OK;
            server->InvalidatePassword();
        }
        switch (rc)
        {
            case PGCONN_OK:
            {
                int icon;
                if (server->GetConnected())
                    icon = serverFactory.GetIconId();
                else
                    icon = serverFactory.GetClosedIconId();
                wxLogInfo(wxT("pgServer object initialised as required."));
                browser->AppendItem(form->GetServerCollection()->GetId(), server->GetFullName(), 
                    icon, -1, server);

                browser->Expand(form->GetServerCollection()->GetId());
                wxString label;
                label.Printf(_("Servers (%d)"), form->GetBrowser()->GetChildrenCount(form->GetServerCollection()->GetId(), false));
                browser->SetItemText(form->GetServerCollection()->GetId(), label);
                form->StoreServers();
                return 0;
            }
            case PGCONN_DNSERR:
            {
                delete server;
                break;
            }
            case PGCONN_BAD:
            case PGCONN_BROKEN:
            {
                form->ReportConnError(server);
                delete server;

                break;
            }
            default:
            {
                wxLogInfo(__("pgServer object didn't initialise because the user aborted."));
                delete server;
                return 0;
            }
        }
    }
    return 0;
}


startServiceFactory::startServiceFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("Start service"), _("Start PostgreSQL Service"));
}


wxWindow *startServiceFactory::StartDialog(frmMain *form, pgObject *obj)
{
    pgServer *server= (pgServer*)obj;
    form->StartMsg(_("Starting service"));
    bool rc = server->StartService();
    if (rc)
        form->execSelChange(server->GetId(), true);
    form->EndMsg(rc);
    return 0;
}


bool startServiceFactory::CheckEnable(pgObject *obj)
{
    if (obj && obj->IsCreatedBy(serverFactory))
    {
        pgServer *server=(pgServer*)obj;
        return server->GetServerControllable() && !server->GetServerRunning();
    }
    return false;
}


stopServiceFactory::stopServiceFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("Stop service"), _("Stop PostgreSQL Service"));
}


wxWindow *stopServiceFactory::StartDialog(frmMain *form, pgObject *obj)
{
    pgServer *server= (pgServer*)obj;
	wxMessageDialog msg(form, _("Are you sure you wish to shutdown this server?"),
            _("Stop service"), wxYES_NO | wxICON_QUESTION);
    if (msg.ShowModal() == wxID_YES)
    {
        form->StartMsg(_("Stopping service"));

        bool done=server->StopService();

        if (done)
	    {
            if (server->Disconnect(form))
            {
                form->GetBrowser()->DeleteChildren(server->GetId());
                form->execSelChange(server->GetId(), true);
            }
	    }
        form->EndMsg(done);
    }
    return 0;
}


bool stopServiceFactory::CheckEnable(pgObject *obj)
{
    if (obj && obj->IsCreatedBy(serverFactory))
    {
        pgServer *server=(pgServer*)obj;
        return server->GetServerControllable() && server->GetServerRunning();
    }
    return false;
}


connectServerFactory::connectServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("&Connect"), _("Connect to the selected server."));
}


wxWindow *connectServerFactory::StartDialog(frmMain *form, pgObject *obj)
{
    pgServer *server = (pgServer *)obj;
    form->ReconnectServer(server);
    return 0;
}


bool connectServerFactory::CheckEnable(pgObject *obj)
{
    if (obj && obj->IsCreatedBy(serverFactory))
        return !((pgServer*)obj)->GetConnected();

    return false;
}


disconnectServerFactory::disconnectServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("Disconnec&t"), _("Disconnect from the selected server."));
}


wxWindow *disconnectServerFactory::StartDialog(frmMain *form, pgObject *obj)
{
    pgServer *server=(pgServer*)obj;
    server->Disconnect(form);
    server->UpdateIcon(form->GetBrowser());
    form->GetBrowser()->DeleteChildren(obj->GetId());
    form->execSelChange(obj->GetId(), true);
    return 0;
}


bool disconnectServerFactory::CheckEnable(pgObject *obj)
{
    if (obj && obj->IsCreatedBy(serverFactory))
        return ((pgServer*)obj)->GetConnected();

    return false;
}
