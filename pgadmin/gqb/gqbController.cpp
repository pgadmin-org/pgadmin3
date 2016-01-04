//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbController.cpp - Controller part of MVC Software Pattern used by GQB
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/dnd.h>
#include <wx/dataobj.h>

// App headers
#include "frm/frmQuery.h"
#include "gqb/gqbViewController.h"
#include "gqb/gqbModel.h"
#include "gqb/gqbSchema.h"
#include "gqb/gqbQueryObjs.h"
#include "gqb/gqbBrowser.h"
#include "gqb/gqbEvents.h"
#include "gqb/gqbViewPanels.h"

wxWindowID CTL_NTBKPANELS = ::wxNewId();

gqbController::gqbController(gqbModel *_model, wxWindow *gqbParent, ctlAuiNotebook *gridParent, wxSize size = wxSize(GQB_MIN_WIDTH, GQB_MIN_HEIGHT))
	: wxObject()
{
	pparent = gqbParent;
	model = _model;
	wxSize tablesBrowserSize = wxSize(200, 800);  //Initial Size

	// Initialize Main Splitter
	gqbMainContainer = new wxSplitterWindow(gqbParent, GQB_HORZ_SASH, wxDefaultPosition, wxDefaultSize, wxSP_3D);
	tabs = new wxNotebook(gqbMainContainer, CTL_NTBKPANELS, wxDefaultPosition, wxDefaultSize);

	// Initialize view container with tables browser
	// GQB-TODO: change 976 with a enum value of events.h
	gqbContainer = new gqbSplitter(gqbMainContainer, 976, wxDefaultPosition, wxDefaultSize, wxSP_3D);

	// Initialize view canvas and tables tree
	browserPanel = new wxPanel(gqbContainer, wxID_ANY, wxDefaultPosition, tablesBrowserSize);
	tablesBrowser = new gqbBrowser(browserPanel, GQB_BROWSER, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSIMPLE_BORDER, this);
	view = new gqbView(gqbContainer, gridParent, size, this, model);

	// Set Drag Target
	view->SetDropTarget(new DnDText(tablesBrowser));

	// Set Scroll Bar & split
	view->SetScrollbars( 10, 10, 127, 80 );
	gqbContainer->SplitVertically(browserPanel, view);

	tabs->InsertPage(ti_colsGridPanel, view->getColsGridPanel(), _("Columns"));
	tabs->InsertPage(ti_criteriaPanel, view->getCriteriaPanel(), _("Criteria"));
	tabs->InsertPage(ti_orderPanel, view->getOrderPanel(), _("Ordering"));
	tabs->InsertPage(ti_joinsPanel, view->getJoinsPanel(), _("Joins"));
	gqbMainContainer->SplitHorizontally(gqbContainer, tabs);

	// Fix Sash resize bug
	gqbContainer->setTablesBrowser(tablesBrowser);
	gqbContainer->setBrowserPanel(browserPanel);
}


// Destructor
gqbController::~gqbController()
{
	if(view)
	{
		delete view;
		view = NULL;
	}
}


// Add a table to the model
gqbQueryObject *gqbController::addTableToModel(gqbTable *table, wxPoint p)
{
	gqbQueryObject *added = model->addTable(table, p);
	view->newTableAdded(added);
	return added;
}


// Remove a table from the model
void gqbController::removeTableFromModel(gqbQueryObject *mtable, gqbGridProjTable *gridTable = NULL, gqbGridOrderTable *orderLTable = NULL, gqbGridOrderTable *orderRTable = NULL)
{
	model->deleteTable(mtable);
	gridTable->removeAllRows(mtable);             // GQB-TODO: move this functions to model??
	orderLTable->emptyTableData(mtable);
	orderRTable->emptyTableData(mtable);
}


// Add (Select to be use in projection) a column for table in a model
void gqbController::processColumnInModel(gqbQueryObject *table, gqbColumn *column,  gqbGridProjTable *gridTable = NULL)
{
	if(!table->existsColumn(column))
	{
		table->addColumn(column);
		if(gridTable)
		{
			gridTable->AppendItem(0, table);
			gridTable->AppendItem(1, column);
			gridTable->AppendItem(2, NULL);
		}
	}
	else
	{
		// Removes but don't delete because we needed the column at table [this is just a pointer to it]
		table->removeColumn(column);
		if(gridTable)
		{
			gridTable->removeRow(table, column);
		}
	}
}


