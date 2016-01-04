//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbView.cpp - View implementation for MVC Pattern of GQB
//
//////////////////////////////////////////////////////////////////////////


// 1. READ MODEL STATE FROM gqbModel TO CREATE THE GRAPHIC REPRESENTATION OF THE QUERY
// 2. USE THE CONTROLLER TO CHANGE THE MODEL WITH THE USER INPUT

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/generic/gridctrl.h>
#include <wx/notebook.h>
#include <wx/choicdlg.h>

// App headers
#include "gqb/gqbModel.h"
#include "gqb/gqbEvents.h"
#include "gqb/gqbViewController.h"
#include "gqb/gqbQueryObjs.h"
#include "gqb/gqbGraphSimple.h"
#include "gqb/gqbViewPanels.h"
#include "gqb/gqbObject.h"
#include "gqb/gqbObjectCollection.h"

// Image
#include "images/gqbJoinCursor.pngc"

BEGIN_EVENT_TABLE(gqbView, wxScrolledWindow)
	EVT_SIZE(gqbView::OnSize)
	EVT_PAINT(gqbView::onPaint)
	EVT_MOTION(gqbView::onMotion)
	EVT_LEFT_DOWN(gqbView::onMotion)
	EVT_RIGHT_DOWN(gqbView::onRightClick)
	EVT_LEFT_UP(gqbView::onMotion)
	EVT_LEFT_DCLICK(gqbView::onDoubleClick)
	EVT_ERASE_BACKGROUND(gqbView::onEraseBackGround)  //This erase flicker create by wxStaticText when erasing background but this is not needed
	EVT_KEY_DOWN(gqbView::OnKeyDown)
	EVT_MENU(GQB_RMJ_DELETE,	gqbView::OnMenuJoinDelete)
	EVT_MENU(GQB_RMT_DELETE,	gqbView::OnMenuTableDelete)
	EVT_MENU(GQB_RMT_SETALIAS,	gqbView::OnMenuTableSetAlias)
	EVT_MENU(GQB_REFRESH,           gqbView::OnRefresh)
END_EVENT_TABLE()

gqbView::gqbView(wxWindow *gqbParent, ctlAuiNotebook *gridParent, wxSize size, gqbController *controller, gqbModel *model)
	: wxScrolledWindow(gqbParent, wxID_ANY, wxPoint(201, 0), size,
	                   wxHSCROLL | wxVSCROLL | wxBORDER | wxRETAINED)
{
	this->controller = controller;
	this->model = model;
	pressed = -1;
	selected = -1;
	changeTOpressed = false;
	canvasSize = size;
	collectionSelected = NULL;
	joinSelected = NULL;
	joinSource = NULL;
	joinDest = NULL;
	joinSCol = NULL;
	joinDCol = NULL;
	refreshRate = 3;
	iterator = NULL;
	mode = pt_normal;
	joinCursorImage = *gqbJoinCursor_png_img;
	joinCursor = wxCursor(joinCursorImage);
	m_rightJoins = NULL;
	m_rightTables = NULL;
	m_gqbPopup = NULL;
	jTempSelected = NULL;
	cTempSelected = NULL;

	// Assing kind of join Options
	joinTypeChoices.Add(wxString(wxT(" = ")));
	joinTypeChoices.Add(wxString(wxT(" > ")));
	joinTypeChoices.Add(wxString(wxT(" < ")));
	joinTypeChoices.Add(wxString(wxT(" >= ")));
	joinTypeChoices.Add(wxString(wxT(" <= ")));

	// Assign default graphic behavior [skin of forms inside model]
	this->graphBehavior = new gqbGraphSimple();

	// Create Projection Panel
	// GQB-TODO: move model to grid panel constructor
	this->gridTable = new gqbGridProjTable(this->model->getOrderedColumns(), this->model->getColumnsParents(), this->model->getColumnsAlias());
	this->projectionPanel = new gqbGridPanel(controller->getTabs(), -1, gridTable);

	// Create Restrictions Panel
	this->restrictionsGridTable = new gqbGridRestTable(model->getRestrictions());
	this->criteriaPanel = new gqbCriteriaPanel(controller->getTabs(), model, restrictionsGridTable);

	// Create Joins Panel
	this->joinsGridTable = new gqbGridJoinTable(this->controller);
	this->joinsPanel = new gqbJoinsPanel(controller->getTabs(), model, joinsGridTable, controller);

	// Create Order by Panel
	this->orderByLGridTable = new gqbGridOrderTable(1, model->getOrdByAvailColumns(), model->getOrdByAvailParents(), NULL);
	this->orderByRGridTable = new gqbGridOrderTable(2, model->getOrdByColumns(), model->getOrdByParents(), model->getOrdByKind());
	this->orderPanel = new gqbOrderPanel(controller->getTabs(), orderByLGridTable, orderByRGridTable);

#if !wxCHECK_VERSION(2, 9, 0)
	// does nothing in 2.9+
	SetVirtualSizeHints(size);
#endif

}


