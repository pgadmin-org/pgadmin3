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
#include "wx/generic/gridsel.h"

// App headers
#include "pgAdmin3.h"
#include "frmMain.h"
#include "frmEditGrid.h"
#include "pgTable.h"
#include "pgView.h"

// Icons
#include "images/viewdata.xpm"



BEGIN_EVENT_TABLE(frmEditGrid, wxFrame)
    EVT_MENU(MNU_REFRESH,   frmEditGrid::OnRefresh)
    EVT_MENU(MNU_DELETE,    frmEditGrid::OnDelete)
    EVT_MENU(MNU_SAVE,      frmEditGrid::OnSave)
    EVT_CLOSE(              frmEditGrid::OnClose)
    EVT_GRID_CELL_RIGHT_CLICK(frmEditGrid::OnGridRightClick)
    EVT_GRID_LABEL_RIGHT_CLICK(frmEditGrid::OnGridLabelRightClick)
END_EVENT_TABLE()


frmEditGrid::frmEditGrid(frmMain *form, const wxString& _title, pgConn *_conn, const wxPoint& pos, const wxSize& size, pgSchemaObject *obj)
: wxFrame(NULL, -1, _title, pos, size)
{
    SetIcon(wxIcon(viewdata_xpm));
    wxLogInfo(wxT("Creating EditGrid"));
    connection=_conn;
    mainForm=form;
    thread=0;
    relkind=0;
    relid=(Oid)obj->GetOid();

    contextGridMenu=new wxMenu();
    contextGridMenu->Append(MNU_REFRESH, wxT("&Refresh"),   wxT("Refresh data"));
    contextGridMenu->Append(MNU_SAVE,    wxT("&Save"),   wxT("Save data"));

    contextLabelMenu=new wxMenu();
    contextLabelMenu->Append(MNU_DELETE,  wxT("&Delete"),  	wxT("Delete current row"));

    CreateStatusBar();

    sqlGrid = new ctlSQLGrid(this, CTL_EDITGRID, wxDefaultPosition, wxDefaultSize);
    sqlGrid->SetSizer(new wxBoxSizer(wxVERTICAL));
#ifdef __WIN32__
    wxFont fntLabel(8, wxDEFAULT, wxNORMAL, wxBOLD);
#else
    wxFont fntLabel(12, wxDEFAULT, wxNORMAL, wxBOLD);
#endif
    sqlGrid->SetLabelFont(fntLabel);

    if (obj->GetType() == PG_TABLE)
    {
        pgTable *table = (pgTable*)obj;

        relkind = 'r';
        hasOids = table->GetHasOids();
        tableName = table->GetQuotedFullIdentifier();
        primaryKeyColNumbers = table->GetPrimaryKeyColNumbers();
        orderBy = table->GetPrimaryKey();
        if (orderBy.IsEmpty() && hasOids)
            orderBy=wxT("oid");
    }
    else if (obj->GetType() == PG_VIEW)
    {
        pgView *view=(pgView*)obj;

        relkind = 'v';
        hasOids=false;
        tableName=view->GetQuotedFullIdentifier();
    }
}


void frmEditGrid::OnClose(wxCloseEvent& event)
{
    Abort();
    Destroy();
}


void frmEditGrid::OnRefresh(wxCommandEvent& event)
{
    Go();
}


void frmEditGrid::OnSave(wxCommandEvent& event)
{
    ((sqlTable*)sqlGrid->GetTable())->StoreLine();
}



void frmEditGrid::OnDelete(wxCommandEvent& event)
{
    sqlGrid->BeginBatch();
    wxArrayInt delrows=sqlGrid->GetSelectedRows();
    int i=delrows.GetCount();

    // don't care a lot about optimizing here; doing it line by line
    // just as sqlTable::DeleteRows does
    while (i--)
        sqlGrid->DeleteRows(delrows.Item(i), 1);
    sqlGrid->EndBatch();

    SetStatusText(NumToStr((long)((sqlTable*)sqlGrid->GetTable())->GetNumberStoredRows()) + wxT(" rows."), 0);
}



void frmEditGrid::OnGridRightClick(wxGridEvent& event)
{
    wxPoint pt=event.GetPosition();
    PopupMenu(contextGridMenu, pt);
}


