//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmEditGrid.cpp - Edit Grid Box
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/grid.h>

#ifdef __WX_FULLSOURCE
#include "wx/generic/gridsel.h"
#else
#include "wxgridsel.h"
#endif

// App headers
#include "pgAdmin3.h"
#include "pgDefs.h"
#include "frmMain.h"
#include "menu.h"

#include <wx/generic/gridctrl.h>
#include <wx/clipbrd.h>

#include "frmEditGrid.h"
#include "dlgEditGridOptions.h"
#include "pgTable.h"
#include "pgView.h"


// Icons
#include "images/viewdata.xpm"
#include "images/storedata.xpm"
#include "images/readdata.xpm"
#include "images/delete.xpm"
#include "images/edit_undo.xpm"
#include "images/sortfilter.xpm"
#include "images/help.xpm"
#include "images/clip_copy.xpm"



BEGIN_EVENT_TABLE(frmEditGrid, pgFrame)
    EVT_MENU(MNU_REFRESH,   frmEditGrid::OnRefresh)
    EVT_MENU(MNU_DELETE,    frmEditGrid::OnDelete)
    EVT_MENU(MNU_SAVE,      frmEditGrid::OnSave)
    EVT_MENU(MNU_UNDO,      frmEditGrid::OnUndo)
    EVT_MENU(MNU_OPTIONS,   frmEditGrid::OnOptions)
    EVT_MENU(MNU_HELP,      frmEditGrid::OnHelp)
    EVT_MENU(MNU_COPY,      frmEditGrid::OnCopy)
    EVT_CLOSE(              frmEditGrid::OnClose)
    EVT_KEY_DOWN(           frmEditGrid::OnKey)
    EVT_GRID_RANGE_SELECT(frmEditGrid::OnGridSelectCells)
    EVT_GRID_SELECT_CELL(frmEditGrid::OnCellChange)
    EVT_GRID_EDITOR_SHOWN(frmEditGrid::OnEditorShown)
    EVT_GRID_LABEL_LEFT_DCLICK(frmEditGrid::OnLabelDoubleClick)
    EVT_GRID_LABEL_RIGHT_CLICK(frmEditGrid::OnLabelRightClick)
END_EVENT_TABLE()


