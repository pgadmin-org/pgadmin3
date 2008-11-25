//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgIndex.cpp - PostgreSQL Index Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "dlg/dlgIndex.h"
#include "schema/pgIndex.h"
#include "schema/pgColumn.h"
#include "schema/pgTable.h"


#define cbTablespace    CTRL_COMBOBOX("cbTablespace")
#define cbType          CTRL_COMBOBOX("cbType")
#define chkUnique       CTRL_CHECKBOX("chkUnique")
#define chkClustered    CTRL_CHECKBOX("chkClustered")
#define chkConcurrent   CTRL_CHECKBOX("chkConcurrent")
#define txtWhere        CTRL_TEXT("txtWhere")
#define txtFillFactor   CTRL_TEXT("txtFillFactor")

#define chkDesc         CTRL_CHECKBOX("chkDesc")
//#define rdbNulls        CTRL_RADIOBOX("rdbNulls")
#define rdbNullsFirst   CTRL_RADIOBUTTON("rdbNullsFirst")
#define rdbNullsLast    CTRL_RADIOBUTTON("rdbNullsLast")


BEGIN_EVENT_TABLE(dlgIndexBase, dlgCollistProperty)
    EVT_TEXT(XRCID("cbTablespace"),                 dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbTablespace"),             dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtFillFactor"),                dlgProperty::OnChange)
    EVT_LIST_ITEM_SELECTED(XRCID("lstColumns"),     dlgIndexBase::OnSelectListCol)
    EVT_COMBOBOX(XRCID("cbColumns"),                dlgIndexBase::OnSelectComboCol)
END_EVENT_TABLE();


dlgProperty *pgIndexFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgIndex(this, frame, (pgIndex*)node, (pgTable*)parent);
}


dlgIndexBase::dlgIndexBase(pgaFactory *f, frmMain *frame, const wxString &resName, pgIndexBase *node, pgTable *parentNode)
: dlgCollistProperty(f, frame, resName, parentNode)
{
    index=node;
    wxASSERT(!table || table->GetMetaType() == PGM_TABLE);
}


dlgIndexBase::dlgIndexBase(pgaFactory *f, frmMain *frame, const wxString &resName, ctlListView *colList)
: dlgCollistProperty(f, frame, resName, colList)
{
    index=0;
}


pgObject *dlgIndexBase::GetObject()
{
    return index;
}


int dlgIndexBase::Go(bool modal)
{

    if (index)
    {
        // edit mode: view only
        txtName->Disable();
        cbColumns->Disable();

        if (txtFillFactor)
        {
            txtFillFactor->SetValue(index->GetFillFactor());
        }
    }
    else
    {
        // create mode
    }

    if (txtFillFactor)
    {
        txtFillFactor->SetValidator(numericValidator);
        if (connection->BackendMinimumVersion(8, 2))
            txtFillFactor->Enable();
        else
            txtFillFactor->Disable();
    }

    btnAddCol->Disable();
    btnRemoveCol->Disable();

    return dlgCollistProperty::Go(modal);
}

void dlgIndexBase::OnSelectListCol(wxListEvent &ev)
{
    OnSelectCol();
}

void dlgIndexBase::OnSelectComboCol(wxCommandEvent &ev)
{
    OnSelectCol();
}

void dlgIndexBase::OnSelectCol()
{
	// Can't change the columns on an existing index.
	if (index)
		return;

    if (lstColumns->GetSelection() != wxNOT_FOUND)
        btnRemoveCol->Enable(true);
    else
        btnRemoveCol->Enable(false);

    if (cbColumns->GetSelection() != wxNOT_FOUND && !cbColumns->GetValue().IsEmpty())
        btnAddCol->Enable(true);
    else
        btnAddCol->Enable(false);
}


void dlgIndexBase::CheckChange()
{
    if (index)
    {
        EnableOK(txtComment->GetValue() != index->GetComment() ||
                 cbTablespace->GetOIDKey() != index->GetTablespaceOid());
    }
    else
    {
        bool enable=true;
        txtComment->Enable(!GetName().IsEmpty());
        CheckValid(enable, lstColumns->GetItemCount() > 0, _("Please specify columns."));
        EnableOK(enable);
    }
}


BEGIN_EVENT_TABLE(dlgIndex, dlgIndexBase)
    EVT_BUTTON(XRCID("btnAddCol"),                  dlgIndex::OnAddCol)
    EVT_BUTTON(XRCID("btnRemoveCol"),               dlgIndex::OnRemoveCol)
    EVT_CHECKBOX(XRCID("chkClustered"),             dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkDesc"),                  dlgIndex::OnDescChange)
#ifdef __WXMAC__
    EVT_SIZE(                                       dlgIndex::OnChangeSize)
#endif
END_EVENT_TABLE();

        
dlgIndex::dlgIndex(pgaFactory *f, frmMain *frame, pgIndex *index, pgTable *parentNode)
: dlgIndexBase(f, frame, wxT("dlgIndex"), index, parentNode)
{
    lstColumns->AddColumn(_("Column name"), 90);
    lstColumns->AddColumn(_("Order"), 40);
    lstColumns->AddColumn(_("NULLs Order"), 50);
}


void dlgIndex::CheckChange()
{
    bool fill=false;

    if (index)
    {
        if (txtFillFactor)
        {
            fill = txtFillFactor->GetValue() != index->GetFillFactor();
        }

        EnableOK(fill || 
                 txtComment->GetValue() != index->GetComment() || 
                 chkClustered->GetValue() != index->GetIsClustered() ||
                 cbTablespace->GetOIDKey() != index->GetTablespaceOid());
    }
    else
    {
        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, lstColumns->GetItemCount() > 0, _("Please specify columns."));
        EnableOK(enable);
    }
}


