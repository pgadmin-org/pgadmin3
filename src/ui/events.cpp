//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// events.cpp - Event handlers for frmMain
//
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/tipdlg.h>
#include <wx/stc/stc.h>

// App headers
#include "misc.h"
#include "menu.h"
#include "frmMain.h"
#include "frmAbout.h"
#include "frmConnect.h"
#include "frmOptions.h"
#include "frmPassword.h"
#include "frmQuery.h"
#include "frmStatus.h"
#include "ctlSQLBox.h"
#include "pgConn.h"
#include "pgLanguage.h"
#include "pgDatabase.h"
#include "pgSet.h"
#include "pgServer.h"
#include "pgObject.h"
#include "pgCollection.h"
#include "pgFunction.h"
#include "frmQueryBuilder.h"
#include "frmEditGrid.h"
#include "frmHelp.h"
#include "dlgProperty.h"
#include "frmMaintenance.h"
#include "frmIndexcheck.h"
#include "frmGrantWizard.h"

extern wxString loadPath;


// Event table
BEGIN_EVENT_TABLE(frmMain, wxFrame)
    EVT_MENU(MNU_SQL,                       frmMain::OnSql)
    EVT_MENU(MNU_MAINTENANCE,               frmMain::OnMaintenance)
    EVT_MENU(MNU_INDEXCHECK,                frmMain::OnIndexcheck)
    EVT_MENU(MNU_GRANTWIZARD,               frmMain::OnGrantWizard)
    EVT_MENU(MNU_CONTENTS,                  frmMain::OnContents)
    EVT_MENU(MNU_HELP,                      frmMain::OnHelp)
    EVT_MENU(MNU_FAQ,                       frmMain::OnFaq)
    EVT_MENU(MNU_BUGREPORT,                 frmMain::OnBugreport)
    EVT_MENU(MNU_PGSQLHELP,                 frmMain::OnPgsqlHelp)
    EVT_MENU(MNU_ABOUT,                     frmMain::OnAbout)
    EVT_MENU(MNU_ADDSERVER,                 frmMain::OnAddServer)
    EVT_MENU(MNU_REFRESH,                   frmMain::OnRefresh)
    EVT_MENU(MNU_CONNECT,                   frmMain::OnConnect)
    EVT_MENU(MNU_DISCONNECT,                frmMain::OnDisconnect)
    EVT_MENU(MNU_DELETE,                    frmMain::OnDelete)
    EVT_MENU(MNU_DROP,                      frmMain::OnDrop)
    EVT_MENU(MNU_CREATE,                    frmMain::OnCreate)
    EVT_MENU(MNU_PROPERTIES,                frmMain::OnProperties)
    EVT_MENU(MNU_EXIT,                      frmMain::OnExit)
    EVT_MENU(MNU_STATUS,                    frmMain::OnStatus)
    EVT_MENU(MNU_VIEWDATA,                  frmMain::OnViewData)
    EVT_MENU(MNU_VIEWFILTEREDDATA,          frmMain::OnViewFilteredData)
    EVT_MENU(MNU_OPTIONS,                   frmMain::OnOptions)
    EVT_MENU(MNU_PASSWORD,                  frmMain::OnPassword)
    EVT_MENU(MNU_SAVEDEFINITION,            frmMain::OnSaveDefinition)
    EVT_MENU(MNU_SYSTEMOBJECTS,             frmMain::OnShowSystemObjects)
    EVT_MENU(MNU_TIPOFTHEDAY,               frmMain::OnTipOfTheDay)
    EVT_MENU(MNU_QUERYBUILDER,              frmMain::OnQueryBuilder)
    EVT_MENU(MNU_RELOAD,                    frmMain::OnReload)
    EVT_MENU(MNU_NEW+PG_DATABASE,           frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_USER,               frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_GROUP,              frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_LANGUAGE,           frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_CAST,               frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_SCHEMA,             frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_AGGREGATE,          frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_CONVERSION,         frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_DOMAIN,             frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_FUNCTION,           frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_TRIGGERFUNCTION,    frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_OPERATOR,           frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_OPERATORCLASS,      frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_SEQUENCE,           frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_TABLE,              frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_TYPE,               frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_VIEW,               frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_COLUMN,             frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_PRIMARYKEY,         frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_FOREIGNKEY,         frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_UNIQUE,             frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_CHECK,              frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_INDEX,              frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_RULE,               frmMain::OnNew)
    EVT_MENU(MNU_NEW+PG_TRIGGER,            frmMain::OnNew)
    EVT_MENU(MNU_NEW+PGA_JOB,               frmMain::OnNew)
    EVT_MENU(MNU_NEW+PGA_STEP,              frmMain::OnNew)
    EVT_MENU(MNU_NEW+PGA_SCHEDULE,          frmMain::OnNew)
    EVT_MENU(MNU_CHECKALIVE,                frmMain::OnCheckAlive)
    EVT_MENU(MNU_CONTEXTMENU,               frmMain::OnContextMenu) 
    EVT_NOTEBOOK_PAGE_CHANGED(CTL_NOTEBOOK, frmMain::OnPageChange)
    EVT_LIST_ITEM_SELECTED(CTL_PROPVIEW,    frmMain::OnPropSelChanged)
    EVT_TREE_SEL_CHANGED(CTL_BROWSER,       frmMain::OnTreeSelChanged)
    EVT_TREE_ITEM_EXPANDING(CTL_BROWSER,    frmMain::OnExpand)
    EVT_TREE_ITEM_COLLAPSING(CTL_BROWSER,   frmMain::OnCollapse)
    EVT_TREE_ITEM_ACTIVATED(CTL_BROWSER,    frmMain::OnSelActivated)
    EVT_TREE_ITEM_RIGHT_CLICK(CTL_BROWSER,  frmMain::OnSelRightClick) 
    EVT_CLOSE(                              frmMain::OnClose)
