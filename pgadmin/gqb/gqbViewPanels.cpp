//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbViewPanels.cpp - All panels used by GQB
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/minifram.h>
#include <wx/generic/gridctrl.h>
#include <wx/notebook.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>

// App headers
#include "gqb/gqbViewPanels.h"
#include "gqb/gqbGridProjTable.h"
#include "gqb/gqbGridRestTable.h"
#include "gqb/gqbGridOrderTable.h"
#include "gqb/gqbGridJoinTable.h"

// Images
#include "images/gqbUp.pngc"
#include "images/gqbUpTop.pngc"
#include "images/gqbDown.pngc"
#include "images/gqbDownBottom.pngc"
#include "images/gqbOrderAddAll.pngc"
#include "images/gqbOrderRemoveAll.pngc"
#include "images/gqbOrderRemove.pngc"
#include "images/gqbOrderAdd.pngc"
#include "images/gqbAddRest.pngc"
#include "images/gqbRemoveRest.pngc"
#include "images/tables.pngc"
#include "images/table-sm.pngc"
#include "images/column-sm.pngc"
#include "images/view-sm.pngc"
#include "images/gqbAdd.pngc"
#include "images/gqbRemove.pngc"

// Get available ID for Criteria & Joins Panel
long CRITERIA_PANEL_RESTRICTION_GRID_ID = ::wxNewId();
long JOINS_PANEL_GRID_ID = ::wxNewId();

//
//    View Columns Grid Panel Class.
//

BEGIN_EVENT_TABLE(gqbGridPanel, wxPanel)
	EVT_GRID_SELECT_CELL(gqbGridPanel::OnGridSelectCell)
	EVT_GRID_RANGE_SELECT(gqbGridPanel::OnGridRangeSelected)
	EVT_BUTTON(GQB_COLS_UP_BUTTON_ID, gqbGridPanel::OnButtonUp)
	EVT_BUTTON(GQB_COLS_UP_TOP_BUTTON_ID, gqbGridPanel::OnButtonUpTop)
	EVT_BUTTON(GQB_COLS_DOWN_BUTTON_ID, gqbGridPanel::OnButtonDown)
	EVT_BUTTON(GQB_COLS_DOWN_BOTTOM_BUTTON_ID, gqbGridPanel::OnButtonDownBottom)
END_EVENT_TABLE()

