//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// events.cpp - Event handlers for frmMain
//
//
//////////////////////////////////////////////////////////////////////////

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
#include "pgAdmin3.h"
#include "misc.h"
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
#include "frmQueryBuilder.h"
#include "frmEditGrid.h"
#include "dlgProperty.h"
#include "frmVacuum.h"


// Event table
BEGIN_EVENT_TABLE(frmMain, wxFrame)
    EVT_MENU(MNU_SQL,                       frmMain::OnSql)
    EVT_MENU(MNU_VACUUM,                    frmMain::OnVacuum)
    EVT_MENU(MNU_ABOUT,                     frmMain::OnAbout)
    EVT_MENU(MNU_ADDSERVER,                 frmMain::OnAddServer)
    EVT_MENU(MNU_REFRESH,                   frmMain::OnRefresh)
    EVT_MENU(MNU_CONNECT,                   frmMain::OnConnect)
    EVT_MENU(MNU_DISCONNECT,                frmMain::OnDisconnect)
    EVT_MENU(MNU_DROP,                      frmMain::OnDrop)
    EVT_MENU(MNU_CREATE,                    frmMain::OnCreate)
    EVT_MENU(MNU_PROPERTIES,                frmMain::OnProperties)
    EVT_MENU(MNU_EXIT,                      frmMain::OnExit)
    EVT_MENU(MNU_STATUS,                    frmMain::OnStatus)
    EVT_MENU(MNU_VIEWDATA,                  frmMain::OnViewData)
    EVT_MENU(MNU_OPTIONS,                   frmMain::OnOptions)
    EVT_MENU(MNU_PASSWORD,                  frmMain::OnPassword)
    EVT_MENU(MNU_SAVEDEFINITION,            frmMain::OnSaveDefinition)
    EVT_MENU(MNU_SYSTEMOBJECTS,             frmMain::OnShowSystemObjects)
    EVT_MENU(MNU_TIPOFTHEDAY,               frmMain::OnTipOfTheDay)
    EVT_MENU(MNU_QUERYBUILDER,              frmMain::OnQueryBuilder)
    EVT_LIST_ITEM_SELECTED(CTL_PROPVIEW,    frmMain::OnPropSelChanged)
    EVT_TREE_SEL_CHANGED(CTL_BROWSER,       frmMain::OnTreeSelChanged)
    EVT_TREE_ITEM_COLLAPSING(CTL_BROWSER,   frmMain::OnCollapse)
    EVT_TREE_ITEM_ACTIVATED(CTL_BROWSER,    frmMain::OnSelActivated)
    EVT_TREE_ITEM_RIGHT_CLICK(CTL_BROWSER,  frmMain::OnSelRightClick) 
    EVT_CLOSE(                              frmMain::OnClose)
    EVT_KEY_DOWN(                           frmMain::OnKeyDown)
END_EVENT_TABLE()



// Event handlers
void frmMain::OnKeyDown(wxKeyEvent& event)
{
    event.m_metaDown=false;
    event.Skip();
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

void frmMain::OnTipOfTheDay()
{
    wxTipProvider *tipProvider = wxCreateFileTipProvider(wxT("tips.txt"), settings->GetNextTipOfTheDay());
    settings->SetShowTipOfTheDay(wxShowTip(this, tipProvider));
    settings->SetNextTipOfTheDay(tipProvider->GetCurrentTip());
    delete tipProvider;
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


void frmMain::OnStatus(wxCommandEvent &event)
{
    wxTreeItemId item=browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

    pgDatabase *db=data->GetDatabase();
    if (!db)
        return;

    pgServer *server=db->GetServer();

    pgConn *conn= new pgConn(server->GetName(), db->GetName(), server->GetUsername(), server->GetPassword(), server->GetPort());
    if (conn->GetStatus() == PGCONN_OK)
    {
        wxString txt = "pgAdmin III Server Status - " + server->GetDescription() 
            + wxT(" (") + server->GetName() + ":" + NumToStr((long)server->GetPort()) + wxT(")");

        wxPoint pos(settings->Read(wxT("frmStatus/Left"), 100), settings->Read(wxT("frmStatus/Top"), 100));
        wxSize size(settings->Read(wxT("frmStatus/Width"), 400), settings->Read(wxT("frmStatus/Height"), 240));
        CheckOnScreen(pos, size, 200, 150);
        frmStatus *status = new frmStatus(this, txt, conn, pos, size);
        frames.Append(status);
        status->Go();
    }
}

void frmMain::OnPassword(wxCommandEvent& event)
{
    frmPassword *winPassword = new frmPassword(this);

    // We need to pass the server to the password form
    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    wxTreeItemId item = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);
    int type = data->GetType();

    switch (type) {
        case PG_SERVER:
            winPassword->SetServer((pgServer *)data);
            winPassword->Show(TRUE);
            break;

        default:
            // Should never see this
            wxLogError(wxT("You must select a server before changing your password!"));
            break;
    }
}


void frmMain::OnVacuum(wxCommandEvent &ev)
{
    wxTreeItemId item=browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);
    if (data->GetType() != PG_TABLE && data->GetType() != PG_DATABASE)
        return;

    frmVacuum *frm=new frmVacuum(this, data);
    frm->Go();
}


