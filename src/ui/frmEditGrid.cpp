//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmQuery.cpp - SQL Query Box
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"
#include "frmEditGrid.h"

// Icons
#include "images/sql.xpm"



BEGIN_EVENT_TABLE(frmEditGrid, wxFrame)
END_EVENT_TABLE()


frmEditGrid::frmEditGrid(frmMain *form, const wxString& _title, pgConn *_conn, const wxPoint& pos, const wxSize& size, pgSchemaObject *obj)
: wxFrame(NULL, -1, _title, pos, size)
{
    SetIcon(wxIcon(sql_xpm));
    wxLogInfo(wxT("Creating EditGrid"));
    connection=_conn;
    mainForm=form;
    thread=0;
    columnTypes=0;

    pgSet *set=connection->ExecuteSet(wxT(
        "SELECT relkind, relhasoids, reltuples\n"
        "  FROM pg_class\n"
        " WHERE oid=") + obj->GetOidStr());
    if (set)
    {
        relkind = set->GetVal(wxT("relkind"));
        hasOids = set->GetBool(wxT("relhasoids"));
        rowsEstimated = set->GetDouble(wxT("reltuples"));

        tableName=obj->GetQuotedFullIdentifier();
        sqlGrid = new ctlSQLGrid(this, CTL_EDITGRID, wxDefaultPosition, wxDefaultSize);
        sqlGrid->SetSizer(new wxBoxSizer(wxVERTICAL));
    }
}



class sqlTable : public wxGridTableBase
{
public:
    sqlTable(pgSet *set);
    ~sqlTable();
    int GetNumberRows();
    int GetNumberCols();
    bool IsEmptyCell(int rows, int col) { return false; }
    wxString GetValue(int row, int col);
    void SetValue(int row, int col, const wxString &value);
    wxString GetColLabelValue(int col);
    wxString GetRowLabelValue(int row);

private:
    pgSet *dataSet;


    int nRows, rowsAdded, rowsDeleted;
    Oid *colTypes;
};


sqlTable::sqlTable(pgSet *set)
{
    dataSet=set;
    rowsAdded=0;
    rowsDeleted=0;
    nRows = set->NumRows();
    int i, nCols=set->NumCols();
    colTypes = new Oid[nCols];

    for (i=0 ; i < nCols ; i++)
    {
        colTypes[i] = set->ColTypeOid(i);
//      need to check for basetype here
//      instead of OID we need precision etc.
    }
}


sqlTable::~sqlTable()
{
    delete[] colTypes;
}


int sqlTable::GetNumberCols()
{
    return dataSet->NumCols();
}


int sqlTable::GetNumberRows()
{
    return nRows + rowsAdded - rowsDeleted;
}


wxString sqlTable::GetColLabelValue(int col)
{
    return dataSet->ColName(col) + wxT("\n") + dataSet->ColType(col);
}


wxString sqlTable::GetRowLabelValue(int row)
{
    wxString label;
    label.Printf(wxT("%d"), row);
    return label;
}



void sqlTable::SetValue(int row, int col, const wxString &value)
{
}


wxString sqlTable::GetValue(int row, int col)
{
    if (row != dataSet->CurrentPos() -1)
        dataSet->Locate(row+1);
    return dataSet->GetVal(col);
}



void frmEditGrid::Go()
{
    if (relkind == 'r')
        Show(true);
    else
    {
        wxMessageBox("No Table.");
        Close();
        Destroy();
    }
    wxString qry=wxT("SELECT ");
    if (hasOids)
        qry += wxT("Oid, ");
    qry += wxT("* FROM ") + tableName;
    thread=new pgQueryThread(connection->connection(), qry);
    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        Abort();
        return;
    }

    thread->Run();

    while (thread->IsRunning())
    {
        wxYield();
        wxUsleep(10);
    }
    if (!thread->DataValid())
    {
        Abort();
        return;
    }
    sqlGrid->SetTable(new sqlTable(thread->DataSet()));
    sqlGrid->Update();
}


frmEditGrid::~frmEditGrid()
{
    wxLogInfo(wxT("Destroying SQL EditGrid"));
    mainForm->RemoveFrame(this);
    settings->Write(wxT("frmEditGrid/Width"), GetSize().x);
    settings->Write(wxT("frmEditGrid/Height"), GetSize().y);
    settings->Write(wxT("frmEditGrid/Left"), GetPosition().x);
    settings->Write(wxT("frmEditGrid/Top"), GetPosition().y);
    if (connection)
        delete connection;
}





void frmEditGrid::OnAbort(wxCommandEvent& event)
{
    Abort();
}


void frmEditGrid::Abort()
{
    if (thread)
    {
        thread->Delete();
        delete thread;
        thread=0;
    }
    if (columnTypes)
    {
        delete[] columnTypes;
        columnTypes=0;
    }
}




ctlSQLGrid::ctlSQLGrid(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxGrid(parent, id, pos, size)
{
}

