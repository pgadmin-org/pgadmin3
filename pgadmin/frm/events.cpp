//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
#include "ctl/ctlMenuToolbar.h"
#include "db/pgConn.h"
#include "schema/pgDatabase.h"
#include "db/pgSet.h"
#include "schema/pgServer.h"
#include "schema/pgObject.h"
#include "schema/pgCollection.h"
#include "schema/pgTable.h"
#include "schema/edbPrivateSynonym.h"
#include "dlg/dlgProperty.h"

// Mutex to protect the "currentObject" from race conditions.
//
static wxMutex s_currentObjectMutex;

// Event table
BEGIN_EVENT_TABLE(frmMain, pgFrame)
	EVT_CHILD_FOCUS(			frmMain::OnChildFocus)
	EVT_ERASE_BACKGROUND(                   frmMain::OnEraseBackground)
	EVT_SIZE(                               frmMain::OnSize)
	EVT_MENU(MNU_ACTION,                    frmMain::OnAction)

	EVT_MENU(MNU_COPY,			frmMain::OnCopy)
	EVT_MENU(MNU_DELETE,                    frmMain::OnDelete)
	EVT_MENU(MNU_SAVEDEFINITION,            frmMain::OnSaveDefinition)
	EVT_MENU(MNU_SQLPANE,                   frmMain::OnToggleSqlPane)
	EVT_MENU(MNU_OBJECTBROWSER,             frmMain::OnToggleObjectBrowser)
	EVT_MENU(MNU_TOOLBAR,                   frmMain::OnToggleToolBar)
	EVT_MENU(MNU_DEFAULTVIEW,               frmMain::OnDefaultView)
	EVT_MENU(MNU_CHECKALIVE,                frmMain::OnCheckAlive)
	EVT_MENU(MNU_CONTEXTMENU,               frmMain::OnContextMenu)

	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY,	frmMain::OnPageChange)
	EVT_LIST_ITEM_SELECTED(CTL_PROPVIEW,    frmMain::OnPropSelChanged)
	EVT_LIST_ITEM_ACTIVATED(CTL_PROPVIEW,   frmMain::OnPropSelActivated)
	EVT_LIST_ITEM_RIGHT_CLICK(CTL_PROPVIEW, frmMain::OnPropRightClick)
	EVT_LIST_ITEM_SELECTED(CTL_STATVIEW,    frmMain::OnSelectItem)
	EVT_LIST_ITEM_SELECTED(CTL_DEPVIEW,     frmMain::OnSelectItem)
	EVT_LIST_ITEM_SELECTED(CTL_REFVIEW,     frmMain::OnSelectItem)
	EVT_TREE_SEL_CHANGED(CTL_BROWSER,       frmMain::OnTreeSelChanged)
	EVT_TREE_ITEM_EXPANDING(CTL_BROWSER,    frmMain::OnExpand)
	EVT_TREE_ITEM_COLLAPSING(CTL_BROWSER,   frmMain::OnCollapse)
	EVT_TREE_ITEM_ACTIVATED(CTL_BROWSER,    frmMain::OnSelActivated)
	EVT_TREE_ITEM_RIGHT_CLICK(CTL_BROWSER,  frmMain::OnSelRightClick)
	EVT_STC_UPDATEUI(CTL_SQLPANE,           frmMain::OnPositionStc)
	EVT_CLOSE(                              frmMain::OnClose)

	EVT_AUI_PANE_CLOSE(                     frmMain::OnAuiUpdate)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY,    frmMain::OnAuiNotebookPageClose)

#ifdef __WXGTK__
	EVT_TREE_KEY_DOWN(CTL_BROWSER,          frmMain::OnTreeKeyDown)
#endif

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
	EVT_COMMAND (wxID_ANY, SSH_TUNNEL_ERROR_EVENT, frmMain::OnSSHTunnelEvent)
#endif

END_EVENT_TABLE()

