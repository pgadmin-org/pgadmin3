//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmQuery.h - The SQL Query form
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRMEDITGRID_H
#define __FRMEDITGRID_H

#include <wx/grid.h>

#define CTL_EDITGRID 357


class cacheLine
{
public:
    cacheLine() { cols=0; stored=false; }
    ~cacheLine() { if (cols) delete[] cols; }

    wxString *cols;
    bool stored;
};


class cacheLinePool
{
public:
    cacheLinePool(int initialLines);
    ~cacheLinePool();
    cacheLine *operator[] (int line) { return Get(line); }
    cacheLine *Get(int lineNo);
    void Delete(int lineNo);

private:
    cacheLine **ptr;
    int anzLines;
};




// we cannot derive from wxGridCellAttr because destructor is private but not virtual 
class sqlCellAttr
{
public:
    sqlCellAttr()  { attr = new wxGridCellAttr; }
    ~sqlCellAttr() { attr->DecRef(); }
    int size();
    int precision();

    wxGridCellAttr *attr;
    wxString Quote(const wxString &value);
    Oid type;
    long typlen, typmod;
    wxString name, typeName;
    bool numeric;
};



class ctlSQLGrid;
class sqlTable : public wxGridTableBase
{
public:
    sqlTable(pgConn *conn, pgQueryThread *thread, const wxString& tabName, const Oid relid, bool _hasOid, const wxString& _pkCols, char _relkind);
    ~sqlTable();
    void StoreLine();

    int GetNumberRows();
    int GetNumberStoredRows();
    int GetNumberCols();
    wxString GetColLabelValue(int col);
    wxString GetRowLabelValue(int row);
    wxGridCellAttr* GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind);

    wxString GetValue(int row, int col);
    void SetValue(int row, int col, const wxString &value);

    bool IsEmptyCell(int rows, int col) { return false; }
    bool AppendRows(size_t rows);
    bool DeleteRows(size_t pos, size_t rows);


private:
    pgQueryThread *thread;
    pgConn *connection;
    bool hasOids;
    char relkind;
    wxString tableName;
    Oid relid;
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



class ctlSQLGrid : public wxGrid
{
public:
    ctlSQLGrid(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size);

    // Reimplementation needed, because the 2.4.0 version doesn't update internal dimension vars
    bool InsertRows( int pos = 0, int numRows = 1, bool updateLabels=TRUE );
    bool AppendRows( int numRows = 1, bool updateLabels=TRUE );
    bool DeleteRows( int pos = 0, int numRows = 1, bool updateLabels=TRUE );
    bool InsertCols( int pos = 0, int numCols = 1, bool updateLabels=TRUE );
    bool AppendCols( int numCols = 1, bool updateLabels=TRUE );
    bool DeleteCols( int pos = 0, int numCols = 1, bool updateLabels=TRUE );

    // overriding original version to allow re-setting the table
    bool SetTable( wxGridTableBase *table, bool takeOwnership=FALSE );

private:
    void OnCellChange(wxGridEvent& event);
    DECLARE_EVENT_TABLE();
};




class frmEditGrid : public wxFrame
{
public:
    frmEditGrid(frmMain *form, const wxString& _title, pgConn *conn, const wxPoint& pos, const wxSize& size, pgSchemaObject *obj);
    ~frmEditGrid();

    void Go();

private:
    void OnClose(wxCloseEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnGridRightClick(wxGridEvent& event);
    void OnGridLabelRightClick(wxGridEvent& event);

    void Abort();

    ctlSQLGrid *sqlGrid;

    frmMain *mainForm;
    pgConn *connection;
    pgQueryThread *thread;
    wxMenu *contextGridMenu, *contextLabelMenu;

    char relkind;
    Oid relid;
    bool hasOids;
    wxString tableName;
    wxString primaryKeyColNumbers;
    wxString orderBy;

    DECLARE_EVENT_TABLE();
};
#endif