gqbView::~gqbView()
{
	if(graphBehavior)
		delete graphBehavior;
	if(iterator)
		delete iterator;

	if(m_rightTables)
		delete m_rightTables;

	if(m_rightJoins)
		delete m_rightJoins;

	if (m_gqbPopup)
		delete m_gqbPopup;

	if(orderByRGridTable)
		delete orderByRGridTable;

	if(orderByLGridTable)
		delete orderByLGridTable;
}


// Overwrite and disable onEraseBackground Event to avoid Flicker
void gqbView::onEraseBackGround(wxEraseEvent &event)
{
}


// Detect when should be drawn the canvas with the model information
void gqbView::onPaint(wxPaintEvent &event)
{
	wxPaintDC dcc(this);                          // Prepare Context for Buffered Draw
	wxBufferedDC dc(&dcc, canvasSize);
	drawAll(dc, true);                            // Call Function to draw all
}


// GQB-TODO: remove all possible modification to model from here to controller.
void gqbView::onRightClick(wxMouseEvent &event)
{
	// GQB-TODO: Validate Alias
	gqbObject *anySelected = NULL;
	wxPoint pdc = event.GetPosition();
	pdc.x = event.GetPosition().x;
	pdc.y = event.GetPosition().y;
	this->CalcUnscrolledPosition(pdc.x, pdc.y, &pdc.x, &pdc.y);
	anySelected = controller->getModelSelected(pdc, cTempSelected, jTempSelected, false);
	if(anySelected)
	{
		if(anySelected->getType() == GQB_QUERYOBJ)
		{
			if(!m_rightTables)
			{
				m_rightTables = new wxMenu;
				m_rightTables->Append(GQB_RMT_SETALIAS, _("&Set Alias for table"));
				m_rightTables->Append(GQB_RMT_DELETE, _("&Delete Table"));
				m_rightTables->AppendSeparator();
				m_rightTables->Append(GQB_REFRESH, _("&Refresh"));

			}
			cTempSelected = (gqbQueryObject *) (gqbObjectCollection *) anySelected;
			jTempSelected = NULL;
			PopupMenu(m_rightTables, event.GetPosition());
		}

		if(anySelected->getType() == GQB_JOIN)
		{
			if(!m_rightJoins)
			{
				m_rightJoins = new wxMenu;
				m_rightJoins->Append(GQB_RMJ_DELETE, _("&Delete Join"));
				m_rightJoins->AppendSeparator();
				m_rightJoins->Append(GQB_REFRESH, _("&Refresh"));
			}
			cTempSelected = NULL;
			jTempSelected = (gqbQueryJoin *) anySelected;;
			PopupMenu(m_rightJoins, event.GetPosition());
		}
	}
	else
	{
		if(!m_gqbPopup)
		{
			m_gqbPopup = new wxMenu;
			m_gqbPopup->Append(GQB_REFRESH, _("&Refresh"));
		}
		PopupMenu(m_gqbPopup, event.GetPosition());
	}
}


