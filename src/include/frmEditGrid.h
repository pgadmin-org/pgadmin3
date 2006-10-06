//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmEditGrid.h 4988 2006-02-06 10:37:57Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmEditGrid.h - The SQL Edit Grid form
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRMEDITGRID_H
#define __FRMEDITGRID_H

#include <wx/grid.h>

// wxAUI
#include <wx/aui/aui.h>

#define CTL_EDITGRID 357
#include "dlgClasses.h"
#include "ctl/ctlSQLGrid.h"

#ifdef __WXMAC__
#define FRMEDITGRID_DEFAULT_PERSPECTIVE wxT("layout1|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=240;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=limitBar;caption=Limit bar;state=16788208;dir=1;layer=10;row=0;pos=243;prop=100000;bestw=72;besth=21;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=415;floaty=793;floatw=-1;floath=-1|name=sqlGrid;caption=Data grid;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=82;besth=32;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=scratchPad;caption=Scratch pad;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=300;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=347;floaty=725;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=84|dock_size(3,0,0)=173|")
#else
#ifdef __WXGTK__
#define FRMEDITGRID_DEFAULT_PERSPECTIVE wxT("layout1|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=290;besth=25;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=limitBar;caption=Limit bar;state=16788208;dir=1;layer=10;row=0;pos=243;prop=100000;bestw=72;besth=25;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=415;floaty=793;floatw=-1;floath=-1|name=sqlGrid;caption=Data grid;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=82;besth=32;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=scratchPad;caption=Scratch pad;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=300;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=347;floaty=725;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=84|dock_size(3,0,0)=173|")
#else
#define FRMEDITGRID_DEFAULT_PERSPECTIVE wxT("layout1|name=toolBar;caption=Tool bar;state=16788208;dir=1;layer=10;row=0;pos=0;prop=100000;bestw=232;besth=23;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=limitBar;caption=Limit bar;state=16788208;dir=1;layer=10;row=0;pos=243;prop=100000;bestw=72;besth=21;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=415;floaty=793;floatw=-1;floath=-1|name=sqlGrid;caption=Data grid;state=1020;dir=5;layer=0;row=0;pos=0;prop=100000;bestw=82;besth=32;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1|name=scratchPad;caption=Scratch pad;state=16779260;dir=3;layer=0;row=0;pos=0;prop=100000;bestw=400;besth=300;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=347;floaty=725;floatw=-1;floath=-1|dock_size(1,10,0)=25|dock_size(5,0,0)=84|dock_size(3,0,0)=173|")
#endif
#endif

class cacheLine
{
public:
    cacheLine() { cols=0; stored=false; readOnly=false; }
    ~cacheLine() { if (cols) delete[] cols; }

    wxString *cols;
    bool stored, readOnly;
};


class cacheLinePool
{
public:
    cacheLinePool(int initialLines);
    ~cacheLinePool();
    cacheLine *operator[] (int line) { return Get(line); }
    cacheLine *Get(int lineNo);
    bool IsFilled(int lineNo);
    void Delete(int lineNo);

private:
    cacheLine **ptr;
    int anzLines;
};




// we cannot derive from wxGridCellAttr because destructor is private but not virtual 
class sqlCellAttr
{
public:
    sqlCellAttr()  { attr = new wxGridCellAttr; isPrimaryKey=false; needResize=false; }
    ~sqlCellAttr() { attr->DecRef(); }
    int size();
    int precision();

    wxGridCellAttr *attr;
    wxString Quote(pgConn *conn, const wxString &value);
    OID type;
    long typlen, typmod;
    wxString name, typeName, displayTypeName;
    bool numeric, isPrimaryKey, needResize;
};


class sqlTable;

class ctlSQLEditGrid : public ctlSQLGrid
{
public:
    ctlSQLEditGrid(wxFrame *parent, wxWindowID id, const wxPoint& pos, const wxSize& size);

    sqlTable *GetTable() { return (sqlTable*)wxGrid::GetTable(); }
    //wxSize GetBestSize(int row, int col);
    void ResizeEditor(int row, int col);
    wxArrayInt GetSelectedRows() const;
    bool CheckRowPresent(int row);
    virtual bool IsColText(int col);

#if wxCHECK_VERSION(2,5,0)
    // problems are fixed
#else
    bool SetTable(wxGridTableBase *table, bool takeOwnership=false);
#endif
};


class sqlTable : public wxGridTableBase
{
public:
    sqlTable(pgConn *conn, pgQueryThread *thread, const wxString& tabName, const OID relid, bool _hasOid, const wxString& _pkCols, char _relkind);
    ~sqlTable();
    bool StoreLine();
    void UndoLine(int row);

