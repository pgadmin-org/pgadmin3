//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTextTableItemFigure.cpp - Draw a column inside a table
//
////////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/numdlg.h>

// App headers
#include "dd/dditems/figures/ddTextTableItemFigure.h"
#include "dd/dditems/figures/ddRelationshipItem.h"
#include "dd/dditems/tools/ddColumnTextTool.h"
#include "dd/dditems/utilities/ddDataType.h"
#include "hotdraw/figures/hdSimpleTextFigure.h"
#include "hotdraw/main/hdDrawingView.h"
#include "dd/ddmodel/ddDrawingEditor.h"
#include "dd/ddmodel/ddDatabaseDesign.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/utilities/ddPrecisionScaleDialog.h"
#include "hotdraw/utilities/hdRemoveDeleteDialog.h"

ddTextTableItemFigure::ddTextTableItemFigure(wxString &columnName, ddDataType dataType, ddColumnFigure *owner):
	hdSimpleTextFigure(columnName)
{
	setKindId(DDTEXTTABLEITEMFIGURE);
	ownerTable = NULL; //table name item is the only one case of use of this variable
	oneTimeNoAlias = false;
	columnType = dataType;
	this->setEditable(true);
	enablePopUp();
	ownerColumn = owner;
	showDataType = true;
	showAlias = false;
	recalculateDisplayBox();
	precision = -1;
	scale = -1;

	if(owner)  //is Column Object
	{
		fontColorAttribute->fontColor = owner->getOwnerTable()->fontColorAttribute->fontColor;
		fontSelColorAttribute->fontColor = owner->getOwnerTable()->fontSelColorAttribute->fontColor;
	}
}

ddTextTableItemFigure::~ddTextTableItemFigure()
{
}

void ddTextTableItemFigure::recalculateDisplayBox()
{
	if (ownerColumn && ownerColumn->getOwnerTable())
	{
		hdSimpleTextFigure::recalculateDisplayBox();
		displayBox().width = ownerColumn->getOwnerTable()->getFiguresMaxWidth();
	}
}

void ddTextTableItemFigure::displayBoxUpdate()
{
	recalculateDisplayBox();
}

void ddTextTableItemFigure::setOwnerTable(ddTableFigure *table)
{
	ownerTable = table;
	if(table)//is Table Name object
	{
		fontColorAttribute->fontColor = ownerTable->fontColorAttribute->fontColor;
		fontSelColorAttribute->fontColor = ownerTable->fontSelColorAttribute->fontColor;
	}
}

wxString &ddTextTableItemFigure::getText(bool extended)
{
	if(showDataType && extended && getOwnerColumn())
	{
		wxString ddType = dataTypes()[getDataType()];   //Should use getDataType() & getPrecision(), because when column is fk, type is not taken from this column, instead from original column (source of fk)
		bool havePrecision = columnType == dt_numeric || columnType == dt_bit || columnType == dt_char || columnType == dt_interval || columnType == dt_varbit || columnType == dt_varchar;
		if( havePrecision && getPrecision() >= 0)
		{
			ddType.Truncate(ddType.Find(wxT("(")));
			if(getScale() == -1)
				ddType += wxString::Format(wxT("(%d)"), getPrecision());
			else
				ddType += wxString::Format(wxT("(%d,%d)"), getPrecision(), getScale());
		}
		//Fix to serial is integer at automatically generated foreign key
		if(getDataType() == dt_serial && getOwnerColumn()->isGeneratedForeignKey())
			ddType = dataTypes()[dt_integer];

		out = wxString( hdSimpleTextFigure::getText() + wxString(wxT(" : ")) + ddType );
		return  out;
	}
	else if( showAlias && getOwnerColumn() == NULL )
	{
		if(!oneTimeNoAlias)
			out = wxString( hdSimpleTextFigure::getText() + wxString(wxT(" ( ")) + colAlias + wxString(wxT(" ) ")) );
		else
		{
			out = wxString( hdSimpleTextFigure::getText() );
			oneTimeNoAlias = false;
		}
		return out;
	}
	else
	{
		return hdSimpleTextFigure::getText();
	}
}