void gqbView::OnMenuJoinDelete(wxCommandEvent &WXUNUSED(event))
{
	if(jTempSelected)
	{
		this->joinsGridTable->removeJoin(jTempSelected);
		controller->removeJoin(jTempSelected);
		jTempSelected = NULL;
		this->Refresh();
	}
}


void gqbView::OnMenuTableDelete(wxCommandEvent &WXUNUSED(event))
{
	if(cTempSelected)
	{
		joinsGridTable->removeJoins(cTempSelected);
		controller->removeTableFromModel(cTempSelected, gridTable, orderByLGridTable, orderByRGridTable);
		cTempSelected = NULL;
		this->Refresh();
	}
}


void gqbView::OnMenuTableSetAlias(wxCommandEvent &event)
{
	if(cTempSelected)
	{
		// Because a bug that scrolled automatically the panel of the view if this dialog is called, then assign
		// as his parent the main container of the view, and void the bug
		wxTextEntryDialog dialog(controller->getDialogParent(),
		                         wxString::Format(_("Enter an alias for table %s"), cTempSelected->getName().c_str()),
		                         _("Please enter an alias for the table."),
		                         wxT(""),
		                         wxOK | wxCANCEL | wxCENTRE);
		dialog.SetValue(cTempSelected->getAlias());
		if (dialog.ShowModal() == wxID_OK)
		{
			cTempSelected->setAlias(dialog.GetValue());
			joinsPanel->Refresh();
		}
		cTempSelected = NULL;
		this->Refresh();
	}
}


void gqbView::onDoubleClick(wxMouseEvent &event)
{
	// GQB-TODO: Validate Alias
	gqbObject *anySelected = NULL;
	wxPoint pdc = event.GetPosition();
	pdc.x = event.GetPosition().x;
	pdc.y = event.GetPosition().y;
	this->CalcUnscrolledPosition(pdc.x, pdc.y, &pdc.x, &pdc.y);

	anySelected = controller->getModelSelected(pdc, cTempSelected, jTempSelected, false);
	if(anySelected)
	{
		if(anySelected->getType() == GQB_QUERYOBJ)
		{
			gqbQueryObject *t = (gqbQueryObject *) (gqbObjectCollection *) anySelected;

			// Because a bug that scrolled automatically the panel of the view if this dialog is called, then assign
			// as his parent the main container of the view, and void the bug
			wxTextEntryDialog dialog(controller->getDialogParent(),
			                         wxString::Format(_("Enter an alias for table %s"), t->getName().c_str()),
			                         _("Please enter an alias for the table."),
			                         wxT(""),
			                         wxOK | wxCANCEL | wxCENTRE);
			dialog.SetValue(t->getAlias());
			if (dialog.ShowModal() == wxID_OK)
			{
				t->setAlias(dialog.GetValue());
				joinsPanel->Refresh();

				// hack to avoid misplaced joins anchors after insert an alias that trigger a table graph resize (bigger)
				this->Refresh();
				this->Update(); //force refresh
				graphBehavior->UpdatePosObject(t, t->position.x, t->position.y, 0);
			}
		}
		else if(anySelected->getType() == GQB_JOIN)
		{
			gqbQueryJoin *j = (gqbQueryJoin *) anySelected;

			controller->getTabs()->ChangeSelection(ti_joinsPanel);
			gqbJoinsPanel *jPanel = wxDynamicCast( joinsPanel, gqbJoinsPanel );
			jPanel->selectJoin(j);
		}
	}
	this->Refresh();
}


