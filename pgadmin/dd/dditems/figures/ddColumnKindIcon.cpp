//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnKindIcon.cpp - Figure container for kind of Column Images (fk,pk,uk) only used
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "dd/dditems/figures/ddColumnKindIcon.h"
#include "hotdraw/main/hdDrawingView.h"
#include "dd/dditems/figures/ddColumnFigure.h"
#include "dd/dditems/figures/ddTableFigure.h"

//Images
#include "images/ddforeignkey.pngc"
#include "images/ddprimarykey.pngc"
#include "images/ddunique.pngc"
#include "images/ddprimaryforeignkey.pngc"
#include "images/ddprimarykeyuniquekey.pngc"
#include "images/ddforeignkeyfromuk.pngc"
#include "images/ddprimaryforeignkeyfromuk.pngc"
#include "images/ddforeignkeyuniquekey.pngc"
#include "images/ddforeignkeyuniquekeyfromuk.pngc"

ddColumnKindIcon::ddColumnKindIcon(ddColumnFigure *owner)
{
	setKindId(DDCOLUMNKINDICON);
	ownerColumn = owner;
	// Initialize with an image to allow initial size calculations
	icon = wxBitmap(*ddprimarykey_png_img);
	iconToDraw = NULL;
	getBasicDisplayBox().SetSize(wxSize(getWidth(), getHeight()));
	isPk = false;
	ukIndex = -1;

	//Set Value default Attributes
	fontAttribute->font().SetPointSize(owner->fontAttribute->font().GetPointSize());
}

ddColumnKindIcon::~ddColumnKindIcon()
{
}

void ddColumnKindIcon::createMenu(wxMenu &mnu)
{
	wxMenuItem *item;

	item = mnu.AppendCheckItem(MNU_DDCTPKEY, _("Primary key"));
	item->Check(isPrimaryKey());
	item->Enable(!getOwnerColumn()->isGeneratedForeignKey());
	item = mnu.AppendCheckItem(MNU_DDCTUKEY, _("Unique key"));
	item->Check(isUniqueKey());
}

void ddColumnKindIcon::OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view)
{
	toggleColumnKind((ddColumnType)event.GetId(), view);
}

ddColumnFigure *ddColumnKindIcon::getOwnerColumn()
{
	return ownerColumn;
}

//if columntype attribute (type) is active then disable, is disable then active.
void ddColumnKindIcon::toggleColumnKind(ddColumnType type, hdDrawingView *view, bool interaction)
{

	switch(type)
	{
		case pk:
			//Set Pk attribute
			if(isPrimaryKey())
			{
				disablePrimaryKey();
			}
			else
			{
				enablePrimaryKey();
				if(getOwnerColumn()->isNull())
				{
					getOwnerColumn()->setColumnOption(notnull);
				}
			}
			break;
		case uk:
			if(isUniqueKey())
			{
				disableUniqueKey();
			}
			else
			{
				uniqueConstraintManager(false, view, interaction);
			}
			break;
		default:
			break;
	}
	getBasicDisplayBox().SetSize(wxSize(getWidth(), getHeight()));
}

void ddColumnKindIcon::basicDraw(wxBufferedDC &context, hdDrawingView *view)
{
	if(iconToDraw)
	{
		hdRect copy = displayBox().gethdRect(view->getIdx());
		view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
		//Adding a yellow circle to increase visibility of uk index
		if(isUniqueKey())
		{
			context.SetBrush(wxBrush(wxColour(wxT("YELLOW")), wxSOLID));
			context.SetPen(wxPen(wxColour(wxT("YELLOW"))));
			context.DrawCircle(copy.x + 6, copy.y + 7, 4);
		}
		//Draw icon
		context.DrawBitmap(*iconToDraw, copy.GetPosition(), true);
		//Draw Uk index if needed
		if(isUniqueKey() && ukIndex > 0)
		{
			wxFont font = fontAttribute->font();
			font.SetPointSize(6);
			context.SetFont(font);
			wxString inumber = wxString::Format(wxT("%d"), (int)ukIndex + 1);
			context.DrawText(inumber, copy.x + 4, copy.y + 2);
		}

	}
}