wxString ddTextTableItemFigure::getType(bool raw)
{
	wxString ddType = dataTypes()[columnType];
	if(raw)
		return ddType;

	bool havePrecision = columnType == dt_numeric || columnType == dt_bit || columnType == dt_char || columnType == dt_interval || columnType == dt_varbit || columnType == dt_varchar;
	if( havePrecision && getPrecision() >= 0)
	{
		ddType.Truncate(ddType.Find(wxT("(")));
		if(getScale() == -1)
			ddType += wxString::Format(wxT("(%d)"), getPrecision());
		else
			ddType += wxString::Format(wxT("(%d,%d)"), getPrecision(), getScale());
	}

	//Fix to serial is integer at automatically generated foreign key
	if(columnType == dt_serial && getOwnerColumn()->isGeneratedForeignKey())
		ddType = dataTypes()[dt_integer];

	return ddType;
}

//WARNING: event ID must match enum ddDataType!!! this event was created on view
void ddTextTableItemFigure::OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view)
{
	wxTextEntryDialog *nameDialog = NULL;
	ddPrecisionScaleDialog *numericDialog = NULL;
	wxString tmpString;
	int answer;
	int tmpprecision;
	long tmpvalue;
	hdRemoveDeleteDialog *delremDialog = NULL;

	switch(event.GetId())
	{
		case MNU_DDADDCOLUMN:
			nameDialog = new wxTextEntryDialog(view, wxT("New column name"), wxT("Add a column"));
			answer = nameDialog->ShowModal();
			if (answer == wxID_OK)
			{
				tmpString = nameDialog->GetValue();
				getOwnerColumn()->getOwnerTable()->addColumn(view->getIdx(), new ddColumnFigure(tmpString, getOwnerColumn()->getOwnerTable()));
				view->notifyChanged();
			}
			delete nameDialog;
			break;
		case MNU_DELCOLUMN:
			answer = wxMessageBox(wxT("Are you sure you wish to delete column ") + getText(true) + wxT("?"), wxT("Delete column?"), wxYES_NO | wxNO_DEFAULT, view);
			if (answer == wxYES)
			{
				getOwnerColumn()->getOwnerTable()->removeColumn(view->getIdx(), getOwnerColumn());
				view->notifyChanged();
			}
			break;
		case MNU_AUTONAMCOLUMN:
			getOwnerColumn()->activateGenFkName();
			getOwnerColumn()->getFkSource()->syncAutoFkName();
			view->notifyChanged();
			break;
		case MNU_RENAMECOLUMN:
			nameDialog = new wxTextEntryDialog(view, wxT("New column name"), wxT("Rename Column"), getText());
			nameDialog->ShowModal();
			if(getOwnerColumn()->isGeneratedForeignKey()) //after a manual user column rename, deactivated automatic generation of fk name.
				getOwnerColumn()->deactivateGenFkName();
			setText(nameDialog->GetValue());
			delete nameDialog;
			view->notifyChanged();
			break;
		case MNU_NOTNULL:
			if(getOwnerColumn()->isNotNull())
				getOwnerColumn()->setColumnOption(null);
			else
				getOwnerColumn()->setColumnOption(notnull);
			view->notifyChanged();
			break;
		case MNU_PKEY:
			if(getOwnerColumn()->isPrimaryKey())
			{
				getOwnerColumn()->disablePrimaryKey();
			}
			else
			{
				getOwnerColumn()->enablePrimaryKey();
				getOwnerColumn()->setColumnOption(notnull);
			}
			view->notifyChanged();
			break;
		case MNU_UKEY:
			getOwnerColumn()->toggleColumnKind(uk, view);
			view->notifyChanged();
			break;
		case MNU_TYPESERIAL:
			setDataType(dt_serial);  //Should use setDataType always to set this value to allow fk to work flawlessly
			recalculateDisplayBox();
			getOwnerColumn()->displayBoxUpdate();
			getOwnerColumn()->getOwnerTable()->updateTableSize();
			view->notifyChanged();
			break;
		case MNU_TYPEBOOLEAN:
			setDataType(dt_boolean);
			recalculateDisplayBox();
			getOwnerColumn()->displayBoxUpdate();
			getOwnerColumn()->getOwnerTable()->updateTableSize();
			view->notifyChanged();
			break;
		case MNU_TYPEINTEGER:
			setDataType(dt_integer);
			recalculateDisplayBox();
			getOwnerColumn()->displayBoxUpdate();
			getOwnerColumn()->getOwnerTable()->updateTableSize();
			view->notifyChanged();
			break;
		case MNU_TYPEMONEY:
			setDataType(dt_money);
			recalculateDisplayBox();
			getOwnerColumn()->displayBoxUpdate();
			getOwnerColumn()->getOwnerTable()->updateTableSize();
			view->notifyChanged();
			break;
		case MNU_TYPEVARCHAR:
			setDataType(dt_varchar);
			tmpprecision = wxGetNumberFromUser(_("Varchar size"),
			                                   _("Size for varchar datatype"),
			                                   _("Varchar size"),
			                                   getPrecision(), 0, 255, view);
			if (tmpprecision >= 0)
			{
				setPrecision(tmpprecision);
				setScale(-1);
			}
			recalculateDisplayBox();
			getOwnerColumn()->displayBoxUpdate();
			getOwnerColumn()->getOwnerTable()->updateTableSize();
			view->notifyChanged();
			break;
		case MNU_TYPEOTHER:
			answer = wxGetSingleChoiceIndex(wxT("New column datatype"), wxT("Column Datatypes"), dataTypes(), view);
			if(answer >= 0)
			{
				view->notifyChanged();
				if(answer == dt_varchar || answer == dt_bit || answer == dt_char || answer == dt_interval || answer == dt_varbit)
				{
					tmpprecision = wxGetNumberFromUser(_("datatype size"),
					                                   _("Size for datatype"),
					                                   _("size"),
					                                   getPrecision(), 0, 255, view);
					if (tmpprecision >= 0)
					{
						setPrecision(tmpprecision);
						setScale(-1);
					}
					recalculateDisplayBox();
					getOwnerColumn()->displayBoxUpdate();
					getOwnerColumn()->getOwnerTable()->updateTableSize();
				}
				if(answer == dt_numeric)
				{
					numericDialog = new ddPrecisionScaleDialog(	view,
					        NumToStr((long)getPrecision()),
					        NumToStr((long)getScale()));
					numericDialog->ShowModal();
					numericDialog->GetValue1().ToLong(&tmpvalue);
					setPrecision(tmpvalue);
					numericDialog->GetValue2().ToLong(&tmpvalue);
					setScale(tmpvalue);
					delete numericDialog;
					recalculateDisplayBox();
					getOwnerColumn()->displayBoxUpdate();
					getOwnerColumn()->getOwnerTable()->updateTableSize();
				}


				setDataType( (ddDataType) answer );
				recalculateDisplayBox();
				getOwnerColumn()->displayBoxUpdate();
				getOwnerColumn()->getOwnerTable()->updateTableSize();
			}
			break;
		case MNU_TYPEPKEY_CONSTRAINTNAME:
			tmpString = wxGetTextFromUser(wxT("New name of primary key:"), getOwnerColumn()->getOwnerTable()->getPkConstraintName(), getOwnerColumn()->getOwnerTable()->getPkConstraintName(), view);
			if(tmpString.length() > 0)
			{
				getOwnerColumn()->getOwnerTable()->setPkConstraintName(tmpString);
				view->notifyChanged();
			}
			break;
		case MNU_TYPEUKEY_CONSTRAINTNAME:
			answer = wxGetSingleChoiceIndex(wxT("Select Unique Key constraint to edit name"), wxT("Select Unique Constraint to edit name:"), getOwnerColumn()->getOwnerTable()->getUkConstraintsNames(), view);
			if(answer >= 0)
			{
				tmpString = wxGetTextFromUser(wxT("Change name of Unique Key constraint:"), getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().Item(answer), getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().Item(answer), view);
				if(tmpString.length() > 0)
				{
					getOwnerColumn()->getOwnerTable()->getUkConstraintsNames().Item(answer) = tmpString;
					view->notifyChanged();
				}
			}
			break;
		case MNU_DELTABLE:

			delremDialog = new hdRemoveDeleteDialog(wxT("Are you sure you wish to delete table ") + getOwnerColumn()->getOwnerTable()->getTableName() + wxT("?"), wxT("Delete table?"), view);
			answer = delremDialog->ShowModal();
			ddTableFigure *table = getOwnerColumn()->getOwnerTable();
			if (answer == DD_DELETE)
			{
				ddDrawingEditor *editor = (ddDrawingEditor *) view->editor();
				//Unselect table at all diagrams
				editor->removeFromAllSelections(table);
				//Drop foreign keys with this table as origin or destination
				table->processDeleteAlert(view->getDrawing());
				//Drop table
				editor->deleteModelFigure(table);
				editor->getDesign()->refreshBrowser();
				view->notifyChanged();
			}
			else if(answer == DD_REMOVE)
			{
				ddDrawingEditor *editor = (ddDrawingEditor *) view->editor();
				editor->getExistingDiagram(view->getIdx())->removeFromSelection(table);
				editor->getExistingDiagram(view->getIdx())->remove(table);
				view->notifyChanged();
			}
			delete delremDialog;
			break;
	}
}

