//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// factory.cpp - Object classes factory
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

#include "ctl/ctlMenuToolbar.h"
#include "schema/pgCollection.h"
#include "frm/menu.h"

// wxWindows headers
#include <wx/imaglist.h>

wxArrayPtrVoid *factoryArray = 0;

#define FACTORY_OFFSET 100

pgaFactory::pgaFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm)
{
	if (!factoryArray)
		factoryArray = new wxArrayPtrVoid;
	id = factoryArray->GetCount() + FACTORY_OFFSET;
	factoryArray->Add(this);
	collectionFactory = 0;
	smallIconId = -1;
	typeName = (wxChar *)tn;
	if (ns)
		newString = (wxChar *)ns;
	else
		newString = typeName;
	if (nls)
		newLongString = (wxChar *)nls;
	else
		newLongString = newString;
	metaType = PGM_UNKNOWN;

	if (img && img->IsOk())
	{
		image = *img;
		iconId = addIcon(img);
		if (imgSm && imgSm->IsOk())
			smallIconId = addIcon(imgSm);
	}
	else
		iconId = -1;
}


bool pgaFactory::WantSmallIcon()
{
#ifdef __WXMSW__
	return true;
#else
	return false;
#endif
}

int pgaFactory::GetIconId()
{
	if (WantSmallIcon() && smallIconId >= 0)
		return smallIconId;

	return iconId;
}

pgaFactory *pgaFactory::GetFactory(int id)
{
	id -= FACTORY_OFFSET;
	if (id >= 0 && id < (int)factoryArray->GetCount())
		return (pgaFactory *)factoryArray->Item(id);;

	return 0;
}


pgaFactory *pgaFactory::GetFactory(const wxString &name)
{
	int i;
	pgaFactory *factory;

	for (i = FACTORY_OFFSET ; (factory = GetFactory(i)) != 0 ; i++)
	{
		if (name.Matches(factory->GetTypeName()))
			return factory;
	}
	return 0;
}

pgaFactory *pgaFactory::GetFactoryByMetaType(const int type)
{
	int i;
	pgaFactory *factory;

	for (i = FACTORY_OFFSET ; (factory = GetFactory(i)) != 0 ; i++)
	{
		if (factory->GetMetaType() == type)
			return factory;
	}
	return 0;
}


#include "images/property.pngc"
#include "images/statistics.pngc"

wxArrayPtrVoid *deferredImagesArray = 0;

int pgaFactory::addIcon(wxImage *img)
{
	if (!imageList)
	{
		if (!deferredImagesArray)
		{
			//Setup the global imagelist
			deferredImagesArray = new wxArrayPtrVoid;

			deferredImagesArray->Add(property_png_img);
			deferredImagesArray->Add(statistics_png_img);
		}

		deferredImagesArray->Add(img);

		return deferredImagesArray->GetCount() - 1;
	}
	else
	{
		wxBitmap bmp(*img);
		wxIcon *ico = new wxIcon();
		ico->CopyFromBitmap(bmp);
		return imageList->Add(*ico);
	}
}

void pgaFactory::RealizeImages()
{
	if (!imageList && deferredImagesArray)
	{
		imageList = new wxImageList(16, 16, true, deferredImagesArray->GetCount());
		size_t i;
		for (i = 0 ; i < deferredImagesArray->GetCount() ; i++)
		{
			wxImage *img = (wxImage *)deferredImagesArray->Item(i);
			wxBitmap bmp(*img);
			wxIcon *ico = new wxIcon();
			ico->CopyFromBitmap(bmp);
			imageList->Add(*ico);
		}

		delete deferredImagesArray;
		deferredImagesArray = 0;
	}
}

void pgaFactory::RegisterMenu(wxWindow *w, wxObjectEventFunction func)
{
	w->Connect(GetFactory(FACTORY_OFFSET)->GetId() + MNU_NEW,
	           GetFactory(factoryArray->GetCount() + FACTORY_OFFSET - 1)->GetId() + MNU_NEW,
	           wxEVT_COMMAND_MENU_SELECTED, func);
}