void ddColumnKindIcon::basicDrawSelected(wxBufferedDC &context, hdDrawingView *view)
{
	basicDraw(context, view);
}

int ddColumnKindIcon::getWidth()
{
	if(iconToDraw)
		return iconToDraw->GetWidth();
	else
		return 8;
}

int ddColumnKindIcon::getHeight()
{
	if(iconToDraw)
		return iconToDraw->GetHeight();
	else
		return 10;
}

/*
ddColumnType ddColumnKindIcon::getKind()
{
	return colType;
}
*/

int ddColumnKindIcon::getUniqueConstraintIndex()
{
	return ukIndex;
}

void ddColumnKindIcon::setUniqueConstraintIndex(int i)
{
	ukIndex = i;
}

bool ddColumnKindIcon::uniqueConstraintManager(bool ukCol, hdDrawingView *view, bool interaction)
{
	wxString tmpString;
	bool isColUk = true;

	if(ukCol) //if already this column kind is Unique Key then convert in a normal column
	{
		syncUkIndexes();
		setUniqueConstraintIndex(-1);
	}
	else //colType!=uk
	{
		if(interaction)
		{
			if(ownerColumn->getOwnerTable()->getUkConstraintsNames().Count() == 0)
			{
				tmpString = getOwnerColumn()->getOwnerTable()->getTableName();
				tmpString.append(wxT("_uk"));
				tmpString = wxGetTextFromUser(wxT("Name of new Unique Key constraint:"), tmpString, tmpString, view);
				if(tmpString.length() > 0)
				{
					getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().Add(tmpString);
					ukIndex = 0;
				}
				else
				{
					setUniqueConstraintIndex(-1);
				}
			}
			else  //>0
			{
				getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().Add(wxString(wxT("Add new Unique Constraint...")));
				int i = wxGetSingleChoiceIndex(wxT("Select Unique Key to add Column"), wxT("Select Unique Key to add Column:"), getOwnerColumn()->getOwnerTable()->getUkConstraintsNames(), view);
				getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().RemoveAt(getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().Count() - 1);
				if(i >= 0)
				{
					if(i == getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().Count())
					{
						tmpString = getOwnerColumn()->getOwnerTable()->getTableName();
						tmpString.append(wxT("_uk"));

						int newIndex = i + 1;
						wxString inumber = wxString::Format(wxT("%s%d"), tmpString.c_str(), (int)newIndex);
						//Validate new name of uk doesn't exists
						while(getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().Index(inumber, false) != -1)
						{
							newIndex++;
							inumber = wxString::Format(wxT("%s%d"), tmpString.c_str(), (int)newIndex);
						}
						inumber = wxString::Format(wxT("%d"), (int)newIndex);
						tmpString.append(inumber);
						tmpString = wxGetTextFromUser(wxT("Name of new Unique Key constraint:"), tmpString, tmpString, view);
						if(tmpString.length() > 0)
						{
							getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().Add(tmpString);
							ukIndex = i;
						}
						else
						{
							setUniqueConstraintIndex(-1);
						}
					}
					else
					{
						ukIndex = i;
					}
				}
				else
				{
					setUniqueConstraintIndex(-1);
				}
			}
		}
		else //without user interaction
		{

		}
	}

	//synchronize observers if this uk column is used as source of fk
	setRightIconForColumn();
	getOwnerColumn()->getOwnerTable()->updateFkObservers();

	if(!isUniqueKey())
		return false;
	else
		return true;
}