void frmMain::OnSql(wxCommandEvent &ev)
{
    wxTreeItemId item=browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

    pgDatabase *db=data->GetDatabase();
    if (!db)
        return;

    pgServer *server=db->GetServer();

    pgConn *conn= new pgConn(server->GetName(), db->GetName(), server->GetUsername(), server->GetPassword(), server->GetPort());
    if (conn->GetStatus() == PGCONN_OK)
    {
        wxString txt = "pgAdmin III Query - " + server->GetDescription() + wxT(" (") + server->GetName() + ":" + NumToStr((long)server->GetPort()) + ") - " + db->GetName();

        wxPoint pos(settings->Read(wxT("frmQuery/Left"), 100), settings->Read(wxT("frmQuery/Top"), 100));
        wxSize size(settings->Read(wxT("frmQuery/Width"), 600), settings->Read(wxT("frmQuery/Height"), 500));
        CheckOnScreen(pos, size, 200, 150);

        wxString qry;
        if (!data->GetSystemObject())
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
    wxTreeItemId item=browser->GetSelection();
    pgSchemaObject *data = (pgSchemaObject *)browser->GetItemData(item);
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

        wxPoint pos(settings->Read(wxT("frmEditGrid/Left"), 100), settings->Read(wxT("frmEditGrid/Top"), 100));
        wxSize size(settings->Read(wxT("frmEditGrid/Width"), 600), settings->Read(wxT("frmEditGrid/Height"), 500));
        CheckOnScreen(pos, size, 200, 150);

        frmEditGrid *eg= new frmEditGrid(this, txt, conn, pos, size, data);
        frames.Append(eg);
        eg->Go();
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
        wxLogError(wxT("There is nothing in the SQL pane to save!"));
        return;
    }

    wxFileDialog filename(this, wxT("Select output file"), wxT(""), wxT(""), wxT("SQL Scripts (*.sql)|*.sql|All files (*.*)|*.*"));
    filename.SetStyle(wxSAVE | wxOVERWRITE_PROMPT);

    // Show the dialogue
    if (filename.ShowModal() == wxID_OK) {

        // Write the file
        wxFile *file = new wxFile(filename.GetPath(), wxFile::write);
        if (!file->Write(sqlPane->GetText()))
            wxLogError(wxT("Failed to write to the output file: %s"), filename.GetPath().c_str());

        file->Close();
        return;

    } else {
        wxLogInfo(wxT("User cancelled"));
        return;
    }

}

void frmMain::OnShowSystemObjects(wxCommandEvent& event)
{
    // Warn the user
    if (wxMessageBox(wxT("Changing the 'Show System Objects' option will cause all connections to be closed, and the treeview to be rebuilt.\n\nAre you sure you wish to continue?"),
                     wxT("Continue?"), wxYES_NO | wxICON_QUESTION) == wxNO) {
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
    // Create a server object and connec it.
    pgServer *server = new pgServer(settings->GetLastServer(), settings->GetLastDatabase(), settings->GetLastUsername(), settings->GetLastPort());
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
        wxLogError(wxT("Error connecting to the server: %s"), server->GetLastError().c_str());

        delete server;
        OnAddServer(ev);

    } else {
        wxLogInfo(wxT("pgServer object didn't initialise because the user aborted."));
        delete server;
    }

    // Reset the Servers node text
    wxString label;
    label.Printf(wxT("Servers (%d)"), browser->GetChildrenCount(servers, FALSE));
    browser->SetItemText(servers, label);
    StoreServers();
}