// Manages user input [Mouse click, drag & drop] over the Canvas
void gqbView::onMotion(wxMouseEvent &event)
{
	static int refresh = 1;                       // refresh counter, everytime this values reaches
	// "refreshRate" value then Refresh while dragging
	// Discover area where event ocurrs
	pos.x = event.GetPosition().x;
	pos.y = event.GetPosition().y;
	this->CalcUnscrolledPosition(pos.x, pos.y, &pos.x, &pos.y);
	gqbObject *anySelected = NULL;

	// Button Down Event is triggered
	if(event.ButtonDown() && !changeTOpressed)
	{
		this->SetFocus();

		// Which kind of button down was? join creation [click on any column at the
		// right of checkbox and drag & drop] or table moving [click on title and drag & drop]
		anySelected = controller->getModelSelected(pos, collectionSelected, joinSelected, false);
		if(anySelected)
		{
			// Anything before just forget about it
			changeTOpressed = false;
			joinSource = NULL;
			joinSCol = NULL;
			joinDCol = NULL;
			joinDest = NULL;
			jpos.x = 0;
			jpos.y = 0;

			if(anySelected->getType() == GQB_QUERYOBJ)
			{
				gqbQueryObject *t = (gqbQueryObject *) (gqbObjectCollection *) anySelected;

				// If click on the title area AND don't click on the columns selection checkbox
				if( (pos.y - t->position.y <= graphBehavior->getTitleRowHeight()))
					controller->setPointerMode(pt_normal);
				else if(pos.x - t->position.x <= 17)
					controller->setPointerMode(pt_normal);
				else
					controller->setPointerMode(pt_join);
			}
		}
		else
		{
			anySelected = NULL;
			mode = pt_normal;
		}

		if(mode == pt_normal)                     // pointer is used to move tables & select/unselect columns
		{
			// getSelected Item [Mark it as selected if possible]
			anySelected = controller->getModelSelected(pos, collectionSelected, joinSelected, true);
			changeTOpressed = true;

			// Do conversion of type object if any found
			if(anySelected)
			{
				if(anySelected->getType() == GQB_QUERYOBJ)
				{
					collectionSelected = (gqbQueryObject *) (gqbObjectCollection *) anySelected;
					joinSelected = NULL;
				}
				else if(anySelected->getType() == GQB_JOIN)
				{
					joinSelected = (gqbQueryJoin *) anySelected;
					collectionSelected = NULL;
				}
			}
			else
			{
				collectionSelected = NULL;
				joinSelected = NULL;
			}

			if(!collectionSelected)
			{
				// none selected temp unselect all items
				controller->unsetModelSelected(true);
			}
			else
			{
				gqbColumn *col = graphBehavior->getColumnAtPosition(&pos, collectionSelected);
				if(col)
				{
					// Add or remove column from model & observers (ex: Grid) (projection part of SQL sentence)
					controller->processColumnInModel(collectionSelected, col, gridTable);
				}
			}

			if(!joinSelected)
			{
				controller->unsetModelSelected(false);
			}

		}
		// Pointer is used to add joins
		else if(mode == pt_join)
		{
			anySelected = controller->getModelSelected(pos, collectionSelected, joinSelected, false);

			// Even if I get an object check that it isn't a join
			if( (anySelected) && anySelected->getType() == GQB_QUERYOBJ)
				joinSource = (gqbQueryObject *)(gqbObjectCollection *) anySelected;
			else
				joinSource = NULL;

			if(!joinSource)
			{
				// creation of join starts
				joinSCol = NULL;
				joinDCol = NULL;
				jpos.x = 0;
				jpos.y = 0;
			}
			else
			{
				joinSCol = graphBehavior->getColumnAtPosition(&pos, joinSource, joinSource->getWidth());
				jpos = pos;

				// GQB-TODO then draw line between column & pointer
			}
		}

		this->Refresh();
	}

	// Button Up Event is triggered
	if(event.ButtonUp())
	{
		// Pointer is used to move tables & select/unselect columns
		if(mode == pt_normal)
		{
			changeTOpressed = false;
			anySelected = controller->getModelSelected(pos, collectionSelected, joinSelected, false);
			if (anySelected && anySelected->getType() == GQB_JOIN)
			{
				gqbJoinsPanel *jPanel = wxDynamicCast( joinsPanel, gqbJoinsPanel );
				jPanel->selectJoin((gqbQueryJoin *)anySelected);
			}
		}
		// Pointer is used to add joins
		else if(mode == pt_join)
		{
			anySelected = controller->getModelSelected(pos, collectionSelected, joinSelected, false);

			// Even if I get an object check that it isn't a join
			if( (anySelected) && anySelected->getType() == GQB_QUERYOBJ)
			{
				joinDest = (gqbQueryObject *)(gqbObjectCollection *) anySelected;
				// Validate not self joins [in this version tables can be duplicated to create same effect]
				if(joinDest == joinSource)
				{
					joinDest = NULL;
				}
			}
			else
				joinDest = NULL;

			// Creation of join starts
			if(!joinDest)
			{
				joinSource = NULL;
				joinSCol = NULL;
				joinDCol = NULL;
				joinDest = NULL;
				jpos.x = 0;
				jpos.y = 0;
			}
			else
			{
				joinDCol = graphBehavior->getColumnAtPosition(&pos, joinDest, joinDest->getWidth());
				if(joinDCol)
				{
					// GQB-TODO: Allow other type of joins
					gqbQueryJoin *qj = controller->addJoin(joinSource, joinSCol, joinDest, joinDCol, _equally);
					graphBehavior->calcAnchorPoint(qj);
					this->joinsGridTable->AppendJoin(qj);
				}
				// Let the temporary join line to be draw again [Don't destroy anything because all object where own by other objects this are just pointers]
				joinSource = NULL;
				joinSCol = NULL;
				joinDest = NULL;
				joinDCol = NULL;
				jpos.x = 0;
				jpos.y = 0;
			}
		}

		controller->setPointerMode(pt_normal);    //when button is up, pointer mode should be only normal
		this->Refresh();
	}

	// Mouse is Dragged while mouse button is down
	if (event.Dragging() && pressed)
	{
		if(mode == pt_normal)
		{
			if(collectionSelected)
			{
				// GQB-TODO: same as gqbGraphBehavior.h [find a way to not hard code the 17 default value]
				if((pos.x > collectionSelected->position.x + 17) || (pos.x < collectionSelected->position.x) )
				{
					graphBehavior->UpdatePosObject(collectionSelected, pos.x, pos.y, 40);
				}

				// Don't draw too much when dragging table around canvas [lower cpu use]
				if(refresh % refreshRate == 0)
				{
					this->Refresh();
					refresh = 1;
				}
				else
					refresh++;

			}
		}
		else if(mode == pt_join)
		{
			if(joinSource && !joinDest)
			{
				this->Refresh();
			}

		}
	}
}


