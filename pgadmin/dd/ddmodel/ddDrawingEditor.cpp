//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawingEditor.cpp - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/ddmodel/ddDrawingEditor.h"
#include "dd/dditems/utilities/ddTableNameDialog.h"
#include "dd/ddmodel/ddDatabaseDesign.h"
#include "dd/ddmodel/ddDrawingView.h"
#include "dd/wxhotdraw/utilities/wxhdRemoveDeleteDialog.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "frm/frmDatabaseDesigner.h"


ddDrawingEditor::ddDrawingEditor(wxWindow *owner,  wxWindow *frmOwner, ddDatabaseDesign *design)
	: wxhdDrawingEditor(owner, true)
{
	databaseDesign = design;
	frm = (frmDatabaseDesigner *) frmOwner;
}

wxhdDrawing *ddDrawingEditor::createDiagram(wxWindow *owner, bool fromXml)
{

	wxhdDrawing *_tmpModel = new wxhdDrawing(this);

	ddDrawingView *_viewTmp = new ddDrawingView(_diagrams->count(), owner, this, wxSize(1200, 1200), _tmpModel);

	// Set Scroll Bar & split
	_viewTmp->SetScrollbars( 10, 10, 127, 80 );
	_viewTmp->EnableScrolling(true, true);
	_viewTmp->AdjustScrollbars();

	_tmpModel->registerView(_viewTmp);

	_viewTmp->SetDropTarget(new ddDropTarget(databaseDesign, _tmpModel));

	if(!fromXml)
	{
		//Add a new position inside each figure to allow use of this new diagram existing figures.
		int i;
		wxhdIFigure *tmp;
		for(i = 0; i < _model->count(); i++)
		{
			tmp = (wxhdIFigure *) _model->getItemAt(i);
			tmp->AddPosForNewDiagram();
		}
	}
	//Add Diagram
	_diagrams->addItem((wxhdObject *) _tmpModel);
	modelChanged = true;
	return _tmpModel;
}


void ddDrawingEditor::remOrDelSelFigures(int diagramIndex)
{
	wxhdIFigure *tmp;
	ddTableFigure *table;
	ddRelationshipFigure *relation;
	wxhdIteratorBase *iterator;
	wxhdCollection *tmpSelection;
	int answer;
	int numbTables = 0;
	int numbRelationships = 0;

	if (getExistingDiagram(diagramIndex)->countSelectedFigures() == 1)
	{
		tmp = (wxhdIFigure *) getExistingDiagram(diagramIndex)->selectedFigures()->getItemAt(0);
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			numbTables = 1;
			table = (ddTableFigure *)tmp;
			wxhdRemoveDeleteDialog dialog(_("Are you sure you wish to delete table ") + table->getTableName() + wxT("?"), _("Delete table?"), getExistingView(diagramIndex));
			answer = dialog.ShowModal();
		}
		if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
		{
			numbRelationships = 1;
			relation = (ddRelationshipFigure *)tmp;
			 //Relationship can be delete only NOT REMOVED
			wxhdRemoveDeleteDialog dialog2(_("Are you sure you wish to delete relationship ") + relation->getConstraintName() + wxT("?"), _("Delete relationship?"), getExistingView(diagramIndex),false);
			answer = dialog2.ShowModal();
		}
	}
	else if (getExistingDiagram(diagramIndex)->countSelectedFigures() > 1)
	{
		numbTables = 0;
		iterator = getExistingDiagram(diagramIndex)->selectionFigures();
		while(iterator->HasNext())
		{
			tmp = (wxhdIFigure *)iterator->Next();
			if(tmp->getKindId() == DDTABLEFIGURE)
				numbTables++;
		}
		delete iterator;


		//Improve messages to display about relationships and tables and only relationship
		wxhdRemoveDeleteDialog dialog3(
		    wxString::Format(_("Are you sure you wish to delete %d tables, removing from model related relationships?"), numbTables),
		    _("Delete tables?"), getExistingView(diagramIndex));
		answer = dialog3.ShowModal();
	}

	if (answer == DD_DELETE || answer == DD_REMOVE)
	{
		modelChanged = true;
		tmpSelection =  new wxhdCollection(new wxhdArrayCollection());

		//Preprocess relationships counting and storing at temporary collection
		numbRelationships = 0;
		iterator = getExistingDiagram(diagramIndex)->selectionFigures();
		while(iterator->HasNext())
		{
			tmp = (wxhdIFigure *)iterator->Next();
			if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
			{
				numbRelationships++;
				tmpSelection->addItem(tmp);
			}
		}
		delete iterator;

		while(numbTables > 0)
		{
			tmp = (wxhdIFigure *) getExistingDiagram(diagramIndex)->selectedFigures()->getItemAt(0);
			if(tmp->getKindId() == DDTABLEFIGURE)
			{
				table = (ddTableFigure *)tmp;
				if(table && answer == DD_REMOVE)
				{
					getExistingDiagram(diagramIndex)->removeFromSelection(table);
					getExistingDiagram(diagramIndex)->remove(table);
				}
				//if table is going to be delete all others diagrams should be alerted about it
				if(table && answer == DD_DELETE)
				{
					removeFromAllSelections(table);
					table->processDeleteAlert(getExistingDiagram(diagramIndex));
					deleteModelFigure(table);
					databaseDesign->refreshBrowser();
				}
				numbTables--;
			}
			else if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
			{
				getExistingDiagram(diagramIndex)->removeFromSelection(tmp); //isn't a tables is probably a relationship
			}
			else
			{
				wxMessageBox(_("Invalid figure kind found"), _("Error while deleting tables"), wxCENTRE | wxICON_ERROR, getExistingView(diagramIndex));
			}
		}

		if( numbRelationships > 0 )
		{

			//check again: Are there relationships selected that wasn't delete (only selected relationship not source/destination table)
			numbRelationships = 0;
			iterator = tmpSelection->createIterator();  //getExistingDiagram(diagramIndex)->selectionFigures();
			while(iterator->HasNext())
			{
				tmp = (wxhdIFigure *)iterator->Next();
				//only way a relationship don't exists at diagram is
				//it had been deleted before by deleting source/destination table that owns it
				if(getExistingDiagram(diagramIndex)->includes(tmp))
				{
					if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
					{
						numbRelationships++;
					}
					else
					{
						wxMessageBox(_("Invalid figure kind found"), _("Error while deleting table"), wxCENTRE | wxICON_ERROR, getExistingView(diagramIndex));
					}
				}
				else
				{
					tmpSelection->removeItem(tmp);
				}
			}
			delete iterator;

			while(numbRelationships > 0 && tmpSelection->count() == numbRelationships)
			{
				tmp = (wxhdIFigure *) tmpSelection->getItemAt(0);
				if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
				{
					relation = (ddRelationshipFigure *)tmp;
					if(relation && answer == DD_REMOVE)
					{
						getExistingDiagram(diagramIndex)->removeFromSelection(relation);
						getExistingDiagram(diagramIndex)->remove(relation);
					}
					//if relation is going to be delete all others diagrams should be alerted about it
					if(relation && answer == DD_DELETE)
					{
						removeFromAllSelections(relation);
						relation->removeForeignKeys();
						relation->disconnectEnd();
						relation->disconnectStart();
						deleteModelFigure(relation);
						databaseDesign->refreshBrowser();
					}
					numbRelationships--;
				}
				else
				{
					wxMessageBox(_("Invalid figure kind found"), _("Error while deleting realtionships"), wxCENTRE | wxICON_ERROR, getExistingView(diagramIndex));
					//	getExistingDiagram(diagramIndex)->removeFromSelection(tmp); //isn't neither a table or relationship
				}
			}
		}
		getExistingDiagram(diagramIndex)->clearSelection();  //after delete all items all relationships remains at selection and should be removed
		tmpSelection->removeAll();
		delete tmpSelection;
	}
}

