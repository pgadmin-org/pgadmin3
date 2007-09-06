//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// events.cpp - Event handlers for frmMain
//
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/stc/stc.h>
#include <wx/busyinfo.h>
#include <wx/aui/aui.h>

// App headers
#include "utils/misc.h"
#include "frm/menu.h"
#include "frm/frmMain.h"
#include "frm/frmOptions.h"
#include "ctl/ctlSQLBox.h"
#include "db/pgConn.h"
#include "schema/pgDatabase.h"
#include "db/pgSet.h"
#include "schema/pgServer.h"
#include "schema/pgObject.h"
#include "schema/pgCollection.h"
#include "schema/pgTable.h"
#include "dlg/dlgProperty.h"

extern wxString loadPath;


// Event table
BEGIN_EVENT_TABLE(frmMain, pgFrame)
    EVT_ERASE_BACKGROUND(                   frmMain::OnEraseBackground)
    EVT_SIZE(                               frmMain::OnSize)
    EVT_MENU(MNU_ACTION,                    frmMain::OnAction)

    EVT_MENU(MNU_COPY,						frmMain::OnCopy)
    EVT_MENU(MNU_DELETE,                    frmMain::OnDelete)
    EVT_MENU(MNU_SAVEDEFINITION,            frmMain::OnSaveDefinition)
    EVT_MENU(MNU_SQLPANE,                   frmMain::OnToggleSqlPane)
    EVT_MENU(MNU_OBJECTBROWSER,             frmMain::OnToggleObjectBrowser)
    EVT_MENU(MNU_TOOLBAR,                   frmMain::OnToggleToolBar)
    EVT_MENU(MNU_DEFAULTVIEW,               frmMain::OnDefaultView)
    EVT_MENU(MNU_CHECKALIVE,                frmMain::OnCheckAlive)
    EVT_MENU(MNU_CONTEXTMENU,               frmMain::OnContextMenu) 

    EVT_NOTEBOOK_PAGE_CHANGED(CTL_NOTEBOOK, frmMain::OnPageChange)
    EVT_LIST_ITEM_SELECTED(CTL_PROPVIEW,    frmMain::OnPropSelChanged)
    EVT_LIST_ITEM_ACTIVATED(CTL_PROPVIEW,   frmMain::OnPropSelActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(CTL_PROPVIEW, frmMain::OnPropRightClick)
    EVT_TREE_SEL_CHANGED(CTL_BROWSER,       frmMain::OnTreeSelChanged)
    EVT_TREE_ITEM_EXPANDING(CTL_BROWSER,    frmMain::OnExpand)
    EVT_TREE_ITEM_COLLAPSING(CTL_BROWSER,   frmMain::OnCollapse)
    EVT_TREE_ITEM_ACTIVATED(CTL_BROWSER,    frmMain::OnSelActivated)
    EVT_TREE_ITEM_RIGHT_CLICK(CTL_BROWSER,  frmMain::OnSelRightClick) 
    EVT_STC_UPDATEUI(CTL_SQLPANE,           frmMain::OnPositionStc)
    EVT_CLOSE(                              frmMain::OnClose)

    EVT_AUI_PANE_CLOSE(                     frmMain::OnAuiUpdate)

#ifdef __WXGTK__
    EVT_TREE_KEY_DOWN(CTL_BROWSER,          frmMain::OnTreeKeyDown)
#endif
END_EVENT_TABLE()

void frmMain::OnEraseBackground(wxEraseEvent& event)
{
    event.Skip();
}

void frmMain::OnSize(wxSizeEvent& event)
{
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
	    Refresh(currentObject);
	    break;
	case WXK_DELETE:
	    OnDelete(event);
	    break;
	default:
	    event.Skip();
	    break;
    }
}

void frmMain::OnExit(wxCommandEvent& event)
{
    Close(false);   // Allow sub windows to stop us
    event.Skip();
}