#ifdef __WXGTK__
    EVT_KEY_DOWN(                           frmMain::OnKeyDown)
    EVT_TREE_KEY_DOWN(CTL_BROWSER,          frmMain::OnTreeKeyDown)
#endif
END_EVENT_TABLE()



// Event handlers
void frmMain::OnKeyDown(wxKeyEvent& event)
{
    event.m_metaDown=false;
    event.Skip();
}


// unfortunately, under GTK we won't get the original wxKeyEvent
// to reset m_metaDown
void frmMain::OnTreeKeyDown(wxTreeEvent& event)
{
    switch (event.GetKeyCode())
    {
	case WXK_F1:
	    OnHelp(event);
	    break;
	case WXK_F5:
	    OnRefresh(event);
	    break;
	case WXK_DELETE:
	    OnDrop(event);
	    break;
	default:
	    event.Skip();
	    break;
    }
}


void frmMain::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(FALSE);   // Allow sub windows to stop us
}



void frmMain::OnClose(wxCloseEvent& event)
{
    wxWindow *fr;
    windowList::Node *node;
    while ((node=frames.GetFirst()) != NULL)
    {
        fr=node->GetData();
        if (!fr->Close(!event.CanVeto()))
        {
            if (event.CanVeto())
            {
                event.Veto();
                return;
            }
        }
        delete node;
        fr->Destroy();
    }
    Destroy();
}


void frmMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    frmAbout *winAbout = new frmAbout(this);
    winAbout->Show(TRUE);
}


void frmMain::OnTipOfTheDay(wxCommandEvent& WXUNUSED(event))
{
extern wxString docPath;
extern wxLocale *locale;

    wxString file;
    
    file = docPath + wxT("/") + locale->GetCanonicalName() + wxT("/tips.txt");

    if (!wxFile::Exists(file))
        file = docPath + wxT("/en_US/tips.txt");    

    if (!wxFile::Exists(file)) {
        wxLogError(_("Couldn't open a tips.txt file!"));
        return;
    }

    wxTipProvider *tipProvider = wxCreateFileTipProvider(file, settings->GetNextTipOfTheDay());
    settings->SetShowTipOfTheDay(wxShowTip(this, tipProvider));
    settings->SetNextTipOfTheDay(tipProvider->GetCurrentTip());

    delete tipProvider;
}


void frmMain::OnReload(wxCommandEvent& WXUNUSED(event))
{
    pgFunction *func = (pgFunction*)GetSelectedObject();
    if (func)
    {
        StartMsg(wxT("Reloading library ") + func->GetBin());
        func->ReloadLibrary();
        EndMsg();
    }
}


void frmMain::OnContents(wxCommandEvent& event)
{
    DisplayHelp(this, wxT("index"));
}


void frmMain::OnPgsqlHelp(wxCommandEvent& event)
{
    DisplaySqlHelp(this, wxT("index"));
}


void frmMain::OnFaq(wxCommandEvent& event)
{
    frmHelp *h=new frmHelp(this);
    h->Show(true);
    if (!h->Load(wxT("http://www.pgadmin.org/pgadmin3/faq/")))
        h->Destroy();
}


