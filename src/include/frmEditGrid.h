//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmEditGrid.h - The SQL Edit Grid form
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRMEDITGRID_H
#define __FRMEDITGRID_H

#include <wx/grid.h>

#define CTL_EDITGRID 357
#include "dlgClasses.h"

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
    wxString Quote(const wxString &value);
    OID type;
    long typlen, typmod;
    wxString name, typeName, typeNspName;
    bool numeric, isPrimaryKey, needResize;
};


class sqlTable;

class ctlSQLGrid : public wxGrid
{
public:
    ctlSQLGrid(wxFrame *parent, wxWindowID id, const wxPoint& pos, const wxSize& size);

    sqlTable *GetTable() { return (sqlTable*)wxGrid::GetTable(); }
    wxSize GetBestSize(int row, int col);
    void ResizeEditor(int row, int col);
    wxArrayInt GetSelectedRows() const;

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
    void StoreLine();
    void UndoLine(int row);

    int GetNumberRows();
    int GetNumberStoredRows();
    int GetNumberCols();
    wxString GetColLabelValue(int col);
    wxString GetRowLabelValue(int row);
    wxGridCellAttr* GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind);

    bool CanGetValueAs(int row, int col, const wxString& typeName);
    bool CanSetValueAs(int row, int col, const wxString& typeName);
    
    wxString GetValue(int row, int col);
    void SetValue(int row, int col, const wxString &value);
    bool GetValueAsBool(int row, int col);
    void SetValueAsBool(int row, int col, bool b);

    bool IsEmptyCell(int row, int col) { return false; }
    bool needsResizing(int col) { return columns[col].needResize; }
    bool AppendRows(size_t rows);
    bool DeleteRows(size_t pos, size_t rows);
    int  LastRow() { return lastRow; }
    wxString GetExportLine(int row);
    wxString GetExportLine(int row, int col1, int col2);
    wxString sqlTable::GetExportLine(int row, wxArrayInt cols);

    bool CheckInCache(int row);


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

    friend class ctlSQLGrid;
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

private:
    void OnClose(wxCloseEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnCellChange(wxGridEvent& event);
    void OnGridSelectCells(wxGridRangeSelectEvent& event);
    void OnEditorShown(wxGridEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnLabelDoubleClick(wxGridEvent& event);
    void OnLabelRightClick(wxGridEvent& event);
    void Abort();

    ctlSQLGrid *sqlGrid;

    frmMain *mainForm;
    pgConn *connection;
    pgQueryThread *thread;
    wxToolBar *toolBar;
    ctlComboBoxFix *cbLimit;

    char relkind;
    OID relid;
    bool hasOids;
	bool optionsChanged;
    wxString tableName;
    wxString primaryKeyColNumbers;
    wxString orderBy;
	wxString rowFilter;
	int limit;

    DECLARE_EVENT_TABLE();
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