void frmEditGrid::OnGridLabelRightClick(wxGridEvent& event)
{
    if (event.GetRow() >= 0)
    {
        wxPoint pt=event.GetPosition();
        PopupMenu(contextLabelMenu, pt);
    }
}


void frmEditGrid::Go()
{
    if (relkind == 'r' || relkind == 'v')
        Show(true);
    else
    {
        wxMessageBox("No Table or view.");
        Close();
        Destroy();
    }
    SetStatusText(wxT("Refreshing data, please wait."), 0);

    wxString qry=wxT("SELECT ");
    if (hasOids)
        qry += wxT("Oid, ");
    qry += wxT("* FROM ") + tableName;
    if (!orderBy.IsEmpty())
    {
        qry += wxT(" ORDER BY ") + orderBy;
    }

    thread=new pgQueryThread(connection->connection(), qry);
    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        Abort();
        return;
    }

    thread->Run();

    while (thread && thread->IsRunning())
    {
        wxYield();
        wxUsleep(10);
    }
    if (!thread)
        return;

    if (!thread->DataValid())
    {
        Abort();
        return;
    }
    SetStatusText(NumToStr(thread->DataSet()->NumRows()) + wxT(" rows."), 0);
    sqlGrid->BeginBatch();
    sqlGrid->SetTable(new sqlTable(connection, thread, tableName, relid, hasOids, primaryKeyColNumbers, relkind), true);
    sqlGrid->EndBatch();
    sqlGrid->FitInside();
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
    delete contextGridMenu;
    delete contextLabelMenu;
}


void frmEditGrid::Abort()
{
    if (sqlGrid->GetTable())
    {
        // thread is owned by table und will be destroyed there
        sqlGrid->SetTable(0);
    }
    else if (thread)
    {
        SetStatusText(wxT("aborting."), 0);
        if (thread->IsRunning())
            thread->Delete();
        delete thread;
        thread=0;
    }
}



BEGIN_EVENT_TABLE(ctlSQLGrid, wxGrid)
    EVT_GRID_SELECT_CELL(ctlSQLGrid::OnCellChange)
END_EVENT_TABLE();


