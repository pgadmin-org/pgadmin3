//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbViewController.h - View and Controller implementation for MVC Pattern of GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBCONTROLLER_H
#define GQBCONTROLLER_H

#include <wx/dcbuffer.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/dnd.h>
#include <wx/dataobj.h>
#include <wx/aui/aui.h>

// App headers
#include "ctl/ctlAuiNotebook.h"
#include "gqb/gqbObject.h"
#include "gqb/gqbModel.h"
#include "gqb/gqbQueryObjs.h"
#include "gqb/gqbGraphBehavior.h"
#include "gqb/gqbColumn.h"
#include "gqb/gqbGridProjTable.h"
#include "gqb/gqbGridRestTable.h"
#include "gqb/gqbGridOrderTable.h"
#include "gqb/gqbGridJoinTable.h"
#include "gqb/gqbBrowser.h"

#define GQB_MIN_WIDTH  1280
#define GQB_MIN_HEIGHT 800

class gqbView;

enum pointerMode
{
	pt_normal,
	pt_join
};

// Utility Class to avoid a bug when the event sash resize is called
class gqbSplitter: public wxSplitterWindow
{
public:
	gqbSplitter(wxWindow *parent, wxWindowID id, const wxPoint &point, const wxSize &size, long style);
	void setTablesBrowser(gqbBrowser *b)
	{
		tablesBrowser = b;
	};
	void setBrowserPanel(wxPanel *p)
	{
		browserPanel = p;
	};

private:
	void onVerticalSashResize(wxSplitterEvent &event);
	gqbBrowser *tablesBrowser;        // tables Browser Tree
	wxPanel *browserPanel;            // Container of tables Browser Tree
	DECLARE_EVENT_TABLE()
};

// This enum is useful to select particular page from the tabs
enum tabsIndex
{
	ti_colsGridPanel = 0,
	ti_criteriaPanel,
	ti_orderPanel,
	ti_joinsPanel
};

class gqbController: public wxObject
{
public:
	gqbController(gqbModel *_model, wxWindow *gqbParent, ctlAuiNotebook *gridParent, wxSize size);
	~gqbController();
	gqbQueryObject *addTableToModel(gqbTable *table, wxPoint p);
	gqbQueryJoin *addJoin(gqbQueryObject *sTable, gqbColumn *sColumn, gqbQueryObject *dTable, gqbColumn *dColumn, type_Join kind);
	void removeJoin(gqbQueryJoin *join);
	void removeTableFromModel(gqbQueryObject *table, gqbGridProjTable *gridTable, gqbGridOrderTable *orderLTable, gqbGridOrderTable *orderRTable);
	void unsetModelSelected(bool queryTable);
	void processColumnInModel(gqbQueryObject *table, gqbColumn *column, gqbGridProjTable *gridTable);
	void setPointerMode(pointerMode pm);      // Find selected table with their coordinates point
	gqbView *getView()
	{
		return view;
	};
	void nullView()
	{
		view = NULL;
	};
	gqbObject *getModelSelected(wxPoint &pt, gqbQueryObject *lastSelected, gqbQueryJoin *lastJoinSelected, bool mark);
	wxString generateSQL();
	wxSplitterWindow *getViewContainer()
	{
		return gqbMainContainer;
	};
	wxSplitterWindow *getDialogParent()
	{
		return (wxSplitterWindow *) gqbContainer;
	};
	void setSashVertPosition(int pos);
	void setSashHorizPosition(int pos);
	int getSashHorizPosition();
	gqbBrowser *getTablesBrowser()
	{
		return tablesBrowser;
	};
	wxNotebook *getTabs()
	{
		return tabs;
	};
	void emptyModel();
	void calcGridColsSizes();
	gqbQueryRestriction *addRestriction();
	int getTableCount()
	{
		return model->tablesCount();
	};

protected:
	gqbView *view;						// owned by caller application shouldn't be destroy by this class
	wxWindow *pparent;					// GQB-TODO: deberia ser privada no se porque no funciona [la estoy usando?]
	gqbModel *model;					// owned by caller application shouldn't be destroy by this class
	wxNotebook *tabs;
	gqbSplitter *gqbContainer;			// container of canvas & tables browser.
	wxSplitterWindow *gqbMainContainer;
	gqbBrowser *tablesBrowser;			// tables Browser Tree
	wxPanel *browserPanel;				// Container of tables Browser Tree
};