// Get selected item at position pt, marks item as selected and if there is a previous selected then unselect it
gqbObject *gqbController::getModelSelected(wxPoint &pt, gqbQueryObject *lastSelected, gqbQueryJoin *lastJoinSelected, bool mark)
{
	gqbQueryObject *sel = NULL;
	gqbQueryJoin *jSel = NULL;
	gqbIteratorBase *iterator = model->createDownQueryIterator();
	bool found = false;

	while(!found && iterator->HasNext())
	{
		// Try to find if click over a table
		sel = (gqbQueryObject *)iterator->Next();
		if( ((pt.x - sel->position.x > 0 ) && (sel->position.x + sel->getWidth() > pt.x)) && ((pt.y - sel->position.y > 0) && (sel->position.y + sel->getHeight() > pt.y)) )
		{
			// GQB-TODO: this function should be move to the view to allow to
			// recalculate it if the graphBehavior changes
			if(mark)
				sel->setSelected(true);

			jSel = NULL;
			found = true;
			break;
		}

		// Try to find if user click over a Join
		if(sel->getHaveJoins())
		{
			gqbIteratorBase *joinsIterator = sel->createJoinsIterator();
			while(joinsIterator->HasNext())
			{
				// GQB-TODO: don't pass anchor because join it's passed as parameter yet.
				jSel = (gqbQueryJoin *) joinsIterator->Next();
				wxPoint o = jSel->getSourceAnchor();
				wxPoint d = jSel->getDestAnchor();

				if(view->clickOnJoin(jSel, pt, o, d))
				{
					if(mark)
						jSel->setSelected(true);
					found = true;
					sel = NULL;
					break;
				}

			}
			delete joinsIterator;
		}
	}

	delete iterator;

	if(found)
	{
		if(mark)
		{
			if(lastSelected && lastSelected != sel) // Unselect previous table selected if exists
				lastSelected->setSelected(false);
			// Unselect previous join selected if exists
			if(lastJoinSelected && lastJoinSelected != jSel)
				lastJoinSelected->setSelected(false);
		}
		if(sel)
			return sel;
		else if(jSel)
			return jSel;
	}

	return NULL;
}


// Unselect any previously selected item in the model
void gqbController::unsetModelSelected(bool queryTable = true)
{
	gqbQueryObject *sel = NULL;
	gqbQueryJoin *jSel = NULL;

	if(queryTable)                                // QueryTable
	{
		gqbIteratorBase *iterator = model->createQueryIterator();
		while(iterator->HasNext())
		{
			sel = (gqbQueryObject *)iterator->Next();
			if(sel->getSelected())
			{
				sel->setSelected(false);
			}
		}
		delete iterator;
	}

	if(!queryTable)                               // QueryJoin
	{
		gqbIteratorBase *iterator = model->createQueryIterator();
		gqbQueryObject *sel = NULL;
		while(iterator->HasNext())
		{
			sel = (gqbQueryObject *)iterator->Next();
			if(sel->getHaveJoins())
			{
				gqbIteratorBase *joinsIterator = sel->createJoinsIterator();
				while(joinsIterator->HasNext())
				{
					// GQB-TODO: don't pass anchor because join it's passed as parameter yet.
					jSel = (gqbQueryJoin *) joinsIterator->Next();
					if(jSel->getSelected())
					{
						jSel->setSelected(false);
					}
				}

				delete joinsIterator;
			}
		}
		delete iterator;
	}
}


