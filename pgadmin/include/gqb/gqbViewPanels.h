//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbViewPanels.h - All panels used by GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBVIEWPANELS_H
#define GQBVIEWPANELS_H

#include <wx/minifram.h>

// App headers
#include "gqb/gqbViewController.h"
#include "gqb/gqbGridProjTable.h"
#include "gqb/gqbGridRestTable.h"
#include "gqb/gqbGridOrderTable.h"
#include "gqb/gqbGridJoinTable.h"

enum gridButtons
{
	GQB_COLS_UP_BUTTON_ID,
	GQB_COLS_UP_TOP_BUTTON_ID,
	GQB_COLS_DOWN_BUTTON_ID,
	GQB_COLS_DOWN_BOTTOM_BUTTON_ID,
	GQB_COLS_ADD_BUTTON_ID,
	GQB_COLS_DROP_BUTTON_ID,

	GQB_JOIN_COLS_ADD_BUTTON_ID,
	GQB_JOIN_COLS_DELETE_BUTTON_ID,

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
	gqbGridPanel(wxWindow *parent, wxWindowID id, gqbGridProjTable *gridModel);
	~gqbGridPanel();
	wxBitmapButton *buttonUp, *buttonDown, *buttonUpTop, *buttonDownBottom;
	wxBitmap upBitmap, upTopBitmap, downBitmap, downBottomBitmap;
	void SetGridColsSize();

	// Events for wxGrid
	void OnGridSelectCell( wxGridEvent &ev );
	void OnGridRangeSelected( wxGridRangeSelectEvent &ev );
	void OnButtonUp(wxCommandEvent &);
	void OnButtonUpTop(wxCommandEvent &);
	void OnButtonDown(wxCommandEvent &);
	void OnButtonDownBottom(wxCommandEvent &);

private:
	bool allowSelCells;
	int selTop, selBottom;        // Range Selection of wxGrid, -1 it's value not set.
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
	gqbColsTree(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style);
	wxTreeItemId &createRoot(wxString &Name);
	wxTreeItemId &getRootNode()
	{
		return rootNode;
	}
	void refreshTree(gqbModel *model, gqbQueryObject *doNotInclude = NULL);
	virtual void DeleteAllItems();
	~gqbColsTree()
	{
		DeleteAllItems();
	}

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
	gqbColsPopUp(wxWindow *parent, wxWindowID id, wxString title, wxPoint pos, const wxSize size);
	virtual void refreshTree(gqbModel *_model);
	void OnPopUpOKClick(wxCommandEvent &event);
	void OnPopUpTreeClick(wxTreeEvent &event);
	void OnPopUpTreeDoubleClick(wxTreeEvent &event);
	void setEditText(wxString text);
	wxString getEditText()
	{
		return editTree->GetValue();
	};
	void setUsedCell(wxGrid *grid, int row, int col)
	{
		usedGrid = grid;
		_row = row;
		_col = col;
	};
	void focus();

protected:
	int _row, _col;
	wxGrid *usedGrid;
	gqbColsTree *colsTree;
	wxTextCtrl *editTree;
	wxButton *buttonTree;
	gqbModel *model;              // Not owned shouldn't be deleted at this class

};

class gqbJoinsPanel;

class gqbJoinsPopUp: public gqbColsPopUp
{
public:
	gqbJoinsPopUp(
	    gqbJoinsPanel *parent, wxWindowID id, wxString title, wxPoint pos,
	    const wxSize size, gqbQueryJoin *_join, bool isSource,
	    gqbGridJoinTable *_gmodel);
	void OnPopUpOKClick(wxCommandEvent &event);
	void OnPopUpTreeClick(wxTreeEvent &event);
	void OnPopUpTreeDoubleClick(wxTreeEvent &event);

	// This should be called through OnPopUpOKClick & OnPopUpTreeDoubleClick
	void updateJoin();