void frmMain::OnChildFocus(wxChildFocusEvent &event)
{
	// Grab the focussed control and stash it away for later use
	currentControl = dynamic_cast<wxControl *>(event.GetEventObject());

	// Setup the menu controls according to the control that's selected
	// and it's status.

	// Defaults.
	editMenu->Enable(MNU_COPY, false);

	// ctlSQLBox?
	ctlSQLBox *sb = dynamic_cast<ctlSQLBox *>(event.GetEventObject());
	if (sb)
	{
		// Copy
		editMenu->Enable(MNU_COPY, !sb->GetSelectedText().IsEmpty());
	}

	// Listview?
	ctlListView *lv = dynamic_cast<ctlListView *>(event.GetEventObject());
	if (lv)
	{
		// Copy
		editMenu->Enable(MNU_COPY, lv->GetSelectedItemCount() > 0);
	}
}

void frmMain::OnEraseBackground(wxEraseEvent &event)
{
	event.Skip();
}

void frmMain::OnSize(wxSizeEvent &event)
{
	event.Skip();
}

// unfortunately, under GTK we won't get the original wxKeyEvent
// to reset m_metaDown
void frmMain::OnTreeKeyDown(wxTreeEvent &event)
{
	int keyCode = event.GetKeyCode();
	switch (keyCode)
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
		// Is tempting to write all cases(this handler) in tree control itself
		case WXK_LEFT:
		case WXK_RIGHT:
			browser->NavigateTree(keyCode);
			break;
		default:
			event.Skip();
			break;
	}
}

void frmMain::OnExit(wxCommandEvent &event)
{
	Close(false);   // Allow sub windows to stop us
	event.Skip();
}



