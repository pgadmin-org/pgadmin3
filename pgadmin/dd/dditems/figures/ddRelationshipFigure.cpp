//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddRelationshipFigure.cpp - Figure to draw foreign keys between tables.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "dd/dditems/figures/ddRelationshipItem.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"
#include "dd/dditems/utilities/ddDataType.h"
#include "dd/dditems/utilities/ddSelectKindFksDialog.h"

ddRelationshipFigure::ddRelationshipFigure():
	wxhdLineConnection()
{
	constraintName = wxEmptyString;
	setKindId(DDRELATIONSHIPFIGURE);
	fkFromPk = true;
	fkMandatory = true;
	fkOneToMany = true;
	fkIdentifying = false;
	ukIndex = -1;
	disconnectedEndTable = NULL;
	paintingFkColumns = false;
	//DEFERRABLE, NOT DEFERRABLE, INITIALLY IMMEDIATE, INITIALLY DEFERRED can be added in a future
	onUpdateAction = FK_ACTION_NO;
	onDeleteAction = FK_ACTION_NO;
	enablePopUp();
}

ddRelationshipFigure::ddRelationshipFigure(wxhdIFigure *figure1, wxhdIFigure *figure2):
	wxhdLineConnection(figure1, figure2)
{
	enablePopUp();
}

ddRelationshipFigure::~ddRelationshipFigure()
{
	columnsHashMap::iterator it;
	ddRelationshipItem *item;
	for (it = chm.begin(); it != chm.end(); ++it)
	{
		wxString key = it->first;
		item = it->second;
		delete item;
	}
	chm.clear();
}

wxString ddRelationshipFigure::getConstraintName()
{
	return constraintName;
}

//This function avoid a bug with recursive indentifying relationships when
//a column is removed from pk or delete.
void ddRelationshipFigure::prepareFkForDelete(ddColumnFigure *column)
{
	ddTableFigure *endTable = (ddTableFigure *) getEndFigure();
	ddRelationshipItem *fkColumnRelItem;

	columnsHashMap::iterator it;
	for (it = chm.begin(); it != chm.end(); ++it)
	{
		fkColumnRelItem = it->second;
		if(fkColumnRelItem->original == column)
			fkColumnRelItem->original = NULL;
	}
	//go recursive to next table
	if(endTable)
		endTable->prepareForDeleteFkColumn(column);
}