ctlSQLGrid::ctlSQLGrid(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxGrid(parent, id, pos, size)
{
}



bool ctlSQLGrid::InsertRows(int pos, int numRows, bool updateLabels)
{
    bool done=wxGrid::InsertRows(pos, numRows, updateLabels);
    m_numRows=m_table->GetNumberRows();
    return done;
}


bool ctlSQLGrid::AppendRows(int numRows, bool updateLabels)
{
    bool done=wxGrid::AppendRows(numRows, updateLabels);
    m_numRows=m_table->GetNumberRows();
    return done;
}


bool ctlSQLGrid::DeleteRows(int pos, int numRows, bool updateLabels)
{
    bool done=wxGrid::DeleteRows(pos, numRows, updateLabels);
    m_numRows=m_table->GetNumberRows();
    return done;
}


bool ctlSQLGrid::InsertCols(int pos, int numCols, bool updateLabels)
{
    bool done=wxGrid::InsertCols(pos, numCols, updateLabels);
    m_numCols=m_table->GetNumberCols();
    return done;
}


bool ctlSQLGrid::AppendCols(int numCols, bool updateLabels)
{
    bool done=wxGrid::AppendCols(numCols, updateLabels);
    m_numCols=m_table->GetNumberCols();
    return done;
}


bool ctlSQLGrid::DeleteCols(int pos, int numCols, bool updateLabels)
{
    bool done=wxGrid::DeleteCols(pos, numCols, updateLabels);
    m_numCols=m_table->GetNumberCols();
    return done;
}



bool ctlSQLGrid::SetTable(wxGridTableBase *table, bool takeOwnership)
{
    bool done=false;
    if (m_created)
    {
        m_created = false;
        if (m_ownTable)
            delete m_table;

        delete m_selection;

        // stop all processing
        m_table=0;
        m_selection=0;
        m_numRows=0;
        m_numCols=0;
    }
    if (table)
    {
        done= wxGrid::SetTable(table, takeOwnership);

	    int col;
	    wxCoord w, h, wmax;
	    wxClientDC dc(this);
	    dc.SetFont(GetLabelFont());

	    for (col=0 ; col < m_numCols ; col++)
	    {
            wxString str=GetColLabelValue(col);
	        dc.GetTextExtent(str.BeforeFirst('\n'), &wmax, &h);
            int crPos=str.Find('\n');
            if (crPos)
            {
    	        dc.GetTextExtent(str.Mid(crPos+1), &w, &h);
                if (w>wmax)
                    wmax=w;
            }
            wmax += 4;      // looks better
            if (wmax < 40)
                wmax = 40;

            SetColSize(col, wmax);
	    }
    }
    return done;
}


void ctlSQLGrid::OnCellChange(wxGridEvent& event)
{
    sqlTable *table=(sqlTable*)GetTable();
    if (table && table->lastRow >= 0 && table->lastRow != event.GetRow())
        table->StoreLine();
    event.Skip();
}


sqlTable::sqlTable(pgConn *conn, pgQueryThread *_thread, const wxString& tabName, const Oid _relid, bool _hasOid, const wxString& _pkCols, char _relkind)
{
    connection=conn;
    primaryKeyColNumbers = _pkCols;
    relid=_relid;
    relkind=_relkind;
    tableName=tabName;
    hasOids=_hasOid;
    thread=_thread;

    rowsCached=0;
    rowsAdded=0;
    rowsStored=0;
    rowsDeleted=0;

    dataPool=0;
    addPool = new cacheLinePool(100);       // arbitrary initial size
    lastRow=-1;
    int i;

    nRows = thread->DataSet()->NumRows();
    nCols=thread->DataSet()->NumCols();

    columns = new sqlCellAttr[nCols];
    savedLine.cols = new wxString[nCols];


    pgSet *colSet=connection->ExecuteSet(wxT(
        "SELECT t.typname, attname, COALESCE(b.oid, t.oid) AS basetype,\n"
        "       CASE WHEN t.typbasetype::oid=0 THEN att.atttypmod else t.typtypmod END AS typmod,\n"
        "       CASE WHEN t.typbasetype::oid=0 THEN att.attlen else t.typlen END AS typlen\n"
        "  FROM pg_attribute att\n"
        "  JOIN pg_type t ON t.oid=att.atttypid\n"
        "  LEFT OUTER JOIN pg_type b ON b.oid=t.typbasetype\n"
        " WHERE attnum > 0 AND NOT attisdropped AND attrelid=") + NumToStr((long)relid) + wxT("::oid\n"
        " ORDER BY attnum"));



    bool canInsert=false;
    if (colSet)
    {
        if (hasOids)
        {
            columns[0].name = wxT("oid");
            columns[0].numeric = true;
            columns[0].attr->SetReadOnly(true);
        }

        for (i=(hasOids ? 1 : 0) ; i < nCols ; i++)
        {
            columns[i].name = colSet->GetVal(wxT("attname"));
            columns[i].typeName = colSet->GetVal(wxT("typname"));

            columns[i].type = (Oid)colSet->GetOid(wxT("basetype"));
            columns[i].typlen=colSet->GetLong(wxT("typlen"));
            columns[i].typmod=colSet->GetLong(wxT("typmod"));

            int len=0;
            switch (columns[i].type)
            {
                case 16:    // bool
                    columns[i].numeric = false;
                    columns[i].attr->SetReadOnly(false);
                    columns[i].attr->SetEditor(new wxGridCellBoolEditor());
                    break;
                case 20:    // int8
                    SetNumberEditor(i, 20);
                    break;
                case 21:    // int2
                    SetNumberEditor(i, 5);
                    break;
                case 23:    // int4
                    SetNumberEditor(i, 10);
                    break;
                case 26:    // oid
                case 27:    // tid
                case 28:    // xid
                case 29:    // cid
                    SetNumberEditor(i, 10);
                    break;
                case 700:   // float4
                case 701:   // float8
                    columns[i].numeric = true;
                    columns[i].attr->SetReadOnly(false);
                    columns[i].attr->SetEditor(new wxGridCellFloatEditor());
                    break;
                case 790:   // money
                case 1700:  // numeric
                {
                    columns[i].numeric = true;
                    columns[i].attr->SetReadOnly(false);
                    int len=columns[i].size();
                    int prec=columns[i].precision();
                    if (prec>0)
                        len -= (prec+1);
                    columns[i].attr->SetEditor(new wxGridCellFloatEditor(len, prec));
                    break;
                }
                case 2:     // bytea
                case 3:     // char
                case 4:     // name
                case 10:    // test
                default:
                    columns[i].numeric = false;
                    columns[i].attr->SetReadOnly(false);
                    break;
            }

            if (relkind != 'r' || (!hasOids && primaryKeyColNumbers.IsNull()))
            {
                // for security reasons, we need oid or pk to enable updates. If none is available,
                // the table definition can be considered faulty.
                columns[i].attr->SetReadOnly(true);
            }
            if (!columns[i].attr->IsReadOnly())
                canInsert=true;

            colSet->MoveNext();
        }
        delete colSet;

        if (!hasOids)
        {
            wxStringTokenizer collist(primaryKeyColNumbers, ',');
            long cn;
            int pkcolcount=0;

            while (collist.HasMoreTokens())
            {
                cn=StrToLong(collist.GetNextToken());
                columns[cn-1].isPrimaryKey = true;
            }
        }
    }
    else
    {
        for (i=0 ; i < nCols ; i++)
        {
            columns[i].typeName = thread->DataSet()->ColType(i);
            columns[i].name = thread->DataSet()->ColName(i);
        }
    }

    if (nRows)
    {
        dataPool=new cacheLinePool(nRows);
        lineIndex = new int[nRows];
        for (i=0 ; i < nRows ; i++)
            lineIndex[i] = i;
    }

    if (canInsert)
    {
        // an empty line waiting for inserts
        rowsAdded=1;
    }
}


sqlTable::~sqlTable()
{
    if (thread)
        delete thread;
    if (dataPool)
        delete dataPool;

    delete addPool;

    delete[] columns;
}


int sqlTable::GetNumberCols()
{
    return nCols;
}


int sqlTable::GetNumberRows()
{
    return nRows + rowsAdded - rowsDeleted;
}


int sqlTable::GetNumberStoredRows()
{
    return nRows + rowsStored - rowsDeleted;
}


wxString sqlTable::GetColLabelValue(int col)
{
    wxString label=columns[col].name + wxT("\n");
    if (columns[col].isPrimaryKey)
        label += wxT("[PK] ");

    label += columns[col].typeName;
    return label;
}


wxString sqlTable::GetRowLabelValue(int row)
{
    wxString label;
    if (row < nRows-rowsDeleted || GetLine(row)->stored)
        label.Printf(wxT("%d"), row+1);
    else
        label = wxT("*");
    return label;
}



void sqlTable::SetNumberEditor(int col, int len)
{
    wxGridCellNumberEditor *editor=new wxGridCellNumberEditor();
    columns[col].numeric = true;
    columns[col].attr->SetReadOnly(false);
    columns[col].attr->SetEditor(editor);
}


cacheLine *sqlTable::GetLine(int row)
{
    cacheLine *line;
    if (row < nRows-rowsDeleted)
        line=dataPool->Get(lineIndex[row]);
    else
        line=addPool->Get(row-(nRows-rowsDeleted));
    return line;
}



wxString sqlTable::MakeKey(cacheLine *line)
{
    wxString where;
    if (hasOids)
        where = wxT("OID = ") + line->cols[0];
    else
    {
        wxStringTokenizer collist(primaryKeyColNumbers, ',');
        long cn;
        int pkcolcount=0;

        while (collist.HasMoreTokens())
        {
            cn=StrToLong(collist.GetNextToken());
            if (!where.IsEmpty())
                where += wxT(" AND ");
            where += qtIdent(columns[cn-1].name) + wxT(" = ") 
                  + line->cols[cn-1] + wxT("::") + columns[cn-1].typeName;
        }
    }
    return where;
}



void sqlTable::StoreLine()
{
    GetView()->BeginBatch();
    if (lastRow >= 0)
    {
        bool done=false;

        cacheLine *line=GetLine(lastRow);

        int i;
        wxString colList, valList;

        if (line->stored)
        {
            // UPDATE
            for (i=(hasOids? 1 : 0) ; i<nCols ; i++)
            {
                if (columns[i].type == 16)  // bool
                    line->cols[i] = (StrToBool(line->cols[i]) ? wxT("t") : wxT("f"));

                if (savedLine.cols[i] != line->cols[i])
                {
                    if (!valList.IsNull())
                        valList += wxT(", ");
                    valList += qtIdent(columns[i].name) + wxT("=") + columns[i].Quote(line->cols[i]);
                }
            }

            if (valList.IsEmpty())
                done=true;
            else
                done=connection->ExecuteVoid(wxT(
                    "UPDATE ") + tableName + wxT(
                    " SET ") + valList + wxT(
                    " WHERE ") + MakeKey(&savedLine));
        }
        else
        {
            // INSERT

            for (i=0 ; i<nCols ; i++)
            {
                if (!columns[i].attr->IsReadOnly())
                {
                    if (!colList.IsNull())
                    {
                        valList += wxT(", ");
                        colList += wxT(", ");
                    }
                    colList += qtIdent(columns[i].name);
                    if (columns[i].type == 16)  // bool
                        line->cols[i] = (StrToBool(line->cols[i]) ? wxT("t") : wxT("f"));
                    valList += columns[i].Quote(line->cols[i]);
                }
            }
            
            pgSet *set=connection->ExecuteSet(wxT(
                "INSERT INTO ") + tableName
                + wxT("(") + colList 
                + wxT(") VALUES (") + valList
                + wxT(")"));
            if (set)
            {
                if (hasOids)
                    line->cols[0] = NumToStr((long)set->GetInsertedOid());
                delete set;

                done=true;
                rowsStored++;
                ((wxFrame*)GetView()->GetParent())->SetStatusText(NumToStr((long)GetNumberStoredRows()) + wxT(" rows."));
                if (rowsAdded == rowsStored)
                    GetView()->AppendRows();
            }
        }
        if (done)
        {
            line->stored = true;
            lastRow = -1;
        }
    }
    GetView()->EndBatch();
}


void sqlTable::SetValue(int row, int col, const wxString &value)
{
    cacheLine *line=GetLine(row);

    if (!line)
    {
        // Bad problem, no line!
        return;
    }


    if (row != lastRow)
    {
        StoreLine();

        if (!line->cols)
            line->cols = new wxString[nCols];

        // remember line contents for later reference in update ... where
        int i;
        for (i=0 ; i < nCols ; i++)
            savedLine.cols[i] = line->cols[i];
        lastRow=row;
    }
    line->cols[col] = value;
}



wxString sqlTable::GetValue(int row, int col)
{
    wxString val;
    cacheLine *line;
    if (row < nRows-rowsDeleted)
        line=dataPool->Get(lineIndex[row]);
    else
        line=addPool->Get(row-(nRows-rowsDeleted));

    if (!line)
    {
        // Bad problem, no line!
        return val;
    }

    if (!line->cols)
    {
        line->cols = new wxString[nCols];
        if (row < nRows-rowsDeleted)
        {
            if (!thread)
            {
                wxLogError("Unexpected empty cache line: dataSet already closed.");
                return val;
            }

            line->stored = true;
            if (lineIndex[row] != thread->DataSet()->CurrentPos() -1)
                thread->DataSet()->Locate(lineIndex[row]+1);

            int i;
            for (i=0 ; i < nCols ; i++)
                line->cols[i] = thread->DataSet()->GetVal(i);
            rowsCached++;

            if (rowsCached == nRows)
            {
                delete thread;
                thread=0;
            }
        }
    }
    if (columns[col].type == 16)
        line->cols[col] = (StrToBool(line->cols[col]) ? wxT("TRUE") : wxT("FALSE"));

    val = line->cols[col];
    return val;
}



bool sqlTable::CanGetValueAs(int row, int col, const wxString& typeName)
{
    return CanSetValueAs(row, col, typeName);
}


bool sqlTable::CanSetValueAs(int row, int col, const wxString& typeName)
{
    if (typeName == wxGRID_VALUE_STRING)
        return true;

    switch (columns[col].type)
    {
        case 16:
            return typeName == wxGRID_VALUE_BOOL;
        default:
            return false;
    }
}

    
bool sqlTable::GetValueAsBool(int row, int col)
{
    return StrToBool(GetValue(row, col));
}


void sqlTable::SetValueAsBool(int row, int col, bool b)
{
    SetValue(row, col, (b ? wxT("TRUE") : wxT("FALSE")));
}


bool sqlTable::AppendRows(size_t rows)
{
    rowsAdded += rows;
    GetLine(nRows + rowsAdded - rowsDeleted -1);
    return true;
}


bool sqlTable::DeleteRows(size_t pos, size_t rows)
{
    size_t i;
    bool didSome=false;

    for (i=pos ; i < pos+rows ; i++)
    {
        cacheLine *line=GetLine(pos);
        if (!line)
            return didSome;

        if (line->stored)
        {
            bool done=connection->ExecuteVoid(wxT(
                "DELETE FROM ") + tableName + wxT(" WHERE ") + MakeKey(line));
            if (!done)
                return didSome;

            if ((int)pos < nRows - rowsDeleted)
            {
                rowsDeleted++;
                if ((int)pos < nRows - rowsDeleted)
                    memmove(lineIndex+pos, lineIndex+pos+1, sizeof(cacheLine*)*(nRows-rowsDeleted-pos));
            }
            else
            {
                rowsAdded--;
                if (GetLine(pos)->stored)
                    rowsStored--;
                addPool->Delete(pos - (nRows-rowsDeleted));
            }
            didSome=true;
        }
        else
        {
            // last empty line won't be deleted, just cleared
            int j;
            for (j=0 ; j < nCols ; j++)
                line->cols[j] = wxT("");
        }
    }

    return didSome;
}


    

wxGridCellAttr* sqlTable::GetAttr(int row, int col, wxGridCellAttr::wxAttrKind  kind)
{
    columns[col].attr->IncRef();
    return columns[col].attr;
}



wxString sqlCellAttr::Quote(const wxString& value)
{
    if (numeric)
    {
        if (value.IsEmpty())
            return wxT("NULL::") + typeName;
        return value + wxT("::") + typeName;
    }
    return qtString(value) + wxT("::") + typeName;
}




int sqlCellAttr::size()
{
    if (typlen == -1 && typmod > 0)
    {
        return (typmod-4) >> 16;
    }
    else
        return typlen;
}


int sqlCellAttr::precision()
{
    if (typlen == -1 && typmod > 0)
    {
        return (typmod-4) & 0x7fff;
    }
    else
        return 0;
}


cacheLinePool::cacheLinePool(int initialLines)
{
    ptr=new cacheLine*[initialLines];
    if (ptr)
    {
        anzLines=initialLines;
        memset(ptr, 0, sizeof(cacheLine*)*anzLines);
    }
    else
    {
        anzLines=0;
        wxLogError("Out of Memory for cacheLinePool");
    }
}



cacheLinePool::~cacheLinePool()
{
    while (anzLines--)
    {
        if (ptr[anzLines])
            delete ptr[anzLines];
    }
    if (ptr)
        delete[] ptr;
}



void cacheLinePool::Delete(int lineNo)
{
    if (ptr && lineNo >= 0 && lineNo < anzLines)
    {
        if (ptr[lineNo])
            delete ptr[lineNo];
        if (lineNo < anzLines-1)
        {
            // beware: overlapping copy
            memmove(ptr+lineNo, ptr+lineNo+1, sizeof(cacheLine*) * (anzLines-lineNo-1));
        }
        ptr[anzLines]=0;
    }
}


cacheLine *cacheLinePool::Get(int lineNo)
{
    if (lineNo < 0) return 0;

    if (lineNo >= anzLines)
    {
        cacheLine **old=ptr;
        int oldAnz=anzLines;
        anzLines=lineNo+100;
        ptr=new cacheLine*[anzLines];
        if (!ptr)
        {
            anzLines=0;
            wxLogError("Out of Memory for cacheLinePool");
        }
        else
        {
            if (oldAnz)
            {
                memcpy(ptr, old, sizeof(cacheLine*)*oldAnz);
                delete[] old;
            }
            memset(ptr+oldAnz, 0, anzLines-oldAnz);
        }
    }

    if (lineNo < anzLines)
    {
        if (!ptr[lineNo])
            ptr[lineNo] = new cacheLine();
        return ptr[lineNo];
    }
    return 0;
}