//synchronize uk indexes when an uk is change kind from uk to other and other index should be update with that info
void ddColumnKindIcon::syncUkIndexes()
{
	ddColumnFigure *col;
	bool lastUk = true;
	hdIteratorBase *iterator = getOwnerColumn()->getOwnerTable()->figuresEnumerator();
	iterator->Next(); //First Figure is Main Rect
	iterator->Next(); //Second Figure is Table Title
	while(iterator->HasNext())
	{
		col = (ddColumnFigure *) iterator->Next();

		if(col != getOwnerColumn() && (col->getUniqueConstraintIndex() == getOwnerColumn()->getUniqueConstraintIndex()))
		{
			lastUk = false;
			break;
		}
	}
	if(lastUk)
	{
		//here uks indexes are fixed
		iterator->ResetIterator();
		iterator->Next(); //First Figure is Main Rect
		iterator->Next(); //Second Figure is Table Title
		while(iterator->HasNext())
		{
			col = (ddColumnFigure *) iterator->Next();
			if( col->getUniqueConstraintIndex() > getOwnerColumn()->getUniqueConstraintIndex() )
				col->setUniqueConstraintIndex(col->getUniqueConstraintIndex() - 1);
		}
		getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().RemoveAt(getOwnerColumn()->getUniqueConstraintIndex());
		getOwnerColumn()->setUniqueConstraintIndex(-1);
	}
	delete iterator;
}

void ddColumnKindIcon::setRightIconForColumn()
{
	//for a pk Column
	if(isPrimaryKey())
	{
		if(isForeignKey())
		{
			if(getOwnerColumn()->isForeignKeyFromPk())
			{
				icon = wxBitmap(*ddprimaryforeignkey_png_img);
			}
			else
			{
				icon = wxBitmap(*ddprimaryforeignkeyfromuk_png_img);
			}
		}
		else if(isUniqueKey())
		{
			icon = wxBitmap(*ddprimarykeyuniquekey_png_img);
		}
		else
		{
			icon = wxBitmap(*ddprimarykey_png_img);
		}
	}
	else if(isUniqueKey())
	{
		if(isForeignKey())
		{
			if(getOwnerColumn()->isForeignKeyFromPk())
			{
				icon = wxBitmap(*ddforeignkeyuniquekey_png_img);
			}
			else
			{
				icon = wxBitmap(*ddforeignkeyuniquekeyfromuk_png_img);
			}

		}
		else if(isPrimaryKey())
		{
			icon = wxBitmap(*ddprimarykeyuniquekey_png_img);
		}
		else
		{
			icon = wxBitmap(*ddunique_png_img);
		}

	}
	else if(isForeignKey() && !isPrimaryKey() && !isUniqueKey() )
	{

		if(getOwnerColumn()->isForeignKeyFromPk())
		{
			icon = wxBitmap(*ddforeignkey_png_img);
		}
		else
		{
			icon = wxBitmap(*ddforeignkeyfromuk_png_img);
		}
	}


	if(isNone())
	{
		iconToDraw = NULL;
	}
	else
	{
		iconToDraw = &icon;
	}
}

bool ddColumnKindIcon::isPrimaryKey()
{
	return isPk;
}

bool ddColumnKindIcon::isUniqueKey()
{
	return ukIndex >= 0;
}

bool ddColumnKindIcon::isUniqueKey(int uniqueIndex)
{
	return (ukIndex == uniqueIndex);
}

bool ddColumnKindIcon::isNone()
{
	return !isUniqueKey() && !isPrimaryKey() && !isForeignKey();
}
bool ddColumnKindIcon::isForeignKey()
{
	return getOwnerColumn()->isForeignKey();
}

void ddColumnKindIcon::disableUniqueKey()
{
	syncUkIndexes();  //prepare to remove uk attribute to this column
	ukIndex = -1;
	getOwnerColumn()->getOwnerTable()->updateFkObservers();
	setRightIconForColumn();
}

void ddColumnKindIcon::disablePrimaryKey()
{
	getOwnerColumn()->getOwnerTable()->prepareForDeleteFkColumn(getOwnerColumn());
	isPk = false;
	getOwnerColumn()->getOwnerTable()->updateFkObservers();
	setRightIconForColumn();
}

void ddColumnKindIcon::enablePrimaryKey()
{
	isPk = true;
	getOwnerColumn()->getOwnerTable()->updateFkObservers();
	setRightIconForColumn();
}

//Only used for figures created from storage
void ddColumnKindIcon::setPrimaryKey(bool value)
{
	isPk = value;
	setRightIconForColumn();
}

//Only used for figures created from storage
void ddColumnKindIcon::setUkIndex(int ukIdx)
{
	ukIndex = ukIdx;
}