void ddRelationshipFigure::updateForeignKey()
{
	if(getEndFigure() && getStartFigure() && getStartFigure()->ms_classInfo.IsKindOf(&ddTableFigure::ms_classInfo) && getEndFigure()->ms_classInfo.IsKindOf(&ddTableFigure::ms_classInfo))
	{
		ddTableFigure *startTable = (ddTableFigure *) getStartFigure();
		ddTableFigure *endTable = (ddTableFigure *) getEndFigure();
		ddColumnFigure *col;
		ddRelationshipItem *fkColumnRelItem;

		//STEP 0: Look for changes on source columns names and short table names.

		//Before iterate over all columns of source table look if any column used at relationship
		//and changed their name and updated it when needed
		columnsHashMap::iterator it;
		for (it = chm.begin(); it != chm.end(); ++it)
		{
			wxString key = it->first;
			fkColumnRelItem = it->second;
			if(fkColumnRelItem->original != NULL) //original column wasn't mark for delete.
			{
				//if autonaming is set on this item and column name at original source column of fk have been changed
				if(!fkColumnRelItem->original->getColumnName(false).IsSameAs(fkColumnRelItem->originalStartColName, false))
				{
					chm[fkColumnRelItem->original->getColumnName(false)] = fkColumnRelItem;
					chm[fkColumnRelItem->originalStartColName] = NULL;
					chm.erase(it);
					fkColumnRelItem->syncAutoFkName();
					break;
				}
			}
		}

		//Update all column with auto FK naming with different shortname
		for (it = chm.begin(); it != chm.end(); ++it)
		{
			fkColumnRelItem = it->second;
			if(fkColumnRelItem->original != NULL) //original column wasn't mark for delete.
			{
				if(!fkColumnRelItem->original->getOwnerTable()->getShortTableName().IsSameAs(fkColumnRelItem->originalShortName, false))
				{
					fkColumnRelItem->syncAutoFkName();
				}
			}
		}

		//STEP 1: Look at all source table columns add and delete fk
		wxhdIteratorBase *iterator = startTable->figuresEnumerator();
		iterator->Next(); //first figure is main rect
		iterator->Next(); //second figure is table title
		while(iterator->HasNext())
		{
			col = (ddColumnFigure *) iterator->Next();
			if(fkFromPk)  //fk is generated from a pk column: add new fk/pk(s) column(s) from source fk table to destination
			{
				//STEP 1.1a: Add fk columns from table source pk if not exists using same name
				it = chm.find(col->getColumnName());
				bool NotFound = it == chm.end();

				if( col->isPrimaryKey() && NotFound )
				{
					fkColumnRelItem = new ddRelationshipItem(this, col, endTable, (fkMandatory ? notnull : null), (fkIdentifying ? pk : none) );
					chm[col->getColumnName()] = fkColumnRelItem; //hashmap key will be original table name always
					endTable->addColumn(fkColumnRelItem->fkColumn);
					updateConnection();
				}

				//STEP 1.2a: Delete old Fk columns not pk now or deleted from source fk table.
				//This part of code (repeat) is a hack cause every time a column is delete hashmap is modified inside and becomes invalid iterator at that loop
				bool repeat;
				do
				{
					repeat = false;
					for (it = chm.begin(); it != chm.end(); ++it)
					{
						wxString key = it->first;
						fkColumnRelItem = it->second;
						if( fkColumnRelItem->original == NULL || !fkColumnRelItem->original->isPrimaryKey() || !startTable->includes(fkColumnRelItem->original) ) //order matters fkColumnRelItem->original==NULL short circuit evaluation should be first
						{
							if(fkColumnRelItem->isAutomaticallyGenerated()) //don't remove from fk_dest table fk column created from existing column, just mark now as not foreign key
							{
								fkColumnRelItem->destinationTable->removeColumn(fkColumnRelItem->fkColumn);
							}
							else
							{
								fkColumnRelItem->fkColumn->setAsUserCreatedFk(NULL);
							}
							chm.erase(it);
							delete fkColumnRelItem;
							repeat = true;
							updateConnection();
						}
						if (repeat)
							break;
					}
				}
				while(repeat);
			}
			else   //fk is generated from a uk column: add new fk/pk(s) column(s) from source fk table to destination
			{
				//STEP 1.1b: Add fk columns from table source uk if not exists
				columnsHashMap::iterator it = chm.find(col->getColumnName());
				bool NotFound = it == chm.end();

				if( col->isUniqueKey(ukIndex) && NotFound )
				{
					fkColumnRelItem = new ddRelationshipItem(this, col, endTable, (fkMandatory ? notnull : null), (fkIdentifying ? pk : none) );
					chm[col->getColumnName()] = fkColumnRelItem; //hashmap key will be original table name always
					endTable->addColumn(fkColumnRelItem->fkColumn);
					updateConnection();
				}

				//STEP 1.2b: Delete old Fk columns not pk now or deleted from source fk table.
				//This part of code (repeat) is a hack cause every time a column is delete hashmap is modified inside and becomes invalid iterator at that loop
				bool repeat;
				do
				{
					repeat = false;
					for (it = chm.begin(); it != chm.end(); ++it)
					{
						wxString key = it->first;
						fkColumnRelItem = it->second;
						if( fkColumnRelItem->original == NULL || !fkColumnRelItem->original->isUniqueKey(ukIndex) || !startTable->includes(fkColumnRelItem->original) ) //order matters fkColumnRelItem->original==NULL short circuit evaluation should be first
						{
							if(fkColumnRelItem->isAutomaticallyGenerated()) //don't remove from fk_dest table fk column created from existing column, just mark now as not foreign key
							{
								fkColumnRelItem->destinationTable->removeColumn(fkColumnRelItem->fkColumn);
							}
							else
							{
								fkColumnRelItem->fkColumn->setAsUserCreatedFk(NULL);
							}
							chm.erase(it);
							delete fkColumnRelItem;
							repeat = true;
							updateConnection();
						}
						if (repeat)
							break;
					}
				}
				while(repeat);

			}
		}
		delete iterator;
		//Now if relationship is an identifying one, I should alert all observers of my observer to update it.
		if(fkIdentifying)
		{
			if(endTable)
				endTable->updateFkObservers();
		}
	}
	else
	{
		wxMessageBox(wxT("Error invalid kind of start figure at relationship"), wxT("Error invalid kind of start figure at relationship"), wxICON_ERROR);
	}
}