void pgaFactory::AppendMenu(wxMenu *menu)
{
	if (menu && GetNewString())
	{
		wxMenuItem *item = menu->Append(MNU_NEW + GetId(), wxGetTranslation(GetNewString()), wxGetTranslation(GetNewLongString()));
		if (image.IsOk())
		{
			(void)item;
			// doesn't work?
			// item->SetBitmap(wxBitmap(image));
		}
	}
}


int pgaFactory::GetMetaType()
{
	if (IsCollection())
		return ((pgaCollectionFactory *)this)->GetItemFactory()->GetMetaType();
	return metaType;
}


pgaCollectionFactory::pgaCollectionFactory(pgaFactory *f, const wxChar *tn, wxImage *img, wxImage *imgSm)
	: pgaFactory(tn, f->GetNewString(), f->GetNewLongString())
{
	itemFactory = f;
	f->collectionFactory = this;
	if (img && img->IsOk())
	{
		image = *img;
		iconId = addIcon(img);
		if (imgSm && imgSm->IsOk())
			smallIconId = addIcon(imgSm);
	}
	else
		iconId = f->GetIconId();
}


pgObject *pgaCollectionFactory::CreateObjects(pgCollection  *obj, ctlTree *browser, const wxString &restr)
{
	if (itemFactory)
		return itemFactory->CreateObjects(obj, browser, restr);
	return 0;
}


dlgProperty *pgaCollectionFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	if (itemFactory)
		return itemFactory->CreateDialog(frame, node, parent);
	return 0;
}


////////////////////////////////////////////////


menuFactoryList::~menuFactoryList()
{
	while (GetCount())
	{
		delete (menuFactory *)Item(0);
		RemoveAt(0);
	}
}

actionFactory *menuFactoryList::GetFactory(int id, bool actionOnly)
{
	id -= MNU_ACTION;
	if (id >= 0 && id < (int)GetCount())
	{
		actionFactory *f = (actionFactory *)Item(id);
		if (f->IsAction() || !actionOnly)
			return f;
	}
	return 0;
}


void menuFactoryList::RegisterMenu(wxWindow *w, wxObjectEventFunction func)
{
	w->Connect(MNU_ACTION, MNU_ACTION + GetCount() - 1,
	           wxEVT_COMMAND_MENU_SELECTED, func);
}


void menuFactoryList::CheckMenu(pgObject *obj, wxMenuBar *menubar, ctlMenuToolbar *toolbar)
{
	size_t id;
	for (id = MNU_ACTION ; id < GetCount() + MNU_ACTION ; id++)
	{
		actionFactory *f = GetFactory(id);
		if (f)
		{
			bool how = f->CheckEnable(obj);
			if (menubar->FindItem(id))
				menubar->Enable(id, how);
			if (toolbar)
				toolbar->EnableTool(id, how);

			bool chk = f->CheckChecked(obj);
			wxMenuItem *itm = menubar->FindItem(id);
			if (itm && itm->IsCheckable())
				menubar->Check(id, chk);
		}
	}
	for (id = 0 ; id < GetCount() ; id++)
	{
		actionFactory *f = (actionFactory *)Item(id);
		if (f->IsSubmenu())
			EnableSubmenu(menubar, id + MNU_ACTION);
	}
}


void menuFactoryList::EnableSubmenu(wxMenuBar *menuBar, int id)
{
	wxMenuItem *item = menuBar->FindItem(id);
	if (item)
	{
		wxMenu *menu = item->GetSubMenu();
		wxASSERT(menu);
		if (!menu)
			return;

		size_t position;
		for (position = 0 ; position < menu->GetMenuItemCount() ; position++)
		{
			item = menu->FindItemByPosition(position);
			if (item && item->IsEnabled())
			{
				menuBar->Enable(id, true);
				return;
			}
		}

		menuBar->Enable(id, false);
	}
}