void gqbView::OnKeyDown(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_DELETE)
	{
		if(collectionSelected)
		{
			this->joinsGridTable->removeJoins(collectionSelected);
			controller->removeTableFromModel(collectionSelected, gridTable, orderByLGridTable, orderByRGridTable);
			collectionSelected = NULL;
			this->Refresh();
		}

		if(joinSelected)
		{
			this->joinsGridTable->removeJoin(joinSelected);
			controller->removeJoin(joinSelected);
			joinSelected = NULL;
			this->Refresh();
		}
	}
}


void gqbView::drawAll(wxMemoryDC &bdc, bool adjustScrolling)
{
	bdc.Clear();
	if(!iterator)
		// Get an iterator for the objects (tables/views) in the model.
		iterator = this->model->createQueryIterator();
	else
		iterator->ResetIterator();

	// First Draw Tables
	while(iterator->HasNext())
	{
		gqbQueryObject *tmp = (gqbQueryObject *)iterator->Next();
		wxPoint pt = wxPoint(tmp->position);      // Use a copy because I don't want to store the modified
		// version of point after CalcScrolledPosition was called

		if (adjustScrolling)
		{
			// adjust coordinates
			this->CalcScrolledPosition(pt.x, pt.y, &pt.x, &pt.y);
		}
		graphBehavior->drawTable(bdc, &pt, tmp);  // graph table
	}

	// Later Draw Joins over Tables
	iterator->ResetIterator();
	while(iterator->HasNext())
	{
		gqbQueryObject *tmp = (gqbQueryObject *)iterator->Next();

		if(tmp->getHaveJoins())
		{
			gqbIteratorBase *joinsIterator = tmp->createJoinsIterator();
			while(joinsIterator->HasNext())
			{
				gqbQueryJoin *join = (gqbQueryJoin *) joinsIterator->Next();
				wxPoint o = join->getSourceAnchor();
				wxPoint d = join->getDestAnchor();

				if (adjustScrolling)
				{
					// adjust coordinates origin
					this->CalcScrolledPosition(o.x, o.y, &o.x, &o.y);

					// adjust coordinates destination
					this->CalcScrolledPosition(d.x, d.y, &d.x, &d.y);
				}
				graphBehavior->drawJoin(bdc, o, d, join->getAnchorsUsed(), join->getSelected(), join->getKindofJoin());
			}
			delete joinsIterator;
		}

	}

	// This iterator is delete at destroyer for reuse purposes
	if(joinSource)
	{
		// Draw temporary line while creating a join
		wxPoint source = jpos;
		wxPoint destination = pos;
		if(adjustScrolling)
		{
			this->CalcScrolledPosition(source.x, source.y, &source.x, &source.y);
			this->CalcScrolledPosition(destination.x, destination.y, &destination.x, &destination.y);
		}
		graphBehavior->drawTempJoinLine(bdc, source, destination);
	}
}