wxString dlgIndex::GetColumns()
{
    wxString sql;

    int pos;
    // iterate cols
    for (pos=0 ; pos < lstColumns->GetItemCount() ; pos++)
    {
        if (pos)
            sql += wxT(", ");

        sql += qtIdent(lstColumns->GetItemText(pos));

        if (this->database->BackendMinimumVersion(8, 3))
        {
            wxString order = lstColumns->GetText(pos, 1);
            if (!order.IsEmpty())
                sql += wxT(" ") + order;

            wxString nullsOrder = lstColumns->GetText(pos, 2);
            if (!nullsOrder.IsEmpty())
                sql += wxT(" NULLS ") + nullsOrder;
        }
    }
    return sql;
}


int dlgIndex::Go(bool modal)
{
    if (!connection->BackendMinimumVersion(7, 4))
        chkClustered->Disable();

    if (index)
    {
        // edit mode: view only

		wxArrayString colsArr = index->GetColumnList();
        if (this->database->BackendMinimumVersion(8, 3))
        {
            wxString colDef, colRest, colName, descDef, nullsDef;
            const wxString firstOrder = wxT(" NULLS FIRST"), lastOrder = wxT(" NULLS LAST"), descOrder = wxT(" DESC");

            for (int colIdx=0,colsCount=colsArr.Count(); colIdx<colsCount; colIdx++)
            {
                colDef = colsArr.Item(colIdx);

                if (colDef.EndsWith(firstOrder.GetData(), &colRest))
                {
                    colDef = colRest;
                    nullsDef = wxT("FIRST");
                }
                else if (colDef.EndsWith(lastOrder.GetData(), &colRest))
                {
                    colDef = colRest;
                    nullsDef = wxT("LAST");
                }
                else
                    nullsDef = wxT("");

                if (colDef.EndsWith(descOrder.GetData(), &colRest))
                {
                    colDef = colRest;
                    descDef = wxT("DESC");
                    if (nullsDef.IsEmpty())
                        nullsDef = wxT("FIRST");
                }
                else
                {
                    descDef = wxT("ASC");
                    if (nullsDef.IsEmpty())
                        nullsDef = wxT("LAST");
                }

			    lstColumns->InsertItem(colIdx, colDef, columnFactory.GetIconId());
                lstColumns->SetItem(colIdx, 1, descDef);
                lstColumns->SetItem(colIdx, 2, nullsDef);
            }
        }
        else
            for (int colIdx=0,colsCount=colsArr.Count(); colIdx<colsCount; colIdx++)
			    lstColumns->InsertItem(colIdx, colsArr.Item(colIdx), columnFactory.GetIconId());

        cbType->Append(index->GetIndexType());
        chkUnique->SetValue(index->GetIsUnique());
        chkClustered->SetValue(index->GetIsClustered());
        txtWhere->SetValue(index->GetConstraint());
        cbType->SetSelection(0);
        cbType->Disable();
        txtWhere->Disable();
        chkUnique->Disable();
        chkConcurrent->Disable();
        PrepareTablespace(cbTablespace, index->GetTablespaceOid());
        chkDesc->Disable();
        rdbNullsFirst->Disable();
        rdbNullsLast->Disable();
    }
    else
    {
        // create mode
        PrepareTablespace(cbTablespace);
        cbType->Append(wxT(""));
        pgSet *set=connection->ExecuteSet(wxT(
            "SELECT amname FROM pg_am"));
        if (set)
        {
            while (!set->Eof())
            {
                cbType->Append(set->GetVal(0));
                set->MoveNext();
            }
            delete set;
        }

        if (!this->database->BackendMinimumVersion(8, 2))
            chkConcurrent->Disable();

        if (!this->database->BackendMinimumVersion(8, 3))
        {
            chkDesc->Disable();
            rdbNullsFirst->Disable();
            rdbNullsLast->Disable();
        }

        // Add the default tablespace 
        cbTablespace->Insert(_("<default tablespace>"), 0, (void *)0);
        cbTablespace->SetSelection(0);
    }

    int returnCode = dlgIndexBase::Go(modal);

    #ifdef __WXMAC__
    wxSizeEvent event(wxSize(GetSize().GetWidth() - 25, GetSize().GetHeight() + 200));
    OnChangeSize(event);
    #endif

    return returnCode;
}