	virtual void refreshTree(gqbModel *_model);

private:
	gqbQueryJoin *join; // Not owned, shouldn't be deletedat this class
	gqbQueryObject *selectedTbl; //Not owned, shouldn't be deletedat this class
	gqbColumn *selectedCol; //Not owned, shouldn't be deletedat this class
	bool isSource;
	gqbGridJoinTable *gModel; //Not owned, shouldn't be deletedat this class
};

//
//   Criterias Panel
//

class gqbCustomGrid: public wxGrid
{
public:
	gqbCustomGrid(wxWindow *parent, wxWindowID id);
	void ComboBoxEvent(wxGridEvent &event);
	void RevertSel();

private:
	wxGridSelection *m_selTemp;

};

class gqbCriteriaPanel: public wxPanel
{
public:
	gqbCriteriaPanel(wxWindow *parent, gqbModel *_model, gqbGridRestTable *gridModel);
	void OnCellLeftClick(wxGridEvent &ev);
	void refreshTree(gqbModel *_model);
	void OnButtonAdd(wxCommandEvent &);
	void OnButtonDrop(wxCommandEvent &);
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

class gqbJoinsPanel: public wxPanel
{
public:
	gqbJoinsPanel(wxWindow *parent, gqbModel *_model, gqbGridJoinTable *_gmodel, gqbController *_controller);
	void OnCellLeftClick(wxGridEvent &ev);
	void refreshTree(gqbModel *_model);
	void OnButtonAdd(wxCommandEvent &);
	void OnButtonDrop(wxCommandEvent &);
	void SetGridColsSize();
	void updateView(gqbQueryObject *tbl);
	void selectJoin(gqbQueryJoin *join);

private:
	wxBitmapButton *buttonAdd, *buttonDrop;
	wxBitmap addBitmap, dropBitmap;
	void showColsPopUp(int row, int col, wxPoint pos);
	gqbCustomGrid *joinsGrid;
	gqbModel *model;              // Not owned shouldn't be deleted at this class
	gqbJoinsPopUp *joinsPopUp;    // It will be automatically deleted
	gqbGridJoinTable *gModel;     // Not owned shouldn't be deleted at this class
	gqbController *controller;    // Not owned shouldn't be deleted at this class
	DECLARE_EVENT_TABLE()

};

//
// Order by Panel
//

class gqbOrderPanel: public wxPanel
{
public:
	gqbOrderPanel(wxWindow *parent, gqbGridOrderTable *gridTableLeft, gqbGridOrderTable *gridTableRight);
	void SetGridColsSize();

private:
	bool allowSelCells;
	void OnButtonUp(wxCommandEvent &);
	void OnButtonUpTop(wxCommandEvent &);
	void OnButtonDown(wxCommandEvent &);
	void OnButtonDownBottom(wxCommandEvent &);
	void OnButtonRemove(wxCommandEvent &);
	void OnButtonRemoveAll(wxCommandEvent &);
	void OnButtonAdd(wxCommandEvent &);
	void OnButtonAddAll(wxCommandEvent &);
	void OnGridSelectCell( wxGridEvent &ev );
	void OnGridRangeSelected( wxGridRangeSelectEvent &ev );
	void OnCellLeftClick(wxGridEvent &ev);
	int selLeft, selRightTop, selRightBottom;
	gqbGridOrderTable *tableLeft, *tableRight;
	wxGrid *availableColumns, *usedColumns;
	wxBitmapButton *buttonAdd, *buttonAddAll, *buttonRemove, *buttonRemoveAll;
	wxBitmap addBitmap, addAllBitmap, removeBitmap, removeAllBitmap;
	wxBitmapButton *buttonUp, *buttonDown, *buttonUpTop, *buttonDownBottom;
	wxBitmap upBitmap, upTopBitmap, downBitmap, downBottomBitmap;
	DECLARE_EVENT_TABLE()
};
#endif