void frmMain::OnBugreport(wxCommandEvent& event)
{
    DisplayHelp(this, wxT("bugreport"));
}


void frmMain::OnHelp(wxCommandEvent& event)
{
    wxString page;

    pgObject *obj = GetSelectedObject();
    if (obj)
        page=obj->GetHelpPage(true);

    if (page.IsEmpty())
        page = wxT("sql-commands");

    DisplaySqlHelp(this, page);
}


void frmMain::OnOptions(wxCommandEvent& event)
{
    frmOptions *winOptions = new frmOptions(this);
    winOptions->Show(TRUE);
}

void frmMain::OnCollapse(wxTreeEvent &event)
{
#ifdef WIN32
    // This is weird stuff, but somewhere comes a collapse after we have done
    // connecting the server and expanding the tree.
    // Possibly not necessary 
    if (event.GetItem() == denyCollapseItem)
        event.Veto();
#endif
    denyCollapseItem=wxTreeItemId();
}


void frmMain::OnExpand(wxTreeEvent &event)
{
    wxCookieType cookie;
    wxTreeItemId item=browser->GetFirstChild(event.GetItem(), cookie);
    if (item && !browser->GetItemData(item))
    {
        // the expanding node has a dummy item.
        // delete dummy item, and expand kids.
        execSelChange(event.GetItem(), browser->GetSelection() == item);

        // we don't have any kids, so don't expand
        if (!browser->GetChildrenCount(event.GetItem()))
            event.Veto();
    }
}


void frmMain::OnStatus(wxCommandEvent &event)
{
    pgObject *data = GetSelectedObject();
    if (!data)
        return;

    pgDatabase *db=data->GetDatabase();
    if (!db)
        return;

    pgServer *server=db->GetServer();

    pgConn *conn= new pgConn(server->GetName(), db->GetName(), server->GetUsername(), server->GetPassword(), server->GetPort());
    if (conn->GetStatus() == PGCONN_OK)
    {
        wxString txt = wxT("pgAdmin III Server Status - ") + server->GetDescription() 
            + wxT(" (") + server->GetName() + wxT(":") + NumToStr((long)server->GetPort()) + wxT(")");

        wxPoint pos(settings->Read(wxT("frmStatus"), wxPoint(100, 100)));
        wxSize size(settings->Read(wxT("frmStatus"), wxSize(400, 240)));
        CheckOnScreen(pos, size, 200, 150);
        frmStatus *status = new frmStatus(this, txt, conn, pos, size);
        frames.Append(status);
        status->Go();
    }
}


void frmMain::OnCheckAlive(wxCommandEvent &event)
{
    bool userInformed = false;
    bool closeIt;

    wxCookieType cookie;
    wxTreeItemId item=browser->GetFirstChild(servers, cookie);
    while (item)
    {
        pgServer *server=(pgServer*)browser->GetItemData(item);
        if (server && server->GetType() == PG_SERVER && server->connection())
        {
            if (server->connection()->IsAlive())
            {
                wxCookieType cookie2;
                item = browser->GetFirstChild(server->GetId(), cookie2);
                while (item)
                {
                    pgObject *obj=(pgObject*)browser->GetItemData(item);
                    if (obj && obj->GetType() == PG_DATABASES)
                    {
                        wxCookieType cookie3;
                        item = browser->GetFirstChild(obj->GetId(), cookie3);
                        while (item)
                        {
                            pgDatabase *db=(pgDatabase*)browser->GetItemData(item);
                            if (db && db->GetType() == PG_DATABASE && db->GetConnected() && db->connection())
                            {
                                if (!db->connection()->IsAlive())
                                {
                                    if (!userInformed)
                                    {
                                        wxMessageDialog dlg(this, _("Close database browser? If you abort, the object browser will not show accurate data."),
                                        wxString::Format(_("Connection to database %s lost."), db->GetName().c_str()), 
                                            wxICON_EXCLAMATION|wxYES_NO|wxYES_DEFAULT);

                                        closeIt = (dlg.ShowModal() == wxID_YES);
                                        userInformed = true;
                                    }
                                    if (closeIt)
                                    {
                                        browser->DeleteChildren(db->GetId());
                                        browser->SetItemImage(db->GetId(), PGICON_CLOSEDDATABASE, wxTreeItemIcon_Selected);
                                        browser->SetItemImage(db->GetId(), PGICON_CLOSEDDATABASE, wxTreeItemIcon_Selected);
                                        db->Disconnect();
                                    }
                                }
                            }
                            item = browser->GetNextChild(obj->GetId(), cookie3);
                        }
                    }
                    item = browser->GetNextChild(server->GetId(), cookie2);
                }
            }
            else
            {
                if (!userInformed)
                {
                    wxMessageDialog dlg(this, _("Close server browser? If you abort, the object browser will not show accurate data."),
                        wxString::Format(_("Connection to server %s lost."), server->GetName().c_str()), 
                        wxICON_EXCLAMATION|wxYES_NO|wxYES_DEFAULT);

                    closeIt = (dlg.ShowModal() == wxID_YES);
                    userInformed = true;
                }
                if (closeIt)
                {
                    browser->DeleteChildren(server->GetId());
                    browser->SetItemImage(server->GetId(), PGICON_SERVERBAD, wxTreeItemIcon_Normal);
                    browser->SetItemImage(server->GetId(), PGICON_SERVERBAD, wxTreeItemIcon_Selected);
                    server->Disconnect();
                }
            }
        }

        item = browser->GetNextChild(servers, cookie);
    }
}