// GQB-TODO: Create a less complex & simpler generation function
// Generate the SQL Sentence from the model
wxString gqbController::generateSQL()
{
	wxString sentence = wxT("");
	if(model->tablesCount() > 0)
	{
		sentence += wxT("SELECT \n");

		// Add selected columns for Query
		gqbQueryObject *sel = NULL;
		gqbIteratorBase *iteratorRestrictions = NULL;
		gqbIteratorBase *iteratorModel = NULL;
		gqbIteratorBase *iteratorJoins = NULL;

		// Projection Part [Select x,x,x...]
		gqbObjsArray *cols = model->getOrderedColumns();
		gqbObjsArray *tables = model->getColumnsParents();
		wxArrayString *alias = model->getColumnsAlias();

		int i, size = cols->GetCount();

		for(i = 0; i < size; i++)
		{
			if(alias->Item(i).length() > 0)
			{
				if(((gqbQueryObject *)tables->Item(i))->getAlias().length() > 0)
				{
					sentence += wxT("  ") +
					            qtIdent(((gqbQueryObject *)tables->Item(i))->getAlias()) +
					            wxT(".") +
					            qtIdent(((gqbColumn *)cols->Item(i))->getName()) +
					            wxT(" AS ") +
					            qtIdent(alias->Item(i)) +
					            wxT(", \n");
				}
				else
				{
					sentence += wxT("  ") +
					            qtIdent(((gqbQueryObject *)tables->Item(i))->getName()) +
					            wxT(".") +
					            qtIdent(((gqbColumn *)cols->Item(i))->getName()) +
					            wxT(" AS ") +
					            qtIdent(alias->Item(i)) +
					            wxT(", \n");
				}
			}
			else
			{
				if(((gqbQueryObject *)tables->Item(i))->getAlias().length() > 0)
				{
					sentence += wxT("  ") +
					            qtIdent(((gqbQueryObject *)tables->Item(i))->getAlias()) +
					            wxT(".") +
					            qtIdent(((gqbColumn *)cols->Item(i))->getName()) +
					            wxT(", \n");
				}
				else
				{
					sentence += wxT("  ") +
					            qtIdent(((gqbQueryObject *)tables->Item(i))->getName()) +
					            wxT(".") +
					            qtIdent(((gqbColumn *)cols->Item(i))->getName()) +
					            wxT(", \n");
				}
			}
		}

		if(!size)
			sentence += wxT("  * \n");
		else
		{
			// remove last ", "
			sentence.Truncate(sentence.Length() - 3);
			sentence += wxT("\n");
		}

		sentence += wxT("FROM \n");

		iteratorModel = model->createQueryIterator();
		while(iteratorModel->HasNext())
		{
			sel = (gqbQueryObject *)iteratorModel->Next();
			gqbSchema *schema = (gqbSchema *)sel->parent->getOwner();
			if(sel->getAlias().length() > 0)
			{
				sentence += wxT("  ") +
				            qtIdent(schema->getName()) +
				            wxT(".") +
				            qtIdent(sel->getName()) +
				            wxT(" ") +
				            qtIdent(sel->getAlias()) +
				            wxT(", \n");
			}
			else
			{
				sentence += wxT("  ") +
				            qtIdent(schema->getName()) +
				            wxT(".") +
				            qtIdent(sel->getName()) +
				            wxT(", \n");
			}

		}
		sentence.Truncate(sentence.Length() - 3); // remove last ", "

		// WHERE PART
		// [Joins]
		bool first = true, truncAnd = false;
		gqbQueryJoin *tmp;
		iteratorModel->ResetIterator();
		while(iteratorModel->HasNext())
		{
			sel = (gqbQueryObject *)iteratorModel->Next();
			if(sel->getHaveJoins())
			{
				truncAnd = true;
				iteratorJoins = sel->createJoinsIterator();
				while(iteratorJoins->HasNext())
				{
					if(first)
					{
						first = false;
						sentence += wxT("\nWHERE \n");
					}
					tmp = (gqbQueryJoin *)iteratorJoins->Next();

					if(tmp->getSourceQTable()->getAlias().length() > 0)
						sentence += wxT("  ") +
						            qtIdent(tmp->getSourceQTable()->getAlias()) +
						            wxT(".") +
						            qtIdent(tmp->getSourceCol());
					else
						sentence += wxT("  ") +
						            qtIdent(tmp->getSourceQTable()->getName()) +
						            wxT(".") +
						            qtIdent(tmp->getSourceCol());

					switch(tmp->getKindofJoin())
					{
						case _equally:
							sentence += wxT(" = ");
							break;
						case _greater:
							sentence += wxT(" > ");
							break;
						case _lesser:
							sentence += wxT(" < ");
							break;
						case _equgreater:
							sentence += wxT(" >= ");
							break;
						case _equlesser:
							sentence += wxT(" <= ");
							break;

					}

					if(tmp->getDestQTable()->getAlias().length() > 0)
						sentence += qtIdent(tmp->getDestQTable()->getAlias()) +
						            wxT(".") +
						            qtIdent(tmp->getDestCol()) +
						            wxT(" AND\n");
					else
						sentence += qtIdent(tmp->getDestQTable()->getName()) +
						            wxT(".") +
						            qtIdent(tmp->getDestCol()) +
						            wxT(" AND\n");
				}
				delete iteratorJoins;
			}
		}
		delete iteratorModel;

		gqbRestrictions *restrictions = model->getRestrictions();

		// Remove last " AND " from joins if there isn't restrictions, only left white space
		if(truncAnd && (restrictions->restrictionsCount() <= 0))
			sentence.Truncate(sentence.Length() - 5);

		// Never found a join
		if (!truncAnd && (restrictions->restrictionsCount() > 0))
			sentence += wxT("\nWHERE \n");

		//GQB-TODO: VALIDATE RESTRICTIONS
		iteratorRestrictions = restrictions->createRestrictionsIterator();
		gqbQueryRestriction *r = NULL;

		while(iteratorRestrictions->HasNext())
		{
			r = (gqbQueryRestriction *)iteratorRestrictions->Next();
			sentence += wxT("  ") +
			            r->getLeft() +
			            wxT(" ") +
			            r->getRestriction() +
			            wxT(" ") +
			            r->getValue_s() +
			            wxT(" ") +
			            r->getConnector() +
			            wxT(" \n");
		}
		delete iteratorRestrictions;

		if(restrictions->restrictionsCount() > 0)
		{
			if(r->getConnector().Contains(wxT("AND")))
			{
				sentence.Truncate(sentence.Length() - 6);
			}
			else
			{
				sentence.Truncate(sentence.Length() - 5);
			}
		}
		// ORDER BY PART
		gqbObjsArray *orderByColumns = model->getOrdByColumns();
		gqbObjsArray *orderByParents = model->getOrdByParents();
		charArray *typeOfOrder = model->getOrdByKind();

		size = orderByColumns->GetCount();
		if(size > 0)
		{
			sentence += wxT("\nORDER BY\n");
		}
		wxString typeOrder = wxT("");
		for(i = 0; i < size; i++)
		{
			switch(typeOfOrder->Item(i))
			{
				case 'A':
					typeOrder = wxT(" ASC");
					break;
				case 'D':
					typeOrder = wxT(" DESC");
					break;
			};
			if(((gqbQueryObject *)orderByParents->Item(i))->getAlias().length() > 0)
				sentence += wxT("  ") +
				            qtIdent(((gqbQueryObject *)orderByParents->Item(i))->getAlias()) +
				            wxT(".") +
				            qtIdent(((gqbColumn *)orderByColumns->Item(i))->getName()) +
				            typeOrder +
				            wxT(", \n");
			else
				sentence += wxT("  ") +
				            qtIdent(((gqbQueryObject *)orderByParents->Item(i))->getName()) +
				            wxT(".") +
				            qtIdent(((gqbColumn *)orderByColumns->Item(i))->getName()) +
				            typeOrder +
				            wxT(", \n");
		}

		if(size > 0)
			sentence.Truncate(sentence.Length() - 3);

		sentence += wxT(";");
	}                                             // Close Tables Count > 0 on model
	return sentence;
}