void gqbView::setPointerMode(pointerMode pm)
{
	mode = pm;
	if(mode == pt_join)
		this->SetCursor(joinCursor);
	else
		this->SetCursor(wxNullCursor);
}


bool gqbView::clickOnJoin (gqbQueryJoin *join, wxPoint &pt, wxPoint &origin, wxPoint &dest)
{
	return graphBehavior->clickOnJoin(join, pt, origin, dest);
}


void gqbView::emptyPanelsData()
{
	gridTable->emptyTableData();
	this->joinsGridTable->emptyTableData();
}


void gqbView::newTableAdded(gqbQueryObject *item)
{
	// Refresh Order By Panel's Left Grid
	if (orderByLGridTable->GetView() )
	{
		wxGridTableMessage msg( orderByLGridTable,
		                        wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
		                        orderByLGridTable->GetNumberRows() - 1,
		                        item->parent->countCols() );
		orderByLGridTable->GetView()->ProcessTableMessage( msg );
	}
}

void gqbView::updateTable(gqbQueryObject *queryTable)
{
	if (queryTable->getHaveJoins())
	{
		gqbIteratorBase *j = queryTable->createJoinsIterator();
		while (j->HasNext())
		{
			gqbQueryJoin *tmp = (gqbQueryJoin *)j->Next();
			graphBehavior->calcAnchorPoint(tmp);
		}
		delete j;
	}

	// Update position of anchor points of Joins that come from others tables
	if (queryTable->getHaveRegJoins())
	{
		gqbIteratorBase *r = queryTable->createRegJoinsIterator();
		while (r->HasNext())
		{
			gqbQueryJoin *tmp = (gqbQueryJoin *)r->Next();
			graphBehavior->calcAnchorPoint(tmp);
		}
		delete r;
	}
	this->Refresh();
}


void gqbView::OnRefresh(wxCommandEvent &ev)
{
	updateModelSize(NULL, true);
	this->Update();
}