void ddRelationshipFigure::createMenu(wxMenu &mnu)
{
	wxMenu *submenu;
	wxMenuItem *item;
	wxString tmp;

	submenu = new wxMenu(_("Select one"));

	tmp = _("Foreign Key From");
	mnu.AppendSubMenu(submenu, tmp);
	item = submenu->AppendCheckItem(MNU_FKEYFROMPKEY, _("Primary Key"));
	item->Check(fkFromPk);

	if(getStartFigure())
	{
		ddTableFigure *startTable = (ddTableFigure *) getStartFigure();
		int i, last = startTable->getUkConstraintsNames().Count();
		int eventID;  //Hack to allow multiple submenu items for select uk as fk origin
		for(i = 0; i < last; i++)
		{
			eventID =  MNU_FKEYFROMUKEYBASE + i;
			tmp = _("Unique Key: ");
			tmp += startTable->getUkConstraintsNames()[i];
			item = submenu->AppendCheckItem(eventID, tmp );
			item->Check(!fkFromPk && ukIndex == i);
		}
	}
	mnu.AppendSeparator();
	//disable right now item = mnu.AppendCheckItem(MNU_FKEYCUSTOMMAPPING, _("Foreign Key Columns Custom Mapping"));
	item = mnu.AppendCheckItem(MNU_MANDATORYRELATIONSHIP, _("Mandatory relationship kind"));
	item->Check(fkMandatory);
	item = mnu.AppendCheckItem(MNU_IDENTIFYINGRELATIONSHIP, _("Identifying relationship"));
	item->Check(fkIdentifying);
	mnu.AppendSeparator();
	item = mnu.AppendCheckItem(MNU_1MRELATIONSHIP, _("1:M"));
	item->Check(fkOneToMany);
	mnu.AppendSeparator();
	mnu.Append(MNU_FKCONSTRAINTNAME, _("Foreign Key Constraint Name"));

	submenu = new wxMenu(_("Select one"));
	item = submenu->AppendCheckItem(MNU_FKMATCHTYPESIMPLE, _("Type Simple"));
	item->Check(matchSimple);
	item = submenu->AppendCheckItem(MNU_FKMATCHTYPEFULL, _("Type Full"));
	item->Check(!matchSimple);
	tmp = _("Match Type ");
	if(matchSimple)
		tmp += _("[ Simple ]");
	else
		tmp += _("[ Full ]");
	mnu.AppendSubMenu(submenu, tmp);

	tmp = _("On Delete ");
	submenu = new wxMenu(_("Select one"));
	item = submenu->AppendCheckItem(MNU_FKONDELETENOACTION, _("No Action"));
	item->Check(onDeleteAction == FK_ACTION_NO);
	if(onDeleteAction == FK_ACTION_NO)
		tmp += _("[ No Action ]");
	item = submenu->AppendCheckItem(MNU_FKONDELETERESTRICT, _("Restrict"));
	item->Check(onDeleteAction == FK_RESTRICT);
	if(onDeleteAction == FK_RESTRICT)
		tmp += _("[ Restrict ]");
	item = submenu->AppendCheckItem(MNU_FKONDELETECASCADE, _("Cascade"));
	item->Check(onDeleteAction == FK_CASCADE);
	if(onDeleteAction == FK_CASCADE)
		tmp += _("[ Cascade ]");
	item = submenu->AppendCheckItem(MNU_FKONDELETESETNULL, _("Set Null"));
	item->Check(onDeleteAction == FK_SETNULL);
	if(onDeleteAction == FK_SETNULL)
		tmp += _("[ Set Null ]");
	item = submenu->AppendCheckItem(MNU_FKONDELETESETDEFAULT, _("Set Default"));
	item->Check(onDeleteAction == FK_SETDEFAULT);
	if(onDeleteAction == FK_SETDEFAULT)
		tmp += _("[ Set Default ]");
	mnu.AppendSubMenu(submenu, tmp);

	tmp = _("On Update ");
	submenu = new wxMenu(_("Select one"));
	item = submenu->AppendCheckItem(MNU_FKONUPDATENOACTION, _("No Action"));
	item->Check(onUpdateAction == FK_ACTION_NO);
	if(onUpdateAction == FK_ACTION_NO)
		tmp += _("[ No Action ]");
	item = submenu->AppendCheckItem(MNU_FKONUPDATERESTRICT, _("Restrict"));
	item->Check(onUpdateAction == FK_RESTRICT);
	if(onUpdateAction == FK_RESTRICT)
		tmp += _("[ Restrict ]");
	item = submenu->AppendCheckItem(MNU_FKONUPDATECASCADE, _("Cascade"));
	item->Check(onUpdateAction == FK_CASCADE);
	if(onUpdateAction == FK_CASCADE)
		tmp += _("[ Cascade ]");
	item = submenu->AppendCheckItem(MNU_FKONUPDATESETNULL, _("Set Null"));
	item->Check(onUpdateAction == FK_SETNULL);
	if(onUpdateAction == FK_SETNULL)
		tmp += _("[ Set Null ]");
	item = submenu->AppendCheckItem(MNU_FKONUPDATESETDEFAULT, _("Set Default"));
	item->Check(onUpdateAction == FK_SETDEFAULT);
	if(onUpdateAction == FK_SETDEFAULT)
		tmp += _("[ Set Default ]");
	mnu.AppendSubMenu(submenu, tmp);

	mnu.AppendSeparator();
	mnu.Append(MNU_DELETERELATIONSHIP, _("Delete Relationship..."));
};

