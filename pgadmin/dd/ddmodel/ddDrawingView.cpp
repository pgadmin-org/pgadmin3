//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddDrawingView.cpp - Main canvas where all tables and relationships are drawn
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/ddmodel/ddDrawingView.h"
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/figures/ddRelationshipFigure.h"
#include "dd/dditems/utilities/ddDataType.h"

ddDrawingView::ddDrawingView(wxWindow *ddParent, wxhdDrawingEditor *editor ,wxSize size, wxhdDrawing *drawing)
:wxhdDrawingView(ddParent,editor,size,drawing)
{
}

void ddDrawingView::deleteSelectedFigures()
{
	wxhdIFigure *tmp;
    ddTableFigure *table;
	ddRelationshipFigure *relation;
	int answer;
	int numbTables = 0;
	int numbRelationships = 0;

    if (selection->count() == 1)
    {
		tmp = (wxhdIFigure*) selection->getItemAt(0);
		if(tmp->getKindId() == DDTABLEFIGURE)
		{
			numbTables = 1;
			table = (ddTableFigure *)tmp;	
			answer = wxMessageBox(_("Are you sure you wish to delete table ") + table->getTableName() + wxT("?"), _("Delete table?"), wxYES_NO|wxNO_DEFAULT);
		}
		if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
		{
			numbRelationships = 1;
			relation = (ddRelationshipFigure *)tmp;	
			answer = wxMessageBox(_("Are you sure you wish to delete relationship ") + relation->getConstraintName() + wxT("?"), _("Delete relationship?"), wxYES_NO|wxNO_DEFAULT);
		}
    }
    else if (selection->count() > 1)
    {
		numbTables = 0;
		wxhdIteratorBase *iterator=selection->createIterator();
		while(iterator->HasNext())
		{
		 tmp=(wxhdIFigure *)iterator->Next();
		 if(tmp->getKindId() == DDTABLEFIGURE)
			numbTables++;
		}
		delete iterator;

		answer = wxMessageBox(
          wxString::Format(_("Are you sure you wish to delete %d tables?"), numbTables),
          _("Delete tables?"), wxYES_NO|wxNO_DEFAULT);

		if(numbTables==0)
		{
			iterator=selection->createIterator();
			while(iterator->HasNext())
			{
			 tmp=(wxhdIFigure *)iterator->Next();
			 if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
				numbRelationships++;
			}
			delete iterator;
		}
    }

    if (answer == wxYES)
    {
        while(numbTables > 0)
        {
			tmp = (wxhdIFigure*) selection->getItemAt(0);
			if(tmp->getKindId() == DDTABLEFIGURE)
			{
				table = (ddTableFigure *)tmp;	
				removeFromSelection(table);
				table->processDeleteAlert(this);
				remove(table);
				if(table)
					delete table;
				numbTables--;
			}
			else
			{
				removeFromSelection(tmp); //isn't a tables is probably a relationship
			}
        }
		
		if( numbRelationships>0 && numbTables==0 )
		{
			while(numbRelationships > 0)
			{
				tmp = (wxhdIFigure*) selection->getItemAt(0);
				if(tmp->getKindId() == DDRELATIONSHIPFIGURE)
				{
					relation = (ddRelationshipFigure *)tmp;	
					relation->removeForeignKeys();
					relation->disconnectEnd();
					relation->disconnectStart();
					removeFromSelection(relation);
					remove(relation);
					if(relation)
						delete relation;
					numbRelationships--;
				}
				else
				{
					removeFromSelection(tmp); //isn't neither a table or relationship
				}
			}
		}

		clearSelection();  //after delete all items all relationships remains at selection and should be removed
	}
}