/*
*  updateModelSize
*     - Update the model size.
*     - Calculate the maximum width and maximum height of the model
*  * When removed a table/view from model, the obj parameter must be null,
*    and update parameter should be true, otherwise update parameter should
*    be false (Dragging event)
*/
void gqbView::updateModelSize(gqbQueryObject *obj, bool updateAnyWay)
{
	static int callCount = 0;
	callCount++;
	if (!obj)
	{
		// Do not update model size, everytime it gets called
		// Update the size once in 10 times
		// Update the size only if update flag is true
		if (callCount < 10 && !updateAnyWay)
			return;
		callCount = 0;
		// Figure out the actual model size.
		// Remove table
		int w = 0, h = 0, maxW = 0, maxH = 0;
		if(!iterator)
			// Get an iterator for the objects (tables/views) in the model.
			iterator = this->model->createQueryIterator();
		else
			iterator->ResetIterator();

		while (iterator->HasNext())
		{
			gqbQueryObject *tmp = (gqbQueryObject *)iterator->Next();;
			w = tmp->position.x + tmp->getWidth();
			h = tmp->position.y + tmp->getHeight();

			if (maxW < w)
				maxW = w;
			if (maxH < h)
				maxH = h;
		}

		// Reset Model size
		modelSize.Set(maxW, maxH);
	}
	else
	{
		int w = 0, h = 0;
		w = obj->position.x + obj->getWidth();
		h = obj->position.y + obj->getHeight();

		if (w > modelSize.GetWidth())
			modelSize.SetWidth(w);
		if (h > modelSize.GetHeight())
			modelSize.SetHeight(h);
	}
	bool updateView = false;
	int viewW, viewH;
	GetSize(&viewW, &viewH);

	if (viewW < GQB_MIN_WIDTH)
		viewW = GQB_MIN_WIDTH;

	if (viewH < GQB_MIN_HEIGHT)
		viewH = GQB_MIN_HEIGHT;

	if ((modelSize.GetWidth() > viewW || canvasSize.GetWidth() > viewW) &&
	        modelSize.GetWidth() != canvasSize.GetWidth())
	{
		canvasSize.SetWidth((modelSize.GetWidth() > viewW ? modelSize.GetWidth() : viewW));
		updateView = true;
	}
	if ((modelSize.GetHeight() > viewH || canvasSize.GetHeight() > viewH ) &&
	        modelSize.GetHeight() != canvasSize.GetHeight())
	{
		canvasSize.SetHeight((modelSize.GetHeight() > viewH ? modelSize.GetHeight() : viewH));
		updateView = true;
	}

	if (canvasSize.GetWidth() < viewW)
	{
		canvasSize.SetWidth(viewW);
		updateView = true;
	}

	if (canvasSize.GetHeight() < viewH)
	{
		canvasSize.SetHeight(viewH);
		updateView = true;
	}

	if (updateView)
	{
		SetVirtualSize(canvasSize);
	}

	FitInside();
	Refresh();
}

void gqbView::OnSize(wxSizeEvent &event)
{
	updateModelSize(NULL, true);
}


bool gqbView::canSaveAsImage()
{
	updateModelSize(NULL, true);
	return !(modelSize.GetWidth() == 0 || modelSize.GetHeight() == 0);
}

void gqbView::SaveAsImage(const wxString &fileName, wxBitmapType imgType)
{

	updateModelSize(NULL, true);

	if (modelSize.GetWidth() == 0 || modelSize.GetHeight() == 0)
	{
		wxMessageBox(_("Nothing to be saved!"), _("Save As an image"), wxOK | wxICON_INFORMATION);
		return;
	}

	int width = 0, height = 0;
	GetVirtualSize(&width, &height);

	/*
	* Create the bitmap from the Explain window
	*/
	wxMemoryDC memDC;
	wxBitmap tempBitmap(width, height);

	memDC.SelectObject(tempBitmap);
	memDC.Clear();

	// Draw the diagram on the bitmap (Memory Device Context)
	drawAll(memDC, false);

	memDC.SelectObject(wxNullBitmap);

	if (!tempBitmap.SaveFile(fileName, imgType))
	{
		wxLogError(_("Could not write the file %s: Errcode=%d."), fileName.c_str(), wxSysErrorCode());
	}
}



