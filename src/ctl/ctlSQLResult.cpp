//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSQLResult.cpp - SQL Query result window
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>


#include "ctlSQLResult.h"
#include "frmExport.h"


ctlSQLResult::ctlSQLResult(wxWindow *parent, pgConn *_conn, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxListView(parent, id, pos, size, wxLC_REPORT | wxSUNKEN_BORDER)
{
    conn=_conn;
    thread=0;
}



ctlSQLResult::~ctlSQLResult()
{
    Abort();
}


bool ctlSQLResult::Export()
{
    if (rowsRetrieved>0 || (thread && thread->DataSet()->NumRows() > 0))
    {
        frmExport dlg(this);
        if (dlg.ShowModal() > 0)
        {
            if (rowsRetrieved> 0)
                return dlg.Export(this);
            else
                return dlg.Export(thread->DataSet());
        }
    }
    return false;
}



wxString ctlSQLResult::GetExportLine(int row)
{
    if (GetColumnCount() <= 1)
        return GetItemText(row);

    wxString str;
    int col;
    for (col=1 ; col < GetColumnCount() ; col++)
    {
        if (col > 1)
            str.Append(settings->GetExportColSeparator());

        wxString text=GetItemText(row, col);

        bool needQuote=(settings->GetExportQuoting() > 1);

    
        switch (colTypClasses.Item(col))
        {
            case PGTYPCLASS_NUMERIC:
            case PGTYPCLASS_BOOL:
                break;
            default:
                needQuote=true;
                break;
        }
        if (needQuote)
            str.Append(settings->GetExportQuoteChar());
        str.Append(text);
        if (needQuote)
            str.Append(settings->GetExportQuoteChar());
    }    
    return str;
}


int ctlSQLResult::Execute(const wxString &query, int resultToRetrieve)
{
    Abort();

    colSizes.Empty();
    colHeaders.Empty();
    int i;
    wxListItem item;
    item.SetMask(wxLIST_MASK_TEXT|wxLIST_MASK_WIDTH);

    for (i=0 ; i < GetColumnCount() ; i++)
    {
        GetColumn(i, item);
        colHeaders.Add(item.GetText());
        colSizes.Add(item.GetWidth());
    }

    ClearAll();

    rowsRetrieved=0;
    colNames.Empty();
    colTypes.Empty();
    colTypClasses.Empty();

    thread = new pgQueryThread(conn, query, resultToRetrieve);

    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        Abort();
        return -1;
    }

    thread->Run();
    return RunStatus();
}


int ctlSQLResult::Abort()
{
    if (thread)
    {
        thread->Delete();
        delete thread;
    }
    thread=0;
    return 0;
}


int ctlSQLResult::RetrieveOne()
{
    if (!thread || !thread->DataValid())
        return -1;

    if (thread->ReturnCode() != PGRES_TUPLES_OK)
        return -1;

    if (!rowsRetrieved)
    {
        int w, h;
        if (colSizes.GetCount() == 1)
            w = colSizes.Item(0);
        else
            GetSize(&w, &h);

        colNames.Add(thread->DataSet()->ColName(0));
        colTypes.Add(wxT(""));
        colTypClasses.Add(0L);


        InsertColumn(0, thread->DataSet()->ColName(0), wxLIST_FORMAT_LEFT, w);

        while (!thread->DataSet()->Eof())
        {
            InsertItem(rowsRetrieved, thread->DataSet()->GetVal(0));
            rowsRetrieved++;
            thread->DataSet()->MoveNext();
        }
        return rowsRetrieved;
    }
    return 0;
}


int ctlSQLResult::Retrieve(long chunk)
{
    if (!thread || !thread->DataValid())
        return 0;

    if (chunk<0)
        chunk=thread->DataSet()->NumRows();
    wxLogInfo(wxT("retrieve %ld: did %ld of %ld"), chunk, rowsRetrieved, NumRows());

    long col, nCols=thread->DataSet()->NumCols();
    if (!rowsRetrieved)
    {
        wxString colName, colType;
        colTypes.Add(wxT(""));
        colTypClasses.Add(0L);

        Freeze();
        InsertColumn(0, _("Row"), wxLIST_FORMAT_RIGHT, 30);
        colNames.Add(wxT("Row"));

        size_t hdrIndex=0;

        for (col=0 ; col < nCols ; col++)
        {
            colName = thread->DataSet()->ColName(col);
            colType = thread->DataSet()->ColType(col);
            colNames.Add(colName);
            colTypes.Add(colType);
            colTypClasses.Add(thread->DataSet()->ColTypClass(col));

            wxString colHeader=colName +wxT(" (")+ colType +wxT(")");

            int w;
            if (hdrIndex < colHeaders.GetCount() && colHeaders.Item(hdrIndex) == colHeader)
                w = colSizes.Item(hdrIndex++);
            else
            {
                if (hdrIndex+1 < colHeaders.GetCount() && colHeaders.Item(hdrIndex+1) == colHeader)
                {
                    hdrIndex++;
                    w = colSizes.Item(hdrIndex++);
                }
                else
                    w=-1;
            }

            InsertColumn(col+1, colHeader, wxLIST_FORMAT_LEFT, w);
        }
        Thaw();
    }

    long count=0;
    long maxColSize=settings->GetMaxColSize();

    while (chunk-- && !thread->DataSet()->Eof())
    {
        InsertItem(rowsRetrieved, NumToStr(rowsRetrieved+1L));

        for (col=0 ; col < nCols ; col++)
        {
            wxString value = thread->DataSet()->GetVal(col);
            if (maxColSize > 0)
            {
                if ((int)value.Length() > maxColSize)
                    value = value.Left(maxColSize) + wxT(" (..)");
            }
            SetItem(rowsRetrieved, col+1, value);
        }
        
        thread->DataSet()->MoveNext();
        rowsRetrieved++;
        count++;
    }
    wxLogInfo(wxT("retrieve done %ld: did %ld of %ld"), count, rowsRetrieved, NumRows());

    return (count);
}


wxString ctlSQLResult::GetItemText(int row, int col)
{
    if (col < 0)
        return wxListCtrl::GetItemText(row);

    wxListItem item;
    item.m_mask=wxLIST_MASK_TEXT;
    item.m_itemId=row;
    item.m_col=col;
    GetItem(item);
    return item.GetText();
}


wxString ctlSQLResult::GetMessagesAndClear()
{
    if (thread)
        return thread->GetMessagesAndClear();
    return wxEmptyString;
}


wxString ctlSQLResult::GetErrorMessage()
{
    return conn->GetLastError();
}



long ctlSQLResult::NumRows() const
{
    if (thread && thread->DataValid())
        return thread->DataSet()->NumRows();
    return 0;
}


long ctlSQLResult::InsertedCount() const
{
    if (thread)
        return thread->RowsInserted();
    return -1;
}


OID ctlSQLResult::InsertedOid() const
{
    if (thread)
        return thread->InsertedOid();
    return (OID)-1;
}


int ctlSQLResult::RunStatus()
{
    if (!thread)
        return -1;
    
    if (thread->IsRunning())
        return CTLSQL_RUNNING;

    return thread->ReturnCode();
}


