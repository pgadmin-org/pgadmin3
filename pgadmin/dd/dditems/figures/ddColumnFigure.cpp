//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnFigure.cpp - Minimal Composite Figure for a column of a table
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "dd/dditems/figures/ddColumnFigure.h"
#include "dd/dditems/figures/ddTextTableItemFigure.h"
#include "dd/dditems/tools/ddColumnFigureTool.h"
#include "dd/dditems/figures/ddColumnKindIcon.h"
#include "dd/dditems/figures/ddColumnOptionIcon.h"
#include "dd/dditems/utilities/ddDataType.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/figures/ddRelationshipItem.h"

ddColumnFigure::ddColumnFigure(wxString &columnName, ddTableFigure *owner, ddRelationshipItem *sourceFk)
{
	fkSource = sourceFk;
	usedAsFkDestFor = NULL;
	setOwnerTable(owner);
	deactivateGenFkName(); //initializae by default at not generate auto fk name
	columnText = new ddTextTableItemFigure(columnName, dt_null, this);
	leftImage = new ddColumnKindIcon(this);
	centerImage = new ddColumnOptionIcon(this);


	//Initialize displaybox and image coords
	basicDisplayBox.SetPosition(wxPoint(0, 0));
	basicDisplayBox.SetSize( columnText->displayBox().GetSize());
	if(leftImage && centerImage)
	{
		basicDisplayBox.width += leftImage->displayBox().GetSize().GetWidth() + 3;
		basicDisplayBox.width += centerImage->displayBox().GetSize().GetWidth() + 3;
		columnText->displayBox().x += leftImage->displayBox().GetSize().GetWidth() + 2;
		columnText->displayBox().x += centerImage->displayBox().GetSize().GetWidth() + 3;
	}
	else
	{
		basicDisplayBox.width += 22; //default value = 1 + 8 + 3 + 8 + 2
		columnText->displayBox().x += 22;
	}

	//Set Value default Attributes
	fontAttribute->font().SetPointSize(owner->fontAttribute->font().GetPointSize());
}

ddColumnFigure::~ddColumnFigure()
{
	if(columnText)
		delete columnText;
	if(leftImage)
		delete leftImage;
	if(centerImage)
		delete centerImage;
}

void ddColumnFigure::basicMoveBy(int x, int y)
{
	wxhdAbstractFigure::basicMoveBy(x, y);
	if(leftImage)
		leftImage->moveBy(x, y);
	if(centerImage)
		centerImage->moveBy(x, y);
	columnText->moveBy(x, y);
}

void ddColumnFigure::moveTo(int x, int y)
{
	wxhdAbstractFigure::moveTo(x, y);
	int distance = 0;

	if(leftImage)
	{
		leftImage->moveTo(x, y);
		distance += leftImage->displayBox().GetSize().GetWidth() + 3;
	}
	else
	{
		distance += 11; //value = 8 + 3
	}

	if(centerImage)
	{
		centerImage->moveTo(x + distance, y);
		distance += centerImage->displayBox().GetSize().GetWidth() + 3;
	}
	else
	{
		distance += 11; //value = 8 + 3
	}

	columnText->moveTo(x + distance, y);
}


bool ddColumnFigure::containsPoint(int x, int y)
{
	bool out = false;

	if(columnText->containsPoint(x, y))
		out = true;
	else if(leftImage->containsPoint(x, y))
		out = true;
	else if(centerImage->containsPoint(x, y))
		out = true;

	return out;
}

wxhdRect &ddColumnFigure::getBasicDisplayBox()
{
	return basicDisplayBox;
}

void ddColumnFigure::basicDraw(wxBufferedDC &context, wxhdDrawingView *view)
{
	columnText->draw(context, view);
	if(leftImage)
		leftImage->draw(context, view);
	if(centerImage)
		centerImage->draw(context, view);
}

void ddColumnFigure::basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view)
{
	columnText->drawSelected(context, view);
	if(leftImage)
		leftImage->drawSelected(context, view);
	if(centerImage)
		centerImage->drawSelected(context, view);
}