void frmMain::OnPassword(wxCommandEvent& event)
{
    frmPassword *winPassword = new frmPassword(this);

    // We need to pass the server to the password form
    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    pgObject *data = GetSelectedObject();
    if (data)
    {
        int type = data->GetType();

        switch (type) {
            case PG_SERVER:
                winPassword->SetServer((pgServer *)data);
                winPassword->Show(TRUE);
                break;

            default:
                // Should never see this
                wxLogError(__("You must select a server before changing your password!"));
                break;
        }
    }
}


void frmMain::OnMaintenance(wxCommandEvent &ev)
{
    pgObject *data = GetSelectedObject();

    if (data)
    {
        frmMaintenance *frm=new frmMaintenance(this, data);
        frm->Go();
    }
}


void frmMain::OnIndexcheck(wxCommandEvent &ev)
{
    pgObject *data = GetSelectedObject();

    if (data)
    {
        frmIndexcheck *frm=new frmIndexcheck(this, data);
        frm->Go();
    }
}


void frmMain::OnGrantWizard(wxCommandEvent &ev)
{
    pgObject *data = GetSelectedObject();

    if (data)
    {
        frmGrantWizard *frm=new frmGrantWizard(this, data);
        frm->Go();
    }
}


void frmMain::OnSql(wxCommandEvent &ev)
{
    pgObject *data = GetSelectedObject();
    if (!data)
        return;

    pgDatabase *db=data->GetDatabase();
    if (!db)
        return;

    pgServer *server=db->GetServer();

    pgConn *conn= new pgConn(server->GetName(), db->GetName(), server->GetUsername(), server->GetPassword(), server->GetPort());
    if (conn->GetStatus() == PGCONN_OK)
    {
        wxString txt = wxT("pgAdmin III Query - ") + server->GetDescription() + wxT(" (") + server->GetName() + wxT(":") + NumToStr((long)server->GetPort()) + wxT(") - ") + db->GetName();

        wxPoint pos(settings->Read(wxT("frmQuery"), wxPoint(100, 100)));
        wxSize size(settings->Read(wxT("frmQuery"), wxSize(600, 500)));
        CheckOnScreen(pos, size, 200, 150);

        wxString qry;
        if (settings->GetStickySql()) 
            qry = sqlPane->GetText();
        frmQuery *fq= new frmQuery(this, txt, conn, pos, size, qry);
        frames.Append(fq);
        fq->Go();
    }
    else
    {
		wxLogError(conn->GetLastError());
        delete conn;
    }
}

void frmMain::OnViewData(wxCommandEvent& event)
{
	ViewData(false);
}

void frmMain::OnViewFilteredData(wxCommandEvent& event)
{
	ViewData(true);
}

void frmMain::ViewData(bool filter)
{
    pgSchemaObject *data = (pgSchemaObject *)GetSelectedObject();
    if (!data)
        return;
    if (data->GetType() != PG_TABLE && data->GetType() != PG_VIEW)
        return;

    pgDatabase *db=data->GetDatabase();
    if (!db)
        return;

    pgServer *server=db->GetServer();
    if (!server)
        return;

    pgConn *conn= new pgConn(server->GetName(), db->GetName(), server->GetUsername(), server->GetPassword(), server->GetPort());

    if (conn->GetStatus() == PGCONN_OK)
    {
        wxString txt = wxT("pgAdmin III Edit Data - ")
            + server->GetDescription() 
            + wxT(" (") + server->GetName() 
            + wxT(":") + NumToStr((long)server->GetPort()) 
            + wxT(") - ") + db->GetName()
            + wxT(" - ") + data->GetFullIdentifier();

        wxPoint pos(settings->Read(wxT("frmEditGrid"), wxPoint(100, 100)));
        wxSize size(settings->Read(wxT("frmEditGrid"), wxSize(600, 500)));
        CheckOnScreen(pos, size, 200, 150);

        frmEditGrid *eg= new frmEditGrid(this, txt, conn, pos, size, data);
        frames.Append(eg);
        eg->ShowForm(filter);
    }
    else
    {
		wxLogError(conn->GetLastError());
        delete conn;
    }
}