void dlgIndex::OnAddCol(wxCommandEvent &ev)
{
    wxString colName=cbColumns->GetValue();

    if (!colName.IsEmpty())
    {
        long colIndex = lstColumns->InsertItem(lstColumns->GetItemCount(), colName, columnFactory.GetIconId());

        if (this->database->BackendMinimumVersion(8, 3))
        {
            if (chkDesc->GetValue())
            {
                lstColumns->SetItem(colIndex, 1, wxT("DESC"));

                if (rdbNullsLast->GetValue())
                    lstColumns->SetItem(colIndex, 2, wxT("LAST"));
                else
                    lstColumns->SetItem(colIndex, 2, wxT("FIRST"));
            }
            else
            {
                lstColumns->SetItem(colIndex, 1, wxT("ASC"));

                if (rdbNullsFirst->GetValue())
                    lstColumns->SetItem(colIndex, 2, wxT("FIRST"));
                else
                    lstColumns->SetItem(colIndex, 2, wxT("LAST"));
            }
        }

        cbColumns->Delete(cbColumns->GetCurrentSelection());
        if (cbColumns->GetCount())
            cbColumns->SetSelection(0);

        CheckChange();
        if (!cbColumns->GetCount())
            btnAddCol->Disable();
    }
}


void dlgIndex::OnRemoveCol(wxCommandEvent &ev)
{
    long pos=lstColumns->GetSelection();
    if (pos >= 0)
    {
        wxString colName=lstColumns->GetItemText(pos);

        lstColumns->DeleteItem(pos);
        cbColumns->Append(colName);

        CheckChange();
        btnRemoveCol->Disable();
    }
}

#ifdef __WXMAC__
void dlgIndex::OnChangeSize(wxSizeEvent &ev)
{
	lstColumns->SetSize(wxDefaultCoord, wxDefaultCoord,
	    ev.GetSize().GetWidth(), ev.GetSize().GetHeight() - 700);
    if (GetAutoLayout())
    {
        Layout();
    }
}
#endif

wxString dlgIndex::GetSql()
{
    wxString sql;

    if (table)
    {
        wxString name=GetName();
        if (!index)
        {
            sql = wxT("CREATE ");
            if (chkUnique->GetValue())
                sql += wxT("UNIQUE ");

            sql += wxT("INDEX ");

            if (chkConcurrent->GetValue())
                sql += wxT("CONCURRENTLY ");

            sql += qtIdent(name);

            sql += wxT("\n   ON ") + table->GetQuotedFullIdentifier();

            if (cbType->GetCurrentSelection() > 0)
                AppendIfFilled(sql, wxT(" USING "), cbType->GetValue());

            sql += wxT(" (") + GetColumns()
                + wxT(")");

            if (txtFillFactor)
            {
                if (connection->BackendMinimumVersion(8, 2) && txtFillFactor->GetValue().Length() > 0)
                    sql += wxT("\n  WITH (FILLFACTOR=") + txtFillFactor->GetValue() + wxT(")");
            }

            if (cbTablespace->GetOIDKey() > 0)
                AppendIfFilled(sql, wxT("\n  TABLESPACE "), qtIdent(cbTablespace->GetValue()));

            AppendIfFilled(sql, wxT(" WHERE "), txtWhere->GetValue());
            sql +=  wxT(";\n");
        }
        else
        {
            if (connection->BackendMinimumVersion(8, 0) && cbTablespace->GetOIDKey() != index->GetTablespaceOid())
                sql += wxT("ALTER INDEX ") + qtIdent(index->GetSchema()->GetName()) + wxT(".") + qtIdent(name) 
                    +  wxT(" SET TABLESPACE ") + qtIdent(cbTablespace->GetValue())
                    + wxT(";\n");
        }
        if (connection->BackendMinimumVersion(7, 4))
        {
            if (index && index->GetIsClustered() && !chkClustered->GetValue())
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" SET WITHOUT CLUSTER;\n");
            else if (chkClustered->GetValue() && (!index || !index->GetIsClustered()))
                sql += wxT("ALTER TABLE ") + table->GetQuotedFullIdentifier()
                    +  wxT(" CLUSTER ON ") + qtIdent(name) + wxT(";\n");
        }
        if (index && connection->BackendMinimumVersion(8, 2) && txtFillFactor->GetValue().Length() > 0)
            sql += wxT("ALTER INDEX ") + qtIdent(name)
                +  wxT(" SET (FILLFACTOR=") + txtFillFactor->GetValue() + wxT(");\n");
        AppendComment(sql, wxT("INDEX"), table->GetSchema(), index);
    }
    return sql;
}


pgObject *dlgIndex::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=indexFactory.CreateObjects(collection, 0, wxT(
        "\n   AND cls.relname=") + qtDbString(name) + wxT(
        "\n   AND cls.relnamespace=") + table->GetSchema()->GetOidStr());
    return obj;
}

void dlgIndex::OnDescChange(wxCommandEvent &ev)
{
    if (chkDesc->GetValue())
    {
        rdbNullsFirst->SetValue(true);
    }
    else
    {
        rdbNullsLast->SetValue(true);
    }
}