wxhdIFigure *ddColumnFigure::findFigure(int x, int y)
{
	wxhdIFigure *out = NULL;

	if(columnText->containsPoint(x, y))
		out = columnText;

	if(leftImage && leftImage->containsPoint(x, y))
		out = leftImage;

	if(centerImage && centerImage->containsPoint(x, y))
		out = centerImage;

	return out;
}

wxhdITool *ddColumnFigure::CreateFigureTool(wxhdDrawingEditor *editor, wxhdITool *defaultTool)
{
	return new ddColumnFigureTool(editor, this, defaultTool);
}

wxhdIFigure *ddColumnFigure::getFigureAt(int pos)
{
	if(pos == 0)
		return (wxhdIFigure *) leftImage;

	if(pos == 1)
		return (wxhdIFigure *) centerImage;

	if(pos == 2)
		return (wxhdIFigure *) columnText;

	return NULL;
}

ddTableFigure *ddColumnFigure::getOwnerTable()
{
	return ownerTable;
}

void ddColumnFigure::setOwnerTable(ddTableFigure *table)
{
	ownerTable = table;
}

void ddColumnFigure::displayBoxUpdate()
{
	if(leftImage && centerImage)
	{
		columnText->displayBoxUpdate();
		basicDisplayBox.width = columnText->displayBox().GetSize().GetWidth();
		basicDisplayBox.width += leftImage->displayBox().GetSize().GetWidth() + 3;
		basicDisplayBox.width += centerImage->displayBox().GetSize().GetWidth() + 3;
	}
	else
	{
		columnText->displayBoxUpdate();
		basicDisplayBox.width += 22; //default value = 1 + 8 + 3 + 8 +2
	}
}

bool ddColumnFigure::isNull()
{
	return centerImage->getOption() == null;
}

bool ddColumnFigure::isNotNull()
{
	return centerImage->getOption() == notnull;
}

bool ddColumnFigure::isNone()
{
	return leftImage->isNone();
}

bool ddColumnFigure::isPrimaryKey()
{
	return leftImage->isPrimaryKey();
}

void ddColumnFigure::disablePrimaryKey()
{
	leftImage->disablePrimaryKey();
}

void ddColumnFigure::enablePrimaryKey()
{
	leftImage->enablePrimaryKey();
}

bool ddColumnFigure::isUniqueKey()
{
	return leftImage->isUniqueKey();
}

bool ddColumnFigure::isUniqueKey(int uniqueIndex)
{
	return leftImage->isUniqueKey(uniqueIndex);
}

bool ddColumnFigure::isPlain()
{
	return leftImage->isNone();
}

void ddColumnFigure::setColumnKindToNone()
{
	leftImage->disablePrimaryKey();
	leftImage->disableUniqueKey();
	//Foreign key cannot be changed by set / toggle functions
}

void ddColumnFigure::setRightIconForColumn()
{
	leftImage->setRightIconForColumn();
}

void ddColumnFigure::toggleColumnKind(ddColumnType type, wxhdDrawingView *view)
{
	leftImage->toggleColumnKind(type, view);
}

void ddColumnFigure::setColumnOption(ddColumnOptionType type)
{
	centerImage->changeIcon(type);
}

wxString &ddColumnFigure::getColumnName(bool datatype)
{
	return columnText->getText(datatype);
}

int ddColumnFigure::getPrecision()
{
	return columnText->getPrecision();
}

int ddColumnFigure::getScale()
{
	return columnText->getScale();
}

int ddColumnFigure::getUniqueConstraintIndex()
{
	return leftImage->getUniqueConstraintIndex();
}

void ddColumnFigure::setUniqueConstraintIndex(int i)
{
	leftImage->setUniqueConstraintIndex(i);
}

/*
ddColumnType ddColumnFigure::getColumnKind()
{
	return leftImage->getKind();
}
*/

ddColumnOptionType ddColumnFigure::getColumnOption()
{
	return centerImage->getOption();
}

ddDataType ddColumnFigure::getDataType()
{
	return columnText->getDataType();
}

void ddColumnFigure::setDataType(ddDataType type)
{
	columnText->setDataType(type);
}