void ddDrawingEditor::checkRelationshipsConsistency(int diagramIndex)
{
	wxhdIFigure *tmp;
	ddRelationshipFigure *relation;
	wxhdDrawing *diagram = getExistingDiagram(diagramIndex);

	// First Step Removel all orphan [relations without source or destination] relationships
	// from DIAGRAM but NOT from MODEL
	wxhdIteratorBase *iterator = diagram->figuresEnumerator();
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
		{
			relation = (ddRelationshipFigure *)tmp;
			//test if all tables of a relationship are included if this is not the case then remove relationship from this diagram
			bool sourceExists = diagram->getFiguresCollection()->existsObject(relation->getStartTable());
			bool destinationExists = diagram->getFiguresCollection()->existsObject(relation->getEndTable());
			if(!sourceExists || !destinationExists)
			{
				diagram->remove(relation);
			}

		}
	}
	delete iterator;

	// Now add all existing relationships in MODEL to DIAGRAM if both source and destination
	// tables exists at DIAGRAM
	iterator = _model->createIterator();
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
		{
			relation = (ddRelationshipFigure *)tmp;

			//test if all tables of a relationship are included if this is the case then include relationship at this diagram
			bool sourceExists = diagram->getFiguresCollection()->existsObject(relation->getStartTable());
			bool destinationExists = diagram->getFiguresCollection()->existsObject(relation->getEndTable());
			bool relationExists = diagram->getFiguresCollection()->existsObject(relation);
			if(sourceExists && destinationExists && !relationExists)
			{
				diagram->add(relation);
				relation->updateConnection(diagramIndex);
			}
		}
	}
	delete iterator;
}

void ddDrawingEditor::checkAllDigramsRelConsistency()
{
	int i, size = modelCount();

	for(i = 0; i < size ; i++)
	{
		checkRelationshipsConsistency(i);
	}
}

void ddDrawingEditor::notifyChanged()
{
	if(frm)
		frm->setModelChanged(true);
	modelChanged = true;
}