void frmMain::OnSaveDefinition(wxCommandEvent& event)
{

    wxLogInfo(wxT("Saving object definition"));

    if (sqlPane->GetText().IsNull()) {
        wxLogError(__("There is nothing in the SQL pane to save!"));
        return;
    }

    wxFileDialog filename(this, _("Select output file"), wxT(""), wxT(""), _("SQL Scripts (*.sql)|*.sql|All files (*.*)|*.*"));
    filename.SetStyle(wxSAVE | wxOVERWRITE_PROMPT);

    // Show the dialogue
    if (filename.ShowModal() == wxID_OK)
    {
        // Write the file
        if (!FileWrite(filename.GetPath(), sqlPane->GetText()))
            wxLogError(__("Failed to write to the output file: %s"), filename.GetPath().c_str());
    }
    else
    {
        wxLogInfo(wxT("User cancelled"));
    }
}

void frmMain::OnShowSystemObjects(wxCommandEvent& event)
{
    // Warn the user
    if (wxMessageBox(_("Changing the 'Show System Objects' option will cause all connections to be closed, and the treeview to be rebuilt.\n\nAre you sure you wish to continue?"),
                     _("Continue?"), wxYES_NO | wxICON_QUESTION) == wxNO) {
        viewMenu->Check(MNU_SYSTEMOBJECTS, settings->GetShowSystemObjects());
        return;
    }

    if (settings->GetShowSystemObjects()) {
        settings->SetShowSystemObjects(FALSE);
        viewMenu->Check(MNU_SYSTEMOBJECTS, FALSE);
    } else {
        settings->SetShowSystemObjects(TRUE);
        viewMenu->Check(MNU_SYSTEMOBJECTS, TRUE);
    }

    wxLogInfo(wxT("Clearing treeview to toggle ShowSystemObjects"));

    // Clear the treeview
    browser->DeleteAllItems();

    // Add the root node
    pgObject *serversObj = new pgServers();
    servers = browser->AddRoot(wxT("Servers"), PGICON_SERVER, -1, serversObj);
    RetrieveServers();
    browser->Expand(servers);
    browser->SelectItem(servers);
#ifdef __WIN32__
    denyCollapseItem = servers;
#endif
}

void frmMain::OnAddServer(wxCommandEvent &ev)
{
    // Create a server object and connect it.
    pgServer *server = new pgServer(settings->GetLastServer(), settings->GetLastDescription(), settings->GetLastDatabase(), 
        settings->GetLastUsername(), settings->GetLastPort(), false, settings->GetLastSSL());
    int res = server->Connect(this);

    // Check the result, and handle it as appropriate
    if (res == PGCONN_OK) {
        wxLogInfo(wxT("pgServer object initialised as required."));
        browser->AppendItem(servers, server->GetFullName(), PGICON_SERVER, -1, server);
        browser->Expand(servers);

    } else if (res == PGCONN_DNSERR)  {
        delete server;
        OnAddServer(ev);

    } else if (res == PGCONN_BAD)  {
        wxLogError(__("Error connecting to the server: %s"), server->GetLastError().c_str());

        delete server;
        OnAddServer(ev);

    } else {
        wxLogInfo(__("pgServer object didn't initialise because the user aborted."));
        delete server;
    }

    // Reset the Servers node text
    wxString label;
    label.Printf(_("Servers (%d)"), browser->GetChildrenCount(servers, FALSE));
    browser->SetItemText(servers, label);
    StoreServers();
}


void frmMain::OnPropSelChanged(wxListEvent& event)
{
    wxTreeItemId item=browser->GetSelection();
    pgObject *data=(pgObject*)browser->GetItemData(item);
    if (data && data->IsCollection())
    {
        data=((pgCollection*)data)->FindChild(browser, event.GetIndex());
        if (data)
        {
            setDisplay(data);
            sqlPane->SetReadOnly(false);
            sqlPane->SetText(data->GetSql(browser));
            sqlPane->SetReadOnly(true);
        }
    }
}