void ddRelationshipFigure::OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view)
{
	int answer;
	ddTableFigure *startTable = NULL;
	ddTableFigure *endTable = NULL;
	wxTextEntryDialog *nameDialog = NULL;
	ddSelectKindFksDialog *mappingDialog = NULL;
	wxString tmpString;

	switch(event.GetId())
	{
		case MNU_MANDATORYRELATIONSHIP:
			fkMandatory = !fkMandatory;
			if(fkMandatory)
			{
				setLinePen(wxPen(*wxBLACK_PEN));
				setOptionAtForeignKeys(notnull);
			}
			else
			{
				fkIdentifying = false;
				setLinePen(wxPen(*wxBLACK, 1, wxSHORT_DASH));
				setOptionAtForeignKeys(null);
			}
			break;
		case MNU_IDENTIFYINGRELATIONSHIP:
			fkMandatory = true;
			setLinePen(wxPen(*wxBLACK_PEN));
			fkIdentifying = !fkIdentifying;
			fkOneToMany = true;
			updatePkAtFkCols();
			break;
		case MNU_FKCONSTRAINTNAME:
			startTable = (ddTableFigure *) getStartFigure();
			endTable = (ddTableFigure *) getEndFigure();
			if(constraintName.IsEmpty() && startTable && endTable )
			{
				if(!endTable->getShortTableName().IsEmpty())
					constraintName = endTable->getShortTableName();
				else
					constraintName = endTable->getTableName();
				constraintName += _("_");
				if(!startTable->getShortTableName().IsEmpty())
					constraintName += startTable->getShortTableName();
				else
					constraintName += startTable->getTableName();
			}
			nameDialog = new wxTextEntryDialog(view, wxT("Change Constraint Name"), wxT("Constraint Name"), constraintName );
			answer = nameDialog->ShowModal();
			if (answer == wxID_OK)
			{
				tmpString = nameDialog->GetValue();
				constraintName = tmpString;
			}
			delete nameDialog;
			break;
		case MNU_FKMATCHTYPEFULL:
			matchSimple = false;
			break;
		case MNU_FKMATCHTYPESIMPLE:
			matchSimple = true;
			break;

		case MNU_FKONDELETENOACTION:
			onDeleteAction = FK_ACTION_NO;
			break;
		case MNU_FKONDELETERESTRICT:
			onDeleteAction = FK_RESTRICT;
			break;
		case MNU_FKONDELETECASCADE:
			onDeleteAction = FK_CASCADE;
			break;
		case MNU_FKONDELETESETNULL:
			onDeleteAction = FK_SETNULL;
			break;
		case MNU_FKONDELETESETDEFAULT:
			onDeleteAction = FK_SETDEFAULT;
			break;

		case MNU_FKONUPDATENOACTION:
			onUpdateAction = FK_ACTION_NO;
			break;
		case MNU_FKONUPDATERESTRICT:
			onUpdateAction = FK_RESTRICT;
			break;
		case MNU_FKONUPDATECASCADE:
			onUpdateAction = FK_CASCADE;
			break;
		case MNU_FKONUPDATESETNULL:
			onUpdateAction = FK_SETNULL;
			break;
		case MNU_FKONUPDATESETDEFAULT:
			onUpdateAction = FK_SETDEFAULT;
			break;

		case MNU_1MRELATIONSHIP:
			fkOneToMany = !fkOneToMany;
			break;
		case MNU_DELETERELATIONSHIP:
			if(getStartFigure() && getEndFigure())
			{
				ddTableFigure *t1 = (ddTableFigure *)getStartFigure();
				ddTableFigure *t2 = (ddTableFigure *)getEndFigure();
				answer = wxMessageBox(wxT("Are you sure you wish to delete relationship between tables ") + t1->getTableName() + wxT(" and ") + t2->getTableName() + wxT("?"), wxT("Delete relationship?"), wxYES_NO | wxNO_DEFAULT, (wxScrolledWindow *)view);
				if (answer == wxYES)
				{
					if(view->isFigureSelected(this))
						view->removeFromSelection(this);
					disconnectStart();
					disconnectEnd();
					//Hack to autodelete relationship
					ddRelationshipFigure *r = this;
					view->remove(this);
					if(r)
						delete r;
				}
			}
			break;
		case MNU_FKEYFROMPKEY:
			fkFromPk = true;
			updateForeignKey();
			break;
		case MNU_FKEYCUSTOMMAPPING:
			//disable right now
			/*
			mappingDialog = new ddSelectKindFksDialog(view,this);
			mappingDialog->ShowModal();
			delete mappingDialog;
			*/
			break;
		default:
			answer = event.GetId();
			if( answer >= MNU_FKEYFROMUKEYBASE)  //Hack to allow multiple selection of Uk in submenu
			{
				fkFromPk = false;
				ukIndex = answer - MNU_FKEYFROMUKEYBASE;
				updateForeignKey();
			}
			break;
	}
}