void frmMain::OnClose(wxCloseEvent &event)
{
	wxWindow *fr;
	windowList::Node *node;
	while ((node = frames.GetFirst()) != NULL)
	{
		fr = node->GetData();

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


void frmMain::UpdateAllRecentFiles()
{
	wxWindow *fr;
	windowList::Node *node;
	node = frames.GetFirst();
	while (node)
	{
		fr = node->GetData();
		((frmQuery *)fr)->UpdateRecentFiles(false);
		node = node->GetNext();
	}
}


void frmMain::UpdateAllFavouritesList()
{
	wxWindow *fr;
	windowList::Node *node;
	node = frames.GetFirst();
	while (node)
	{
		fr = node->GetData();
		((frmQuery *)fr)->UpdateFavouritesList();
		node = node->GetNext();
	}
}

void frmMain::UpdateAllMacrosList()
{
	wxWindow *fr;
	windowList::Node *node;
	node = frames.GetFirst();
	while (node)
	{
		fr = node->GetData();
		((frmQuery *)fr)->UpdateMacrosList();
		node = node->GetNext();
	}
}


void frmMain::OnAction(wxCommandEvent &ev)
{
	actionFactory *af = menuFactories->GetFactory(ev.GetId());
	if (af)
	{
		wxWindow *wnd = af->StartDialog(this, currentObject);
		if (wnd)
			AddFrame(wnd);
	}
}

wxString frmMain::GetHelpPage() const
{
	wxString page;

	if (currentObject)
		page = currentObject->GetHelpPage(true);

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
	denyCollapseItem = wxTreeItemId();
}


void frmMain::OnExpand(wxTreeEvent &event)
{
	wxCookieType cookie;
	wxTreeItemId item = browser->GetFirstChild(event.GetItem(), cookie);
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



void frmMain::OnPropSelChanged(wxListEvent &event)
{
	if (properties->GetSelectedItemCount() == 1)
	{
		wxTreeItemId item = browser->GetSelection();
		if (item)
		{
			pgObject *data = browser->GetObject(item);
			if (data && data->IsCollection())
			{
				currentObject = ((pgCollection *)data)->FindChild(browser, event.GetIndex());
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

	editMenu->Enable(MNU_COPY, properties->GetSelectedItemCount() > 0);

// The generic list view control on the Mac doesn't fire focus events
// as it should, so we set currentControl here instead of relying on
// the ChildFocusEvent. The native list view does fire the events, but
// does weird things with multi-select items so we currently disable
// it (see the creation of the listviews in frmMain.cpp).
#ifdef __WXMAC__
	currentControl = properties;
#endif
}


void frmMain::OnSelectItem(wxListEvent &event)
{
	ctlListView *list;

	switch(listViews->GetSelection())
	{
		case NBP_STATISTICS:
			list = statistics;
			break;
		case NBP_DEPENDENCIES:
			list = dependencies;
			break;
		case NBP_DEPENDENTS:
			list = dependents;
			break;
		default:
			// This shouldn't happen.
			// If it does, it's no big deal, we just need to get out.
			return;
			break;
	}

	editMenu->Enable(MNU_COPY, list->GetSelectedItemCount() > 0);

// The generic list view control on the Mac doesn't fire focus events
// as it should, so we set currentControl here instead of relying on
// the ChildFocusEvent. The native list view does fire the events, but
// does weird things with multi-select items so we currently disable
// it (see the creation of the listviews in frmMain.cpp).
#ifdef __WXMAC__
	currentControl = list;
#endif
}

void frmMain::OnPropSelActivated(wxListEvent &event)
{
	if (propFactory->CheckEnable(currentObject))
		propFactory->StartDialog(this, currentObject);
}


void frmMain::OnPropRightClick(wxListEvent &event)
{
	OnPropSelChanged(event);

	if (currentObject)
		doPopup(properties, event.GetPoint(), currentObject);
}


void frmMain::OnTreeSelChanged(wxTreeEvent &event)
{
	/*
	    * Do not honour the tree selection change, while a property dialog is
	    * closed and refresh is in progress
	*/
	if (m_refreshing)
		return;

	denyCollapseItem = wxTreeItemId();
	// Reset the listviews/SQL pane
	if (event.GetItem())
		execSelChange(event.GetItem(), true);
}


// Reset the list controls
void frmMain::ResetLists()
{
	properties->ClearAll();
	properties->AddColumn(_("Properties"), properties->GetSize().GetWidth() - 10);
	properties->InsertItem(0, _("No properties are available for the current selection"), PGICON_PROPERTY);
	statistics->ClearAll();
	statistics->AddColumn(_("Statistics"), properties->GetSize().GetWidth() - 10);
	statistics->InsertItem(0, _("No statistics are available for the current selection"), PGICON_PROPERTY);
	dependencies->ClearAll();
	dependencies->AddColumn(_("Dependencies"), properties->GetSize().GetWidth() - 10);
	dependencies->InsertItem(0, _("No dependency information is available for the current selection"), PGICON_PROPERTY);
	dependents->ClearAll();
	dependents->AddColumn(_("Dependents"), properties->GetSize().GetWidth() - 10);
	dependents->InsertItem(0, _("No dependent information is available for the current selection"), PGICON_PROPERTY);
}


void frmMain::execSelChange(wxTreeItemId item, bool currentNode)
{
	static bool refresh = true;

	if (currentNode)
	{
		ResetLists();
		sqlPane->Clear();
	}

	// Get the item data, and feed it to the relevant handler,
	// cast as required.
	//
	// Lock the assignment to prevent the race conditions between onSelRightClick and execSelChange.
	//
	s_currentObjectMutex.Lock();
	currentObject = browser->GetObject(item);
	s_currentObjectMutex.Unlock();

	// If we didn't get an object, then we may have a right click, or
	// invalid click, so ignore.
	if (!currentObject)
	{
		menuFactories->CheckMenu(currentObject, menuBar, toolBar);
	}
	else
	{
		int settingRefreshOnClick = settings->GetRefreshOnClick();

		if (settingRefreshOnClick != REFRESH_OBJECT_NONE
		        && refresh
		        && currentObject->GetTypeName() != wxT("Server")
		        && currentObject->GetTypeName() != wxT("Servers")
		        && currentObject->GetTypeName() != wxT("Database")
		        && !currentObject->IsCollection())
		{
			refresh = false;

			if (settingRefreshOnClick == REFRESH_OBJECT_ONLY )
			{
				// We can not update the schema, because it would cause an update to the entire tree.
				if (currentObject->GetTypeName() != wxT("Schema"))
				{
					wxTreeItemId currentItem = currentObject->GetId();

					// Do not refresh and instead bail out if dialog of the currently selected
					// node or it's child node is open, as refresh would delete this node's object
					// and could cause a crash
					pgObject *obj = NULL;
					if (currentItem)
						obj = browser->GetObject(currentItem);

					if (obj && obj->CheckOpenDialogs(browser, currentItem))
					{
						properties->Freeze();
						setDisplay(currentObject, properties, sqlPane);
						properties->Thaw();
						refresh = true;
						return;
					}

					pgObject *newData = currentObject->Refresh(browser, currentItem);

					if (newData != 0)
					{
						wxLogInfo(wxT("Replacing with new node %s %s for refresh"), newData->GetTypeName().c_str(), newData->GetQuotedFullIdentifier().c_str());

						browser->DeleteChildren(currentItem);
						newData->SetId(currentItem);    // not done automatically
						browser->SetItemData(currentItem, newData);

						// Update the node text if this is an object, as it may have been renamed
						if (!newData->IsCollection())
							browser->SetItemText(currentItem, newData->GetDisplayName());

						delete currentObject;
						currentObject = newData;
					}
					else
					{
						// OK, we failed to refresh, so select the parent and delete the child.
						browser->SelectItem(browser->GetItemParent(currentItem));
						browser->Delete(currentItem);
					}
				}
			}
			else
				Refresh(currentObject);

			refresh = true;
		}


		if (currentNode)
		{
			properties->Freeze();
			setDisplay(currentObject, properties, sqlPane);
			properties->Thaw();
		}
		else
			setDisplay(currentObject, 0, 0);
	}
}


void frmMain::setDisplay(pgObject *data, ctlListView *props, ctlSQLBox *sqlbox)
{
	pgServer *server = 0;


	bool showTree = false;

	pgaFactory *factory = data->GetFactory();
	if (factory)
	{
		if (factory == &serverFactory)
		{
			server = (pgServer *)data;

			data->ShowTree(this, browser, props, sqlbox);
			showTree = false;
		}
		else
			showTree = true;
	}
	else
		showTree = false;

	if (showTree)
		data->ShowTree(this, browser, props, sqlbox);

	if (sqlbox)
	{
		sqlbox->SetReadOnly(false);
		sqlbox->SetText(data->GetSql(browser));
		sqlbox->SetReadOnly(true);
	}

	pgConn *conn = data->GetConnection();
	if (conn && (conn->GetStatus() == PGCONN_BROKEN || conn->GetStatus() == PGCONN_BAD))
	{
		CheckAlive();
		return;
	}

	unsigned int i;
	wxMenuItem *menuItem;
	i = newMenu->GetMenuItemCount();
	while (i--)
	{
		menuItem = newMenu->GetMenuItems().Item(i)->GetData();
		if (menuItem)
			delete newMenu->Remove(menuItem);
	}

	i = newContextMenu->GetMenuItemCount();
	while (i--)
	{
		menuItem = newContextMenu->GetMenuItems().Item(i)->GetData();
		if (menuItem)
			delete newContextMenu->Remove(menuItem);
	}

	editMenu->Enable(newMenuFactory->GetId(), false);

	wxMenu *indivMenu = data->GetNewMenu();
	if (indivMenu)
	{
		if (indivMenu->GetMenuItemCount())
		{
			editMenu->Enable(newMenuFactory->GetId(), true);

			for (i = 0 ; i < indivMenu->GetMenuItemCount() ; i++)
			{
				menuItem = indivMenu->GetMenuItems().Item(i)->GetData();
#if wxCHECK_VERSION(2, 9, 0)
				wxString lab = menuItem->GetItemLabelText();
#else
				wxString lab = menuItem->GetLabel(); // deprecated
#endif

				newMenu->Append(menuItem->GetId(), lab, menuItem->GetHelp());
				newContextMenu->Append(menuItem->GetId(), lab, menuItem->GetHelp());
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

				denyCollapseItem = item;
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

	browser->Expand(item);
}

void frmMain::doPopup(wxWindow *win, wxPoint point, pgObject *object)
{
	if (treeContextMenu)
		delete treeContextMenu;

	treeContextMenu = new wxMenu();

	menuFactories->AppendEnabledMenus(menuBar, treeContextMenu);

	wxMenuItem *newItem = treeContextMenu->FindItem(newMenuFactory->GetId());

	if (newItem)
	{
		size_t newItemPos;

		wxMenuItemList mil = treeContextMenu->GetMenuItems();
		for (newItemPos = 0 ; newItemPos < mil.GetCount() ; newItemPos++)
		{
			if (mil.Item(newItemPos)->GetData()->GetId() == newItem->GetId())
				break;
		}

		if (object)
		{
			wxMenu *indivMenu = object->GetNewMenu();
			if (indivMenu)
			{

				if (indivMenu->GetMenuItemCount() > 1)
				{
					wxMenuItem *menuItem = menuBar->FindItem(newMenuFactory->GetId());
#if wxCHECK_VERSION(2, 9, 0)
					wxString lab = menuItem->GetItemLabelText();
#else
					wxString lab = menuItem->GetLabel(); // deprecated
#endif

					treeContextMenu->Insert(newItemPos, newMenuFactory->GetId(), lab, indivMenu, menuItem->GetHelp());
				}
				else
				{
					if (indivMenu->GetMenuItemCount() == 1)
					{
						wxMenuItem *menuItem = indivMenu->GetMenuItems().Item(0)->GetData();
#if wxCHECK_VERSION(2, 9, 0)
						wxString lab = menuItem->GetItemLabelText();
#else
						wxString lab = menuItem->GetLabel(); // deprecated
#endif
						treeContextMenu->Insert(newItemPos, menuItem->GetId(), lab, menuItem->GetHelp());
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
void frmMain::OnContextMenu(wxCommandEvent &event)
{
	wxPoint point;

	if (FindFocus() == browser)
	{
		wxRect rect;
		wxTreeItemId item = browser->GetSelection();

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
void frmMain::OnSelRightClick(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if (item != browser->GetSelection())
	{
		browser->SelectItem(item);

		// Prevent changes to "currentObject" by "execSelchange" function by another
		// thread. Will hold the lock until we have the actual object in hand.
		s_currentObjectMutex.Lock();
		currentObject = browser->GetObject(item);
		s_currentObjectMutex.Unlock();
	}

	if (currentObject)
		doPopup(browser, event.GetPoint(), currentObject);
}


void frmMain::OnDelete(wxCommandEvent &ev)
{
	if (currentObject->CanDrop())
		ExecDrop(false);
}


void frmMain::ExecDrop(bool cascaded)
{
	wxTreeItemId item = browser->GetSelection();
	pgCollection *collection = (pgCollection *)browser->GetObject(item);

	// Get any table object for later update
	wxTreeItemId owneritem;
	pgObject *node = (pgObject *)browser->GetObject(item);

	int metatype = node->GetMetaType();

	switch (metatype)
	{
		case PGM_COLUMN:
			owneritem = node->GetTable()->GetId();
			break;

		case PGM_CHECK:
		case PGM_CONSTRAINT:
		case PGM_EXCLUDE:
		case PGM_FOREIGNKEY:
		case PGM_INDEX:
		case PGM_PRIMARYKEY:
		case PGM_UNIQUE:
		case PGM_TRIGGER:
		case PGM_RULE: // Rules are technically table objects! Yeuch
		case EDB_PACKAGEFUNCTION:
		case EDB_PACKAGEVARIABLE:
		case PGM_SCHEDULE:
		case PGM_STEP:
			if (node->IsCollection())
				owneritem = browser->GetParentObject(node->GetId())->GetId();
			else
				owneritem = browser->GetParentObject(browser->GetParentObject(node->GetId())->GetId())->GetId();
			break;

		default:
			break;
	}

	// Grab the parent item to re-focus on.
	wxString parent = GetNodePath(item).BeforeLast('/');

	bool success = false;
	if (collection == currentObject)
		success = dropSingleObject(currentObject, true, cascaded);
	else
	{
		if (collection && collection->IsCollection())
		{
			long index = properties->GetFirstSelected();

			if (index >= 0)
			{
				pgObject *data = collection->FindChild(browser, index);

				if (!data || !data->CanDrop())
					return;

				if (properties->GetSelectedItemCount() == 1)
				{
					success = dropSingleObject(data, true, cascaded);
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
						wxMessageDialog msg(this, text, caption, wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT);
						if (msg.ShowModal() != wxID_YES)
						{
							return;
						}
					}

					bool done = true;
					long count = 0;
					while (done && data && index >= 0)
					{
						if (data->GetSystemObject())
						{
							wxMessageDialog msg(this,
							                    data->GetTranslatedMessage(CANNOTDROPSYSTEM),
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
								data = collection->FindChild(browser, index);

							success = true;
						}
					}
				}
			}
		}
	}

	if (success)
	{
		// If the collection has a table, refresh that as well.
		if (owneritem)
		{
			ObjectBrowserRefreshing(true);
			Refresh(browser->GetObject(owneritem));
			ObjectBrowserRefreshing(false);
		}

		// Now re-focus on the parent of the deleted node
		if (!parent.IsEmpty())
			SetCurrentNode(browser->GetRootItem(), parent);
	}
}


bool frmMain::dropSingleObject(pgObject *data, bool updateFinal, bool cascaded)
{
	if (updateFinal)
	{
		// accelerator can bypass disabled menu, so we need to check
		if (!data || !data->CanDrop())
			return false;

		if (data->CheckOpenDialogs(browser, browser->GetSelection()))
		{
			wxString msg = _("There are properties dialogues open for one or more objects that would be dropped. Please close the properties dialogues and try again.");
			wxMessageBox(msg, _("Cannot drop object"), wxICON_WARNING | wxOK);
			return false;
		}

		if (data->GetSystemObject())
		{
			wxMessageDialog msg(this, data->GetTranslatedMessage(CANNOTDROPSYSTEM),
			                    _("Trying to drop system object"), wxICON_EXCLAMATION);
			msg.ShowModal();
			return false;
		}

		if (cascaded || data->RequireDropConfirm() || settings->GetConfirmDelete())
		{
			wxString text, caption;
			if (cascaded)
			{
				text = data->GetTranslatedMessage(DROPINCLUDINGDEPS);
				caption = data->GetTranslatedMessage(DROPCASCADETITLE);
			}
			else
			{
				/*
				*  currentObject is set using the following command.
				*  i.e. currentObject = browser->GetObject(item);
				*  While fetching this object using this code, somehow it looses its virtual table pointer.
				*  Hence, it is not able to call the GetFullIdentifier - virtual function from the
				*  particular class, but it will always call this functions from pgObject class always.
				*  To rectify this problem, we need to explicitly check the meta data type and call the
				*  function from the particular class.
				*/
				if (data->GetMetaType() == PGM_SERVER)
					text = wxString::Format(_("Are you sure you wish to drop server \"%s\"?"),
					                        ((pgServer *)data)->GetFullIdentifier().c_str());
				else if (data->GetMetaType() == EDB_SYNONYM)
					text = ((edbPrivateSynonym *)data)->GetTranslatedMessage(DROPEXCLUDINGDEPS);
				else
					text = data->GetTranslatedMessage(DROPEXCLUDINGDEPS);
				caption = data->GetTranslatedMessage(DROPTITLE);
			}
			wxMessageDialog msg(this, text, caption, wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT);
			if (msg.ShowModal() != wxID_YES)
			{
				return false;
			}
		}
	}
	bool done = data->DropObject(this, browser, cascaded);

	if (done)
	{
		wxLogInfo(wxT("Dropping %s %s"), data->GetTypeName().c_str(), data->GetIdentifier().c_str());

		wxTreeItemId parentItem = browser->GetItemParent(data->GetId());

		if (updateFinal)
		{
			wxTreeItemId nextItem;
			if (browser->IsVisible(data->GetId()))
				nextItem = browser->GetNextVisible(data->GetId());

			if (nextItem)
			{
				pgObject *nextData = browser->GetObject(nextItem);
				if (!nextData || nextData->GetType() != data->GetType())
					nextItem = browser->GetPrevSibling(data->GetId());
			}
			else
				nextItem = browser->GetPrevSibling(data->GetId());

			if (nextItem)
				browser->SelectItem(nextItem);
		}
		pgaFactory *droppedCollFactory = data->GetFactory()->GetCollectionFactory();

		wxTreeItemId oldgroupitem;
		wxString oldgroupname;
		if (data->IsCreatedBy(serverFactory))
		{
			oldgroupname = ((pgServer *)data)->GetGroup();
			oldgroupitem = browser->GetItemParent(data->GetId());
		}

		browser->Delete(data->GetId());
		// data is invalid now

		if (updateFinal)
		{
			if (!oldgroupname.IsEmpty())
			{
				int total = browser->GetChildrenCount(oldgroupitem, false);
				if (total == 0)
					browser->Delete(oldgroupitem);
				else
				{
					wxString label = oldgroupname + wxT(" (") + NumToStr((long)total) + wxT(")");
					browser->SetItemText(oldgroupitem, label);
				}
			}
			else
			{
				pgCollection *collection = 0;

				while (parentItem)
				{
					collection = (pgCollection *)browser->GetObject(parentItem);
					if (collection && collection->IsCollection() && collection->GetFactory() == droppedCollFactory)
					{
						collection->UpdateChildCount(browser);
						break;
					}
					parentItem = browser->GetItemParent(parentItem);
				}
			}
		}

		// Update the server list, if we dropped a server
		StoreServers();
	}
	return done;
}


void frmMain::OnNew(wxCommandEvent &ev)
{
	pgaFactory *factory = pgaFactory::GetFactory(ev.GetId() - MNU_NEW);

	if (factory == &serverFactory)
	{
		if (currentObject && currentObject->IsCreatedBy(serverFactory))
		{
			pgServer *server = (pgServer *)currentObject;
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


void frmMain::OnSaveDefinition(wxCommandEvent &event)
{

	wxLogInfo(wxT("Saving object definition"));

	if (sqlPane->GetText().IsNull())
	{
		wxLogError(__("There is nothing in the SQL pane to save!"));
		return;
	}

	wxString file;
	settings->Read(wxT("frmMain/LastFile"), &file, wxEmptyString);

#ifdef __WXMSW__
	wxFileDialog filename(this, _("Select output file"), ::wxPathOnly(file), file, _("SQL Scripts (*.sql)|*.sql|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
#else
	wxFileDialog filename(this, _("Select output file"), ::wxPathOnly(file), file, _("SQL Scripts (*.sql)|*.sql|All files (*)|*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
#endif

	// Show the dialogue
	if (filename.ShowModal() == wxID_OK)
	{
		// Write the file
		if (!FileWrite(filename.GetPath(), sqlPane->GetText()))
		{
			wxLogError(__("Could not write the file %s: Errcode=%d."), filename.GetPath().c_str(), wxSysErrorCode());
		}
	}
	else
	{
		wxLogInfo(wxT("User cancelled"));
	}

	settings->Write(wxT("frmMain/LastFile"), filename.GetPath());
}

void frmMain::OnToggleSqlPane(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_SQLPANE))
		manager.GetPane(wxT("sqlPane")).Show(true);
	else
		manager.GetPane(wxT("sqlPane")).Show(false);
	manager.Update();
}

void frmMain::OnToggleObjectBrowser(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_OBJECTBROWSER))
		manager.GetPane(wxT("objectBrowser")).Show(true);
	else
		manager.GetPane(wxT("objectBrowser")).Show(false);
	manager.Update();
}

void frmMain::OnToggleToolBar(wxCommandEvent &event)
{
	if (viewMenu->IsChecked(MNU_TOOLBAR))
		manager.GetPane(wxT("toolBar")).Show(true);
	else
		manager.GetPane(wxT("toolBar")).Show(false);
	manager.Update();
}

void frmMain::OnAuiUpdate(wxAuiManagerEvent &event)
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

void frmMain::OnAuiNotebookPageClose(wxAuiNotebookEvent &event)
{
	// Prevent the user closing the four main tabs.
	if (event.GetSelection() < 4)
	{
		wxMessageBox(_("This tab cannot be closed."), _("Close tab"), wxICON_INFORMATION | wxOK);
		event.Veto();
		return;
	}

	event.Skip();
}

void frmMain::OnDefaultView(wxCommandEvent &event)
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

void frmMain::OnPositionStc(wxStyledTextEvent &event)
{
	if (sqlPane->GetSelectedText().IsNull())
		editMenu->Enable(MNU_COPY, false);
	else
		editMenu->Enable(MNU_COPY, true);
}