void frmMain::OnTreeSelChanged(wxTreeEvent& event)
{
    denyCollapseItem=wxTreeItemId();
	// Reset the listviews/SQL pane
    execSelChange(event.GetItem(), true);
}


void frmMain::execSelChange(wxTreeItemId item, bool currentNode)
{
    properties->ClearAll();
    properties->AddColumn(_("Properties"), 500);
    properties->InsertItem(0, _("No properties are available for the current selection"), PGICON_PROPERTY);
    sqlPane->Clear();

    // Reset the toolbar & password menu options
	// Handle the menus associated with the buttons
    SetButtons(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);


    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    pgObject *data = (pgObject *)browser->GetItemData(item);

    // If we didn't get an object, then we may have a right click, or 
    // invalid click, so ignore.
    if (!data) return;

    if (currentNode)
    {
        properties->Freeze();
        statistics->Freeze();
        setDisplay(data, properties, sqlPane);
        properties->Thaw();
        statistics->Thaw();
    }
    else
        setDisplay(data, 0, 0);
}


void frmMain::setDisplay(pgObject *data, ctlListView *props, ctlSQLBox *sqlbox)
{
    data->RemoveDummyChild(browser);

    int type = data->GetType();
    pgServer *server=0;


    bool canReload=false;
    bool canConnect=false;
    bool canDisconnect=false;
    bool canReindex=false;
    bool canIndexCheck=false;
    bool canGrantWizard=false;

    bool showTree=true;

    switch (type)
    {
        case PG_SERVER:
            StartMsg(_("Retrieving server properties"));

            server = (pgServer *)data;
            if (!server->GetConnected())
            {
                canConnect=true;
            }
			else
            {
                canDisconnect=true;
                canReindex=true;
            }
            data->ShowTree(this, browser, props, sqlbox);
            showTree=false;
            EndMsg();
            break;

        case PG_FUNCTION:
        case PG_TRIGGERFUNCTION:
        {
            canReload=((pgFunction*)data)->CanReload();
            break;
        }
        case PG_DATABASE:
        case PG_SCHEMAS:
        case PG_SCHEMA:
        case PG_TABLES:
            canIndexCheck=true;
            canGrantWizard=true;
            break;
        case PG_FUNCTIONS:
        case PG_TRIGGERFUNCTIONS:
        case PG_SEQUENCES:
        case PG_VIEWS:
            canGrantWizard=true;
            break;
        case PG_TABLE:
            canIndexCheck=true;
            break;
        case PG_DATABASES:
        case PG_GROUPS:
        case PG_USERS:
        case PG_GROUP:
        case PG_USER:
        case PG_LANGUAGES:
        case PG_LANGUAGE:
        case PG_AGGREGATES:
        case PG_AGGREGATE:
        case PG_CASTS:
        case PG_CAST:
        case PG_CONVERSIONS:
        case PG_CONVERSION:
        case PG_DOMAINS:
        case PG_DOMAIN:
        case PG_OPERATORS:
        case PG_OPERATOR:
        case PG_OPERATORCLASSES:
        case PG_OPERATORCLASS:
        case PG_SEQUENCE:
        case PG_TYPES:
        case PG_TYPE:
        case PG_VIEW:
        case PG_CHECK:
        case PG_COLUMNS:
        case PG_COLUMN:
        case PG_CONSTRAINTS:
        case PG_PRIMARYKEY:
        case PG_UNIQUE:
        case PG_FOREIGNKEY:
        case PG_INDEXES:
        case PG_INDEX:
        case PG_RULES:
        case PG_RULE:
        case PG_TRIGGERS:
        case PG_TRIGGER:
        case PGA_AGENT:
        case PGA_JOB:
        case PGA_STEP:
        case PGA_SCHEDULE:
            break;
        default:        
            showTree=false;
			break;
    }

    if (showTree)
        data->ShowTree(this, browser, props, sqlbox);

    if (sqlbox)
    {
        sqlbox->SetReadOnly(false);
        sqlbox->SetText(data->GetSql(browser));
        sqlbox->SetReadOnly(true);
    }

    unsigned int i;
    wxMenuItem *menuItem;
    i=newMenu->GetMenuItemCount();
    while (i--)
    {
        menuItem=newMenu->GetMenuItems().Item(i)->GetData();
        if (menuItem)
            delete newMenu->Remove(menuItem);
    }

    i=newContextMenu->GetMenuItemCount();
    while (i--)
    {
        menuItem=newContextMenu->GetMenuItems().Item(i)->GetData();
        if (menuItem)
            delete newContextMenu->Remove(menuItem);
    }

    editMenu->Enable(MNU_NEWOBJECT, false);
    treeContextMenu->Enable(MNU_NEWOBJECT, false);

    wxMenu *indivMenu=data->GetNewMenu();
    if (indivMenu)
    {
        if (indivMenu->GetMenuItemCount())
        {
            editMenu->Enable(MNU_NEWOBJECT, true);
            treeContextMenu->Enable(MNU_NEWOBJECT, true);

            for (i=0 ; i < indivMenu->GetMenuItemCount() ; i++)
            {
                menuItem=indivMenu->GetMenuItems().Item(i)->GetData();
                newMenu->Append(menuItem->GetId(), menuItem->GetLabel(), menuItem->GetHelp());
                newContextMenu->Append(menuItem->GetId(), menuItem->GetLabel(), menuItem->GetHelp());
            }
        }
        delete indivMenu;
    }
    else
    {
    }
    toolsMenu->Enable(MNU_RELOAD, canReload);
    treeContextMenu->Enable(MNU_RELOAD, canReload);
    toolsMenu->Enable(MNU_CONNECT, canConnect);
    treeContextMenu->Enable(MNU_CONNECT, canConnect);               
    toolsMenu->Enable(MNU_DISCONNECT, canDisconnect);
//    toolsMenu->Enable(MNU_INDEXCHECK, canIndexCheck);
    toolsMenu->Enable(MNU_GRANTWIZARD, canGrantWizard);
    treeContextMenu->Enable(MNU_DISCONNECT, canDisconnect);
    fileMenu->Enable(MNU_PASSWORD, canDisconnect);
}