void ddColumnFigure::setColumnName(wxString name)
{
	columnText->setText(name);
}

bool ddColumnFigure::isForeignKeyFromPk()
{
	if(fkSource)
	{
		return fkSource->isForeignKeyFromPk();
	}
	else if(usedAsFkDestFor)
	{
		return usedAsFkDestFor->isForeignKeyFromPk();
	}
	return false;
}

bool ddColumnFigure::isForeignKey()
{
	bool a = isUserCreatedForeignKey();
	bool b = isGeneratedForeignKey();
	return (a || b);
}

bool ddColumnFigure::isGeneratedForeignKey()
{
	return fkSource != NULL;
}

bool ddColumnFigure::isUserCreatedForeignKey()
{
	return usedAsFkDestFor != NULL;
}

ddRelationshipItem *ddColumnFigure::getRelatedFkItem()
{
	return usedAsFkDestFor;
}

void ddColumnFigure::setAsUserCreatedFk(ddRelationshipItem *relatedFkItem)
{
	usedAsFkDestFor = relatedFkItem;
	//Now fix icons of kind of column
	if(relatedFkItem == NULL)
	{
		leftImage->setRightIconForColumn();
	}
}

bool ddColumnFigure::isFkNameGenerated()
{
	return generateFkName;
}

void ddColumnFigure::activateGenFkName()
{
	generateFkName = true;
}

void ddColumnFigure::deactivateGenFkName()
{
	generateFkName = false;
}

wxString ddColumnFigure::generateSQL()
{
	wxString tmp;
	tmp = getColumnName();
	tmp += wxT(" ");
	tmp += columnText->getType();
	return tmp;
}

ddRelationshipItem *ddColumnFigure::getFkSource()
{
	return fkSource;
}

void ddColumnFigure::setTextColour(wxColour colour)
{
	columnText->fontColorAttribute->fontColor = colour;
}


//Validate status of column for SQL DDL generation
bool ddColumnFigure::validateColumn(wxString &errors)
{
	bool out = true;
	wxString tmp;

	if(usedAsFkDestFor)
	{
		//Validate if relationship is marked as identifying but column isn't marked as primary key
		wxString sourceTableName = usedAsFkDestFor->sourceTableName();
		wxString destTableName = usedAsFkDestFor->destTableName();
		wxString fkColName = usedAsFkDestFor->fkColumn->getColumnName(false);
		wxString sourceColName = usedAsFkDestFor->original->getColumnName(false);

		if(usedAsFkDestFor->relationIsIdentifying() && !this->isPrimaryKey())
		{
			out = false;
			tmp =  _("relation between table: ") + sourceTableName + _(" and table: ") + destTableName;
			tmp += _(" is marked as identifying but user created foreign key column: ") + fkColName;
			tmp += _(" isn't set as primary key \n");
			errors.Append(tmp);
		}
		//Validate if relationship is marked as optional but column is mandatory
		if( !usedAsFkDestFor->relationIsMandatory() && this->isNotNull())
		{
			out = false;
			tmp =  _("relation between table:") + sourceTableName + _(" and table: ") + destTableName;
			tmp += _("is marked as optional but user created foreign key column: ") + fkColName;
			tmp += _("is set as mandatory \n");
			errors.Append(tmp);
		}
		//Validate if relationship is marked as mandatory buy column is optional
		if( usedAsFkDestFor->relationIsMandatory() && this->isNull())
		{
			out = false;
			tmp =   _("relation between table:") + sourceTableName + _(" and table: ") + destTableName +
			        tmp +=  _(" is marked as mandatory but user created foreign key column: ") + fkColName;
			tmp += _("is set as optional \n");
			errors.Append(tmp);
		}

		//Validate datatype compatibility (right now only exactly same datatype)
		if(this->getDataType() != usedAsFkDestFor->original->getDataType())
		{
			out = false;
			tmp =  _("User created foreign key column: ") + fkColName + _("have different datatype from source column of relationship: ");
			tmp += sourceColName + _(" \n");
			errors.Append(tmp);
		}
	}

	return out;
}