gqbGridPanel::gqbGridPanel(wxWindow *parent, wxWindowID id = wxID_ANY, gqbGridProjTable *gridModel = NULL):
	wxPanel(parent, -1)
{
	gModel = gridModel;
	allowSelCells = true;
	selTop = -1;
	selBottom = -1;
	upBitmap = *gqbUp_png_bmp;
	upTopBitmap = *gqbUpTop_png_bmp;
	downBitmap = *gqbDown_png_bmp;
	downBottomBitmap = *gqbDownBottom_png_bmp;

	buttonUp = new wxBitmapButton( this, GQB_COLS_UP_BUTTON_ID,  upBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Up") );
	buttonUp->SetToolTip(_("Move the selected column up"));
	buttonUpTop  = new wxBitmapButton( this, GQB_COLS_UP_TOP_BUTTON_ID,  upTopBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Top") );
	buttonUpTop->SetToolTip(_("Move the selected column to the top"));
	buttonDown = new wxBitmapButton( this, GQB_COLS_DOWN_BUTTON_ID,  downBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Down") );
	buttonDown->SetToolTip(_("Move the selected column down"));
	buttonDownBottom = new wxBitmapButton( this, GQB_COLS_DOWN_BOTTOM_BUTTON_ID,  downBottomBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Bottom") );
	buttonDownBottom->SetToolTip(_("Move the selected column to the bottom"));

	this->colsGrid = new wxGrid(this, -1, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP , wxT(""));
	colsGrid->SetTable(gModel, true, wxGrid::wxGridSelectCells);

	// Adjust the default row height to be more compact
	wxFont font = colsGrid->GetLabelFont();
	int nWidth = 0;
	int nHeight = 18;
	colsGrid->GetTextExtent(wxT("W"), &nWidth, &nHeight, NULL, NULL, &font);
	colsGrid->SetColLabelSize(nHeight + 6);
	colsGrid->SetRowLabelSize(35);
#ifdef __WXGTK__
	colsGrid->SetDefaultRowSize(nHeight + 8, TRUE);
#else
	colsGrid->SetDefaultRowSize(nHeight + 4, TRUE);
#endif

	wxBoxSizer *horizontalSizer = new wxBoxSizer( wxHORIZONTAL );
	horizontalSizer->Add(colsGrid,
	                     1,                                        // make vertically stretchable
	                     wxEXPAND |                                // make horizontally stretchable
	                     wxALL,                                    //   and make border all around
	                     3 );                                      // set border width to 10

	wxBoxSizer *buttonsSizer = new wxBoxSizer( wxVERTICAL );

	buttonsSizer->Add(
	    this->buttonUpTop,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	buttonsSizer->Add(
	    this->buttonUp,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	buttonsSizer->Add(
	    this->buttonDown,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	buttonsSizer->Add(
	    this->buttonDownBottom,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	horizontalSizer->Add(
	    buttonsSizer,
	    0,                                        // make vertically unstretchable
	    wxALIGN_CENTER );                         // no border and centre horizontally

	this->SetSizer(horizontalSizer);
}


gqbGridPanel::~gqbGridPanel()
{
	if (buttonUp)
		delete buttonUp;
	if(buttonDown)
		delete buttonDown;
	if(buttonUpTop)
		delete buttonUpTop;
	if(buttonDownBottom)
		delete buttonDownBottom;
}


void gqbGridPanel::SetGridColsSize()
{
	// After sizers determine the width of Grid then calculate the % space for each column about 33% each one
	int size = (int)((colsGrid->GetSize().GetWidth() - colsGrid->GetRowLabelSize()) * 0.30);
	colsGrid->SetColSize(0, size);
	colsGrid->SetColSize(1, size);
	colsGrid->SetColSize(2, size);
}


void gqbGridPanel::OnGridSelectCell( wxGridEvent &ev )
{
	if(allowSelCells)
	{
		if ( ev.Selecting() )
		{
			selTop = ev.GetRow();
			selBottom = -1;
		}
		else
		{
			selTop = -1;
			selBottom = -1;
		}
	}
	ev.Skip();
}


void gqbGridPanel::OnGridRangeSelected( wxGridRangeSelectEvent &ev )
{
	if(allowSelCells)
	{
		if ( ev.Selecting() )
		{
			selTop = ev.GetTopRow();
			selBottom = ev.GetBottomRow();
		}
		else
		{
			selTop = -1;
			selBottom = -1;
		}
	}
	ev.Skip();
}


void gqbGridPanel::OnButtonUp(wxCommandEvent &)
{
	// A single row is selected
	allowSelCells = false;
	if((selTop >= 0 && selBottom == -1) || (selTop == selBottom))
	{
		--selTop;
		gModel->changesPositions(selTop, selTop + 1);
		if(selTop < 0)
		{
			selTop = 0;
		}
		colsGrid->SelectBlock(selTop, 0, selTop, 1, false);
		colsGrid->SetGridCursor(selTop, 0);
		selBottom = -1;
	}
	else
	{
		// A range of rows is selected
		if (selTop >= 0 && selBottom >= 0)
		{
			int newTop = selTop - 1;
			gModel->changesRangeOnePos(selTop, selBottom, newTop);
			if(selTop > 0)                        // recalculate new selection area & avoid bad selection area
			{
				selTop--;
				selBottom--;
			}
			colsGrid->SelectBlock(selTop, 0, selBottom, 1, false);
			colsGrid->SetGridCursor(selTop, 0);
		}
	}
	allowSelCells = true;
}


void gqbGridPanel::OnButtonUpTop(wxCommandEvent &)
{
	allowSelCells = false;

	// A Single Row is selected
	if((selTop >= 0 && selBottom == -1) || (selTop == selBottom))
	{
		selBottom = selTop - 1;
		selTop = 0;
		gModel->changesRangeOnePos(selTop, selBottom, 1);
		colsGrid->SelectBlock(0, 0, 0, 1, false);
		colsGrid->SetGridCursor(0, 0);

		// Put variables in correct values now.
		selTop = 0;
		selBottom = -1;
	}
	// A range of rows is selected
	else
	{
		int newTop = 0;
		if (selTop >= 0 && selBottom >= 0)
		{
			// Move all range only one pos the require times to get the top
			for(int i = selTop; i > 0; i--)
			{
				newTop = selTop - 1;
				gModel->changesRangeOnePos(selTop, selBottom, newTop);

				// Recalculate new selection area & avoid bad selection area
				if(selTop > 0)
				{
					selTop--;
					selBottom--;
				}
				colsGrid->SelectBlock(selTop, 0, selBottom, 1, false);
				colsGrid->SetGridCursor(selTop, 0);
			}
		}
	}
	allowSelCells = true;
}


void gqbGridPanel::OnButtonDown(wxCommandEvent &)
{

	allowSelCells = false;

	// A single row is selected
	if((selTop >= 0 && selBottom == -1) || (selTop == selBottom))
	{
		++selTop;
		gModel->changesPositions(selTop, selTop - 1);

		// Adjust selection when selected item it's last item.
		if(selTop == gModel->GetNumberRows())
		{
			selTop--;
		}
		colsGrid->SelectBlock(selTop, 0, selTop, 1, false);
		colsGrid->SetGridCursor(selTop, 0);
		selBottom = -1;
	}
	// A range of rows is selected
	else
	{
		if (selTop >= 0 && selBottom >= 0)
		{
			int newTop = selTop + 1;
			gModel->changesRangeOnePos(selTop, selBottom, newTop);

			// Recalculate new selection area & avoid bad selection area
			if(selBottom < gModel->GetNumberRows() - 1)
			{
				selTop++;
				selBottom++;
			}
			colsGrid->SelectBlock(selTop, 0, selBottom, 1, false);
			colsGrid->SetGridCursor(selTop, 0);
		}
	}
	allowSelCells = true;
}


void gqbGridPanel::OnButtonDownBottom(wxCommandEvent &)
{
	allowSelCells = false;

	// A Single Row is selected
	if((selTop >= 0 && selBottom == -1) || (selTop == selBottom))
	{
		selBottom = gModel->GetNumberRows() - 1;
		selTop = selTop + 1;
		int newBottom = gModel->GetNumberRows() - 1;
		gModel->changesRangeOnePos(selTop, selBottom, selTop - 1);
		colsGrid->SelectBlock(newBottom, 0, newBottom, 1, false);
		colsGrid->SetGridCursor(newBottom, 0);

		// Put variables in correct values now.
		selTop = newBottom;
		selBottom = -1;
	}
	// A range of rows is selected
	else
	{
		int newTop = 0, size = gModel->GetNumberRows();

		if (selTop >= 0 && selBottom >= 0)
		{
			for(int i = selBottom; i < size; i++)
			{
				newTop = selTop + 1;
				gModel->changesRangeOnePos(selTop, selBottom, newTop);

				// Recalculate new selection area & avoid bad selection area
				if(selBottom < gModel->GetNumberRows() - 1)
				{
					selTop++;
					selBottom++;
				}
				colsGrid->SelectBlock(selTop, 0, selBottom, 1, false);
				colsGrid->SetGridCursor(selTop, 0);
			}
		}
	}
	allowSelCells = true;
}


//
//    Tree with Columns & tables inside a query model
//

gqbColsTree::gqbColsTree(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
	: wxTreeCtrl(parent, id, pos, size, style)
{
	wxImageList *imageList = new wxImageList(16, 16);
	imageList->Add(*tables_png_ico);
	imageList->Add(*table_sm_png_ico);
	imageList->Add(*column_sm_png_ico);
	imageList->Add(*view_sm_png_ico);
	this->AssignImageList(imageList);
	wxString a = _("Select column");
	createRoot(a);
	this->Expand(rootNode);
}


// Create root node
wxTreeItemId &gqbColsTree::createRoot(wxString &Name)
{
	rootNode = this->AddRoot(Name, 0, 0);
	return rootNode;
}


// Override the DeleteAllItems virtual function
// Needs to set null as item-data, otherwise they will delete
// the gqbQueryObject(s) and gqbColumn(s), while deleting these
// items
void gqbColsTree::DeleteAllItems()
{
	wxTreeItemId tableId;
	wxTreeItemIdValue tableCookie;
	wxTreeItemId rootId = this->GetRootItem();

	if ( this->GetChildrenCount(rootId, false) != 0 )
	{
		wxTreeItemId lastTableId = this->GetLastChild(rootId);
		tableId = this->GetFirstChild(rootId, tableCookie);
		while ( true )
		{
			this->SetItemData(tableId, NULL);
			wxTreeItemIdValue colCookie;
			wxTreeItemId colId = this->GetFirstChild(tableId, colCookie);
			wxTreeItemId lastColId = this->GetLastChild(tableId);
			if ( this->GetChildrenCount(tableId, false) != 0 )
			{
				while ( true )
				{
					this->SetItemData(colId, NULL);
					if ( colId != lastColId )
						colId = this->GetNextSibling(colId);
					else
						break;
				}
			}
			if ( lastTableId != tableId )
				tableId = this->GetNextSibling(tableId);
			else
				break;
		}
	}
	wxTreeCtrl::DeleteAllItems();
}

void gqbColsTree::refreshTree(gqbModel *model, gqbQueryObject *doNotInclude)
{
	// This remove and delete data inside tree's node
	this->DeleteAllItems();
	wxString a = _("Select column");
	createRoot(a);
	this->Expand(rootNode);

	wxTreeItemId parent;
	gqbIteratorBase *iterator = model->createQueryIterator();
	while(iterator->HasNext())
	{
		gqbQueryObject *tmpTable = (gqbQueryObject *)iterator->Next();

		if (doNotInclude && tmpTable == doNotInclude)
			continue;

		int iconIndex;
		if (tmpTable->parent->getType() == GQB_TABLE)
			iconIndex = 1;
		else // Must be a view
			iconIndex = 3;

		if(tmpTable->getAlias().length() > 0)
		{
			parent = this->AppendItem(rootNode, tmpTable->getAlias() , iconIndex, iconIndex, tmpTable);
		}
		else
		{
			parent = this->AppendItem(rootNode, tmpTable->getName() , iconIndex, iconIndex, tmpTable);
		}
		gqbIteratorBase *colsIterator = tmpTable->parent->createColumnsIterator();
		while(colsIterator->HasNext())
		{
			gqbColumn *tmpColumn = (gqbColumn *)colsIterator->Next();
			this->AppendItem(parent, tmpColumn->getName() , 2, 2, tmpColumn);
		}
		delete colsIterator;
	}
	delete iterator;

	if (rootNode)
	{
		this->SortChildren(rootNode);
		this->Expand(rootNode);
	}
}


//
//    Popup Window for gqbColsTree
//

gqbColsPopUp::gqbColsPopUp(wxWindow *parent, wxWindowID id, wxString title, wxPoint pos, const wxSize size):
	wxDialog(parent, id, title, pos, size, wxRESIZE_BORDER | wxCAPTION)
{
	this->SetSize(wxSize(243, 165));
	wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *vSizer = new wxBoxSizer(wxVERTICAL);

	editTree = new wxTextCtrl(this, QR_TREE, wxT(""), wxPoint(5, 5), wxSize(193, 22));
	editTree->AcceptsFocus();
	editTree->SetFocus();
	editTree->SetEditable(true);

	hSizer->Add(editTree, 1, wxEXPAND | wxRIGHT, 3);

	buttonTree = new wxButton(this, QR_TREE_OK, _("OK") , wxPoint(199, 5), wxDefaultSize, wxBU_EXACTFIT);
	this->Connect(QR_TREE_OK, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &gqbColsPopUp::OnPopUpOKClick);

	hSizer->Add(buttonTree);
	vSizer->Add(hSizer, 0, wxEXPAND | wxALL, 3);

	colsTree = new gqbColsTree(this, QR_TREE, wxPoint(5, 31),  wxSize(224, 104), wxTR_HAS_BUTTONS | wxSIMPLE_BORDER | wxTR_SINGLE);
	this->Connect(QR_TREE, wxEVT_COMMAND_TREE_ITEM_ACTIVATED, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) &gqbColsPopUp::OnPopUpTreeDoubleClick);
	this->Connect(QR_TREE, wxEVT_COMMAND_TREE_SEL_CHANGED, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) &gqbColsPopUp::OnPopUpTreeClick);

	vSizer->Add(colsTree, 1, wxEXPAND | wxALL, 3);
	SetSizer(vSizer);
}


void gqbColsPopUp::refreshTree(gqbModel *_model)
{
	model = _model;
	if(colsTree && _model)
	{
		colsTree->refreshTree(model);
	}

}


void  gqbColsPopUp::OnPopUpOKClick(wxCommandEvent &event)
{
	this->usedGrid->SetCellValue(_row, _col, this->getEditText());
	this->MakeModal(false);
	this->Hide();
	this->GetParent()->Refresh();
}


void  gqbColsPopUp::OnPopUpTreeDoubleClick(wxTreeEvent &event)
{
	if(colsTree)
	{
		wxTreeItemId itemId = event.GetItem();
		wxTreeItemId itemIdParent =  colsTree->GetItemParent(itemId);
		if(!colsTree->ItemHasChildren(itemId) && (colsTree->GetRootItem() != itemId))
		{
			this->usedGrid->SetCellValue(_row, _col, this->getEditText());
			if(this->usedGrid->GetCellValue(_row, _col).length() <= 0)
			{
				this->usedGrid->SetCellValue(_row, _col, _("Set value"));
			}
			this->MakeModal(false);
			this->Hide();
			this->GetParent()->Refresh();
		}
	}
}

void  gqbColsPopUp::OnPopUpTreeClick(wxTreeEvent &event)
{
	if(colsTree)
	{
		wxTreeItemId itemId = event.GetItem();
		wxTreeItemId itemIdParent =  colsTree->GetItemParent(itemId);

		if(!colsTree->ItemHasChildren(itemId) && (colsTree->GetRootItem() != itemId))
		{
			this->editTree->SetValue(qtIdent(colsTree->GetItemText(itemIdParent)) + wxT(".") + qtIdent(colsTree->GetItemText(itemId)));
		}
	}
}


void gqbColsPopUp::setEditText(wxString text)
{
	this->editTree->SetValue(text);
}


void gqbColsPopUp::focus()
{
	this->editTree->SetFocus();
	this->editTree->SetSelection(-1, -1);
}


//
//    View Selection Criteria Panel Class.
//

BEGIN_EVENT_TABLE(gqbCriteriaPanel, wxPanel)
	EVT_BUTTON(GQB_COLS_ADD_BUTTON_ID, gqbCriteriaPanel::OnButtonAdd)
	EVT_BUTTON(GQB_COLS_DROP_BUTTON_ID, gqbCriteriaPanel::OnButtonDrop)
END_EVENT_TABLE()

gqbCriteriaPanel::gqbCriteriaPanel(wxWindow *parent, gqbModel *_model, gqbGridRestTable *gridModel):
	wxPanel(parent, -1)
{
	model = _model;
	gModel = gridModel;
	colsPopUp = NULL;

	this->restrictionsGrid = new gqbCustomGrid(this, CRITERIA_PANEL_RESTRICTION_GRID_ID);
	restrictionsGrid->SetTable(gModel, true, wxGrid::wxGridSelectCells);
	this->restrictionsGrid->SetSelectionMode(wxGrid::wxGridSelectRows);

	this->Connect(CRITERIA_PANEL_RESTRICTION_GRID_ID, wxEVT_GRID_CELL_LEFT_CLICK, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &gqbCriteriaPanel::OnCellLeftClick);
	// GQB-TODO: in a future implement OnMouseWheel

	addBitmap = *gqbAddRest_png_bmp;
	dropBitmap = *gqbRemoveRest_png_bmp;
	buttonAdd = new wxBitmapButton( this, GQB_COLS_ADD_BUTTON_ID,  addBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add"));
	buttonAdd->SetToolTip(_("Add a new criteria line"));
	buttonDrop = new wxBitmapButton( this, GQB_COLS_DROP_BUTTON_ID,  dropBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove"));
	buttonDrop->SetToolTip(_("Remove the selected criteria line"));

	wxBoxSizer *horizontalSizer = new wxBoxSizer( wxHORIZONTAL );
	horizontalSizer->Add(restrictionsGrid,
	                     1,                                        // make vertically stretchable
	                     wxEXPAND |                                // make horizontally stretchable
	                     wxALL,                                    //   and make border all around
	                     3 );                                     // set border width to 10

	wxBoxSizer *buttonsSizer = new wxBoxSizer( wxVERTICAL );

	buttonsSizer->Add(
	    this->buttonAdd,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    10 );                                     // set border width to 10

	buttonsSizer->Add(
	    this->buttonDrop,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    10 );                                     // set border width to 10

	horizontalSizer->Add(
	    buttonsSizer,
	    0,                                        // make vertically unstretchable
	    wxALIGN_CENTER );                         // no border and centre horizontally

	this->SetSizer(horizontalSizer);
}


void gqbCriteriaPanel::showColsPopUp(int row, int col, wxPoint pos)
{
	if(!colsPopUp)
	{
		colsPopUp = new gqbColsPopUp(this, -1, wxT("Set Value"), wxDefaultPosition, wxDefaultSize);
	}

	refreshTree(model);

	// Set initial Value
	colsPopUp->setEditText(restrictionsGrid->GetCellValue(row, col));

	// Set Position for Pop Up Tree
	// Position of wxNotebook
	wxPoint p = this->GetParent()->GetPosition();
	p.x += pos.x;
	p.y += pos.y;
	wxPoint p2 = this->GetPosition();

	// Position of panel inside wxNotebook
	p.x += p2.x;
	p.y += p2.y + 40;
	colsPopUp->SetPosition(p);
	colsPopUp->Show();
	colsPopUp->MakeModal(true);
	colsPopUp->focus();
	colsPopUp->setUsedCell(restrictionsGrid, row, col);
}


void gqbCriteriaPanel::refreshTree(gqbModel *_model)
{
	// GQB-TODO: Do this through controller...
	model = _model;
	if(colsPopUp && model)
		colsPopUp->refreshTree(model);

}


void gqbCriteriaPanel::OnCellLeftClick(wxGridEvent &event)
{
	wxObject *object = event.GetEventObject();
	gqbCustomGrid *grid = wxDynamicCast( object, gqbCustomGrid );

	// Only show editor y case of column 1
	if(event.GetCol() == 1)
	{
		grid->ComboBoxEvent(event);
	}
	else if(event.GetCol() == 0 || event.GetCol() == 2)
	{
		// Allow mini browser frame to be visible to user
		wxRect cellSize = grid->CellToRect(event.GetRow(), event.GetCol());
		wxPoint p = event.GetPosition();
		restrictionsGrid->CalcUnscrolledPosition(p.x, p.y, &p.x, &p.y);

		// Number 17 is button's width at cellRender function [nButtonWidth]
		if((grid->GetRowLabelSize() + cellSize.GetRight()) - (p.x) <= 17 )
		{
			p = event.GetPosition();
			showColsPopUp(event.GetRow(), event.GetCol(), p);
		}
	}                                             //GQB-TODO 1,2 can be integrate with 3
	else if(event.GetCol() == 3)
	{
		// Change connector of the Restriction between AND/OR
		wxRect cellSize = restrictionsGrid->CellToRect(event.GetRow(), event.GetCol());
		wxPoint p = event.GetPosition();
		restrictionsGrid->CalcUnscrolledPosition(p.x, p.y, &p.x, &p.y);

		// Number 17 is button's width at cellRender function [nButtonWidth]
		if((restrictionsGrid->GetRowLabelSize() + cellSize.GetRight()) - (p.x) <= 17 )
		{
			p = event.GetPosition();
			if( gModel->GetValue(event.GetRow(), 3).Contains(wxT("AND")) )
			{
				gModel->SetValue(event.GetRow(), 3, wxT("OR"));
			}
			else
			{
				gModel->SetValue(event.GetRow(), 3, wxT("AND"));
			}
			restrictionsGrid->Refresh();
		}
	}
	event.Skip();
}


void gqbCriteriaPanel::OnButtonAdd(wxCommandEvent &)
{
	// GQB-TODO: use controller one
	gqbQueryRestriction *r = new gqbQueryRestriction();
	gModel->AppendItem(r);
	int row = model->getRestrictions()->restrictionsCount() - 1;

	wxString strChoices[16] = {wxT("="), wxT("!="), wxT("<"), wxT("<="), wxT(">"), wxT(">="), wxT("BETWEEN"), wxT("LIKE"), wxT("NOT LIKE"), wxT("ILIKE"), wxT("NOT ILIKE"), wxT("IN"), wxT("NOT IN"), wxT("NOT BETWEEN"), wxT("IS NULL"), wxT("IS NOT NULL")};
	restrictionsGrid->SetCellRenderer(row, 1, new wxGridCellComboBoxRenderer);
	restrictionsGrid->SetCellRenderer(row, 0, new wxGridCellButtonRenderer);
	restrictionsGrid->SetCellEditor(row, 1, new dxGridCellSizedChoiceEditor(WXSIZEOF(strChoices), strChoices));
	restrictionsGrid->SetCellRenderer(row, 2, new wxGridCellButtonRenderer);
	restrictionsGrid->SetCellRenderer(row, 3, new wxGridCellButtonRenderer);
	restrictionsGrid->SetReadOnly(row, 3, true);
}


void gqbCriteriaPanel::OnButtonDrop(wxCommandEvent &)
{
	if(restrictionsGrid->GetGridCursorRow() >= 0)
		gModel->DeleteRows(restrictionsGrid->GetGridCursorRow(), 1);
	restrictionsGrid->Refresh();
}


void gqbCriteriaPanel::SetGridColsSize()
{
	// After sizers determine the width of Grid then calculate the % space for each column about 33% each one
	int size = (int)((restrictionsGrid->GetSize().GetWidth() - restrictionsGrid->GetRowLabelSize()) * 0.225);
	restrictionsGrid->SetColSize(0, size);
	restrictionsGrid->SetColSize(1, size);
	restrictionsGrid->SetColSize(2, size);
	restrictionsGrid->SetColSize(3, size);
}


//
//  View Selection Criteria Panel Class's Grid.
//
gqbCustomGrid::gqbCustomGrid(wxWindow *parent, wxWindowID id):
	wxGrid(parent, id, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP, wxT("")),
	m_selTemp(NULL)
{
	// Adjust the default row height to be more compact
	wxFont font = GetLabelFont();
	int nWidth = 0;
	int nHeight = 18;
	GetTextExtent(wxT("W"), &nWidth, &nHeight, NULL, NULL, &font);
	SetColLabelSize(nHeight + 6);
	SetRowLabelSize(35);
#ifdef __WXGTK__
	SetDefaultRowSize(nHeight + 8, TRUE);
#else
	SetDefaultRowSize(nHeight + 4, TRUE);
#endif
}


void gqbCustomGrid::RevertSel()
{
	if (m_selTemp)
	{
		wxASSERT(m_selection == NULL);
		m_selection = m_selTemp;
		m_selTemp = NULL;
	}
}


void gqbCustomGrid::ComboBoxEvent(wxGridEvent &event)
{

	// This forces the cell to go into edit mode directly
	this->m_waitForSlowClick = TRUE;
	int row = event.GetRow();
	int col = event.GetCol();

	this->SetGridCursor(row, col);

	// Store the click co-ordinates in the editor if possible
	// if an editor has created a ClientData area, we presume it's
	// a wxPoint and we store the click co-ordinates
	wxGridCellEditor *pEditor  = this->GetCellEditor(event.GetRow(), event.GetCol());
	wxPoint *pClickPoint = (wxPoint *)pEditor->GetClientData();
	if (pClickPoint)
	{
		*pClickPoint = this->ClientToScreen(event.GetPosition());
#ifndef __WINDOWS__
		EnableCellEditControl(true);
#endif
	}

	// hack to prevent selection from being lost when click combobox
	if (this->IsInSelection(event.GetRow(), event.GetCol()))
	{
		this->m_selTemp = this->m_selection;
		this->m_selection = NULL;
	}
	pEditor->DecRef();
}


//
//
//   Order by Panel
//
//

BEGIN_EVENT_TABLE(gqbOrderPanel, wxPanel)
	EVT_BUTTON(GQB_ORDER_DROP_BUTTON_ID, gqbOrderPanel::OnButtonRemove)
	EVT_BUTTON(GQB_ORDER_DROP_ALL_BUTTON_ID, gqbOrderPanel::OnButtonRemoveAll)
	EVT_BUTTON(GQB_ORDER_ADD_BUTTON_ID, gqbOrderPanel::OnButtonAdd)
	EVT_BUTTON(GQB_ORDER_ADD_ALL_BUTTON_ID, gqbOrderPanel::OnButtonAddAll)
	EVT_GRID_SELECT_CELL(gqbOrderPanel::OnGridSelectCell)
	EVT_GRID_RANGE_SELECT(gqbOrderPanel::OnGridRangeSelected)
	EVT_BUTTON(GQB_ORDER_UP_BUTTON_ID, gqbOrderPanel::OnButtonUp)
	EVT_BUTTON(GQB_ORDER_UP_TOP_BUTTON_ID, gqbOrderPanel::OnButtonUpTop)
	EVT_BUTTON(GQB_ORDER_DOWN_BUTTON_ID, gqbOrderPanel::OnButtonDown)
	EVT_BUTTON(GQB_ORDER_DOWN_BOTTOM_BUTTON_ID, gqbOrderPanel::OnButtonDownBottom)
END_EVENT_TABLE()

gqbOrderPanel::gqbOrderPanel(wxWindow *parent, gqbGridOrderTable *gridTableLeft, gqbGridOrderTable *gridTableRight):
	wxPanel(parent, -1)
{

	// GQB-TODO: change bitmap buttons
	selLeft = -1;
	selRightTop = -1;
	selRightBottom = -1;
	tableLeft = gridTableLeft;
	tableRight = gridTableRight;
	allowSelCells = true;

	addBitmap = *gqbOrderAdd_png_bmp;
	addAllBitmap = *gqbOrderAddAll_png_bmp;
	removeBitmap = *gqbOrderRemove_png_bmp;
	removeAllBitmap = *gqbOrderRemoveAll_png_bmp;

	buttonAdd = new wxBitmapButton( this, GQB_ORDER_ADD_BUTTON_ID,  addBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add Column") );
	buttonAdd->SetToolTip(_("Add the selected column"));
	buttonAddAll = new wxBitmapButton( this, GQB_ORDER_ADD_ALL_BUTTON_ID,  addAllBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add All Columns") );
	buttonAddAll->SetToolTip(_("Add all columns"));
	buttonRemove = new wxBitmapButton( this, GQB_ORDER_DROP_BUTTON_ID,  removeBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove Column") );
	buttonRemove->SetToolTip(_("Remove the selected column"));
	buttonRemoveAll = new wxBitmapButton( this, GQB_ORDER_DROP_ALL_BUTTON_ID,  removeAllBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove All Columns") );
	buttonRemoveAll->SetToolTip(_("Remove all columns"));

	upBitmap = *gqbUp_png_bmp;
	upTopBitmap = *gqbUpTop_png_bmp;
	downBitmap = *gqbDown_png_bmp;
	downBottomBitmap = *gqbDownBottom_png_bmp;

	buttonUp = new wxBitmapButton( this, GQB_ORDER_UP_BUTTON_ID,  upBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Up") );
	buttonUp->SetToolTip(_("Move the selected column up"));
	buttonDown  = new wxBitmapButton( this, GQB_ORDER_DOWN_BUTTON_ID,  downBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Down") );
	buttonDown->SetToolTip(_("Move the selected column down"));
	buttonUpTop = new wxBitmapButton( this, GQB_ORDER_UP_TOP_BUTTON_ID,  upTopBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Top") );
	buttonUpTop->SetToolTip(_("Move the selected column to the top"));
	buttonDownBottom = new wxBitmapButton( this, GQB_ORDER_DOWN_BOTTOM_BUTTON_ID,  downBottomBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Bottom") );
	buttonDownBottom->SetToolTip(_("Move the selected column to the bottom"));

	availableColumns = new wxGrid(this, -1, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP , wxT("Available Columns"));
	availableColumns->SetTable(gridTableLeft);
	availableColumns->EnableEditing(false);

	// Adjust the default row height to be more compact
	wxFont font = availableColumns->GetLabelFont();
	int nWidth = 0;
	int nHeight = 18;
	availableColumns->GetTextExtent(wxT("W"), &nWidth, &nHeight, NULL, NULL, &font);
	availableColumns->SetColLabelSize(nHeight + 6);
	availableColumns->SetRowLabelSize(35);
#ifdef __WXGTK__
	availableColumns->SetDefaultRowSize(nHeight + 8, TRUE);
#else
	availableColumns->SetDefaultRowSize(nHeight + 4, TRUE);
#endif


	wxBoxSizer *horizontalSizer = new wxBoxSizer( wxHORIZONTAL );

	horizontalSizer->Add(availableColumns,
	                     1,                                        // make vertically stretchable
	                     wxEXPAND |                                // make horizontally stretchable
	                     wxALL,                                    //   and make border all around
	                     3 );                                      // set border width to 10

	wxBoxSizer *buttonsSizer1 = new wxBoxSizer( wxVERTICAL );

	buttonsSizer1->Add(
	    this->buttonAdd,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	buttonsSizer1->Add(
	    this->buttonAddAll,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	buttonsSizer1->Add(
	    this->buttonRemove,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	buttonsSizer1->Add(
	    this->buttonRemoveAll,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	horizontalSizer->Add(
	    buttonsSizer1,
	    0,                                        // make vertically unstretchable
	    wxALIGN_CENTER );                         // no border and centre horizontally

	// GQB-TODO: change 333 for a new and better one.
	usedColumns = new wxGrid(this, 333, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP , wxT("Columns Order"));
	usedColumns->SetTable(gridTableRight);
	usedColumns->EnableEditing(false);

	// Adjust the default row height to be more compact
	font = usedColumns->GetLabelFont();
	nWidth = 0;
	nHeight = 18;
	usedColumns->GetTextExtent(wxT("W"), &nWidth, &nHeight, NULL, NULL, &font);
	usedColumns->SetColLabelSize(nHeight + 6);
	usedColumns->SetRowLabelSize(35);
#ifdef __WXGTK__
	usedColumns->SetDefaultRowSize(nHeight + 8, TRUE);
#else
	usedColumns->SetDefaultRowSize(nHeight + 4, TRUE);
#endif

	this->Connect(333, wxEVT_GRID_CELL_LEFT_CLICK, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &gqbOrderPanel::OnCellLeftClick);

	horizontalSizer->Add(usedColumns,
	                     1,                                        // make vertically stretchable
	                     wxEXPAND |                                // make horizontally stretchable
	                     wxALL,                                    //   and make border all around
	                     3 );                                      // set border width to 10

	wxBoxSizer *buttonsSizer2 = new wxBoxSizer( wxVERTICAL );

	buttonsSizer2->Add(
	    this->buttonUpTop,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	buttonsSizer2->Add(
	    this->buttonUp,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	buttonsSizer2->Add(
	    this->buttonDown,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	buttonsSizer2->Add(
	    this->buttonDownBottom,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    3 );                                      // set border width to 10

	horizontalSizer->Add(
	    buttonsSizer2,
	    0,                                        // make vertically unstretchable
	    wxALIGN_CENTER );                         // no border and centre horizontally

	this->SetSizer(horizontalSizer);
}


void gqbOrderPanel::OnButtonRemove(wxCommandEvent &)
{
	if(usedColumns->GetRows() > 0)
	{
		if(selRightTop != -1)
		{
			gqbColumn *col = (gqbColumn *) tableRight->getObjectAt(selRightTop, 0);
			gqbQueryObject *colParent = (gqbQueryObject *) tableRight->getObjectAt(selRightTop, 1);
			tableLeft->AppendItem(col, colParent, 'N');
			tableRight->removeRowAt(selRightTop);
		}
	}
}


void gqbOrderPanel::OnButtonRemoveAll(wxCommandEvent &)
{
	if(usedColumns->GetRows() > 0)
	{
		for(int i = usedColumns->GetRows() - 1; i >= 0; i--)
		{
			gqbColumn *col = (gqbColumn *) tableRight->getObjectAt(i, 0);
			gqbQueryObject *colParent = (gqbQueryObject *) tableRight->getObjectAt(i, 1);
			tableLeft->AppendItem(col, colParent, 'N');
			tableRight->removeRowAt(i);
		}
	}
}


void gqbOrderPanel::OnButtonAdd(wxCommandEvent &)
{
	if(availableColumns->GetRows() > 0)
	{
		if(selLeft != -1)
		{
			gqbColumn *col = (gqbColumn *) tableLeft->getObjectAt(selLeft, 0);
			gqbQueryObject *colParent = (gqbQueryObject *) tableLeft->getObjectAt(selLeft, 1);
			tableRight->AppendItem(col, colParent, 'A');
			usedColumns->SetCellRenderer((usedColumns->GetRows() - 1), 1, new wxGridCellButtonRenderer);
			tableLeft->removeRowAt(selLeft);
		}
	}
}


void gqbOrderPanel::OnButtonAddAll(wxCommandEvent &)
{
	if(availableColumns->GetRows() > 0)
	{
		for(int i = availableColumns->GetRows() - 1; i >= 0; i--)
		{
			gqbColumn *col = (gqbColumn *) tableLeft->getObjectAt(i, 0);
			gqbQueryObject *colParent = (gqbQueryObject *) tableLeft->getObjectAt(i, 1);
			tableRight->AppendItem(col, colParent, 'A');
			usedColumns->SetCellRenderer((usedColumns->GetRows() - 1), 1, new wxGridCellButtonRenderer);
			tableLeft->removeRowAt(i);
		}
	}
}


void gqbOrderPanel::OnGridSelectCell( wxGridEvent &ev )
{
	if(allowSelCells)
	{
		if ( ev.Selecting() )
		{
			wxObject *object = ev.GetEventObject();
			wxGrid *grid = wxDynamicCast( object, wxGrid );
			if(grid->GetCols() == 1)              // Left Grid
			{
				selLeft = ev.GetRow();
			}
			else
			{
				// Right Grid
				selRightTop = ev.GetRow();
				selRightBottom = -1;
			}
		}
		else
		{
			selRightTop = -1;                     // GQB-TODO: this is correct??
			selRightBottom = -1;
			selLeft = -1;
		}
	}
	ev.Skip();
}


void gqbOrderPanel::OnGridRangeSelected( wxGridRangeSelectEvent &ev )
{
	if(allowSelCells)
	{
		if ( ev.Selecting() )
		{
			selRightTop = ev.GetTopRow();
			selRightBottom = ev.GetBottomRow();
		}
		else
		{
			selRightTop = -1;
			selRightBottom = -1;
		}
	}
	ev.Skip();
}


void gqbOrderPanel::OnButtonUp(wxCommandEvent &)
{
	// A single row is selected
	allowSelCells = false;
	if((selRightTop >= 0 && selRightBottom == -1) || (selRightTop == selRightBottom))
	{
		--selRightTop;
		tableRight->changesPositions(selRightTop, selRightTop + 1);
		if(selRightTop < 0)
		{
			selRightTop = 0;
		}
		usedColumns->SelectBlock(selRightTop, 0, selRightTop, 1, false);
		usedColumns->SetGridCursor(selRightTop, 0);
		selRightBottom = -1;
	}
	else
	{
		// A range of rows is selected
		if (selRightTop >= 0 && selRightBottom >= 0)
		{
			int newTop = selRightTop - 1;
			tableRight->changesRangeOnePos(selRightTop, selRightBottom, newTop);
			if(selRightTop > 0)                   // Recalculate new selection area & avoid bad selection area
			{
				selRightTop--;
				selRightBottom--;
			}
			usedColumns->SelectBlock(selRightTop, 0, selRightBottom, 1, false);
			usedColumns->SetGridCursor(selRightTop, 0);
		}
	}
	allowSelCells = true;
}


void gqbOrderPanel::OnButtonUpTop(wxCommandEvent &)
{
	allowSelCells = false;

	// A Single Row is selected
	if((selRightTop >= 0 && selRightBottom == -1) || (selRightTop == selRightBottom))
	{
		selRightBottom = selRightTop - 1;
		selRightTop = 0;
		tableRight->changesRangeOnePos(selRightTop, selRightBottom, 1);
		usedColumns->SelectBlock(0, 0, 0, 1, false);
		usedColumns->SetGridCursor(0, 0);

		// Put variables in correct values now.
		selRightTop = 0;
		selRightBottom = -1;
	}
	// A range of rows is selected
	else
	{
		int newTop = 0;
		if (selRightTop >= 0 && selRightBottom >= 0)
		{
			// Move all range only one pos the require times to get the top
			for(int i = selRightTop; i > 0; i--)
			{
				newTop = selRightTop - 1;
				tableRight->changesRangeOnePos(selRightTop, selRightBottom, newTop);

				// Recalculate new selection area & avoid bad selection area
				if(selRightTop > 0)
				{
					selRightTop--;
					selRightBottom--;
				}
				usedColumns->SelectBlock(selRightTop, 0, selRightBottom, 1, false);
				usedColumns->SetGridCursor(selRightTop, 0);
			}
		}
	}
	allowSelCells = true;
}


void gqbOrderPanel::OnButtonDown(wxCommandEvent &)
{

	allowSelCells = false;

	// A single row is selected
	if((selRightTop >= 0 && selRightBottom == -1) || (selRightTop == selRightBottom))
	{
		++selRightTop;
		tableRight->changesPositions(selRightTop, selRightTop - 1);

		// Adjust selection when selected item it's last item.
		if(selRightTop == tableRight->GetNumberRows())
		{
			selRightTop--;
		}
		usedColumns->SelectBlock(selRightTop, 0, selRightTop, 1, false);
		usedColumns->SetGridCursor(selRightTop, 0);
		selRightBottom = -1;
	}
	// A range of rows is selected
	else
	{
		if (selRightTop >= 0 && selRightBottom >= 0)
		{
			int newTop = selRightTop + 1;
			tableRight->changesRangeOnePos(selRightTop, selRightBottom, newTop);

			// Recalculate new selection area & avoid bad selection area
			if(selRightBottom < tableRight->GetNumberRows() - 1)
			{
				selRightTop++;
				selRightBottom++;
			}
			usedColumns->SelectBlock(selRightTop, 0, selRightBottom, 1, false);
			usedColumns->SetGridCursor(selRightTop, 0);
		}
	}
	allowSelCells = true;
}


void gqbOrderPanel::OnButtonDownBottom(wxCommandEvent &)
{
	allowSelCells = false;

	// A Single Row is selected
	if((selRightTop >= 0 && selRightBottom == -1) || (selRightTop == selRightBottom))
	{
		selRightBottom = tableRight->GetNumberRows() - 1;
		selRightTop = selRightTop + 1;
		int newBottom = tableRight->GetNumberRows() - 1;
		tableRight->changesRangeOnePos(selRightTop, selRightBottom, selRightTop - 1);
		usedColumns->SelectBlock(newBottom, 0, newBottom, 1, false);
		usedColumns->SetGridCursor(newBottom, 0);

		// Put variables in correct values now.
		selRightTop = newBottom;
		selRightBottom = -1;
	}
	// A range of rows is selected
	else
	{
		int newTop = 0, size = tableRight->GetNumberRows();

		if (selRightTop >= 0 && selRightBottom >= 0)
		{
			for(int i = selRightBottom; i < size; i++)
			{
				newTop = selRightTop + 1;
				tableRight->changesRangeOnePos(selRightTop, selRightBottom, newTop);

				// Recalculate new selection area & avoid bad selection area
				if(selRightBottom < tableRight->GetNumberRows() - 1)
				{
					selRightTop++;
					selRightBottom++;
				}
				usedColumns->SelectBlock(selRightTop, 0, selRightBottom, 1, false);
				usedColumns->SetGridCursor(selRightTop, 0);
			}
		}
	}
	allowSelCells = true;
}


void gqbOrderPanel::SetGridColsSize()
{
	// After sizers determine the width of Grid then calculate the % space for each column
	int size = (int)((availableColumns->GetSize().GetWidth() - availableColumns->GetRowLabelSize()) * 0.90);
	availableColumns->SetColSize(0, size);

	size = (int)((usedColumns->GetSize().GetWidth() - usedColumns->GetRowLabelSize()) * 0.65);
	int size2 = (int)((usedColumns->GetSize().GetWidth() - usedColumns->GetRowLabelSize()) * 0.25);
	usedColumns->SetColSize(0, size);
	usedColumns->SetColSize(1, size2);
}


void gqbOrderPanel::OnCellLeftClick(wxGridEvent &event)
{

	// Only show editor y case of column 1
	if(event.GetCol() == 1)
	{
		// Change kind of order of the columns between ASC and DESC
		wxRect cellSize = usedColumns->CellToRect(event.GetRow(), event.GetCol());
		wxPoint p = event.GetPosition();
		usedColumns->CalcUnscrolledPosition(p.x, p.y, &p.x, &p.y);

		// Number 17 is button's width at cellRender function [nButtonWidth]
		if((usedColumns->GetRowLabelSize() + cellSize.GetRight()) - (p.x) <= 17 )
		{
			p = event.GetPosition();
			if( tableRight->GetValue(event.GetRow(), 1).Contains(wxT("ASC")) )
			{
				tableRight->SetValue(event.GetRow(), 1, wxT("DESC"));
			}
			else
			{
				tableRight->SetValue(event.GetRow(), 1, wxT("ASC"));
			}
			usedColumns->Refresh();
		}
	}
	event.Skip();
}

// Popup window for gqbJoinsPanel
//
gqbJoinsPopUp::gqbJoinsPopUp(
    gqbJoinsPanel *parent, wxWindowID id, wxString title,
    wxPoint pos, const wxSize size, gqbQueryJoin *_join,
    bool isSource, gqbGridJoinTable *_gmodel)
	: gqbColsPopUp(parent, id, title, pos, size)
{
	this->editTree->SetEditable(false);

	// Handles different events for Ok button, single mouse click, double mouse click
	this->Connect(QR_TREE_OK, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &gqbJoinsPopUp::OnPopUpOKClick);
	this->Connect(QR_TREE, wxEVT_COMMAND_TREE_ITEM_ACTIVATED, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) &gqbJoinsPopUp::OnPopUpTreeDoubleClick);
	this->Connect(QR_TREE, wxEVT_COMMAND_TREE_SEL_CHANGED, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) &gqbJoinsPopUp::OnPopUpTreeClick);

	this->selectedTbl = NULL;
	this->selectedCol = NULL;
	this->join = _join;
	this->isSource = isSource;
	this->gModel = _gmodel;
}

void gqbJoinsPopUp::refreshTree(gqbModel *_model)
{
	model = _model;
	if(colsTree && _model)
	{
		// Do not include already included Table
		// For self join, other entity for the same table should be used with alias
		if ( join )
			colsTree->refreshTree(model, isSource ? join->getDestQTable() : join->getSourceQTable());
		else
			colsTree->refreshTree(model);
	}
}

// single mouse click on tree
void  gqbJoinsPopUp::OnPopUpTreeClick(wxTreeEvent &event)
{
	if( colsTree )
	{
		wxTreeItemId itemId = event.GetItem();
		wxTreeItemId itemIdParent =  colsTree->GetItemParent(itemId);

		if(!colsTree->ItemHasChildren(itemId) && (colsTree->GetRootItem() != itemId))
		{
			selectedCol = (gqbColumn *)colsTree->GetItemData(itemId);
			selectedTbl = (gqbQueryObject *)colsTree->GetItemData(itemIdParent);
			this->editTree->SetValue(qtIdent(colsTree->GetItemText(itemIdParent)) + wxT(".") + qtIdent(colsTree->GetItemText(itemId)));
		}
		else
		{
			selectedCol = NULL;
			selectedTbl = NULL;
		}
	}
}


void gqbJoinsPopUp::OnPopUpOKClick(wxCommandEvent &event)
{
	if( colsTree && selectedCol && selectedTbl )
	{
		// This should update the selected Join with the new values.
		updateJoin();
	}

	this->MakeModal(false);
	this->Hide();
	this->GetParent()->Refresh();
	this->join = NULL;
	this->selectedCol = NULL;
	this->selectedTbl = NULL;
}


// Update the view fo this query table, involved in
// the whole operation
void gqbJoinsPanel::updateView(gqbQueryObject *table)
{
	if (table)
		controller->getView()->updateTable(table);
}

void gqbJoinsPopUp::updateJoin()
{
	if ((isSource ? join->getSCol() : join->getDCol()) != selectedCol)
	{
		// Create a new join with the existing data
		// Replace it in the gqbJoinTable with the existing one
		// Unregister the join, if exists
		gqbQueryObject *srcTbl = ( isSource ? selectedTbl : join->getSourceQTable() );
		gqbQueryObject *destTbl = ( isSource ? join->getDestQTable() : selectedTbl );
		gqbColumn *srcCol = ( isSource ? selectedCol : join->getSCol() );
		gqbColumn *destCol = ( isSource ? join->getDCol() : selectedCol );
		type_Join joinType = join->getKindofJoin();

		gqbQueryJoin *newJoin = NULL;
		if( srcTbl && destTbl )
		{
			newJoin = srcTbl->addJoin(srcTbl, destTbl, srcCol, destCol, joinType);
			((gqbJoinsPanel *)GetParent())->updateView(newJoin->getSourceQTable());
		}
		else
			newJoin = new gqbQueryJoin(srcTbl, destTbl, srcCol, destCol, joinType);

		gModel->ReplaceJoin(join, newJoin);

		if (join->getSourceQTable() && join->getDestQTable())
		{
			// This will remove the join object too
			gqbQueryObject *srcObj = join->getSourceQTable();
			srcObj->removeJoin(join, true);
		}
		else
		{
			delete join;
		}
		join = newJoin;
	}
}


void  gqbJoinsPopUp::OnPopUpTreeDoubleClick(wxTreeEvent &event)
{
	if(colsTree)
	{
		wxTreeItemId itemId = event.GetItem();
		wxTreeItemId itemIdParent =  colsTree->GetItemParent(itemId);
		if(!colsTree->ItemHasChildren(itemId) && (colsTree->GetRootItem() != itemId))
		{
			selectedCol = (gqbColumn *)colsTree->GetItemData(itemId);
			selectedTbl = (gqbQueryObject *)colsTree->GetItemData(itemIdParent);

			updateJoin();

			this->MakeModal(false);
			this->Hide();
			this->GetParent()->Refresh();
			this->join = NULL;
			this->selectedCol = NULL;
			this->selectedTbl = NULL;
		}
	}
}

//
//    View Selection Joins Panel Class.
//

BEGIN_EVENT_TABLE(gqbJoinsPanel, wxPanel)
	EVT_BUTTON(GQB_JOIN_COLS_ADD_BUTTON_ID, gqbJoinsPanel::OnButtonAdd)
	EVT_BUTTON(GQB_JOIN_COLS_DELETE_BUTTON_ID, gqbJoinsPanel::OnButtonDrop)
END_EVENT_TABLE()

gqbJoinsPanel::gqbJoinsPanel(wxWindow *parent, gqbModel *_model, gqbGridJoinTable *_gmodel, gqbController *_controller):
	wxPanel(parent, wxID_ANY)
{
	model = _model;
	gModel = _gmodel;
	controller = _controller;
	joinsPopUp = NULL;

	this->joinsGrid = new gqbCustomGrid(this, JOINS_PANEL_GRID_ID);
	joinsGrid->CreateGrid(0, 5, wxGrid::wxGridSelectRows);
	joinsGrid->SetTable(gModel, true, wxGrid::wxGridSelectCells);
	this->joinsGrid->SetSelectionMode(wxGrid::wxGridSelectRows);

	this->Connect(JOINS_PANEL_GRID_ID, wxEVT_GRID_CELL_LEFT_CLICK, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &gqbJoinsPanel::OnCellLeftClick);
	// GQB-TODO: in a future implement OnMouseWheel

	addBitmap = *gqbAdd_png_bmp;
	dropBitmap = *gqbRemove_png_bmp;
	buttonAdd = new wxBitmapButton( this, GQB_JOIN_COLS_ADD_BUTTON_ID,  addBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add"));
	buttonAdd->SetToolTip(_("Add a new join"));
	buttonDrop = new wxBitmapButton( this, GQB_JOIN_COLS_DELETE_BUTTON_ID,  dropBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove"));
	buttonDrop->SetToolTip(_("Remove the selected join"));

	wxBoxSizer *horizontalSizer = new wxBoxSizer( wxHORIZONTAL );
	horizontalSizer->Add(joinsGrid,
	                     1,                                        // make vertically stretchable
	                     wxEXPAND |                                // make horizontally stretchable
	                     wxALL,                                    //   and make border all around
	                     3 );                                     // set border width to 10

	wxBoxSizer *buttonsSizer = new wxBoxSizer( wxVERTICAL );

	buttonsSizer->Add(
	    this->buttonAdd,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    10 );                                     // set border width to 10

	buttonsSizer->Add(
	    this->buttonDrop,
	    0,                                        // make horizontally unstretchable
	    wxALL,                                    // make border all around (implicit top alignment)
	    10 );                                     // set border width to 10

	horizontalSizer->Add(
	    buttonsSizer,
	    0,                                        // make vertically unstretchable
	    wxALIGN_CENTER );                         // no border and centre horizontally

	this->SetSizer(horizontalSizer);
}


void gqbJoinsPanel::showColsPopUp(int row, int col, wxPoint pos)
{
	if( joinsPopUp )
	{
		joinsPopUp->Destroy();
		joinsPopUp = NULL;
	}
	joinsPopUp = new gqbJoinsPopUp(this, -1, wxT("Select Column"), wxDefaultPosition, wxDefaultSize, this->gModel->GetJoin(row), ( col == 0  ? true : false ), this->gModel);

	refreshTree(model);

	// Set initial Value
	joinsPopUp->setEditText(joinsGrid->GetCellValue(row, col));

	// Set Position for Pop Up Tree
	// Position of wxNotebook
	wxPoint p = this->GetParent()->GetPosition();
	p.x += pos.x;
	p.y += pos.y;
	wxPoint p2 = this->GetPosition();

	// Position of panel inside wxNotebook
	p.x += p2.x;
	p.y += p2.y + 40;
	joinsPopUp->SetPosition(p);
	joinsPopUp->Show();
	joinsPopUp->MakeModal(true);
	joinsPopUp->focus();
	joinsPopUp->setUsedCell(joinsGrid, row, col);
}


void gqbJoinsPanel::refreshTree(gqbModel *_model)
{
	model = _model;
	if(joinsPopUp && model)
		joinsPopUp->refreshTree(model);
}


void gqbJoinsPanel::OnCellLeftClick(wxGridEvent &event)
{
	wxObject *object = event.GetEventObject();
	gqbCustomGrid *grid = wxDynamicCast( object, gqbCustomGrid );

	// Only show editor y case of column 1
	if(event.GetCol() == 1)
	{
		grid->ComboBoxEvent(event);
	}
	else if(event.GetCol() == 0 || event.GetCol() == 2)
	{
		// Allow mini browser frame to be visible to user
		wxRect cellSize = grid->CellToRect(event.GetRow(), event.GetCol());
		wxPoint p = event.GetPosition();
		joinsGrid->CalcUnscrolledPosition(p.x, p.y, &p.x, &p.y);

		// Number 17 is button's width at cellRender function [nButtonWidth]
		if((grid->GetRowLabelSize() + cellSize.GetRight()) - (p.x) <= 17 )
		{
			p = event.GetPosition();
			showColsPopUp(event.GetRow(), event.GetCol(), p);
		}
	}
	event.Skip();
}


void gqbJoinsPanel::OnButtonAdd(wxCommandEvent &)
{
	this->gModel->AppendJoin( NULL );
}


void gqbJoinsPanel::OnButtonDrop(wxCommandEvent &)
{
	if(joinsGrid->GetGridCursorRow() >= 0)
	{
		gqbQueryObject *updateTbl = gModel->DeleteRow(joinsGrid->GetGridCursorRow());
		if (updateTbl)
			controller->getView()->updateTable(updateTbl);
	}
	joinsGrid->Refresh();
}


void gqbJoinsPanel::SetGridColsSize()
{
	// After sizers determine the width of Grid then calculate the % space for each column about 33% each one
	int size = (int)((joinsGrid->GetSize().GetWidth() - joinsGrid->GetRowLabelSize()) * 0.3);
	joinsGrid->SetColSize(0, size);
	joinsGrid->SetColSize(1, size);
	joinsGrid->SetColSize(2, size);
}

void gqbJoinsPanel::selectJoin(gqbQueryJoin *join)
{
	joinsGrid->RevertSel();
	gModel->selectJoin(join);
}