void frmMain::OnConnect(wxCommandEvent &ev)
{
    pgServer *server = (pgServer *)GetSelectedObject();
    if (server && server->GetType() == PG_SERVER && !server->GetConnected())
        ReconnectServer(server);
}


void frmMain::OnDisconnect(wxCommandEvent &ev)
{
    pgServer *server = (pgServer *)GetSelectedObject();
    if (server && server->GetType() == PG_SERVER && server->Disconnect())
    {
        browser->SetItemImage(server->GetId(), PGICON_SERVERBAD, wxTreeItemIcon_Normal);
        browser->SetItemImage(server->GetId(), PGICON_SERVERBAD, wxTreeItemIcon_Selected);
        browser->DeleteChildren(server->GetId());
        execSelChange(server->GetId(), true);
    }
}


void frmMain::OnSelActivated(wxTreeEvent &event)
{
    // This handler will primarily deal with displaying item
    // properties in seperate windows and 'Add xxx...' clicks

    // Get the item data, and feed it to the relevant handler,
    // cast as required.

    wxTreeItemId item = event.GetItem();
    pgObject *data = (pgObject *)browser->GetItemData(item);
    if (!data)
        return;
    int type = data->GetType();
    pgServer *server;
    wxCommandEvent nullEvent;

    switch (type) {
        case PG_SERVER:
            server = (pgServer *)data;
            if (!server->GetConnected())
            {
                if (ReconnectServer(server) == PGCONN_OK)
                {
                    // prevent from being collapsed immediately

                    denyCollapseItem=item;
                }
            }
            break;

        default:
            if (settings->GetDoubleClickProperties()) {
                if (data->CanEdit())
                {
                    OnProperties(nullEvent);
                    event.Skip();
                    return;
                }
            }
            break;
    }

#ifndef __WXMSW__
    browser->Expand(item);
#endif
}


////////////////////////////////////////////////////////////////////////////////
// This handler will display a popup menu for the currently selected item
////////////////////////////////////////////////////////////////////////////////
void frmMain::OnContextMenu(wxCommandEvent& event)
{
    if (FindFocus() == browser)
    {
        wxRect rect;
        browser->GetBoundingRect(browser->GetSelection(), rect);
        wxPoint point = rect.GetPosition();
	    wxPoint origin = GetClientAreaOrigin();

	    // Because this Tree is inside a vertical splitter, we
	    // must compensate for the size of the other elements
	    point.x += origin.x;
	    point.y += origin.y;

	    // popup the menu
	    PopupMenu(treeContextMenu, point);
    }
}

    
////////////////////////////////////////////////////////////////////////////////
// This handler will display a popup menu for the item at the mouse position
////////////////////////////////////////////////////////////////////////////////
void frmMain::OnSelRightClick(wxTreeEvent& event)
{
    // Check if item is selected
    wxTreeItemId item = event.GetItem();
    if (item != browser->GetSelection())
        browser->SelectItem(item);

	// Get mouse point data
    wxPoint point = event.GetPoint();
	wxPoint origin = GetClientAreaOrigin();

	// Because this Tree is inside a vertical splitter, we
	// must compensate for the size of the other elements
	point.x += origin.x;
	point.y += origin.y;

	// popup the menu
	PopupMenu(treeContextMenu, point);
}