void frmMain::OnPropSelChanged(wxListEvent& event)
{
    wxTreeItemId item=browser->GetSelection();
    pgObject *data=(pgObject*)browser->GetItemData(item);
    if (data)
        data->SetSql(browser, sqlPane, event.GetIndex());
}


void frmMain::OnTreeSelChanged(wxTreeEvent& event)
{
    denyCollapseItem=wxTreeItemId();
	// Reset the listviews/SQL pane
    properties->ClearAll();
    properties->InsertColumn(0, wxT("Properties"), wxLIST_FORMAT_LEFT, 500);
    properties->InsertItem(0, wxT("No properties are available for the current selection"), PGICON_PROPERTY);
    statistics->ClearAll();
    statistics->InsertColumn(0, wxT("Statistics"), wxLIST_FORMAT_LEFT, 500);
    statistics->InsertItem(0, wxT("No statistics are available for the current selection"), PGICON_STATISTICS);
    sqlPane->Clear();

    // Reset the toolbar & password menu options
	// Handle the menus associated with the buttons
    SetButtons(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);

    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    wxTreeItemId item = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

    // If we didn't get an object, then we may have a right click, or 
    // invalid click, so ignore.
    if (!data) return;

    int type = data->GetType();
    pgServer *server;

    properties->Freeze();
    statistics->Freeze();
    switch (type)
    {
        case PG_SERVER:
            StartMsg(wxT("Retrieving server properties"));

            server = (pgServer *)data;
            if (!server->GetConnected())
            {
                toolsMenu->Enable(MNU_CONNECT, TRUE);
                treeContextMenu->Enable(MNU_CONNECT, TRUE);               
            }
			else
            {
                toolsMenu->Enable(MNU_DISCONNECT, TRUE);
                treeContextMenu->Enable(MNU_DISCONNECT, TRUE);               
            }
            data->ShowTree(this, browser, properties, statistics, sqlPane);
            EndMsg();
            break;

        case PG_DATABASES:
        case PG_GROUPS:
        case PG_USERS:
        case PG_DATABASE:
        case PG_GROUP:
        case PG_USER:
        case PG_LANGUAGES:
        case PG_LANGUAGE:
        case PG_SCHEMAS:
        case PG_SCHEMA:
        case PG_AGGREGATES:
        case PG_AGGREGATE:
        case PG_CASTS:
        case PG_CAST:
        case PG_CONVERSIONS:
        case PG_CONVERSION:
        case PG_DOMAINS:
        case PG_DOMAIN:
        case PG_FUNCTIONS:
        case PG_FUNCTION:
        case PG_TRIGGERFUNCTIONS:
        case PG_TRIGGERFUNCTION:
        case PG_OPERATORS:
        case PG_OPERATOR:
        case PG_OPERATORCLASSES:
        case PG_OPERATORCLASS:
        case PG_SEQUENCES:
        case PG_SEQUENCE:
        case PG_TABLES:
        case PG_TABLE:
        case PG_TYPES:
        case PG_TYPE:
        case PG_VIEWS:
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
            data->ShowTree(this, browser, properties, statistics, sqlPane);
            break;
        default:        
			break;;
    }
    properties->Thaw();
    statistics->Thaw();
    sqlPane->SetText(data->GetSql(browser));
}


void frmMain::OnConnect(wxCommandEvent &ev)
{
    wxTreeItemId item = browser->GetSelection();

    pgServer *server = (pgServer *)browser->GetItemData(item);
    if (server->GetType() == PG_SERVER && !server->GetConnected())
        ReconnectServer(server);
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
        case PG_ADD_SERVER:
            OnAddServer(nullEvent);
            break;

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
            if (data->CanEdit())
            {
                OnProperties(nullEvent);
                event.Skip();
                return;
            }
            break;
    }

#ifndef __WXMSW__
    browser->Expand(item);
#endif
}