bool ddRelationshipFigure::getIdentifying()
{
	return fkIdentifying;
}

bool ddRelationshipFigure::getOneToMany()
{
	return fkOneToMany;
}

bool ddRelationshipFigure::getMandatory()
{
	return fkMandatory;
}


//	relationship is observed by several tables at same time, one is the
//	owner (start connector table) others are just observers of that
//	relationship (end connectors table)
void ddRelationshipFigure::connectEnd(wxhdIConnector *end, wxhdDrawingView *view)
{
	ddSelectKindFksDialog *mappingDialog = NULL;
	wxhdLineConnection::connectEnd(end);
	view->Refresh();
	if(getEndFigure() && getStartFigure())
	{
		mappingDialog = new ddSelectKindFksDialog(view, this);
		mappingDialog->ShowModal();
		delete mappingDialog;
		if(view)
		{
			view->AcceptsFocus();
			view->SetFocus();
		}
		updateForeignKey();
	}

	if(getStartFigure())
		getStartTable()->setSelectFkDestMode(false);
}

bool ddRelationshipFigure::isForeignKeyFromPk()
{
	return fkFromPk;
}

void ddRelationshipFigure::setFkFrom(bool primaryKey, int useUkIndex, bool issueUpdateFk)
{
	if(useUkIndex >= 0)
	{
		fkFromPk = false;
		ukIndex = useUkIndex;
	}
	else
	{
		fkFromPk = true;
		ukIndex = -1;
	}

	if(issueUpdateFk)
		updateForeignKey();
}