void frmMain::OnDelete(wxCommandEvent &ev)
{
    wxWindow *current=wxWindow::FindFocus();
    if (current == browser)
        OnDrop(ev);
}


void frmMain::OnDrop(wxCommandEvent &ev)
{
    // This handler will primarily deal with dropping items

    // Get the item data, and feed it to the relevant handler,
    pgObject *data = GetSelectedObject();

    // accelerator can bypass disabled menu, so we need to check
    if (!data || !data->CanDrop())
        return;

    if (data->GetSystemObject())
    {
        wxMessageDialog msg(this, wxString::Format(_("Cannot drop system %s %s."), 
            wxGetTranslation(data->GetTypeName()), data->GetFullIdentifier().c_str()), 
            _("Trying to drop system object"), wxICON_EXCLAMATION);
        msg.ShowModal();
        return;
    }

    if (data->RequireDropConfirm() || settings->GetConfirmDelete())
    {
        wxMessageDialog msg(this, wxString::Format(_("Are you sure you wish to drop %s %s?"),
                wxGetTranslation(data->GetTypeName()), data->GetFullIdentifier().c_str()),
                wxString::Format(_("Drop %s?"), wxGetTranslation(data->GetTypeName())), wxYES_NO | wxICON_QUESTION);
        if (msg.ShowModal() != wxID_YES)
        {
            return;
        }
    }
    bool done=data->DropObject(this, browser);

    if (done)
    {
        wxLogInfo(wxT("Dropping %s %s"), data->GetTypeName().c_str(), data->GetIdentifier().c_str());

        wxTreeItemId parentItem=browser->GetItemParent(data->GetId());

        wxTreeItemId nextItem=browser->GetNextVisible(data->GetId());
        if (nextItem)
        {
            pgObject *nextData=(pgObject*)browser->GetItemData(nextItem);
            if (!nextData || nextData->GetType() != data->GetType())
                nextItem=browser->GetPrevSibling(data->GetId());
        }
        else
            nextItem=browser->GetPrevSibling(data->GetId());

        if (nextItem)
            browser->SelectItem(nextItem);

        int droppedType = data->GetType();
        browser->Delete(data->GetId());
        // data is invalid now


        pgCollection *collection=0;

        while (parentItem)
        {
            collection = (pgCollection*)browser->GetItemData(parentItem);
            if (collection && collection->IsCollection() && collection->IsCollectionForType(droppedType))
            {
                collection->UpdateChildCount(browser);
                break;
            }
            parentItem=browser->GetItemParent(parentItem);
        }
    }
}


void frmMain::OnRefresh(wxCommandEvent &ev)
{
    // Refresh - Clear the treeview below the current selection

    pgObject *data = GetSelectedObject();
    if (!data)
        return;

    Refresh(data);
}


void frmMain::OnCreate(wxCommandEvent &ev)
{
    pgObject *data = GetSelectedObject();

    if (data)
    {
        dlgProperty::CreateObjectDialog(this, data, -1);
    }
}


void frmMain::OnNew(wxCommandEvent &ev)
{
    int type=ev.GetId() - MNU_NEW;
    if (type == PG_SERVER)
    {
        OnConnect(ev);
        return;
    }
    pgObject *data = GetSelectedObject();

    if (data)
        dlgProperty::CreateObjectDialog(this, data, type);
}


void frmMain::OnProperties(wxCommandEvent &ev)
{
    pgObject *data = GetSelectedObject();

    if (data)
        dlgProperty::EditObjectDialog(this, sqlPane, data);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmMain::OnQueryBuilder(wxCommandEvent &ev)
{
    pgObject *data = GetSelectedObject();

    pgDatabase *db=data->GetDatabase();
    if (!db)
        return;

    pgServer *server=db->GetServer();
    pgConn *conn= new pgConn(server->GetName(), db->GetName(), server->GetUsername(), server->GetPassword(), server->GetPort());
    if (conn->GetStatus() == PGCONN_OK)
    {
	    // Create the Query Builder Form
	    frmQueryBuilder *qbform = new frmQueryBuilder(this, db); 

		frames.Append(qbform);

	    // Show the Query Builder
	    qbform->Show(TRUE);
    }
    else
    {
		wxLogError(conn->GetLastError());
        delete conn;
    }
}