void ddTextTableItemFigure::createMenu(wxMenu &mnu)
{
	wxMenu *submenu;
	wxMenuItem *item;

	mnu.Append(MNU_DDADDCOLUMN, _("Add a column..."));
	item = mnu.Append(MNU_DELCOLUMN, _("Delete the selected column..."));
	if(getOwnerColumn()->isGeneratedForeignKey())
		item->Enable(false);
	mnu.Append(MNU_RENAMECOLUMN, _("Rename the selected column..."));
	if(getOwnerColumn()->isGeneratedForeignKey() && !getOwnerColumn()->isFkNameGenerated())
		mnu.Append(MNU_AUTONAMCOLUMN, _("Activate fk auto-naming..."));
	mnu.AppendSeparator();
	item = mnu.AppendCheckItem(MNU_NOTNULL, _("Not NULL constraint"));
	if(getOwnerColumn()->isNotNull())
		item->Check(true);
	if(getOwnerColumn()->isGeneratedForeignKey())
		item->Enable(false);
	mnu.AppendSeparator();
	item = mnu.AppendCheckItem(MNU_PKEY, _("Primary Key"));
	if(getOwnerColumn()->isPrimaryKey())
		item->Check(true);
	if(getOwnerColumn()->isGeneratedForeignKey())
		item->Enable(false);
	item = mnu.AppendCheckItem(MNU_UKEY, _("Unique Key"));
	if(getOwnerColumn()->isUniqueKey())
		item->Check(true);
	mnu.AppendSeparator();
	submenu = new wxMenu();
	item = mnu.AppendSubMenu(submenu, _("Column datatype"));
	if(getOwnerColumn()->isGeneratedForeignKey())
		item->Enable(false);
	item = submenu->AppendCheckItem(MNU_TYPESERIAL, _("serial"));
	item->Check(columnType == dt_bigint);
	item = submenu->AppendCheckItem(MNU_TYPEBOOLEAN, _("boolean"));
	item->Check(columnType == dt_boolean);
	item = submenu->AppendCheckItem(MNU_TYPEINTEGER, _("integer"));
	item->Check(columnType == dt_integer);
	item = submenu->AppendCheckItem(MNU_TYPEMONEY, _("money"));
	item->Check(columnType == dt_money);
	item = submenu->AppendCheckItem(MNU_TYPEVARCHAR, _("varchar(n)"));
	item->Check(columnType == dt_varchar);
	item = submenu->Append(MNU_TYPEOTHER, _("Choose another datatype..."));
	mnu.AppendSeparator();
	mnu.Append(MNU_TYPEPKEY_CONSTRAINTNAME, _("Primary Key Constraint name..."));
	mnu.Append(MNU_TYPEUKEY_CONSTRAINTNAME, _("Unique Key Constraint name..."));
	mnu.AppendSeparator();
	mnu.Append(MNU_DELTABLE, _("Delete table..."));
};