gqbQueryRestriction *gqbController::addRestriction()
{
	return model->addRestriction();
}


gqbQueryJoin *gqbController::addJoin(gqbQueryObject *sTable, gqbColumn *sColumn, gqbQueryObject *dTable, gqbColumn *dColumn, type_Join kind)
{
	return sTable->addJoin(sTable, dTable, sColumn, dColumn, kind);
}


void gqbController::removeJoin(gqbQueryJoin *join)
{
	join->getSourceQTable()->removeJoin(join, true);
}


void gqbController::setPointerMode(pointerMode pm)
{
	view->setPointerMode(pm);
}


void gqbController::emptyModel()
{
	view->emptyPanelsData();
	model->emptyAll();
}


void gqbController::calcGridColsSizes()
{
	// Recalculate best internals sizes for all columns inside grids.
	((gqbGridPanel *)view->getColsGridPanel())->SetGridColsSize();
	((gqbCriteriaPanel *)view->getCriteriaPanel())->SetGridColsSize();
	((gqbOrderPanel *)view->getOrderPanel())->SetGridColsSize();
	((gqbJoinsPanel *)view->getJoinsPanel())->SetGridColsSize();
}


void gqbController::setSashVertPosition(int pos)
{
	gqbContainer->UpdateSize();
	gqbContainer->SetSashPosition(pos, true);
	gqbContainer->SetMinimumPaneSize(pos);
	gqbContainer->UpdateSize();
}

int gqbController::getSashHorizPosition()
{
	return gqbMainContainer->GetSashPosition();
}


void gqbController::setSashHorizPosition(int pos)
{
	gqbMainContainer->UpdateSize();
	gqbMainContainer->SetSashPosition(pos, true);

	if(pos >= 150 && pos <= 200)
	{
		gqbMainContainer->SetMinimumPaneSize(pos);
	}
	else
	{
		gqbMainContainer->SetMinimumPaneSize(150);
	}
	gqbMainContainer->UpdateSize();
}


//
// Utility Class to avoid a bug when the event sash resize is called [onVerticalSashResize]
//

// GQB-TODO: fix 976 for real one value here and above

BEGIN_EVENT_TABLE(gqbSplitter, wxSplitterWindow)
	EVT_SPLITTER_SASH_POS_CHANGED(976, gqbSplitter::onVerticalSashResize)
END_EVENT_TABLE()

gqbSplitter::gqbSplitter(wxWindow *parent, wxWindowID id, const wxPoint &point, const wxSize &size, long style)
	: wxSplitterWindow(parent, id, point, size, style),
	  tablesBrowser(NULL),
	  browserPanel(NULL)
{
}


void gqbSplitter::onVerticalSashResize(wxSplitterEvent &event)
{
	if(tablesBrowser != NULL && browserPanel != NULL)
	{
		wxSize s = tablesBrowser->GetSize();
		s.SetWidth(event.GetSashPosition());
		browserPanel->SetSize(s);
		tablesBrowser->SetSize(s);
		// GQB-TODO: Set a minimun value
	}
}