////////////////////////////////////////////////////////////////////////////////
// This handler will display a popup menu for the item
////////////////////////////////////////////////////////////////////////////////
void frmMain::OnSelRightClick(wxTreeEvent& event)
{   
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


void frmMain::OnDrop(wxCommandEvent &ev)
{
    // This handler will primarily deal with dropping items

    // Get the item data, and feed it to the relevant handler,
    wxTreeItemId item = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

    if (data->GetSystemObject())
    {
        wxMessageDialog msg(this, wxT("Cannot drop system ") + data->GetTypeName() 
            + wxT(" ") + data->GetFullIdentifier(), wxT("Trying to drop system object"), wxICON_EXCLAMATION);
        msg.ShowModal();
        return;
    }

    if (data->RequireDropConfirm() || settings->GetConfirmDelete())
    {
        wxMessageDialog msg(this, wxT("Are you sure you wish to drop ") + data->GetTypeName() 
                        + wxT(" ") + data->GetFullIdentifier() + wxT("?"),
            wxT("Drop ") + data->GetTypeName() + wxT("?"), wxYES_NO | wxICON_QUESTION);
        if (msg.ShowModal() != wxID_YES)
        {
            return;
        }
    }
    bool done=data->DropObject(this, browser);

    if (done)
    {
        wxLogInfo(wxT("Dropping %s %s"), data->GetTypeName().c_str(), data->GetIdentifier().c_str());

        wxTreeItemId parentItem=browser->GetItemParent(item);

        wxTreeItemId nextItem=browser->GetNextVisible(item);
        if (nextItem)
        {
            pgObject *nextData=(pgObject*)browser->GetItemData(nextItem);
            if (nextData && nextData->GetType() == data->GetType())
                browser->SelectItem(nextItem);
            else
            {
                browser->SelectItem(browser->GetPrevVisible(item));
            }
        }
        else
            browser->SelectItem(browser->GetPrevVisible(item));

        browser->Delete(item);

        pgCollection *collection=0;

        while (parentItem)
        {
            collection = (pgCollection*)browser->GetItemData(parentItem);
            if (collection && collection->IsCollection() && collection->IsCollectionForType(data->GetType()))
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

    long cookie;
    wxTreeItemId currentItem = browser->GetSelection();
    pgObject *data;

    wxTreeItemId item;
    
    browser->Freeze();

    while ((item=browser->GetFirstChild(currentItem, cookie)) != 0)
    {
        data = (pgObject *)browser->GetItemData(item);
        wxLogInfo(wxT("Deleting ") + data->GetTypeName() + wxT(" ") 
            + data->GetQuotedFullIdentifier() + wxT(" for Refresh"));
        // delete data will be performed by browser->Delete
        browser->Delete(item);
    }

	// refresh information about the object

    data = (pgObject *)browser->GetItemData(currentItem);
    data->SetDirty();
    
    pgObject *newData = data->Refresh(browser, currentItem);

    if (newData != data)
    {
        wxLogInfo(wxT("Deleting ") + data->GetTypeName() + wxT(" ") 
            + data->GetQuotedFullIdentifier() + wxT(" for Refresh"));

        browser->SetItemData(currentItem, 0);
        delete data;

        if (newData)
        {
            wxLogInfo(wxT("Replacing with new Node ") + newData->GetTypeName() + wxT(" ") 
                + newData->GetQuotedFullIdentifier() + wxT(" for Refresh"));
            newData->SetId(currentItem);    // not done automatically
            browser->SetItemData(currentItem, newData);
        }
        else
        {
            wxLogInfo(wxT("No object to replace: vanished after refresh."));
            browser->Delete(currentItem);
            return;
        }
    }
    wxTreeEvent event;
	OnTreeSelChanged(event);
    browser->Thaw();
}

void frmMain::OnDisconnect(wxCommandEvent &ev)
{
    // Disconnect -- does nothing yet
	int res = wxMessageBox("This is not yet implemented" );
}



void frmMain::OnCreate(wxCommandEvent &ev)
{
    wxTreeItemId item = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

    if (data)
    {
        pgConn *conn=data->GetConnection();
        if (!conn)
            return;

        dlgProperty::CreateObjectDialog(this, browser, properties, data, conn);
    }
}



void frmMain::OnProperties(wxCommandEvent &ev)
{
    wxTreeItemId item = browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

    if (data)
    {
        pgConn *conn=data->GetConnection();
        if (!conn)
            return;

        dlgProperty::EditObjectDialog(this, browser, properties, statistics, sqlPane, data, conn);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void frmMain::OnQueryBuilder(wxCommandEvent &ev)
{

    wxTreeItemId item=browser->GetSelection();
    pgObject *data = (pgObject *)browser->GetItemData(item);

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