void frmMain::OnClose(wxCloseEvent& event)
{
    wxWindow *fr;
    windowList::Node *node;
    while ((node=frames.GetFirst()) != NULL)
    {
        fr=node->GetData();
        
        // if crashes occur here when closing the app,
        // some actionFactory::StartDialog returned a wxWindow* (which is registered in frames)
        // without code to handle OnClose (esp. removing itself with RemoveFrame)

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


void frmMain::OnAction(wxCommandEvent &ev)
{
    actionFactory *af=menuFactories->GetFactory(ev.GetId());
    if (af)
    {
        wxWindow *wnd=af->StartDialog(this, currentObject);
        if (wnd)
            AddFrame(wnd);
    }
}

wxString frmMain::GetHelpPage() const
{
    wxString page;

    if (currentObject)
        page=currentObject->GetHelpPage(true);

    if (page.IsEmpty())
        page = wxT("pg/sql-commands");

    return page;
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


void frmMain::OnCheckAlive(wxCommandEvent &event)
{
    CheckAlive();
}



void frmMain::OnPropSelChanged(wxListEvent& event)
{
    if (properties->GetSelectedItemCount() == 1)
    {
        wxTreeItemId item=browser->GetSelection();
        if (item)
        {
            pgObject *data=browser->GetObject(item);
            if (data && data->IsCollection())
            {
                currentObject=((pgCollection*)data)->FindChild(browser, event.GetIndex());
                if (currentObject)
                {
                    setDisplay(currentObject);
                    sqlPane->SetReadOnly(false);
                    sqlPane->SetText(currentObject->GetSql(browser));
                    sqlPane->SetReadOnly(true);
                }
            }
        }
    }
}


void frmMain::OnPropSelActivated(wxListEvent& event)
{
    if (propFactory->CheckEnable(currentObject))
        propFactory->StartDialog(this, currentObject);
}


void frmMain::OnPropRightClick(wxListEvent& event)
{
    OnPropSelChanged(event);

    if (currentObject)
        doPopup(properties, event.GetPoint(), currentObject);
}


void frmMain::OnTreeSelChanged(wxTreeEvent& event)
{
    denyCollapseItem=wxTreeItemId();
	// Reset the listviews/SQL pane
    if (event.GetItem())
        execSelChange(event.GetItem(), true);
}


void frmMain::execSelChange(wxTreeItemId item, bool currentNode)
{
    if (currentNode)
    {
        properties->ClearAll();
        properties->AddColumn(_("Properties"), 500);
        properties->InsertItem(0, _("No properties are available for the current selection"), PGICON_PROPERTY);

        sqlPane->Clear();
    }

    // Get the item data, and feed it to the relevant handler,
    // cast as required.
    currentObject = browser->GetObject(item);

    // If we didn't get an object, then we may have a right click, or 
    // invalid click, so ignore.
    if (!currentObject) return;

    if (currentNode)
    {
        properties->Freeze();
        setDisplay(currentObject, properties, sqlPane);
        properties->Thaw();
        ShowObjStatistics(currentObject, listViews->GetSelection());
    }
    else
        setDisplay(currentObject, 0, 0);
}


void frmMain::setDisplay(pgObject *data, ctlListView *props, ctlSQLBox *sqlbox)
{
    browser->RemoveDummyChild(data);

    pgServer *server=0;


    bool showTree;

    pgaFactory *factory=data->GetFactory();
    if (factory)
    {
        if (factory == &serverFactory)
        {
            server = (pgServer *)data;

            data->ShowTree(this, browser, props, sqlbox);
            showTree=false;
        }
        else
            showTree=true;
    }
    else
        showTree=false;

    if (showTree)
        data->ShowTree(this, browser, props, sqlbox);

    if (sqlbox)
    {
        sqlbox->SetReadOnly(false);
        sqlbox->SetText(data->GetSql(browser));
        sqlbox->SetReadOnly(true);
    }

    pgConn *conn=data->GetConnection();
    if (conn && (conn->GetStatus() == PGCONN_BROKEN || conn->GetStatus() == PGCONN_BAD))
    {
        CheckAlive();
        return;
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

    editMenu->Enable(newMenuFactory->GetId(), false);

    wxMenu *indivMenu=data->GetNewMenu();
    if (indivMenu)
    {
        if (indivMenu->GetMenuItemCount())
        {
            editMenu->Enable(newMenuFactory->GetId(), true);

            for (i=0 ; i < indivMenu->GetMenuItemCount() ; i++)
            {
                menuItem=indivMenu->GetMenuItems().Item(i)->GetData();
                newMenu->Append(menuItem->GetId(), menuItem->GetLabel(), menuItem->GetHelp());
                newContextMenu->Append(menuItem->GetId(), menuItem->GetLabel(), menuItem->GetHelp());
            }
        }
        delete indivMenu;
    }

    menuFactories->CheckMenu(data, menuBar, toolBar);

    menuFactories->EnableSubmenu(menuBar, MNU_CONFIGSUBMENU);
    menuFactories->EnableSubmenu(menuBar, MNU_SLONY_SUBMENU);
}


void frmMain::OnSelActivated(wxTreeEvent &event)
{
    // This handler will primarily deal with displaying item
    // properties in seperate windows and 'Add xxx...' clicks

    // Get the item data, and feed it to the relevant handler,
    // cast as required.

    wxTreeItemId item = event.GetItem();
    pgObject *data = browser->GetObject(item);
    if (!data)
        return;
    pgServer *server;
    wxCommandEvent nullEvent;

    if (data->IsCreatedBy(serverFactory))
    {
        server = (pgServer *)data;
        if (!server->GetConnected())
        {
            if (ReconnectServer(server) == PGCONN_OK)
            {
                // prevent from being collapsed immediately

                denyCollapseItem=item;
            }
        }
    }
    else
    {
        if (settings->GetDoubleClickProperties() && propFactory->CheckEnable(data))
        {
            propFactory->StartDialog(this, data);
            event.Skip();
            return;
        }
    }

#ifndef __WXMSW__
    browser->Expand(item);
#endif
}

void frmMain::doPopup(wxWindow *win, wxPoint point, pgObject *object)
{
    if (treeContextMenu)
        delete treeContextMenu;

    treeContextMenu = new wxMenu();

    menuFactories->AppendEnabledMenus(menuBar, treeContextMenu);

    wxMenuItem *newItem=treeContextMenu->FindItem(newMenuFactory->GetId());

    if (newItem)
    {
        size_t newItemPos;

        wxMenuItemList mil = treeContextMenu->GetMenuItems();
        for (newItemPos=0 ; newItemPos < mil.GetCount() ; newItemPos++)
        {
            if (mil.Item(newItemPos)->GetData()->GetId() == newItem->GetId())
                break;
        }

        if (object)
        {
            wxMenu *indivMenu=object->GetNewMenu();
            if (indivMenu)
            {
                if (indivMenu->GetMenuItemCount() > 1)
                {
                    wxMenuItem *menuItem = menuBar->FindItem(newMenuFactory->GetId());
                    treeContextMenu->Insert(newItemPos, newMenuFactory->GetId(), menuItem->GetLabel(), indivMenu, menuItem->GetHelp());
                }
                else
                {
                    if (indivMenu->GetMenuItemCount() == 1)
                    {
                        wxMenuItem *menuItem=indivMenu->GetMenuItems().Item(0)->GetData();
                        treeContextMenu->Insert(newItemPos, menuItem->GetId(), menuItem->GetLabel(), menuItem->GetHelp());
                    }
                    delete indivMenu;
                }
            }
        }
        treeContextMenu->Remove(newItem);
        delete newItem;
    }

    if (treeContextMenu->GetMenuItemCount())
        win->PopupMenu(treeContextMenu, point);
}

////////////////////////////////////////////////////////////////////////////////
// This handler will display a popup menu for the currently selected item
////////////////////////////////////////////////////////////////////////////////
void frmMain::OnContextMenu(wxCommandEvent& event)
{
    wxPoint point;

    if (FindFocus() == browser)
    {
        wxRect rect;
        wxTreeItemId item=browser->GetSelection();

        browser->GetBoundingRect(item, rect);
        point = rect.GetPosition();
	    wxPoint origin = GetClientAreaOrigin();

	    // Because this Tree is inside a vertical splitter, we
	    // must compensate for the size of the other elements
	    point.x += origin.x;
	    point.y += origin.y;

        doPopup(this, point, browser->GetObject(item));
    }

}


////////////////////////////////////////////////////////////////////////////////
// This handler will display a popup menu for the item at the mouse position
////////////////////////////////////////////////////////////////////////////////
void frmMain::OnSelRightClick(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if (item != browser->GetSelection())
    {
        browser->SelectItem(item);
        currentObject = browser->GetObject(item);
    }

    if (currentObject)
        doPopup(browser, event.GetPoint(), currentObject);
}


void frmMain::OnDelete(wxCommandEvent &ev)
{
    ExecDrop(false);
}


void frmMain::ExecDrop(bool cascaded)
{
    wxTreeItemId item=browser->GetSelection();
    pgCollection *collection = (pgCollection*)browser->GetObject(item);

    // Get any table object for later update
    wxTreeItemId tblitem;
    pgObject *node = (pgObject*)browser->GetObject(item);

    if (node->GetMetaType() == PGM_COLUMN || 
        node->GetMetaType() == PGM_CONSTRAINT ||
        node->GetMetaType() == PGM_FOREIGNKEY ||
        node->GetMetaType() == PGM_INDEX ||
        node->GetMetaType() == PGM_TRIGGER ||
        node->GetMetaType() == PGM_PRIMARYKEY ||
        node->GetMetaType() == PGM_CHECK ||
        node->GetMetaType() == PGM_UNIQUE)
        tblitem=node->GetTable()->GetId();

    if (collection == currentObject)
        dropSingleObject(currentObject, true, cascaded);
    else
    {
        if (collection && collection->IsCollection())
        {
            long index=properties->GetFirstSelected();

            if (index >= 0)
            {
                pgObject *data=collection->FindChild(browser, index);

                if (!data || !data->CanDrop())
                    return;

                if (properties->GetSelectedItemCount() == 1)
                {
                    dropSingleObject(data, true, cascaded);
                }
                else
                {
                    if (cascaded || data->RequireDropConfirm() || settings->GetConfirmDelete())
                    {
                        wxString text, caption;
                        if (cascaded)
                        {
                            text = _("Are you sure you wish to drop multiple objects including all objects that depend on them?");
                            caption = _("Drop multiple objects cascaded?");
                        }
                        else
                        {
                            text = _("Are you sure you wish to drop multiple objects?");
                            caption = _("Drop multiple objects?");
                        }
                        wxMessageDialog msg(this, text, caption, wxYES_NO | wxICON_QUESTION);
                        if (msg.ShowModal() != wxID_YES)
                        {
                            return;
                        }
                    }

                    bool done=true;
                    long count=0;
                    while (done && data && index >= 0)
                    {
                        if (data->GetSystemObject())
                        {
                            wxMessageDialog msg(this, wxString::Format(_("Cannot drop system %s"), 
                                data->GetTranslatedTypeName().c_str(), ""), 
                                _("Trying to drop system object"), wxICON_EXCLAMATION);
                            msg.ShowModal();
                            return;
                        }

                        done = dropSingleObject(data, false, cascaded);

                        if (done)
                        {
                            properties->DeleteItem(index);
                            count++;
                            index = properties->GetFirstSelected();

                            if (index >= 0)
                                data=collection->FindChild(browser, index);
                        }
                    }
                }
                Refresh(collection);
            }
        }
    }

    // If the collection has a table, refresh that as well.
    if (tblitem)
        Refresh(browser->GetObject(tblitem));
}


bool frmMain::dropSingleObject(pgObject *data, bool updateFinal, bool cascaded)
{
    if (updateFinal)
    {
        // accelerator can bypass disabled menu, so we need to check
        if (!data || !data->CanDrop())
            return false;

        if (data->GetSystemObject())
        {
            wxMessageDialog msg(this, wxString::Format(_("Cannot drop system %s %s."), 
                data->GetTranslatedTypeName().c_str(), data->GetFullIdentifier().c_str()), 
                _("Trying to drop system object"), wxICON_EXCLAMATION);
            msg.ShowModal();
            return false;
        }

        if (cascaded || data->RequireDropConfirm() || settings->GetConfirmDelete())
        {
            wxString text, caption;
            if (cascaded)
            {
                text = wxString::Format(_("Are you sure you wish to drop %s %s including all objects that depend on it?"),
                    data->GetTranslatedTypeName().c_str(), data->GetFullIdentifier().c_str());
                caption = wxString::Format(_("Drop %s cascaded?"), data->GetTranslatedTypeName().c_str());
            }
            else
            {
                text = wxString::Format(_("Are you sure you wish to drop %s %s?"),
                    data->GetTranslatedTypeName().c_str(), data->GetFullIdentifier().c_str());
                caption = wxString::Format(_("Drop %s?"), data->GetTranslatedTypeName().c_str());
            }
            wxMessageDialog msg(this, text, caption, wxYES_NO | wxICON_QUESTION);
            if (msg.ShowModal() != wxID_YES)
            {
                return false;
            }
        }
    }
    bool done=data->DropObject(this, browser, cascaded);

    if (done)
    {
        wxLogInfo(wxT("Dropping %s %s"), data->GetTypeName().c_str(), data->GetIdentifier().c_str());

        wxTreeItemId parentItem=browser->GetItemParent(data->GetId());

        if (updateFinal)
        {
            wxTreeItemId nextItem;
            if (browser->IsVisible(data->GetId()))
                nextItem=browser->GetNextVisible(data->GetId());

            if (nextItem)
            {
                pgObject *nextData=browser->GetObject(nextItem);
                if (!nextData || nextData->GetType() != data->GetType())
                    nextItem=browser->GetPrevSibling(data->GetId());
            }
            else
                nextItem=browser->GetPrevSibling(data->GetId());

            if (nextItem)
                browser->SelectItem(nextItem);
        }
        pgaFactory *droppedCollFactory = data->GetFactory()->GetCollectionFactory();
        browser->Delete(data->GetId());
        // data is invalid now


        if (updateFinal)
        {
            pgCollection *collection=0;

            while (parentItem)
            {
                collection = (pgCollection*)browser->GetObject(parentItem);
                if (collection && collection->IsCollection() && collection->GetFactory() == droppedCollFactory)
                {
                    collection->UpdateChildCount(browser);
                    break;
                }
                parentItem=browser->GetItemParent(parentItem);
            }
        }
    }
    return done;
}


void frmMain::OnNew(wxCommandEvent &ev)
{
    pgaFactory *factory=pgaFactory::GetFactory(ev.GetId() - MNU_NEW);
    
    if (factory == &serverFactory)
    {
        if (currentObject && currentObject->IsCreatedBy(serverFactory))
        {
            pgServer *server=(pgServer*)currentObject;
            if (!server->GetConnected())
                ReconnectServer(server);
        }
        return;
    }

    if (currentObject)
    {
        if (!dlgProperty::CreateObjectDialog(this, currentObject, factory))
            CheckAlive();
    }
}


void frmMain::OnSaveDefinition(wxCommandEvent& event)
{

    wxLogInfo(wxT("Saving object definition"));

    if (sqlPane->GetText().IsNull()) {
        wxLogError(__("There is nothing in the SQL pane to save!"));
        return;
    }

    wxString file;
    settings->Read(wxT("frmMain/LastFile"), &file, wxEmptyString);

    wxFileDialog filename(this, _("Select output file"), ::wxPathOnly(file), file, _("SQL Scripts (*.sql)|*.sql|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    // Show the dialogue
    if (filename.ShowModal() == wxID_OK)
    {
        // Write the file
        if (!FileWrite(filename.GetPath(), sqlPane->GetText()))
            wxLogError(__("Could not write the file %s: Errcode=%d."), filename.GetPath().c_str(), wxSysErrorCode());
    }
    else
    {
        wxLogInfo(wxT("User cancelled"));
    }

    settings->Write(wxT("frmMain/LastFile"), filename.GetPath());
}

void frmMain::OnToggleSqlPane(wxCommandEvent& event)
{
    if (viewMenu->IsChecked(MNU_SQLPANE))
        manager.GetPane(wxT("sqlPane")).Show(true);
    else
        manager.GetPane(wxT("sqlPane")).Show(false);
    manager.Update();
}

void frmMain::OnToggleObjectBrowser(wxCommandEvent& event)
{
    if (viewMenu->IsChecked(MNU_OBJECTBROWSER))
        manager.GetPane(wxT("objectBrowser")).Show(true);
    else
        manager.GetPane(wxT("objectBrowser")).Show(false);
    manager.Update();
}

void frmMain::OnToggleToolBar(wxCommandEvent& event)
{
    if (viewMenu->IsChecked(MNU_TOOLBAR))
        manager.GetPane(wxT("toolBar")).Show(true);
    else
        manager.GetPane(wxT("toolBar")).Show(false);
    manager.Update();
}

void frmMain::OnAuiUpdate(wxAuiManagerEvent& event)
{
    if(event.pane->name == wxT("objectBrowser"))
    {
        viewMenu->Check(MNU_OBJECTBROWSER, false);
    }
    else if(event.pane->name == wxT("sqlPane"))
    {
        viewMenu->Check(MNU_SQLPANE, false);
    }
    else if(event.pane->name == wxT("toolBar"))
    {
        viewMenu->Check(MNU_TOOLBAR, false);
    }
    event.Skip();
}

void frmMain::OnDefaultView(wxCommandEvent& event)
{
    manager.LoadPerspective(FRMMAIN_DEFAULT_PERSPECTIVE, true);

    // Reset the captions for the current language
    manager.GetPane(wxT("objectBrowser")).Caption(_("Object browser"));
    manager.GetPane(wxT("listViews")).Caption(_("Info pane"));
    manager.GetPane(wxT("sqlPane")).Caption(_("SQL pane"));
    manager.GetPane(wxT("toolBar")).Caption(_("Tool bar"));

    // tell the manager to "commit" all the changes just made
    manager.Update();

    // Sync the View menu options
    viewMenu->Check(MNU_SQLPANE, manager.GetPane(wxT("sqlPane")).IsShown());
    viewMenu->Check(MNU_OBJECTBROWSER, manager.GetPane(wxT("objectBrowser")).IsShown());
    viewMenu->Check(MNU_TOOLBAR, manager.GetPane(wxT("toolBar")).IsShown());
}

void frmMain::OnPositionStc(wxStyledTextEvent& event)
{
    if (sqlPane->GetSelectedText().IsNull())
        editMenu->Enable(MNU_COPY, false);
	else
        editMenu->Enable(MNU_COPY, true);
}