void menuFactoryList::AppendEnabledMenus(wxMenuBar *menuBar, wxMenu *treeContextMenu)
{
	size_t id;
	wxMenuItem *lastItem = 0;
	for (id = MNU_ACTION ; id < GetCount() + MNU_ACTION ; id++)
	{
		actionFactory *f = GetFactory(id, false);
		if (f->IsAction())
		{
			if (f->GetContext())
			{
				wxMenuItem *menuItem = menuBar->FindItem(id);
				if (menuItem && menuItem->IsEnabled())
				{
					if (!menuItem->IsSubMenu())
					{
#if wxCHECK_VERSION(2, 9, 0)
						wxString lab = menuItem->GetItemLabelText();
#else
						wxString lab = menuItem->GetLabel(); // deprecated
#endif
						lastItem = treeContextMenu->Append(id, lab, menuItem->GetHelp(), menuItem->IsCheckable() ? wxITEM_CHECK : wxITEM_NORMAL);
						if (menuItem->IsCheckable() && menuItem->IsChecked())
							treeContextMenu->FindItem(id)->Check();
					}
					else
					{
						/* Copy of submenu */
						wxMenu *oldSubMenu = menuItem->GetSubMenu();
						wxMenu *newSubMenu = new wxMenu();

						size_t i;
						int itemCount = 0;
						wxMenuItem *singleMenuItem = 0;
						for (i = 0; i < oldSubMenu->GetMenuItemCount(); i++)
						{
							wxMenuItem *oldMenuItem = oldSubMenu->FindItemByPosition(i);
							if (oldMenuItem->IsEnabled())
							{
#if wxCHECK_VERSION(2, 9, 0)
								wxString oldLab = oldMenuItem->GetItemLabelText();
#else
								wxString oldLab = oldMenuItem->GetLabel(); // deprecated
#endif
								newSubMenu->Append(oldMenuItem->GetId(), oldLab, oldMenuItem->GetHelp(), menuItem->IsCheckable() ? wxITEM_CHECK : wxITEM_NORMAL);
								if (oldMenuItem->IsCheckable() && oldMenuItem->IsChecked())
									newSubMenu->FindItem(oldMenuItem->GetId())->Check();

								itemCount++;
								singleMenuItem = oldMenuItem;
							}
						}
						if (itemCount > 1)
						{
#if wxCHECK_VERSION(2, 9, 0)
							wxString lab = menuItem->GetItemLabelText();
#else
							wxString lab = menuItem->GetLabel(); // deprecated
#endif
							lastItem = treeContextMenu->Append(id, lab, newSubMenu);
						}
						else
						{
							delete newSubMenu;
							if (itemCount)
							{
#if wxCHECK_VERSION(2, 9, 0)
								wxString lab = singleMenuItem->GetItemLabelText();
#else
								wxString lab = singleMenuItem->GetLabel(); // deprecated
#endif
								lastItem = treeContextMenu->Append(singleMenuItem->GetId(), lab, singleMenuItem->GetHelp(), menuItem->IsCheckable() ? wxITEM_CHECK : wxITEM_NORMAL);
								if (singleMenuItem->IsCheckable() && singleMenuItem->IsChecked())
									treeContextMenu->FindItem(singleMenuItem->GetId())->Check();
							}
						}
					}
				}
			}
		}
		else
		{
			if (lastItem && lastItem->GetId() >= 0)
				lastItem = treeContextMenu->AppendSeparator();
		}
	}

	if (lastItem && lastItem->GetId() < 0)
	{
		treeContextMenu->Remove(lastItem);
		delete lastItem;
	}
}


menuFactory::menuFactory(menuFactoryList *list)
{
	if (list)
		list->Add(this);
}

menuFactory::~menuFactory()
{
}

actionFactory::actionFactory(menuFactoryList *list) : menuFactory(list)
{
	if (list)
		id = list->GetCount() + MNU_ACTION - 1;
	else
		id = 0;
	context = false;
}