const wxArrayString ddTextTableItemFigure::dataTypes()
{
	if(ddDatatypes.IsEmpty())
	{
		//Fast access ddDatatypes
		ddDatatypes.Add(wxT("ANY"));
		ddDatatypes.Add(wxT("serial"));
		ddDatatypes.Add(wxT("boolean"));
		ddDatatypes.Add(wxT("integer"));
		ddDatatypes.Add(wxT("money"));
		ddDatatypes.Add(wxT("varchar(n)"));
		//Normal access ddDatatypes
		ddDatatypes.Add(wxT("bigint"));
		ddDatatypes.Add(wxT("bit(n)"));
		ddDatatypes.Add(wxT("bytea"));
		ddDatatypes.Add(wxT("char(n)"));
		ddDatatypes.Add(wxT("cidr"));
		ddDatatypes.Add(wxT("circle"));
		ddDatatypes.Add(wxT("date"));
		ddDatatypes.Add(wxT("double precision"));
		ddDatatypes.Add(wxT("inet"));
		ddDatatypes.Add(wxT("interval(n)"));
		ddDatatypes.Add(wxT("line"));
		ddDatatypes.Add(wxT("lseg"));
		ddDatatypes.Add(wxT("macaddr"));
		ddDatatypes.Add(wxT("numeric(p,s)"));
		ddDatatypes.Add(wxT("path"));
		ddDatatypes.Add(wxT("point"));
		ddDatatypes.Add(wxT("polygon"));
		ddDatatypes.Add(wxT("real"));
		ddDatatypes.Add(wxT("smallint"));
		ddDatatypes.Add(wxT("text"));
		ddDatatypes.Add(wxT("time"));
		ddDatatypes.Add(wxT("timestamp"));
		ddDatatypes.Add(wxT("varbit(n)"));
	}
	return ddDatatypes;
}