void ddRelationshipFigure::connectStart(wxhdIConnector *start, wxhdDrawingView *view)
{
	wxhdLineConnection::connectStart(start);
	if(getEndFigure() && getStartFigure())
		updateForeignKey();
}

void ddRelationshipFigure::disconnectStart(wxhdDrawingView *view)
{
	paintingFkColumns = false;
	changeFkOSTextColor( *wxBLACK, *wxBLACK, true );
	disconnectedEndTable = (ddTableFigure *) getEndFigure();
	removeForeignKeys();
	wxhdLineConnection::disconnectStart();
}

void ddRelationshipFigure::disconnectEnd(wxhdDrawingView *view)
{
	paintingFkColumns = false;
	changeFkOSTextColor( *wxBLACK, *wxBLACK, true );
	disconnectedEndTable = (ddTableFigure *) getEndFigure();
	wxhdLineConnection::disconnectEnd();
	removeForeignKeys();
}

void ddRelationshipFigure::addExistingColumnFk(ddColumnFigure *startTablesourceCol, wxString destColumn)
{
	ddTableFigure *endTable = (ddTableFigure *) getEndFigure();
	ddRelationshipItem *fkColumnRelItem;
	ddColumnFigure *endTablesourceCol = endTable->getColumnByName(destColumn);
	//Create a new relationship item but with an existing column for fk at destination table
	if(endTablesourceCol)
	{
		fkColumnRelItem = new ddRelationshipItem(this, startTablesourceCol, endTable, (fkMandatory ? notnull : null), (fkIdentifying ? pk : noaction), endTablesourceCol);
		//Mark it as Custom Fk (fk from existing column not an automatic generated)
		endTablesourceCol->setAsUserCreatedFk(fkColumnRelItem);
		chm[startTablesourceCol->getColumnName()] = fkColumnRelItem; //hashmap key will be original table name always
		updateConnection();
	}
}

void ddRelationshipFigure::removeForeignKeys()
{
	if(disconnectedEndTable)
	{
		columnsHashMap::iterator it;
		ddRelationshipItem *fkColumnRelItem;

		//This part of code (repeat) is a hack cause every time a column is delete hashmap is modified inside and becomes invalid iterator at that loop
		bool repeat;
		do
		{
			repeat = false;
			for( it = chm.begin(); it != chm.end(); ++it )
			{
				wxString key = it->first;
				fkColumnRelItem = it->second;
				if(fkColumnRelItem->destinationTable->includes(fkColumnRelItem->fkColumn))
				{
					//Remove fk column only if that column is automatically generated
					if(fkColumnRelItem->isAutomaticallyGenerated())
					{
						fkColumnRelItem->destinationTable->removeColumn(fkColumnRelItem->fkColumn);
					} //is an existing column use as fk
					else
					{
						//Mark as existing column not used as foreign key destination
						fkColumnRelItem->fkColumn->setAsUserCreatedFk(NULL);
					}
					chm.erase(it);
					delete fkColumnRelItem;
					repeat = true;
					break;
				}
			}
		}
		while(repeat);
		chm.clear();
		disconnectedEndTable = NULL;
	}
}

void ddRelationshipFigure::setOptionAtForeignKeys(ddColumnOptionType type)
{
	columnsHashMap::iterator it;
	ddRelationshipItem *item;
	for (it = chm.begin(); it != chm.end(); ++it)
	{
		wxString key = it->first;
		item = it->second;
		if(item->isAutomaticallyGenerated())
			item->fkColumn->setColumnOption(type);
	}
}