    int GetNumberRows();
    int GetNumberStoredRows();
    int GetNumberCols();
    wxString GetColLabelValue(int col);
    wxString GetRowLabelValue(int row);
    wxGridCellAttr* GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind);

    wxString GetValue(int row, int col);
    void SetValue(int row, int col, const wxString &value);

    bool IsEmptyCell(int row, int col) { return false; }
    bool needsResizing(int col) { return columns[col].needResize; }
    bool AppendRows(size_t rows);
    bool DeleteRows(size_t pos, size_t rows);
    int  LastRow() { return lastRow; }
    bool IsColText(int col);

    bool CheckInCache(int row);

    void Paste();

private:
    pgQueryThread *thread;
    pgConn *connection;
    bool hasOids;
    char relkind;
    wxString tableName;
    OID relid;
    wxString primaryKeyColNumbers;

    cacheLine *GetLine(int row);
    wxString MakeKey(cacheLine *line);
    void SetNumberEditor(int col, int len);

    cacheLinePool *dataPool, *addPool;
    cacheLine savedLine;
    int lastRow;

    int *lineIndex;     // reindex of lines in dataSet to handle deleted rows

    int nCols;          // columns from dataSet
    int nRows;          // rows initially returned by dataSet
    int rowsCached;     // rows read from dataset; if nRows=rowsCached, dataSet can be deleted
    int rowsAdded;      // rows added (never been in dataSet)
    int rowsStored;     // rows added and stored to db
    int rowsDeleted;    // rows deleted from initial dataSet
    sqlCellAttr *columns;

    friend class ctlSQLEditGrid;
};


class frmMain;
class pgSchemaObject;

class frmEditGrid : public pgFrame
{
public:
    frmEditGrid(frmMain *form, const wxString& _title, pgConn *conn, pgSchemaObject *obj);
    ~frmEditGrid();

    void ShowForm(bool filter = false);
    void Go();
    wxString GetSortCols() const { return orderBy; } ;
    void SetSortCols(const wxString &cols);
    wxString GetFilter() const { return rowFilter; } ;
    void SetFilter(const wxString &filter);
    int GetLimit() const { return limit; } ;
    void SetLimit(const int rowlimit);
    wxMenu *GetFileMenu() { return fileMenu; };
    wxMenu *GetEditMenu() { return editMenu; };

private:
    void OnEraseBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
    
    void OnCloseWindow(wxCloseEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnContents(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    bool DoSave();
    void OnUndo(wxCommandEvent& event);
    void OnCellChange(wxGridEvent& event);
    void OnGridSelectCells(wxGridRangeSelectEvent& event);
    void OnEditorShown(wxGridEvent& event);
    void OnEditorHidden(wxGridEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnLabelDoubleClick(wxGridEvent& event);
    void OnLabelRightClick(wxGridEvent& event);
    void Abort();
    void OnToggleScratchPad(wxCommandEvent& event);
    void OnToggleLimitBar(wxCommandEvent& event);
    void OnToggleToolBar(wxCommandEvent& event);
    void OnAuiUpdate(wxFrameManagerEvent& event);
    void OnDefaultView(wxCommandEvent& event);

    wxFrameManager manager;
    ctlSQLEditGrid *sqlGrid;

    frmMain *mainForm;
    pgConn *connection;
    pgQueryThread *thread;
    wxMenu *fileMenu, *editMenu, *viewMenu, *helpMenu;
    wxToolBar *toolBar;
    ctlComboBoxFix *cbLimit;
    wxTextCtrl *scratchPad;

    char relkind;
    OID relid;
    bool hasOids;
	bool optionsChanged;
    wxString tableName;
    wxString primaryKeyColNumbers;
    wxString orderBy;
	wxString rowFilter;
	int limit;
    bool editorShown;

    DECLARE_EVENT_TABLE()
};


class editGridFactoryBase : public contextActionFactory
{
public:
    bool CheckEnable(pgObject *obj);

protected:
    editGridFactoryBase(menuFactoryList *list) : contextActionFactory(list) { rowlimit = 0; }
    wxWindow *ViewData(frmMain *form, pgObject *obj, bool filter);
	int rowlimit;
};


class editGridFactory : public editGridFactoryBase
{
public:
    editGridFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class editGridFilteredFactory : public editGridFactoryBase
{
public:
    editGridFilteredFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

class editGridLimitedFactory : public editGridFactoryBase
{
public:
	editGridLimitedFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar, int limit);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

#endif

