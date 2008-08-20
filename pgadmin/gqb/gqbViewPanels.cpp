//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmQuery.cpp 7381 2008-08-11 11:33:42Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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

// Images
#include "images/gqbUp.xpm"
#include "images/gqbUpTop.xpm"
#include "images/gqbDown.xpm"
#include "images/gqbDownBottom.xpm"
#include "images/gqbOrderAddAll.xpm"
#include "images/gqbOrderRemoveAll.xpm"
#include "images/gqbOrderRemove.xpm"
#include "images/gqbOrderAdd.xpm"
#include "images/gqbAddRest.xpm"
#include "images/gqbRemoveRest.xpm"
#include "images/tables.xpm"
#include "images/table-sm.xpm"
#include "images/column-sm.xpm"
#include "images/view-sm.xpm"

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

gqbGridPanel::gqbGridPanel(wxWindow* parent, wxWindowID id = wxID_ANY, gqbGridProjTable *gridModel=NULL):
wxPanel(parent,-1)
{
    gModel=gridModel;
    allowSelCells=true;
    selTop=-1;
    selBottom=-1;
    upBitmap = wxBitmap(gqbUp_xpm);
    upTopBitmap = wxBitmap(gqbUpTop_xpm);
    downBitmap = wxBitmap(gqbDown_xpm);
    downBottomBitmap = wxBitmap(gqbDownBottom_xpm);

    buttonUp = new wxBitmapButton( this, GQB_COLS_UP_BUTTON_ID,  upBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Column(s) Up") );
    buttonUpTop  = new wxBitmapButton( this, GQB_COLS_UP_TOP_BUTTON_ID,  upTopBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Column(s) Up") );
    buttonDown = new wxBitmapButton( this, GQB_COLS_DOWN_BUTTON_ID,  downBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Column(s) Up") );
    buttonDownBottom = new wxBitmapButton( this, GQB_COLS_DOWN_BOTTOM_BUTTON_ID,  downBottomBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Column(s) Up") );

    this->colsGrid = new wxGrid(this, -1, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_BESTWRAP ,wxT(""));
    colsGrid->SetTable(gModel,true,wxGrid::wxGridSelectCells);

    // Adjust the default row height to be more compact
    wxFont font = colsGrid->GetLabelFont();
    int nWidth = 0;
    int nHeight = 18;
    colsGrid->GetTextExtent(wxT("W"), &nWidth, &nHeight, NULL, NULL, &font);
    colsGrid->SetColLabelSize(nHeight+6);
	colsGrid->SetRowLabelSize(35);
#ifdef __WXGTK__
    colsGrid->SetDefaultRowSize(nHeight+8, TRUE);
#else
    colsGrid->SetDefaultRowSize(nHeight+4, TRUE);
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
    int size=(colsGrid->GetSize().GetWidth() - colsGrid->GetRowLabelSize())*0.30;
    colsGrid->SetColSize(0,size);
    colsGrid->SetColSize(1,size);
    colsGrid->SetColSize(2,size);
}


void gqbGridPanel::OnGridSelectCell( wxGridEvent& ev )
{
    if(allowSelCells)
    {
        if ( ev.Selecting() )
        {
            selTop=ev.GetRow();
            selBottom=-1;
        }
        else
        {
            selTop=-1;
            selBottom=-1;
        }
    }
    ev.Skip();
}


void gqbGridPanel::OnGridRangeSelected( wxGridRangeSelectEvent& ev )
{
    if(allowSelCells)
    {
        if ( ev.Selecting() )
        {
            selTop=ev.GetTopRow();
            selBottom=ev.GetBottomRow();
        }
        else
        {
            selTop=-1;
            selBottom=-1;
        }
    }
    ev.Skip();
}


void gqbGridPanel::OnButtonUp(wxCommandEvent&)
{
    // A single row is selected
    allowSelCells=false;
    if((selTop>=0 && selBottom==-1) || (selTop==selBottom))
    {
        gModel->changesPositions(selTop,selTop--);
        if(selTop<0)
        {
            selTop=0;
        }
        colsGrid->SelectBlock(selTop,0,selTop,1,false);
        colsGrid->SetGridCursor(selTop,0);
        selBottom=-1;
    }
    else
    {
        // A range of rows is selected
        if (selTop>=0 && selBottom>=0)
        {
            int newTop=selTop-1;
            gModel->changesRangeOnePos(selTop,selBottom,newTop);
            if(selTop > 0)                        // recalculate new selection area & avoid bad selection area
            {
                selTop--;
                selBottom--;
            }
            colsGrid->SelectBlock(selTop,0,selBottom,1,false);
            colsGrid->SetGridCursor(selTop,0);
        }
    }
    allowSelCells=true;
}


void gqbGridPanel::OnButtonUpTop(wxCommandEvent&)
{
    allowSelCells=false;
    
	// A Single Row is selected
    if((selTop>=0 && selBottom==-1) || (selTop==selBottom))
    {
        selBottom=selTop-1;
        selTop=0;
        gModel->changesRangeOnePos(selTop,selBottom,1);
        colsGrid->SelectBlock(0,0,0,1,false);
        colsGrid->SetGridCursor(0,0);

        // Put variables in correct values now.
        selTop=0;
        selBottom=-1;
    }   
	// A range of rows is selected
    else
    {
        int newTop=0;
        if (selTop>=0 && selBottom>=0)
        {
            // Move all range only one pos the require times to get the top
			for(int i=selTop;i>0;i--)
            {
                newTop=selTop-1;
                gModel->changesRangeOnePos(selTop,selBottom,newTop);

                // Recalculate new selection area & avoid bad selection area
				if(selTop > 0)
                {
                    selTop--;
                    selBottom--;
                }
                colsGrid->SelectBlock(selTop,0,selBottom,1,false);
                colsGrid->SetGridCursor(selTop,0);
            }
        }
    }
    allowSelCells=true;
}


void gqbGridPanel::OnButtonDown(wxCommandEvent&)
{

    allowSelCells=false;

    // A single row is selected
    if((selTop>=0 && selBottom==-1) || (selTop==selBottom))
    {
        gModel->changesPositions(selTop,selTop++);

        // Adjust selection when selected item it's last item.
		if(selTop==gModel->GetNumberRows())
        {
            selTop--;
        }
        colsGrid->SelectBlock(selTop,0,selTop,1,false);
        colsGrid->SetGridCursor(selTop,0);
        selBottom=-1;
    }       
	// A range of rows is selected
    else
    {
        if (selTop>=0 && selBottom>=0)
        {
            int newTop=selTop+1;
            gModel->changesRangeOnePos(selTop,selBottom,newTop);

            // Recalculate new selection area & avoid bad selection area
            if(selBottom < gModel->GetNumberRows()-1)
            {
                selTop++;
                selBottom++;
            }
            colsGrid->SelectBlock(selTop,0,selBottom,1,false);
            colsGrid->SetGridCursor(selTop,0);
        }
    }
    allowSelCells=true;
}


void gqbGridPanel::OnButtonDownBottom(wxCommandEvent&)
{
    allowSelCells=false;

    // A Single Row is selected
    if((selTop>=0 && selBottom==-1) || (selTop==selBottom))
    {
        selBottom=gModel->GetNumberRows()-1;
        selTop=selTop+1;
        int newBottom=gModel->GetNumberRows()-1;
        gModel->changesRangeOnePos(selTop,selBottom,selTop-1);
        colsGrid->SelectBlock(newBottom,0,newBottom,1,false);
        colsGrid->SetGridCursor(newBottom,0);

		// Put variables in correct values now.
        selTop=newBottom;
        selBottom=-1;
    }  
	// A range of rows is selected
    else
    {
        int newTop=0, size=gModel->GetNumberRows();

        if (selTop>=0 && selBottom>=0)
        {
            for(int i=selBottom;i<size;i++)
            {
                newTop=selTop+1;
                gModel->changesRangeOnePos(selTop,selBottom,newTop);

                // Recalculate new selection area & avoid bad selection area
                if(selBottom < gModel->GetNumberRows()-1)
                {
                    selTop++;
                    selBottom++;
                }
                colsGrid->SelectBlock(selTop,0,selBottom,1,false);
                colsGrid->SetGridCursor(selTop,0);
            }
        }
    }
    allowSelCells=true;
}


//
//    Tree with Columns & tables inside a query model
//

gqbColsTree::gqbColsTree(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxTreeCtrl(parent, id, pos, size, style)
{
    wxImageList *imageList = new wxImageList(16, 16);
    imageList->Add(wxIcon(tables_xpm));
    imageList->Add(wxIcon(table_sm_xpm));
    imageList->Add(wxIcon(column_sm_xpm));
    imageList->Add(wxIcon(view_sm_xpm));
    this->AssignImageList(imageList);
    wxString a=wxT("Select Column");
    createRoot(a);
    this->Expand(rootNode);
}


// Create root node
wxTreeItemId& gqbColsTree::createRoot(wxString &Name)
{
    rootNode=this->AddRoot(Name,0,0);
    return rootNode;
}


void gqbColsTree::refreshTree(gqbModel * model)
{
    // This remove and delete data inside tree's node
	this->DeleteAllItems();                       
    wxString a=wxT("Select Column");
    createRoot(a);
    this->Expand(rootNode);

    wxTreeItemId parent;
    gqbIteratorBase *iterator = model->createQueryIterator();
    while(iterator->HasNext())
    {
        gqbQueryObject *tmpTable= (gqbQueryObject *)iterator->Next();
		
		int iconIndex;
		if (tmpTable->parent->getType() == GQB_TABLE)
			iconIndex = 1;
		else // Must be a view
			iconIndex = 3;

        if(tmpTable->getAlias().length()>0)
        {
            parent=this->AppendItem(rootNode, tmpTable->getAlias() , iconIndex, iconIndex, NULL);
        }
        else
        {
            parent=this->AppendItem(rootNode, tmpTable->getName() , iconIndex, iconIndex, NULL);
        }
        gqbIteratorBase *colsIterator = tmpTable->parent->createColumnsIterator();
        while(colsIterator->HasNext())
        {
            gqbColumn *tmpColumn= (gqbColumn *)colsIterator->Next();
            this->AppendItem(parent, tmpColumn->getName() , 2, 2,NULL);
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

gqbColsPopUp::gqbColsPopUp(wxWindow* parent, wxWindowID id, wxString title, wxPoint pos, const wxSize size):
wxDialog(parent, id, title, pos, size, wxRESIZE_BORDER | wxCAPTION)
{
    this->SetSize(wxSize(243,165));
	wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *vSizer = new wxBoxSizer(wxVERTICAL);

    editTree = new wxTextCtrl(this, QR_TREE, wxT(""),wxPoint(5,5),wxSize(193,22));
    editTree->AcceptsFocus();
    editTree->SetFocus();
    editTree->SetEditable(true);

	hSizer->Add(editTree, 1, wxEXPAND | wxRIGHT, 3);

    buttonTree= new wxButton(this, QR_TREE_OK, _("OK") ,wxPoint(199,5), wxDefaultSize, wxBU_EXACTFIT);
    this->Connect(QR_TREE_OK, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &gqbColsPopUp::OnPopUpOKClick);

	hSizer->Add(buttonTree);
	vSizer->Add(hSizer, 0, wxEXPAND | wxALL, 3);

    colsTree = new gqbColsTree(this, QR_TREE, wxPoint(5,31),  wxSize(224,104), wxTR_HAS_BUTTONS | wxSIMPLE_BORDER | wxTR_SINGLE);
    this->Connect(QR_TREE, wxEVT_COMMAND_TREE_ITEM_ACTIVATED, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) &gqbColsPopUp::OnPopUpTreeDoubleClick);
    this->Connect(QR_TREE, wxEVT_COMMAND_TREE_SEL_CHANGED, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) &gqbColsPopUp::OnPopUpTreeClick);

	vSizer->Add(colsTree, 1, wxEXPAND | wxALL, 3);
	SetSizer(vSizer);
}


void gqbColsPopUp::refreshTree(gqbModel *_model)
{
    model=_model;
    if(colsTree && _model)
    {
        colsTree->refreshTree(model);
    }

}


void  gqbColsPopUp::OnPopUpOKClick(wxCommandEvent& event)
{
    this->usedGrid->SetCellValue(_row,_col,this->getEditText());
    this->MakeModal(false);
    this->Hide();
    this->GetParent()->Refresh();
}


void  gqbColsPopUp::OnPopUpTreeDoubleClick(wxTreeEvent& event)
{
    if(colsTree)
    {
        wxTreeItemId itemId = event.GetItem();
        wxTreeItemId itemIdParent =  colsTree->GetItemParent(itemId);
        if(!colsTree->ItemHasChildren(itemId) && (colsTree->GetRootItem()!=itemId))
        {
            this->usedGrid->SetCellValue(_row,_col,this->getEditText());
            if(this->usedGrid->GetCellValue(_row,_col).length()<=0)
            {
                this->usedGrid->SetCellValue(_row,_col,wxT("Set Value"));
            }
            this->MakeModal(false);
            this->Hide();
            this->GetParent()->Refresh();
        }
    }
}

void  gqbColsPopUp::OnPopUpTreeClick(wxTreeEvent& event)
{
    if(colsTree)
    {
        wxTreeItemId itemId = event.GetItem();
        wxTreeItemId itemIdParent =  colsTree->GetItemParent(itemId);

        if(!colsTree->ItemHasChildren(itemId) && (colsTree->GetRootItem()!=itemId))
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
    this->editTree->SetSelection(-1,-1);
}


//
//    View Selection Criteria Panel Class.
//

BEGIN_EVENT_TABLE(gqbCriteriaPanel, wxPanel)
EVT_BUTTON(GQB_COLS_ADD_BUTTON_ID, gqbCriteriaPanel::OnButtonAdd)
EVT_BUTTON(GQB_COLS_DROP_BUTTON_ID, gqbCriteriaPanel::OnButtonDrop)
END_EVENT_TABLE()

gqbCriteriaPanel::gqbCriteriaPanel(wxWindow* parent, gqbModel *_model, gqbGridRestTable *gridModel):
wxPanel(parent,-1)
{
    model=_model;
    gModel=gridModel;
    colsPopUp=NULL;

    // GQB-TODO: add real ID not 321
    // wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_DONTWRAP,wxT(""));
    this->restrictionsGrid = new wxRestrictionGrid(this, 321);
    restrictionsGrid->SetTable(gModel, true, wxGrid::wxGridSelectCells);
    this->restrictionsGrid->SetSelectionMode(wxGrid::wxGridSelectRows);

    this->Connect(321, wxEVT_GRID_CELL_LEFT_CLICK, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &gqbCriteriaPanel::OnCellLeftClick);
    // GQB-TODO: in a future implement OnMouseWheel

    addBitmap= wxBitmap(gqbAddRest_xpm);
    dropBitmap= wxBitmap(gqbRemoveRest_xpm);
    buttonAdd= new wxBitmapButton( this, GQB_COLS_ADD_BUTTON_ID,  addBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add"));
    buttonDrop= new wxBitmapButton( this, GQB_COLS_DROP_BUTTON_ID,  dropBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove"));

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
        colsPopUp = new gqbColsPopUp(this,-1,wxT("Set Value"),wxDefaultPosition,wxDefaultSize);
    }

    refreshTree(model);

    // Set initial Value
    colsPopUp->setEditText(restrictionsGrid->GetCellValue(row,col));

    // Set Position for Pop Up Tree
    // Position of wxNotebook
	wxPoint p=this->GetParent()->GetPosition();
    p.x+=pos.x;
    p.y+=pos.y;
    wxPoint p2=this->GetPosition();

	// Position of panel inside wxNotebook
    p.x+=p2.x;
    p.y+=p2.y+40;
    colsPopUp->SetPosition(p);
    colsPopUp->Show();
    colsPopUp->MakeModal(true);
    colsPopUp->focus();
    colsPopUp->setUsedCell(restrictionsGrid,row,col);
}


void gqbCriteriaPanel::refreshTree(gqbModel *_model)
{
    // GQB-TODO: Do this through controller...
    model=_model;
    if(colsPopUp && model)
        colsPopUp->refreshTree(model);

}


void gqbCriteriaPanel::OnCellLeftClick(wxGridEvent& event)
{
    wxObject *object = event.GetEventObject();
    wxRestrictionGrid *grid = wxDynamicCast( object, wxRestrictionGrid );

    // Only show editor y case of column 1
    if(event.GetCol()==1)
    {
        grid->ComboBoxEvent(event);
    } else if(event.GetCol()==0 || event.GetCol()==2)
    {
        // Allow mini browser frame to be visible to user
        wxRect cellSize=grid->CellToRect(event.GetRow(), event.GetCol());
        wxPoint p =event.GetPosition();
        restrictionsGrid->CalcUnscrolledPosition(p.x,p.y,&p.x,&p.y);

        // Number 17 is button's width at cellRender function [nButtonWidth]
        if((grid->GetRowLabelSize()+cellSize.GetRight())-(p.x) <= 17 )
        {
            p =event.GetPosition();
            showColsPopUp(event.GetRow(), event.GetCol(),p);
        }
    }                                             //GQB-TODO 1,2 can be integrate with 3
    else if(event.GetCol()==3)
    {
        // Change connector of the Restriction between AND/OR
        wxRect cellSize=restrictionsGrid->CellToRect(event.GetRow(), event.GetCol());
        wxPoint p =event.GetPosition();
        restrictionsGrid->CalcUnscrolledPosition(p.x,p.y,&p.x,&p.y);

        // Number 17 is button's width at cellRender function [nButtonWidth]
        if((restrictionsGrid->GetRowLabelSize()+cellSize.GetRight())-(p.x) <= 17 )
        {
            p =event.GetPosition();
            if( gModel->GetValue(event.GetRow(),3).Contains(wxT("AND")) )
            {
                gModel->SetValue(event.GetRow(),3,wxT("OR"));
            }
            else
            {
                gModel->SetValue(event.GetRow(),3,wxT("AND"));
            }
            restrictionsGrid->Refresh();
        }
    }
    event.Skip();
}


void gqbCriteriaPanel::OnButtonAdd(wxCommandEvent&)
{
    // GQB-TODO: use controller one
    gqbQueryRestriction *r = new gqbQueryRestriction();
    gModel->AppendItem(r);
    int row=model->getRestrictions()->restrictionsCount()-1;

    wxString strChoices[16] = {wxT("="), wxT("!="),wxT("<"),wxT("<="),wxT(">"),wxT(">="),wxT("BETWEEN"),wxT("LIKE"),wxT("NOT LIKE"),wxT("ILIKE"),wxT("NOT ILIKE"),wxT("IN"),wxT("NOT IN"),wxT("NOT BETWEEN"),wxT("IS NULL"),wxT("IS NOT NULL")};
    restrictionsGrid->SetCellRenderer(row, 1, new wxGridCellComboBoxRenderer);
    restrictionsGrid->SetCellRenderer(row, 0, new wxGridCellButtonRenderer);
    restrictionsGrid->SetCellEditor(row, 1, new dxGridCellSizedChoiceEditor(WXSIZEOF(strChoices),strChoices));
    restrictionsGrid->SetCellRenderer(row, 2, new wxGridCellButtonRenderer);
    restrictionsGrid->SetCellRenderer(row, 3, new wxGridCellButtonRenderer);
    restrictionsGrid->SetReadOnly(row,3,true);
}


void gqbCriteriaPanel::OnButtonDrop(wxCommandEvent&)
{
    if(restrictionsGrid->GetGridCursorRow()>=0)
        gModel->DeleteRows(restrictionsGrid->GetGridCursorRow(),1);
    restrictionsGrid->Refresh();
}


void gqbCriteriaPanel::SetGridColsSize()
{
    // After sizers determine the width of Grid then calculate the % space for each column about 33% each one
    int size=(restrictionsGrid->GetSize().GetWidth() - restrictionsGrid->GetRowLabelSize())*0.225;
    restrictionsGrid->SetColSize(0,size);
    restrictionsGrid->SetColSize(1,size);
    restrictionsGrid->SetColSize(2,size);
    restrictionsGrid->SetColSize(3,size);
}


//
//  View Selection Criteria Panel Class's Grid.
//
wxRestrictionGrid::wxRestrictionGrid(wxWindow* parent, wxWindowID id):
wxGrid(parent, id, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_BESTWRAP,wxT("")),
m_selTemp(NULL)
{
    // Adjust the default row height to be more compact
    wxFont font = GetLabelFont();
    int nWidth = 0;
    int nHeight = 18;
    GetTextExtent(wxT("W"), &nWidth, &nHeight, NULL, NULL, &font);
    SetColLabelSize(nHeight+6);
	SetRowLabelSize(35);
#ifdef __WXGTK__
    SetDefaultRowSize(nHeight+8, TRUE);
#else
    SetDefaultRowSize(nHeight+4, TRUE);
#endif
}


void wxRestrictionGrid::RevertSel()
{
    if (m_selTemp)
    {
        wxASSERT(m_selection == NULL);
        m_selection = m_selTemp;
        m_selTemp = NULL;
    }
}


void wxRestrictionGrid::ComboBoxEvent(wxGridEvent& event)
{

    // This forces the cell to go into edit mode directly
    this->m_waitForSlowClick = TRUE;
    int row=event.GetRow();
    int col=event.GetCol();

    this->SetGridCursor(row,col);

    // Store the click co-ordinates in the editor if possible
    // if an editor has created a ClientData area, we presume it's
    // a wxPoint and we store the click co-ordinates
    wxGridCellEditor* pEditor  = this->GetCellEditor(event.GetRow(), event.GetCol());
    wxPoint* pClickPoint = (wxPoint*)pEditor->GetClientData();
    if (pClickPoint)
    {
        *pClickPoint = this->ClientToScreen(event.GetPosition());
#ifndef __WINDOWS__
        EnableCellEditControl(true);
#endif
    }

    // hack to prevent selection from being lost when click combobox
    if (event.GetCol() == 0 && this->IsInSelection(event.GetRow(), event.GetCol()))
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

gqbOrderPanel::gqbOrderPanel(wxWindow* parent, gqbGridOrderTable* gridTableLeft, gqbGridOrderTable* gridTableRight):
wxPanel(parent,-1)
{

    // GQB-TODO: change bitmap buttons
    selLeft=-1;
    selRightTop=-1;
    selRightBottom=-1;
    tableLeft=gridTableLeft;
    tableRight=gridTableRight;
    allowSelCells=true;

    addBitmap = wxBitmap(gqbOrderAdd_xpm);
    addAllBitmap = wxBitmap(gqbOrderAddAll_xpm);
    removeBitmap = wxBitmap(gqbOrderRemove_xpm);
    removeAllBitmap = wxBitmap(gqbOrderRemoveAll_xpm);

    buttonAdd=new wxBitmapButton( this, GQB_ORDER_ADD_BUTTON_ID,  addBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add Column") );
    buttonAddAll=new wxBitmapButton( this, GQB_ORDER_ADD_ALL_BUTTON_ID,  addAllBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Add All Columns") );
    buttonRemove=new wxBitmapButton( this, GQB_ORDER_DROP_BUTTON_ID,  removeBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove Column") );
    buttonRemoveAll=new wxBitmapButton( this, GQB_ORDER_DROP_ALL_BUTTON_ID,  removeAllBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Remove All Columns") );

    upBitmap = wxBitmap(gqbUp_xpm);
    upTopBitmap = wxBitmap(gqbUpTop_xpm);
    downBitmap = wxBitmap(gqbDown_xpm);
    downBottomBitmap = wxBitmap(gqbDownBottom_xpm);

    buttonUp = new wxBitmapButton( this, GQB_ORDER_UP_BUTTON_ID,  upBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Column(s) Up") );
    buttonDown  = new wxBitmapButton( this, GQB_ORDER_DOWN_BUTTON_ID,  downBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Column(s) Up") );
    buttonUpTop = new wxBitmapButton( this, GQB_ORDER_UP_TOP_BUTTON_ID,  upTopBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Column(s) Up") );
    buttonDownBottom = new wxBitmapButton( this, GQB_ORDER_DOWN_BOTTOM_BUTTON_ID,  downBottomBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, wxT("Column(s) Up") );

    availableColumns = new wxGrid(this, -1, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_BESTWRAP ,wxT("Available Columns"));
	availableColumns->SetTable(gridTableLeft);
    availableColumns->EnableEditing(false);

	// Adjust the default row height to be more compact
    wxFont font = availableColumns->GetLabelFont();
    int nWidth = 0;
    int nHeight = 18;
    availableColumns->GetTextExtent(wxT("W"), &nWidth, &nHeight, NULL, NULL, &font);
    availableColumns->SetColLabelSize(nHeight+6);
	availableColumns->SetRowLabelSize(35);
#ifdef __WXGTK__
    availableColumns->SetDefaultRowSize(nHeight+8, TRUE);
#else
    availableColumns->SetDefaultRowSize(nHeight+4, TRUE);
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
    usedColumns = new wxGrid(this, 333, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_BESTWRAP ,wxT("Columns Order"));
	usedColumns->SetTable(gridTableRight);
    usedColumns->EnableEditing(false);

    // Adjust the default row height to be more compact
    font = usedColumns->GetLabelFont();
    nWidth = 0;
    nHeight = 18;
    usedColumns->GetTextExtent(wxT("W"), &nWidth, &nHeight, NULL, NULL, &font);
    usedColumns->SetColLabelSize(nHeight+6);
    usedColumns->SetRowLabelSize(35);
#ifdef __WXGTK__
    usedColumns->SetDefaultRowSize(nHeight+8, TRUE);
#else
    usedColumns->SetDefaultRowSize(nHeight+4, TRUE);
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


void gqbOrderPanel::OnButtonRemove(wxCommandEvent&)
{
    if(usedColumns->GetRows()>0)
    {
        if(selRightTop!=-1)
        {
            gqbColumn *col = (gqbColumn*) tableRight->getObjectAt(selRightTop,0);
            gqbQueryObject *colParent = (gqbQueryObject *) tableRight->getObjectAt(selRightTop,1);
            tableLeft->AppendItem(col,colParent,'N');
            tableRight->removeRowAt(selRightTop);
        }
    }
}


void gqbOrderPanel::OnButtonRemoveAll(wxCommandEvent&)
{
    if(usedColumns->GetRows()>0)
    {
        for(int i=usedColumns->GetRows()-1;i>=0;i--)
        {
            gqbColumn *col = (gqbColumn*) tableRight->getObjectAt(i,0);
            gqbQueryObject *colParent = (gqbQueryObject *) tableRight->getObjectAt(i,1);
            tableLeft->AppendItem(col,colParent,'N');
            tableRight->removeRowAt(i);
        }
    }
}


void gqbOrderPanel::OnButtonAdd(wxCommandEvent&)
{
    if(availableColumns->GetRows()>0)
    {
        if(selLeft!=-1)
        {
            gqbColumn *col = (gqbColumn*) tableLeft->getObjectAt(selLeft,0);
            gqbQueryObject *colParent = (gqbQueryObject *) tableLeft->getObjectAt(selLeft,1);
            tableRight->AppendItem(col,colParent,'A');
            usedColumns->SetCellRenderer((usedColumns->GetRows()-1), 1, new wxGridCellButtonRenderer);
            tableLeft->removeRowAt(selLeft);
        }
    }
}


void gqbOrderPanel::OnButtonAddAll(wxCommandEvent&)
{
    if(availableColumns->GetRows()>0)
    {
        for(int i=availableColumns->GetRows()-1;i>=0;i--)
        {
            gqbColumn *col = (gqbColumn*) tableLeft->getObjectAt(i,0);
            gqbQueryObject *colParent = (gqbQueryObject *) tableLeft->getObjectAt(i,1);
            tableRight->AppendItem(col,colParent,'A');
            usedColumns->SetCellRenderer((usedColumns->GetRows()-1), 1, new wxGridCellButtonRenderer);
            tableLeft->removeRowAt(i);
        }
    }
}


void gqbOrderPanel::OnGridSelectCell( wxGridEvent& ev )
{
    if(allowSelCells)
    {
        if ( ev.Selecting() )
        {
            wxObject *object = ev.GetEventObject();
            wxGrid *grid = wxDynamicCast( object, wxGrid );
            if(grid->GetCols()==1)                // Left Grid
            {
                selLeft=ev.GetRow();
            }
            else
            {                                     // Right Grid
                selRightTop=ev.GetRow();
                selRightBottom=-1;
            }
        }
        else
        {
            selRightTop=-1;                       // GQB-TODO: this is correct??
            selRightBottom=-1;
            selLeft=-1;
        }
    }
    ev.Skip();
}


void gqbOrderPanel::OnGridRangeSelected( wxGridRangeSelectEvent& ev )
{
    if(allowSelCells)
    {
        if ( ev.Selecting() )
        {
            selRightTop=ev.GetTopRow();
            selRightBottom=ev.GetBottomRow();
        }
        else
        {
            selRightTop=-1;
            selRightBottom=-1;
        }
    }
    ev.Skip();
}


void gqbOrderPanel::OnButtonUp(wxCommandEvent&)
{
    // A single row is selected
    allowSelCells=false;
    if((selRightTop>=0 && selRightBottom==-1) || (selRightTop==selRightBottom))
    {
        tableRight->changesPositions(selRightTop,selRightTop--);
        if(selRightTop<0)
        {
            selRightTop=0;
        }
        usedColumns->SelectBlock(selRightTop,0,selRightTop,1,false);
        usedColumns->SetGridCursor(selRightTop,0);
        selRightBottom=-1;
    }
    else
    {
		// A range of rows is selected
        if (selRightTop>=0 && selRightBottom>=0)
        {
            int newTop=selRightTop-1;
            tableRight->changesRangeOnePos(selRightTop,selRightBottom,newTop);
            if(selRightTop > 0)                   // Recalculate new selection area & avoid bad selection area
            {
                selRightTop--;
                selRightBottom--;
            }
            usedColumns->SelectBlock(selRightTop,0,selRightBottom,1,false);
            usedColumns->SetGridCursor(selRightTop,0);
        }
    }
    allowSelCells=true;
}


void gqbOrderPanel::OnButtonUpTop(wxCommandEvent&)
{
    allowSelCells=false;
    
	// A Single Row is selected
    if((selRightTop>=0 && selRightBottom==-1) || (selRightTop==selRightBottom))
    {
        selRightBottom=selRightTop-1;
        selRightTop=0;
        tableRight->changesRangeOnePos(selRightTop,selRightBottom,1);
        usedColumns->SelectBlock(0,0,0,1,false);
        usedColumns->SetGridCursor(0,0);

        // Put variables in correct values now.
        selRightTop=0;
        selRightBottom=-1;
    }                                            
	// A range of rows is selected
    else
    {
        int newTop=0;
        if (selRightTop>=0 && selRightBottom>=0)
        {
            // Move all range only one pos the require times to get the top
			for(int i=selRightTop;i>0;i--)
            {
                newTop=selRightTop-1;
                tableRight->changesRangeOnePos(selRightTop,selRightBottom,newTop);

                // Recalculate new selection area & avoid bad selection area
				if(selRightTop > 0)
                {
                    selRightTop--;
                    selRightBottom--;
                }
                usedColumns->SelectBlock(selRightTop,0,selRightBottom,1,false);
                usedColumns->SetGridCursor(selRightTop,0);
            }
        }
    }
    allowSelCells=true;
}


void gqbOrderPanel::OnButtonDown(wxCommandEvent&)
{

    allowSelCells=false;

    // A single row is selected
    if((selRightTop>=0 && selRightBottom==-1) || (selRightTop==selRightBottom))
    {
        tableRight->changesPositions(selRightTop,selRightTop++);

        // Adjust selection when selected item it's last item.
        if(selRightTop==tableRight->GetNumberRows())
        {
            selRightTop--;
        }
        usedColumns->SelectBlock(selRightTop,0,selRightTop,1,false);
        usedColumns->SetGridCursor(selRightTop,0);
        selRightBottom=-1;
    }   
	// A range of rows is selected
    else
    {
        if (selRightTop>=0 && selRightBottom>=0)
        {
            int newTop=selRightTop+1;
            tableRight->changesRangeOnePos(selRightTop,selRightBottom,newTop);

			// Recalculate new selection area & avoid bad selection area
            if(selRightBottom < tableRight->GetNumberRows()-1)
            {
                selRightTop++;
                selRightBottom++;
            }
            usedColumns->SelectBlock(selRightTop,0,selRightBottom,1,false);
            usedColumns->SetGridCursor(selRightTop,0);
        }
    }
    allowSelCells=true;
}


void gqbOrderPanel::OnButtonDownBottom(wxCommandEvent&)
{
    allowSelCells=false;

    // A Single Row is selected
    if((selRightTop>=0 && selRightBottom==-1) || (selRightTop==selRightBottom))
    {
        selRightBottom=tableRight->GetNumberRows()-1;
        selRightTop=selRightTop+1;
        int newBottom=tableRight->GetNumberRows()-1;
        tableRight->changesRangeOnePos(selRightTop,selRightBottom,selRightTop-1);
        usedColumns->SelectBlock(newBottom,0,newBottom,1,false);
        usedColumns->SetGridCursor(newBottom,0);

        // Put variables in correct values now.
        selRightTop=newBottom;
        selRightBottom=-1;
    }
	// A range of rows is selected
    else
    {
        int newTop=0, size=tableRight->GetNumberRows();

        if (selRightTop>=0 && selRightBottom>=0)
        {
            for(int i=selRightBottom;i<size;i++)
            {
                newTop=selRightTop+1;
                tableRight->changesRangeOnePos(selRightTop,selRightBottom,newTop);
       
				// Recalculate new selection area & avoid bad selection area
                if(selRightBottom < tableRight->GetNumberRows()-1)
                {
                    selRightTop++;
                    selRightBottom++;
                }
                usedColumns->SelectBlock(selRightTop,0,selRightBottom,1,false);
                usedColumns->SetGridCursor(selRightTop,0);
            }
        }
    }
    allowSelCells=true;
}


void gqbOrderPanel::SetGridColsSize()
{
    // After sizers determine the width of Grid then calculate the % space for each column
    int size=(availableColumns->GetSize().GetWidth() - availableColumns->GetRowLabelSize())*0.90;
    availableColumns->SetColSize(0,size);

    size=(usedColumns->GetSize().GetWidth() - usedColumns->GetRowLabelSize())*0.65;
    int size2=(usedColumns->GetSize().GetWidth() - usedColumns->GetRowLabelSize())*0.25;
    usedColumns->SetColSize(0,size);
    usedColumns->SetColSize(1,size2);
}


void gqbOrderPanel::OnCellLeftClick(wxGridEvent& event)
{

    // Only show editor y case of column 1
    if(event.GetCol()==1)
    {
        // Change kind of order of the columns between ASC and DESC
        wxRect cellSize=usedColumns->CellToRect(event.GetRow(), event.GetCol());
        wxPoint p =event.GetPosition();
        usedColumns->CalcUnscrolledPosition(p.x,p.y,&p.x,&p.y);

        // Number 17 is button's width at cellRender function [nButtonWidth]
        if((usedColumns->GetRowLabelSize()+cellSize.GetRight())-(p.x) <= 17 )
        {
            p =event.GetPosition();
            if( tableRight->GetValue(event.GetRow(),1).Contains(wxT("ASC")) )
            {
                tableRight->SetValue(event.GetRow(),1,wxT("DESC"));
            }
            else
            {
                tableRight->SetValue(event.GetRow(),1,wxT("ASC"));
            }
            usedColumns->Refresh();
        }
    }
    event.Skip();
}