void ddRelationshipFigure::updatePkAtFkCols()
{
	bool changed = false;
	columnsHashMap::iterator it;
	ddRelationshipItem *item;
	for (it = chm.begin(); it != chm.end(); ++it)
	{
		wxString key = it->first;
		item = it->second;
		if(item->isAutomaticallyGenerated())  //only update fk status at fk NOT created from an existing column
		{
			if(fkIdentifying)
			{
				item->fkColumn->enablePrimaryKey();
			}
			else
			{
				item->fkColumn->disablePrimaryKey();
			}
			changed = true;
		}
	}

	if(changed) //set as identifying relationship (hierarchy)
	{
		ddTableFigure *table = (ddTableFigure *) getEndFigure();
		table->updateFkObservers();
	}
}
wxString ddRelationshipFigure::generateSQL()
{
	wxString tmp;
	if(chm.size() > 0)
	{
		if(!constraintName.IsEmpty())
		{
			tmp = _("CONSTRAINT ");
			tmp += constraintName;
			tmp += _(" ");
		}
		tmp += wxT("FOREIGN KEY ( ");
		columnsHashMap::iterator it, end;
		ddRelationshipItem *item;
		for( it = chm.begin(); it != chm.end(); ++it )
		{
			wxString key = it->first;
			item = it->second;
			tmp += item->fkColumn->getColumnName();
			end = it;
			end++;
			if(end != chm.end())
			{
				tmp += wxT(" , ");
			}
			else
			{
				tmp += wxT(" )");
			}
		}

		tmp += wxT(" REFERENCES ") + ((ddTableFigure *)getStartFigure())->getTableName() + wxT(" ( ");
		for( it = chm.begin(); it != chm.end(); ++it )
		{
			wxString key = it->first;
			item = it->second;
			tmp += item->original->getColumnName();
			end = it;
			end++;
			if(end != chm.end())
			{
				tmp += wxT(" , ");
			}
			else
			{
				tmp += wxT(" )");
			}
		}

		if(matchSimple)
			tmp += wxT(" MATCH SIMPLE ");
		else
			tmp += wxT(" MATCH FULL ");

		tmp += wxT(" ON DELETE ");
		switch(onDeleteAction)
		{
			case FK_ACTION_NO:
				tmp += wxT(" NO ACTION ");
				break;
			case FK_RESTRICT:
				tmp += wxT(" RESTRICT ");
				break;
			case FK_CASCADE:
				tmp += wxT(" CASCADE ");
				break;
			case FK_SETNULL:
				tmp += wxT(" SET NULL ");
				break;
			case FK_SETDEFAULT:
				tmp += wxT(" SET DEFAULT ");
				break;
		}

		tmp += wxT(" ON UPDATE ");
		switch(onUpdateAction)
		{
			case FK_ACTION_NO:
				tmp += wxT(" NO ACTION ");
				break;
			case FK_RESTRICT:
				tmp += wxT(" RESTRICT ");
				break;
			case FK_CASCADE:
				tmp += wxT(" CASCADE ");
				break;
			case FK_SETNULL:
				tmp += wxT(" SET NULL ");
				break;
			case FK_SETDEFAULT:
				tmp += wxT(" SET DEFAULT ");
				break;
		}

	}
	return tmp;
}

ddTableFigure *ddRelationshipFigure::getStartTable()
{
	return (ddTableFigure *) getStartFigure();
}

ddTableFigure *ddRelationshipFigure::getEndTable()
{
	return (ddTableFigure *) getEndFigure();
}

void ddRelationshipFigure::basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdLineConnection::basicDrawSelected(context, view);
	if(getEndFigure() && getStartFigure())
	{
		paintingFkColumns = true;
		changeFkOSTextColor(wxColour(255, 0, 0), wxColour(255, 0, 0));
	}
}

void ddRelationshipFigure::basicDraw(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdLineConnection::basicDraw(context, view);
	if(getEndFigure() && getStartFigure() && paintingFkColumns)
	{
		paintingFkColumns = false;
		changeFkOSTextColor( *wxBLACK, *wxBLACK, true );
	}
}

void ddRelationshipFigure::changeFkOSTextColor(wxColour originalColour, wxColour fkColour, bool reset)
{
	columnsHashMap::iterator it;
	ddRelationshipItem *item;
	for (it = chm.begin(); it != chm.end(); ++it)
	{
		wxString key = it->first;
		item = it->second;
		if(item->original)
		{
			if(!reset)
				item->original->setTextColour(originalColour);
			else
				item->original->setTextColour(item->original->getOwnerTable()->fontColorAttribute->fontColor);

		}
		if(item->fkColumn)
		{
			if(!reset)
				item->fkColumn->setTextColour(fkColour);
			else
				item->fkColumn->setTextColour(item->fkColumn->getOwnerTable()->fontColorAttribute->fontColor);
		}
	}

}