void ddTextTableItemFigure::setText(wxString textString)
{
	hdSimpleTextFigure::setText(textString);

	//Hack to allow column text to submit new size of text signal to tablefigure
	//and then recalculate displaybox. Helps with fk autorenaming too.
	if(ownerColumn)
	{
		ownerColumn->displayBoxUpdate();
		ownerColumn->getOwnerTable()->updateTableSize();
		ownerColumn->getOwnerTable()->updateFkObservers();
	}
}

wxString ddTextTableItemFigure::getAlias()
{
	return colAlias;
}

//Activate use of alias or short names at ddtextTableItems like TableNames [Columns don' use it]
void ddTextTableItemFigure::setAlias(wxString alias)
{
	if(alias.length() <= 0 || alias.length() > 3 )
	{
		showAlias = false;
		colAlias = wxEmptyString;
	}
	else
	{
		showAlias = true;
		colAlias = alias;
	}
	recalculateDisplayBox();
	ownerTable->updateFkObservers(); //Only triggered by a tableName item [Not a column]
	ownerTable->updateTableSize();
}

void ddTextTableItemFigure::setOneTimeNoAlias()
{
	oneTimeNoAlias = true;
}

ddColumnFigure *ddTextTableItemFigure::getOwnerColumn()
{
	return ownerColumn;
}

void ddTextTableItemFigure::setOwnerColumn(ddColumnFigure *column)
{
	ownerColumn = column;
}

void ddTextTableItemFigure::setShowDataType(bool value)
{
	showDataType = value;
}

hdITool *ddTextTableItemFigure::CreateFigureTool(hdDrawingView *view, hdITool *defaultTool)
{
	if(getOwnerColumn())
	{
		return textEditable ? new ddColumnTextTool(view, this, defaultTool, false, wxT("New Column Name"), wxT("Rename Column")) : defaultTool;
	}
	else
	{
		setOneTimeNoAlias();
		return textEditable ? new ddColumnTextTool(view, this, defaultTool, false, wxT("New Table Name"), wxT("Rename Table")) : defaultTool;
	}
}

int ddTextTableItemFigure::getTextWidth()
{
	int w, h;
	getFontMetrics(w, h);
	return w;
}

int ddTextTableItemFigure::getTextHeight()
{
	int w, h;
	getFontMetrics(w, h);
	return h;
}

ddDataType ddTextTableItemFigure::getDataType()
{
	if(!getOwnerColumn()->isGeneratedForeignKey())
		return columnType;
	else
	{
		columnType = getOwnerColumn()->getFkSource()->original->getDataType();
		return columnType;
	}
}

void ddTextTableItemFigure::setDataType(ddDataType type)
{
	columnType = type;
	ownerColumn->getOwnerTable()->updateSizeOfObservers();
}

int ddTextTableItemFigure::getPrecision()
{
	if(getOwnerColumn()->isGeneratedForeignKey())
	{
		precision = getOwnerColumn()->getFkSource()->original->getPrecision();
		return precision;
	}
	else
	{
		return precision;
	}
}

void ddTextTableItemFigure::setPrecision(int value)
{
	if(!getOwnerColumn()->isGeneratedForeignKey())
	{
		precision = value;
		ownerColumn->getOwnerTable()->updateSizeOfObservers();
	}
}

void ddTextTableItemFigure::setScale(int value)
{
	if(!getOwnerColumn()->isGeneratedForeignKey())
	{
		scale = value;
		ownerColumn->getOwnerTable()->updateSizeOfObservers();
	}
}

int ddTextTableItemFigure::getScale()
{
	if(getOwnerColumn()->isGeneratedForeignKey())
	{
		scale = getOwnerColumn()->getFkSource()->original->getScale();
		return scale;
	}
	else
	{
		return scale;
	}
}
