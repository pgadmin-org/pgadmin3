//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmQuery.cpp 7381 2008-08-11 11:33:42Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// gqbViewPanels.h - All panels used by GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBVIEWPANELS_H
#define GQBVIEWPANELS_H

#include <wx/minifram.h>

// App headers
#include "gqb/gqbGridProjTable.h"
#include "gqb/gqbGridRestTable.h"
#include "gqb/gqbGridOrderTable.h"

enum gridButtons
{
    GQB_COLS_UP_BUTTON_ID,
    GQB_COLS_UP_TOP_BUTTON_ID,
    GQB_COLS_DOWN_BUTTON_ID,
    GQB_COLS_DOWN_BOTTOM_BUTTON_ID,
    GQB_COLS_ADD_BUTTON_ID,
    GQB_COLS_DROP_BUTTON_ID,

    GQB_ORDER_DROP_BUTTON_ID,
    GQB_ORDER_DROP_ALL_BUTTON_ID,
    GQB_ORDER_ADD_ALL_BUTTON_ID,
    GQB_ORDER_ADD_BUTTON_ID,
    GQB_ORDER_UP_BUTTON_ID,
    GQB_ORDER_UP_TOP_BUTTON_ID,
    GQB_ORDER_DOWN_BUTTON_ID,
    GQB_ORDER_DOWN_BOTTOM_BUTTON_ID
};

//
//   Projection Panel
//

class gqbGridPanel: public wxPanel
{
public:
    gqbGridPanel(wxWindow* parent, wxWindowID id, gqbGridProjTable *gridModel);
    ~gqbGridPanel();
    wxBitmapButton *buttonUp, *buttonDown, *buttonUpTop, *buttonDownBottom;
    wxBitmap upBitmap, upTopBitmap, downBitmap, downBottomBitmap;
    void SetGridColsSize();

	// Events for wxGrid
    void OnGridSelectCell( wxGridEvent& ev );
    void OnGridRangeSelected( wxGridRangeSelectEvent& ev );
    void OnButtonUp(wxCommandEvent&);
    void OnButtonUpTop(wxCommandEvent&);
    void OnButtonDown(wxCommandEvent&);
    void OnButtonDownBottom(wxCommandEvent&);

private:
    bool allowSelCells;
    int selTop,selBottom;         // Range Selection of wxGrid, -1 it's value not set.
    wxGrid *colsGrid;             // Columns Grid used for order of columns in sentence & single row functions
    gqbGridProjTable *gModel;
    DECLARE_EVENT_TABLE()
};

//
//   Panels reusable components
//

class gqbColsTree : public wxTreeCtrl
{
public:
    gqbColsTree(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
    wxTreeItemId& createRoot(wxString &Name);
    wxTreeItemId& getRootNode(){return rootNode;}
    void refreshTree(gqbModel *model);

private:
    wxTreeItemId rootNode;
};

enum
{
    QR_TREE_OK = 9100,
    QR_TREE
};

class gqbColsPopUp: public wxDialog
{
public:
    gqbColsPopUp(wxWindow* parent, wxWindowID id, wxString title, wxPoint pos, const wxSize size);
    void refreshTree(gqbModel *_model);
    void OnPopUpOKClick(wxCommandEvent& event);
    void OnPopUpTreeClick(wxTreeEvent& event);
    void OnPopUpTreeDoubleClick(wxTreeEvent& event);
    void setEditText(wxString text);
    wxString getEditText(){return editTree->GetValue();};
    void setUsedCell(wxGrid* grid, int row, int col){usedGrid=grid; _row=row; _col=col;};
    void focus();

private:
    int _row,_col;
    wxGrid *usedGrid;
    gqbColsTree *colsTree;
    wxTextCtrl *editTree;
    wxButton *buttonTree;
    gqbModel *model;              // Not owned shouldn't be deleted at this class

};

//
//   Criterias Panel
//

class wxRestrictionGrid: public wxGrid
{
public:
    wxRestrictionGrid(wxWindow* parent, wxWindowID id);
    void ComboBoxEvent(wxGridEvent& event);
    void RevertSel();

private:
    wxGridSelection *m_selTemp;

};

class gqbCriteriaPanel: public wxPanel
{
public:
    gqbCriteriaPanel(wxWindow* parent, gqbModel *_model, gqbGridRestTable *gridModel);
    void OnCellLeftClick(wxGridEvent& ev);
    void refreshTree(gqbModel *_model);
    void OnButtonAdd(wxCommandEvent&);
    void OnButtonDrop(wxCommandEvent&);
    void SetGridColsSize();

private:
    wxBitmapButton *buttonAdd, *buttonDrop;
    wxBitmap addBitmap, dropBitmap;
    void showColsPopUp(int row, int col, wxPoint pos);
    gqbGridRestTable *gModel;
    wxGrid *restrictionsGrid;     // Columns Grid used for order of columns in sentence & single row functions
    gqbModel *model;              // Not owned shouldn't be deleted at this class
    gqbColsPopUp *colsPopUp;
    DECLARE_EVENT_TABLE()

};

//
// Order by Panel
//

class gqbOrderPanel: public wxPanel
{
public:
    gqbOrderPanel(wxWindow* parent, gqbGridOrderTable* gridTableLeft, gqbGridOrderTable* gridTableRight);
    void SetGridColsSize();

private:
    bool allowSelCells;
    void OnButtonUp(wxCommandEvent&);
    void OnButtonUpTop(wxCommandEvent&);
    void OnButtonDown(wxCommandEvent&);
    void OnButtonDownBottom(wxCommandEvent&);
    void OnButtonRemove(wxCommandEvent&);
    void OnButtonRemoveAll(wxCommandEvent&);
    void OnButtonAdd(wxCommandEvent&);
    void OnButtonAddAll(wxCommandEvent&);
    void OnGridSelectCell( wxGridEvent& ev );
    void OnGridRangeSelected( wxGridRangeSelectEvent& ev );
    void OnCellLeftClick(wxGridEvent& ev);
    int selLeft, selRightTop,selRightBottom;
    gqbGridOrderTable *tableLeft, *tableRight;
    wxGrid *availableColumns, *usedColumns;
    wxBitmapButton *buttonAdd, *buttonAddAll, *buttonRemove, *buttonRemoveAll;
    wxBitmap addBitmap, addAllBitmap, removeBitmap, removeAllBitmap;
    wxBitmapButton *buttonUp, *buttonDown, *buttonUpTop, *buttonDownBottom;
    wxBitmap upBitmap, upTopBitmap, downBitmap, downBottomBitmap;
    DECLARE_EVENT_TABLE()
};
#endif