frmEditGrid::frmEditGrid(frmMain *form, const wxString& _title, pgConn *_conn, pgSchemaObject *obj)
: pgFrame(NULL, _title)
{
    wxLogInfo(wxT("Creating EditGrid"));
    SetIcon(wxIcon(viewdata_xpm));
    wxWindowBase::SetFont(settings->GetSystemFont());
    dlgName = wxT("frmEditGrid");
    RestorePosition(-1, -1, 600, 500, 200, 150);
    connection=_conn;
    mainForm=form;
    thread=0;
    relkind=0;
    relid=(Oid)obj->GetOid();


    CreateStatusBar();
    SetStatusBarPane(-1);

    sqlGrid = new ctlSQLGrid(this, CTL_EDITGRID, wxDefaultPosition, wxDefaultSize);
    sqlGrid->SetSizer(new wxBoxSizer(wxVERTICAL));

    wxFont fntLabel(*wxNORMAL_FONT);
    fntLabel.SetWeight(wxBOLD);
    sqlGrid->SetLabelFont(fntLabel);

   
    toolBar = CreateToolBar();

    // Set up toolbar
    toolBar->SetToolBitmapSize(wxSize(16, 16));

    toolBar->AddTool(MNU_SAVE, _("Save"), wxBitmap(storedata_xpm), _("Saved the changed row."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_REFRESH, _("Refresh"), wxBitmap(readdata_xpm), _("Refresh"), wxITEM_NORMAL);
    toolBar->AddTool(MNU_UNDO, _("Undo"), wxBitmap(edit_undo_xpm), _("Undo change of data."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_COPY, _("Copy"), wxBitmap(clip_copy_xpm), _("Copy selected lines to clipboard"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_DELETE, _("Delete"), wxBitmap(delete_xpm), _("Delete selected lines."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_OPTIONS, _("Options"), wxBitmap(sortfilter_xpm), _("Sort/filter options."), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(MNU_HELP, _("Help"), wxBitmap(help_xpm), _("Display help on SQL commands."));

    toolBar->Realize();
    toolBar->EnableTool(MNU_SAVE, false);
    toolBar->EnableTool(MNU_UNDO, false);
    toolBar->EnableTool(MNU_COPY, false);
    toolBar->EnableTool(MNU_DELETE, false);


    wxAcceleratorEntry entries[5];

    entries[0].Set(wxACCEL_CTRL,                (int)'S',      MNU_SAVE);
    entries[1].Set(wxACCEL_NORMAL,              WXK_F5,        MNU_REFRESH);
    entries[2].Set(wxACCEL_CTRL,                (int)'Z',      MNU_UNDO);
    entries[3].Set(wxACCEL_NORMAL,              WXK_F1,        MNU_HELP);
    entries[3].Set(wxACCEL_CTRL,                (int)'C',      MNU_COPY);

    wxAcceleratorTable accel(5, entries);
    SetAcceleratorTable(accel);


    if (obj->GetType() == PG_TABLE)
    {
        pgTable *table = (pgTable*)obj;

        relkind = 'r';
        hasOids = table->GetHasOids();
        tableName = table->GetQuotedFullIdentifier();
        primaryKeyColNumbers = table->GetPrimaryKeyColNumbers();
        orderBy = table->GetQuotedPrimaryKey();
        if (orderBy.IsEmpty() && hasOids)
            orderBy=wxT("oid");
        if (!orderBy.IsEmpty())
            orderBy += wxT(" ASC");
    }
    else if (obj->GetType() == PG_VIEW)
    {
        pgView *view=(pgView*)obj;

        relkind = 'v';
        hasOids=false;
        tableName=view->GetQuotedFullIdentifier();
    }
}

void frmEditGrid::SetSortCols(const wxString &cols) 
{ 
	if (orderBy != cols) { 
		orderBy = cols; 
		optionsChanged = true;
	} 
}

void frmEditGrid::SetFilter(const wxString &filter) 
{ 
	if (rowFilter != filter) { 
		rowFilter = filter; 
		optionsChanged = true;
	} 
}

void frmEditGrid::OnLabelRightClick(wxGridEvent& event)
{
    wxArrayInt rows=sqlGrid->GetSelectedRows();
    if (rows.GetCount())
    {
    }
}

#define EXTRAEXTENT_HEIGHT 6
#define EXTRAEXTENT_WIDTH  6

void frmEditGrid::OnLabelDoubleClick(wxGridEvent& event)
{
#if wxCHECK_VERSION(2, 5, 0)
    // at the moment, not implemented for 2.4
    int maxHeight, maxWidth;
    sqlGrid->GetClientSize(&maxWidth, &maxHeight);
    int row=event.GetRow();
    int col=event.GetCol();

    int extent, extentWant=0;

    if (row >= 0)
    {
        for (col=0 ; col < sqlGrid->GetNumberCols() ; col++)
        {
            extent = sqlGrid->GetBestSize(row, col).GetHeight();
            if (extent > extentWant)
                extentWant=extent;
        }

        extentWant += EXTRAEXTENT_HEIGHT;
        extentWant = wxMax(extentWant, sqlGrid->GetRowMinimalAcceptableHeight());
        extentWant = wxMin(extentWant, maxHeight*3/4);
        int currentHeight=sqlGrid->GetRowHeight(row);
            
        if (currentHeight >= maxHeight*3/4 || currentHeight == extentWant)
            extentWant = sqlGrid->GetRowMinimalAcceptableHeight();
        else if (currentHeight < maxHeight/4)
            extentWant = wxMin(maxHeight/4, extentWant);
        else if (currentHeight < maxHeight/2)
            extentWant = wxMin(maxHeight/2, extentWant);
        else if (currentHeight < maxHeight*3/4)
            extentWant = wxMin(maxHeight*3/4, extentWant);

        if (extentWant != currentHeight)
        {
            sqlGrid->BeginBatch();
            if(sqlGrid->IsCellEditControlShown())
            {
                sqlGrid->HideCellEditControl();
                sqlGrid->SaveEditControlValue();
            }

            sqlGrid->SetRowHeight(row, extentWant);
            sqlGrid->EndBatch();
        }
    }
    else if (col >= 0)
    {
        for (row=0 ; row < sqlGrid->GetNumberRows() ; row++)
        {
            if (sqlGrid->GetTable()->CheckInCache(row))
            {
                extent = sqlGrid->GetBestSize(row, col).GetWidth();
                if (extent > extentWant)
                    extentWant=extent;
            }
        }

        extentWant += EXTRAEXTENT_WIDTH;
        extentWant = wxMax(extentWant, sqlGrid->GetColMinimalAcceptableWidth());
        extentWant = wxMin(extentWant, maxWidth*3/4);
        int currentWidth=sqlGrid->GetColumnWidth(col);
            
        if (currentWidth >= maxWidth*3/4 || currentWidth == extentWant)
            extentWant = sqlGrid->GetColMinimalAcceptableWidth();
        else if (currentWidth < maxWidth/4)
            extentWant = wxMin(maxWidth/4, extentWant);
        else if (currentWidth < maxWidth/2)
            extentWant = wxMin(maxWidth/2, extentWant);
        else if (currentWidth < maxWidth*3/4)
            extentWant = wxMin(maxWidth*3/4, extentWant);

        if (extentWant != currentWidth)
        {
            sqlGrid->BeginBatch();
            if(sqlGrid->IsCellEditControlShown())
            {
                sqlGrid->HideCellEditControl();
                sqlGrid->SaveEditControlValue();
            }
            sqlGrid->SetColumnWidth(col, extentWant);
            sqlGrid->EndBatch();
        }
    }
#endif
}



void frmEditGrid::OnCellChange(wxGridEvent& event)
{
    sqlTable *table=sqlGrid->GetTable();
    if (table)
    {
        if (table->LastRow() >= 0)
        {
            if (table->LastRow() != event.GetRow())
            {
                wxCommandEvent ev;
                OnSave(ev);
            }
        }
        else
        {
            toolBar->EnableTool(MNU_SAVE, false);
            toolBar->EnableTool(MNU_UNDO, false);
        }
    }

    event.Skip();
}


void frmEditGrid::OnCopy(wxCommandEvent &ev)
{
    wxArrayInt rows=sqlGrid->GetSelectedRows();
    size_t i;
    if (rows.GetCount())
    {
        wxString str;
        for (i=0 ; i < rows.GetCount() ; i++)
        {
            str.Append(sqlGrid->GetTable()->GetExportLine(rows.Item(i)));
    
            if (rows.GetCount() > 1)
                str.Append(END_OF_LINE);
        }
        if (wxTheClipboard->Open())
        {
            wxTheClipboard->SetData(new wxTextDataObject(str));
            wxTheClipboard->Close();
        }
    }
    SetStatusText(wxString::Format(_("%d rows copied to clipboard."), rows.GetCount()));
}


void frmEditGrid::OnHelp(wxCommandEvent &ev)
{
    DisplayHelp(this, wxT("editgrid"), viewdata_xpm);
}


void frmEditGrid::OnKey(wxKeyEvent &event)
{
    int curcol=sqlGrid->GetGridCursorCol();
    int currow=sqlGrid->GetGridCursorRow();
    int keycode=event.GetKeyCode();
    wxCommandEvent ev;

    switch (keycode)
    {
#if 0
        // the control will catch these :-(
        case WXK_UP:
            OnSave(ev);
            if (currow)
                sqlGrid->SetGridCursor(currow-1, curcol);
            return;
        case WXK_DOWN:
            OnSave(ev);
            sqlGrid->SetGridCursor(currow+1, curcol);
            return;
#endif
        case WXK_DELETE:
        {
            if (!sqlGrid->IsCurrentCellReadOnly())
            {
                sqlGrid->EnableCellEditControl();
                sqlGrid->ShowCellEditControl();

                wxGridCellEditor *edit=sqlGrid->GetCellEditor(currow, curcol);
                if (edit)
                {
                    wxControl *ctl=edit->GetControl();
                    if (ctl)
                    {
                        wxTextCtrl *txt=wxDynamicCast(ctl, wxTextCtrl);
                        if (txt)
                            txt->SetValue(wxEmptyString);
                    }
                    edit->DecRef();
                }
            }
            return;
        }
        case WXK_RETURN:
            // check for shift etc.
            if (event.ControlDown() || event.ShiftDown())
            {
                // Inject a RETURN into the control
                wxGridCellEditor *edit=sqlGrid->GetCellEditor(currow, curcol);
                if (edit)
                {
                    wxControl *ctl=edit->GetControl();
                    if (ctl)
                    {
                        wxTextCtrl *txt=wxDynamicCast(ctl, wxTextCtrl);
//                        if (txt && txt->IsMultiLine())
                        if (txt) // && txt->IsMultiLine())
                        {
                            long from, to;
                            txt->GetSelection(&from, &to);
                            txt->Replace(from, to, END_OF_LINE);
                        }
                    }
                    edit->DecRef();
                }
                return;
            }
            else
            {
                if (curcol == sqlGrid->GetNumberCols()-1)
                {
                    curcol=0;
                    currow++;
                    // locate first editable column
                    while (sqlGrid->IsReadOnly(currow, curcol) && curcol < sqlGrid->GetNumberCols())
                        curcol++;
                    // next line is completely read-only
                    if (curcol == sqlGrid->GetNumberCols())
                        return;

                }
                else
                    curcol++;

                OnSave(ev);
                sqlGrid->SetGridCursor(currow, curcol);
    
                return;
            }
        default:
            if (sqlGrid->IsEditable() && keycode >= WXK_SPACE && keycode < WXK_START)
            {
                if (sqlGrid->IsCurrentCellReadOnly())
                    return;

                toolBar->EnableTool(MNU_SAVE, true);
                toolBar->EnableTool(MNU_UNDO, true);
            }
            break;
    }
    event.Skip();
}


void frmEditGrid::OnClose(wxCloseEvent& event)
{
    if (toolBar->GetToolEnabled(MNU_SAVE))
    {
        int flag=wxYES_NO | wxICON_QUESTION;
        if (event.CanVeto())
            flag |= wxCANCEL;

        wxMessageDialog msg(this, _("There is unsaved data in a row.\nDo you want to store to the database?"), _("Unsaved data"),
            flag);
        switch (msg.ShowModal())
        {
            case wxID_YES:
            {
                wxCommandEvent ev;
                OnSave(ev);
                break;
            }
            case wxID_CANCEL:
                event.Veto();
                return;
        }
    }
    Abort();
    Destroy();
}


void frmEditGrid::OnUndo(wxCommandEvent& event)
{
    sqlGrid->DisableCellEditControl();
    sqlGrid->GetTable()->UndoLine(sqlGrid->GetGridCursorRow());
    sqlGrid->ForceRefresh();
}


void frmEditGrid::OnRefresh(wxCommandEvent& event)
{
    sqlGrid->DisableCellEditControl();
    Go();
}


void frmEditGrid::OnSave(wxCommandEvent& event)
{
    sqlGrid->HideCellEditControl();
    sqlGrid->SaveEditControlValue();
    sqlGrid->DisableCellEditControl();
    sqlGrid->GetTable()->StoreLine();

    toolBar->EnableTool(MNU_SAVE, false);
    toolBar->EnableTool(MNU_UNDO, false);
}

void frmEditGrid::OnOptions(wxCommandEvent& event)
{
	optionsChanged = false;
    dlgEditGridOptions *winOptions = new dlgEditGridOptions(this, connection, tableName, sqlGrid);
    winOptions->ShowModal();

	if (optionsChanged) Go();
}

template < class T >
int ArrayCmp(T *a, T *b)
{
	if (*a == *b)
		return 0;

	if (*a > *b)
		return 1;
	else
		return -1;
}

void frmEditGrid::OnDelete(wxCommandEvent& event)
{
	wxMessageDialog msg(this, _("Are you sure you wish to delete the selected row(s)?"), _("Delete rows?"), wxYES_NO | wxICON_QUESTION);
    if (msg.ShowModal() != wxID_YES)
        return;

    sqlGrid->BeginBatch();
    wxArrayInt delrows=sqlGrid->GetSelectedRows();
    int i=delrows.GetCount();

	// Sort the grid so we always delete last->first, otherwise we 
	// could end up deleting anything because the array returned by 
	// GetSelectedRows is in the order that rows were selected by
	// the user.
	delrows.Sort(ArrayCmp);

    // don't care a lot about optimizing here; doing it line by line
    // just as sqlTable::DeleteRows does
	while (i--)
		sqlGrid->DeleteRows(delrows.Item(i), 1);


    sqlGrid->EndBatch();

    SetStatusText(wxString::Format(_("%d rows."), sqlGrid->GetTable()->GetNumberStoredRows()), 0);
}


void frmEditGrid::OnEditorShown(wxGridEvent& event)
{
    toolBar->EnableTool(MNU_SAVE, true);
    toolBar->EnableTool(MNU_UNDO, true);

    event.Skip();
}


void frmEditGrid::OnGridSelectCells(wxGridRangeSelectEvent& event)
{
    if (sqlGrid->GetEditable())
    {
        wxArrayInt rows=sqlGrid->GetSelectedRows();

        bool enable=rows.GetCount() > 0;
        if (enable)
        {
            wxCommandEvent nullEvent;
            OnSave(event);

            // check if a readonly line is selected
            int row, col;
            size_t i;

            for (i=0 ; i < rows.GetCount() ; i++)
            {
                row = rows.Item(i);
                bool lineEnabled=false;

                if (row == sqlGrid->GetNumberRows()-1)
                {
                    // the (*) line may not be deleted/copied
                    enable=false;
                    break;
                }
                for (col = 0 ; col < sqlGrid->GetNumberCols() ; col++)
                {
                    if (!sqlGrid->IsReadOnly(row, col))
                    {
                        lineEnabled=true;
                        break;
                    }
                }

                if (!lineEnabled)
                {
                    enable=false;
                    break;
                }
            }
        }
        toolBar->EnableTool(MNU_DELETE, enable);
        toolBar->EnableTool(MNU_COPY,  enable);
    }
    event.Skip();
}


void frmEditGrid::ShowForm(bool filter)
{
	bool abort = false;

    if (relkind == 'r' || relkind == 'v')
	{
		if (filter) 
		{
			dlgEditGridOptions *winOptions = new dlgEditGridOptions(this, connection, tableName, sqlGrid);
			abort = !(winOptions->ShowModal());
		}
		if (abort) {
			Close();
			Destroy();
		} else {
            Show(true);
		    Go();
		}
	}
    else
    {
        wxLogError(__("No Table or view."));
        Close();
        Destroy();
    }
}

void frmEditGrid::Go()
{
    SetStatusText(_("Refreshing data, please wait."), 0);

    wxString qry=wxT("SELECT ");
    if (hasOids)
        qry += wxT("oid, ");
    qry += wxT("* FROM ") + tableName;
    if (!rowFilter.IsEmpty())
    {
        qry += wxT(" WHERE ") + rowFilter;
    }
    if (!orderBy.IsEmpty())
    {
        qry += wxT(" ORDER BY ") + orderBy;
    }

    thread=new pgQueryThread(connection, qry);
    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        Abort();
        return;
    }

    thread->Run();

    while (thread && thread->IsRunning())
    {
        wxYield();
        wxMilliSleep(10);
    }
    if (!thread)
        return;

    if (!thread->DataValid())
    {
        Abort();
        return;
    }
    SetStatusText(wxString::Format(_("%d rows."), thread->DataSet()->NumRows()), 0);

    sqlGrid->BeginBatch();

    // to force the grid to create scrollbars, we make sure the size  so small that scrollbars are needed 
    // later, we will resize the grid's parent to force the correct size (now including scrollbars, even if
    // they are suppressed initially. Win32 won't need this.
    sqlGrid->SetSize(10,10);

    sqlGrid->SetTable(new sqlTable(connection, thread, tableName, relid, hasOids, primaryKeyColNumbers, relkind), true);
    sqlGrid->EndBatch();

    wxSizeEvent event;
    event.m_size = GetSize();
    OnSize(event);
}


frmEditGrid::~frmEditGrid()
{
    wxLogInfo(wxT("Destroying SQL EditGrid"));
    mainForm->RemoveFrame(this);

    if (connection)
        delete connection;
}


void frmEditGrid::Abort()
{
    if (sqlGrid->GetTable())
    {
        sqlGrid->HideCellEditControl();
        // thread is owned by table und will be destroyed there
        sqlGrid->SetTable(0);
    }
    else if (thread)
    {
        SetStatusText(_("aborting."), 0);
        if (thread->IsRunning())
            thread->Delete();
        delete thread;
        thread=0;
    }
}


ctlSQLGrid::ctlSQLGrid(wxFrame *parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxGrid(parent, id, pos, size, wxWANTS_CHARS|wxVSCROLL|wxHSCROLL)
{
}




void ctlSQLGrid::ResizeEditor(int row, int col)
{

    if (GetTable()->needsResizing(col))
    {
        wxGridCellAttr* attr = GetCellAttr(row, col);
        wxGridCellRenderer* renderer = attr->GetRenderer(this, row, col);
        if ( renderer )
        {
            wxClientDC dc(GetGridWindow());
            wxSize size = renderer->GetBestSize(*this, *attr, dc, row, col);
            renderer->DecRef();

            int w=wxMax(size.GetWidth(), 15)+20;
            int h=wxMax(size.GetHeight(), 15)+20;


            wxGridCellEditor *editor=attr->GetEditor(this, row, col);
            if (editor)
            {
                wxRect cellRect = CellToRect(m_currentCellCoords);
                wxRect rect = cellRect;
                rect.SetWidth(w);
                rect.SetHeight(h);

                // we might have scrolled
                CalcUnscrolledPosition(0, 0, &w, &h);
                rect.SetLeft(rect.GetLeft() - w);
                rect.SetTop(rect.GetTop() - h);

                // Clip rect to client size
                GetClientSize(&w, &h);
                rect.SetRight(wxMin(rect.GetRight(), w));
                rect.SetBottom(wxMin(rect.GetBottom(), h));

                // but not smaller than original cell
                rect.SetWidth(wxMax(cellRect.GetWidth(), rect.GetWidth()));
                rect.SetHeight(wxMax(cellRect.GetHeight(), rect.GetHeight()));

                editor->SetSize(rect);
                editor->DecRef();
            }
        }

        attr->DecRef();
    }
}


wxSize ctlSQLGrid::GetBestSize(int row, int col)
{
    wxSize size;

    wxGridCellAttr* attr = GetCellAttr(row, col);
    wxGridCellRenderer* renderer = attr->GetRenderer(this, row, col);
    if ( renderer )
    {
        wxClientDC dc(GetGridWindow());
        size = renderer->GetBestSize(*this, *attr, dc, row, col);
        renderer->DecRef();
    }

    attr->DecRef();

    return size;
}



#if wxCHECK_VERSION(2,5,0)
    // problems are fixed
#else

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


	int col;
	wxCoord w, h, wmax;

        done= wxGrid::SetTable(table, takeOwnership);

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
#endif



wxArrayInt ctlSQLGrid::GetSelectedRows() const
{
    wxArrayInt rows, rows2;

    wxGridCellCoordsArray tl=GetSelectionBlockTopLeft(), br=GetSelectionBlockBottomRight();

    int maxCol=((ctlSQLGrid*)this)->GetNumberCols() -1;
    size_t i;
    for (i=0 ; i < tl.GetCount() ; i++)
    {
        wxGridCellCoords c1=tl.Item(i), c2=br.Item(i);
        if (c1.GetCol() != 0 || c2.GetCol() != maxCol)
            return rows2;

        int j;
        for (j=c1.GetRow() ; j <= c2.GetRow() ; j++)
            rows.Add(j);
    }

    rows2=wxGrid::GetSelectedRows();

    rows.Sort(ArrayCmp);
    rows2.Sort(ArrayCmp);

    size_t i2=0, cellRowMax=rows.GetCount();

    for (i=0 ; i < rows2.GetCount() ; i++)
    {
        int row=rows2.Item(i);
        while (i2 < cellRowMax && rows.Item(i2) < row)
            i2++;
        if (i2 == cellRowMax || row != rows.Item(i2))
            rows.Add(row);
    }

    return rows;
}


class sqlGridTextEditor : public wxGridCellTextEditor
{
public:
    sqlGridTextEditor(bool multiLine=false, int len=0) { isMultiLine=multiLine; textlen=len;  }
    virtual wxGridCellEditor *Clone() const { return new sqlGridTextEditor(isMultiLine, textlen); }
    void Create(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler);
    void BeginEdit(int row, int col, wxGrid* grid);
    bool EndEdit(int row, int col, wxGrid* grid);


protected:
    int textlen;
    bool isMultiLine;
};




void sqlGridTextEditor::Create(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler)
{
    int flags=0;
    if (isMultiLine)
        flags = wxTE_RICH|wxTE_MULTILINE|wxTE_DONTWRAP;

    m_control = new wxTextCtrl(parent, id, wxEmptyString,
                               wxDefaultPosition, wxDefaultSize, flags
                              );

    if (textlen > 0)
        Text()->SetMaxLength(textlen);

    wxGridCellEditor::Create(parent, id, evtHandler);
}


void sqlGridTextEditor::BeginEdit(int row, int col, wxGrid *grid)
{
    wxGridCellTextEditor::BeginEdit(row, col, grid);
    ((ctlSQLGrid*)grid)->ResizeEditor(row, col);
}


bool sqlGridTextEditor::EndEdit(int row, int col, wxGrid *grid)
{
    bool changed = FALSE;
    wxString value = Text()->GetValue();
    changed = TRUE;

    if (changed)
        grid->GetTable()->SetValue(row, col, value);

//    Text()->SetValue(wxEmptyString);

    return changed;

#if 0
    bool rc=wxGridCellTextEditor::EndEdit(row, col, grid);
#ifdef __WXMSW__
    if (Text()->IsRich())
    {
        m_control->wxWindowBase::Show(true);
        m_control->Show(false);
    }
#endif
    return rc;
#endif
}

    
class sqlGridNumericEditor : public wxGridCellTextEditor
{
public:
    sqlGridNumericEditor(int len=-1, int prec=-1) {numlen=len; numprec=prec; }
    virtual wxGridCellEditor *Clone() const { return new sqlGridNumericEditor(numlen, numprec); }
    virtual void Create(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler);

    virtual bool IsAcceptedKey(wxKeyEvent& event);
    virtual void BeginEdit(int row, int col, wxGrid* grid);
    virtual bool EndEdit(int row, int col, wxGrid* grid);

    virtual void Reset() {DoReset(m_startValue); }
    virtual void StartingKey(wxKeyEvent& event);
    virtual void SetParameters(const wxString& params);

protected:
    int numlen, numprec;
    wxString m_startValue;

};



void sqlGridNumericEditor::StartingKey(wxKeyEvent& event)
{
    int keycode = event.GetKeyCode();
    bool allowed=false;

    switch (keycode)
    {
        case WXK_DECIMAL:
        case WXK_NUMPAD_DECIMAL:
        case '.':
            if (numprec)
                allowed=true;
            break;
        case '+':
        case WXK_ADD:
        case WXK_NUMPAD_ADD:
        case '-':
        case WXK_SUBTRACT:
        case WXK_NUMPAD_SUBTRACT:

        case WXK_NUMPAD0:
        case WXK_NUMPAD1:
        case WXK_NUMPAD2:
        case WXK_NUMPAD3:
        case WXK_NUMPAD4:
        case WXK_NUMPAD5:
        case WXK_NUMPAD6:
        case WXK_NUMPAD7:
        case WXK_NUMPAD8:
        case WXK_NUMPAD9:
            allowed=true;
            break;
        default:
            if (wxIsdigit(keycode))
                allowed =true;
            break;
    
    }
    if (allowed)
        wxGridCellTextEditor::StartingKey(event);
    else
        event.Skip();
}



bool sqlGridNumericEditor::IsAcceptedKey(wxKeyEvent& event)
{
    if ( wxGridCellEditor::IsAcceptedKey(event) )
    {
        int keycode = event.GetKeyCode();
        switch ( keycode )
        {
            case WXK_DECIMAL:
            case WXK_NUMPAD_DECIMAL:
                return (numprec != 0);

            case '+':
            case WXK_ADD:
            case WXK_NUMPAD_ADD:
            case '-':
            case WXK_SUBTRACT:
            case WXK_NUMPAD_SUBTRACT:

            case WXK_NUMPAD0:
            case WXK_NUMPAD1:
            case WXK_NUMPAD2:
            case WXK_NUMPAD3:
            case WXK_NUMPAD4:
            case WXK_NUMPAD5:
            case WXK_NUMPAD6:
            case WXK_NUMPAD7:
            case WXK_NUMPAD8:
            case WXK_NUMPAD9:
                return true;
            default:
                return wxIsdigit(keycode) != 0;
        }
    }

    return false;
}



void sqlGridNumericEditor::BeginEdit(int row, int col, wxGrid* grid)
{
    m_startValue = grid->GetTable()->GetValue(row, col);


    wxString value = m_startValue;
    // localize value here

    DoBeginEdit(value);
}



bool sqlGridNumericEditor::EndEdit(int row, int col, wxGrid* grid)
{
    wxASSERT_MSG(m_control,
                 wxT("The sqlGridNumericEditor must be Created first!"));

    bool changed = FALSE;
    wxString value = Text()->GetValue();

    // de-localize value here

    if (value != m_startValue)
        changed = TRUE;

    if (changed)
        grid->GetTable()->SetValue(row, col, value);

    m_startValue = wxEmptyString;
    Text()->SetValue(m_startValue);

    return changed;
}


void sqlGridNumericEditor::SetParameters(const wxString& params)
{
    if ( !params )
    {
        // reset to default
        numlen=-1;
        numprec=-1;
    }
    else
    {
        long tmp;
        if ( params.BeforeFirst(_T(',')).ToLong(&tmp) )
        {
            numlen = (int)tmp;

            if ( params.AfterFirst(_T(',')).ToLong(&tmp) )
            {
                numprec = (int)tmp;

                // skip the error message below
                return;
            }
        }
    }
}


void sqlGridNumericEditor::Create(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler)
{
    m_control = new wxTextCtrl(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize);

    wxGridCellEditor::Create(parent, id, evtHandler);
    Text()->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
}




sqlTable::sqlTable(pgConn *conn, pgQueryThread *_thread, const wxString& tabName, const OID _relid, bool _hasOid, const wxString& _pkCols, char _relkind)
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


    pgSet *colSet=connection->ExecuteSet(
        wxT("SELECT nspname, relname, t.typname, attname, COALESCE(b.oid, t.oid) AS basetype, atthasdef, adsrc,\n")
        wxT("       CASE WHEN t.typbasetype::oid=0 THEN att.atttypmod else t.typtypmod END AS typmod,\n")
        wxT("       CASE WHEN t.typbasetype::oid=0 THEN att.attlen else t.typlen END AS typlen\n")
        wxT("  FROM pg_attribute att\n")
        wxT("  JOIN pg_type t ON t.oid=att.atttypid\n")
        wxT("  JOIN pg_class c ON c.oid=attrelid\n")
        wxT("  JOIN pg_namespace n ON n.oid=relnamespace\n")
        wxT("  LEFT OUTER JOIN pg_type b ON b.oid=t.typbasetype\n")
        wxT("  LEFT OUTER JOIN pg_attrdef def ON adrelid=attrelid AND adnum=attnum\n")
        wxT(" WHERE attnum > 0 AND NOT attisdropped AND attrelid=") + NumToStr(relid) + wxT("::oid\n")
        wxT(" ORDER BY attnum"));



    bool canInsert=false;
    if (colSet)
    {
        if (hasOids)
        {
            columns[0].name = wxT("oid");
            columns[0].numeric = true;
            columns[0].attr->SetReadOnly(true);
            columns[0].type = PGTYPCLASS_NUMERIC;
        }

        for (i=(hasOids ? 1 : 0) ; i < nCols ; i++)
        {
            wxGridCellEditor *editor=0;

            columns[i].name = colSet->GetVal(wxT("attname"));
            columns[i].typeName = colSet->GetVal(wxT("typname"));

            columns[i].type = (Oid)colSet->GetOid(wxT("basetype"));
            if ((columns[i].type == PGOID_TYPE_INT4 || columns[i].type == PGOID_TYPE_INT8)
                && colSet->GetBool(wxT("atthasdef")))
            {
                if (colSet->GetVal(wxT("adsrc")) ==  wxT("nextval('") 
                        + colSet->GetVal(wxT("nspname")) + wxT(".") + colSet->GetVal(wxT("relname"))
                        + wxT("_") + columns[i].name + wxT("_seq'::text)"))
                {
                    if (columns[i].type == PGOID_TYPE_INT4)
                        columns[i].type = (Oid)PGOID_TYPE_SERIAL;
                    else
                        columns[i].type = (Oid)PGOID_TYPE_SERIAL8;
                }
            }
            columns[i].typlen=colSet->GetLong(wxT("typlen"));
            columns[i].typmod=colSet->GetLong(wxT("typmod"));

            switch (columns[i].type)
            {
                case PGOID_TYPE_BOOL:
                    columns[i].numeric = false;
                    columns[i].attr->SetReadOnly(false);
                    editor = new wxGridCellBoolEditor();
                    break;
                case PGOID_TYPE_INT8:
                case PGOID_TYPE_SERIAL8:
                    SetNumberEditor(i, 20);
                    break;
                case PGOID_TYPE_INT2:
                    SetNumberEditor(i, 5);
                    break;
                case PGOID_TYPE_INT4:
                case PGOID_TYPE_SERIAL:
                    SetNumberEditor(i, 10);
                    break;
                case PGOID_TYPE_OID:
                case PGOID_TYPE_TID:
                case PGOID_TYPE_XID:
                case PGOID_TYPE_CID:
                    SetNumberEditor(i, 10);
                    break;
                case PGOID_TYPE_FLOAT4:
                case PGOID_TYPE_FLOAT8:
                    columns[i].numeric = true;
                    columns[i].attr->SetReadOnly(false);
                    editor = new sqlGridNumericEditor();
                    break;
                case PGOID_TYPE_MONEY:
                case PGOID_TYPE_NUMERIC:
                {
                    columns[i].numeric = true;
                    columns[i].attr->SetReadOnly(false);
                    int len=columns[i].size();
                    int prec=columns[i].precision();
                    if (prec>0)
                        len -= (prec);
                    editor = new sqlGridNumericEditor(len, prec);
                    break;
                }
                case PGOID_TYPE_BYTEA:
                case PGOID_TYPE_CHAR:
                case PGOID_TYPE_NAME:
                case PGOID_TYPE_TEXT:
                default:
                    columns[i].numeric = false;
                    columns[i].attr->SetReadOnly(false);
                    columns[i].needResize = true;
//                    editor = new wxGridCellAutoWrapStringEditor();
                    editor = new sqlGridTextEditor(true);
                    break;
            }
            if (editor)
                columns[i].attr->SetEditor(editor);

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
            wxStringTokenizer collist(primaryKeyColNumbers, wxT(","));
            long cn;
            int pkcolcount=0;

            while (collist.HasMoreTokens())
            {
                pkcolcount++;
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


wxString sqlTable::GetExportLine(int row)
{
    wxString str;
    cacheLine *line = GetLine(row);
    if (line)
    {
        int col;
        for (col=0 ; col < nCols ; col++)
        {
            if (col)
                str.Append(settings->GetExportColSeparator());
            bool needQuote = settings->GetExportQuoting() > 1;

            // find out if string
            switch (columns[col].type)
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

            str.Append(line->cols[col]);
        
            if (needQuote)
                str.Append(settings->GetExportQuoteChar());
        }
    }
    return str;
}


wxString sqlTable::GetColLabelValue(int col)
{
    wxString label=columns[col].name + wxT("\n");
    if (columns[col].isPrimaryKey)
        label += wxT("[PK] ");

    switch (columns[col].type)
    {
        case (Oid)PGOID_TYPE_SERIAL:
            label += wxT("serial");
            break;
        case (Oid)PGOID_TYPE_SERIAL8:
            label += wxT("bigserial");
            break;
        default:
            label += columns[col].typeName;
            break;
    }
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
    columns[col].numeric = true;
    columns[col].attr->SetReadOnly(false);
    columns[col].attr->SetEditor(new sqlGridNumericEditor(len, 0));
}


bool sqlTable::CheckInCache(int row)
{
    if (row > nRows - rowsDeleted + rowsAdded)
        return false;
    if (row >= nRows-rowsDeleted)
        return true;

    return dataPool->IsFilled(row);
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
        wxStringTokenizer collist(primaryKeyColNumbers, wxT(","));
        long cn;

        while (collist.HasMoreTokens())
        {
            cn=StrToLong(collist.GetNextToken());

            wxString colval=line->cols[cn-1];
            if (colval.IsEmpty())
                return wxEmptyString;

            if (!where.IsEmpty())
                where += wxT(" AND ");
            where += qtIdent(columns[cn-1].name) + wxT(" = ") 
                  + qtString(colval) + wxT("::") + qtIdent(columns[cn-1].typeName);
        }
    }
    return where;
}



void sqlTable::UndoLine(int row)
{
    if (lastRow >= 0 && row >= 0)
    {
        cacheLine *line=GetLine(row);
        if (line)
        {
            int i;
            for (i=0 ; i < nCols ; i++)
                line->cols[i] = savedLine.cols[i];
            wxToolBar *tb=((wxFrame*)GetView()->GetParent())->GetToolBar();
            if (tb)
            {
                tb->EnableTool(MNU_SAVE, false);
                tb->EnableTool(MNU_UNDO, false);
            }
        }
    }
    lastRow = -1;
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
                if (columns[i].type == PGOID_TYPE_BOOL)  // bool
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
            {
                wxString key=MakeKey(&savedLine);
                wxASSERT(!key.IsEmpty());
                done=connection->ExecuteVoid(wxT(
                    "UPDATE ") + tableName + wxT(
                    " SET ") + valList + wxT(
                    " WHERE ") + key);
            }
        }
        else
        {
            // INSERT

            for (i=0 ; i<nCols ; i++)
            {
                if (!columns[i].attr->IsReadOnly() && !line->cols[i].IsEmpty())
                {
                    if (!colList.IsNull())
                    {
                        valList += wxT(", ");
                        colList += wxT(", ");
                    }
                    colList += qtIdent(columns[i].name);
                    if (columns[i].type == PGOID_TYPE_BOOL)
                        line->cols[i] = (StrToBool(line->cols[i]) ? wxT("t") : wxT("f"));
                    valList += columns[i].Quote(line->cols[i]);
                }
            }

            if (!valList.IsEmpty())
            {
                pgSet *set=connection->ExecuteSet(
                    wxT("INSERT INTO ") + tableName
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
                    ((wxFrame*)GetView()->GetParent())->SetStatusText(wxString::Format(wxT("%d rows."), GetNumberStoredRows()));
                    if (rowsAdded == rowsStored)
                        GetView()->AppendRows();

                    // Read back what we inserted to get default vals
                    wxString key=MakeKey(line);

                    if (key.IsEmpty())
                    {
                        // That's a problem: obviously, the key generated isn't present
                        // because it's serial or default or otherwise generated in the backend
                        // we don't get.
                        // That's why the whole line is declared readonly.

                        line->readOnly=true;
                    }
                    else
                    {
                        set=connection->ExecuteSet(
                            wxT("SELECT * FROM ") + tableName + 
                            wxT(" WHERE ") + key);
                        if (set)
                        {
                            for (i=(hasOids?1:0) ; i < nCols ; i++)
                            {
                                line->cols[i] = set->GetVal(columns[i].name);
                            }
                            delete set;
                        }
                    }
                }
            }
        }
        if (done)
        {
            line->stored = true;
            lastRow = -1;
        }
        else
            UndoLine(lastRow);
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
        if (lastRow >= 0)
            StoreLine();

        if (!line->cols)
            line->cols = new wxString[nCols];

        // remember line contents for later reference in update ... where
        int i;
        for (i=0 ; i < nCols ; i++)
            savedLine.cols[i] = line->cols[i];
        lastRow = row;
    }
    wxToolBar *tb=((wxFrame*)GetView()->GetParent())->GetToolBar();
    if (tb)
    {
        tb->EnableTool(MNU_SAVE, true);
        tb->EnableTool(MNU_UNDO, true);
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
                wxLogError(__("Unexpected empty cache line: dataSet already closed."));
                return val;
            }

            line->stored = true;
            if (lineIndex[row] != thread->DataSet()->CurrentPos() -1)
                thread->DataSet()->Locate(lineIndex[row]+1);

            int i;
            for (i=0 ; i < nCols ; i++)
            {
                wxString val= thread->DataSet()->GetVal(i);
                if (val.IsEmpty())
                {
                    if (!thread->DataSet()->IsNull(i))
                        val = wxT("''");
                }
                else if (val == wxT("''"))
                    val = wxT("\\'\\'");
                line->cols[i] = val;
            }
            rowsCached++;

            if (rowsCached == nRows)
            {
                delete thread;
                thread=0;
            }
        }
    }
    if (columns[col].type == PGOID_TYPE_BOOL)
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
        case PGOID_TYPE_BOOL:
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

    wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_APPENDED, rows);
    GetView()->ProcessTableMessage(msg);

    return true;
}


bool sqlTable::DeleteRows(size_t pos, size_t rows)
{
    size_t i=pos;
    size_t rowsDone=0;

    while (i < pos+rows)
    {
        cacheLine *line=GetLine(pos);
        if (!line)
            break;

        if (line->stored)
        {
            wxString key=MakeKey(line);
            wxASSERT(!key.IsEmpty());
            bool done=connection->ExecuteVoid(wxT(
                "DELETE FROM ") + tableName + wxT(" WHERE ") + key);
            if (!done)
                break;

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
            rowsDone++;
        }
        else
        {
            // last empty line won't be deleted, just cleared
            int j;
            for (j=0 ; j < nCols ; j++)
                line->cols[j] = wxT("");
        }
        i++;
    }

    if (rowsDone > 0 && GetView())
    {
        wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_DELETED, pos, rowsDone);
        GetView()->ProcessTableMessage(msg);
    }
    return (rowsDone != 0);
}


    

wxGridCellAttr* sqlTable::GetAttr(int row, int col, wxGridCellAttr::wxAttrKind  kind)
{
    cacheLine *line=GetLine(row);
    if (line && line->readOnly)
    {
        wxGridCellAttr *attr=new wxGridCellAttr(columns[col].attr);
        attr->SetReadOnly();
        return attr;
    }
    else
    {
        columns[col].attr->IncRef();
        return columns[col].attr;
    }
}



wxString sqlCellAttr::Quote(const wxString& value)
{
    wxString str;
    if (value.IsEmpty())
        str=wxT("NULL");
    else if (numeric)
        str = value;
    else if (value == wxT("\\'\\'"))
        str = qtString(wxT("''"));
    else if (value == wxT("''"))
        str = wxT("''");
    else
        str=qtString(value);

    return str + wxT("::") + typeName;
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
        return -1;
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
        wxLogError(__("Out of Memory for cacheLinePool"));
    }
}



cacheLinePool::~cacheLinePool()
{
    if (ptr)
    {
        while (anzLines--)
        {
            if (ptr[anzLines])
                delete ptr[anzLines];
        }
        delete[] ptr;
    }
}



void cacheLinePool::Delete(int lineNo)
{
    if (ptr && lineNo >= 0 && lineNo < anzLines)
    {
#if 1
        if (ptr[lineNo])
            delete ptr[lineNo];

        if (lineNo < anzLines-1)
        {
            // beware: overlapping copy
            memmove(ptr+lineNo, ptr+lineNo+1, sizeof(cacheLine*) * (anzLines-lineNo-1));
        }
#else
        cacheLine *c;
        c=ptr[0];
        ptr[0]=ptr[1];
        ptr[1]=c;
#endif
        ptr[anzLines-1]=0;
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
            wxLogError(__("Out of Memory for cacheLinePool"));
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


bool cacheLinePool::IsFilled(int lineNo)
{
    return (lineNo < anzLines && ptr[lineNo]);
}