class gqbView: public wxScrolledWindow
{
public:
	gqbView(wxWindow *gqbParent, ctlAuiNotebook *gridParent, wxSize size, gqbController *controller, gqbModel *model);
	~gqbView();
	void SaveAsImage(const wxString &path, wxBitmapType imgType);
	bool canSaveAsImage();
	void drawAll(wxMemoryDC &bdc, bool adjustScrolling);
	void setPointerMode(pointerMode pm);

	// Events for wxScrolledWindow
	void onPaint(wxPaintEvent &event);
	void onMotion(wxMouseEvent &event);
	void onDoubleClick(wxMouseEvent &event);
	void onRightClick(wxMouseEvent &event);
	void onErase(wxEraseEvent &event);
	void onEraseBackGround(wxEraseEvent &event);
	void OnKeyDown(wxKeyEvent &event);
	void OnSize(wxSizeEvent &event);
	wxPanel *getColsGridPanel()
	{
		return (wxPanel *)projectionPanel;
	};
	wxPanel *getCriteriaPanel()
	{
		return (wxPanel *)criteriaPanel;
	};
	wxPanel *getOrderPanel()
	{
		return (wxPanel *)orderPanel;
	};
	wxPanel *getJoinsPanel()
	{
		return joinsPanel;
	}
	void newTableAdded(gqbQueryObject *item);
	bool clickOnJoin (gqbQueryJoin *join, wxPoint &pt, wxPoint &origin, wxPoint &dest);
	void updateTable(gqbQueryObject *table);
	const wxSize &getModelSize()
	{
		return modelSize;
	}

	// Functions for all gqb extra Panels (projection, criteria..)
	void emptyPanelsData();

	void updateModelSize(gqbQueryObject *obj, bool updateAnyWay);

private:
	gqbController *controller;                                  // owned by caller application shouldn't be destroy
	// by this class
	gqbModel *model;                                            // owned by caller application shouldn't be destroy
	// by this class
	gqbGraphBehavior *graphBehavior;                            // This points to the Graph behavior for objects,
	// if change the way objects were draw changes too.
	gqbIteratorBase *iterator;                                  //include here for reuse of iterator, should be
	// delete when class destroy
	wxPanel *projectionPanel, *criteriaPanel, *orderPanel, *joinsPanel;
	gqbGridProjTable *gridTable;                                // Data model for the columns grid internals
	gqbGridRestTable *restrictionsGridTable;                    // Data model for restricions grid internals
	gqbGridJoinTable *joinsGridTable;                           // Data model for joins grid internals

	gqbGridOrderTable *orderByLGridTable, *orderByRGridTable;   // Data model for order by grid internals
	wxSize canvasSize, modelSize;
	bool changeTOpressed;

	// just a point to the selected item on the collection, shouldn't be destroy inside this class
	gqbQueryObject *collectionSelected, *joinSource, *joinDest, *cTempSelected;
	gqbQueryJoin *joinSelected, *jTempSelected;
	gqbColumn *joinSCol, *joinDCol;
	int pressed, selected, refreshRate;
	wxPoint pos, jpos;            // Position of the last event of the mouse & the first event of a join event
	pointerMode mode;             // pointer is used as normally or as in joins by example
	wxImage joinCursorImage;
	wxCursor joinCursor;
	wxMenu *m_rightJoins, *m_rightTables, *m_gqbPopup;
	void OnMenuJoinDelete(wxCommandEvent &event);
	void OnMenuTableDelete(wxCommandEvent &event);
	void OnMenuTableSetAlias(wxCommandEvent &event);
	void OnRefresh(wxCommandEvent &ev);

	wxArrayString joinTypeChoices;

	DECLARE_EVENT_TABLE()
};

// A drop target that do nothing only accept text, if accept then tree add table to model
class DnDText : public wxTextDropTarget
{
public:
	DnDText(gqbBrowser *tablesBrowser)
	{
		tree = tablesBrowser;
	}
	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString &text)
	{
		tree->setDnDPoint(x, y);
		return true;
	}

private:
	gqbBrowser *tree;
};

#endif

