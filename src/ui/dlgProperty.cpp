//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgQuery.cpp - Property Dialog
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/button.h>

// App headers
#include "pgAdmin3.h"
#include "ctlSQLBox.h"
#include "pgCollection.h"
#include "pgDatatype.h"
#include "misc.h"
#include "menu.h"
#include "pgDefs.h"
#include "ctlSecurityPanel.h"

// Images
#include "images/properties.xpm"

#include "frmMain.h"
#include "frmHelp.h"

// Property dialogs
#include "dlgProperty.h"
#include "dlgServer.h"
#include "dlgUser.h"
#include "dlgGroup.h"
#include "dlgDatabase.h"
#include "dlgCast.h"
#include "dlgLanguage.h"
#include "dlgSchema.h"
#include "dlgTablespace.h"
#include "dlgAggregate.h"
#include "dlgConversion.h"
#include "dlgDomain.h"
#include "dlgFunction.h"
#include "dlgOperator.h"
#include "dlgSequence.h"
#include "dlgTable.h"
#include "dlgColumn.h"
#include "dlgIndex.h"
#include "dlgIndexConstraint.h"
#include "dlgForeignKey.h"
#include "dlgCheck.h"
#include "dlgRule.h"
#include "dlgTrigger.h"
#include "dlgType.h"
#include "dlgView.h"
#include "dlgJob.h"
#include "dlgStep.h"
#include "dlgSchedule.h"

#include "pgTable.h"
#include "pgColumn.h"
#include "pgTrigger.h"



BEGIN_EVENT_TABLE(dlgProperty, DialogWithHelp)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),  dlgProperty::OnPageSelect)  

    EVT_TEXT(XRCID("txtName"),                      dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbOwner"),                      dlgProperty::OnChangeOwner)
    EVT_TEXT(XRCID("txtComment"),                   dlgProperty::OnChange)
    
    EVT_BUTTON(wxID_HELP,                           dlgProperty::OnHelp)
    EVT_BUTTON(wxID_OK,                             dlgProperty::OnOK)
    EVT_BUTTON(wxID_APPLY,                          dlgProperty::OnApply)
END_EVENT_TABLE();


dlgProperty::dlgProperty(frmMain *frame, const wxString &resName) : DialogWithHelp(frame)
{
    readOnly=false;
    objectType=-1;
    sqlPane=0;
    processing=false;
    mainForm=frame;
    database=0;
    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(frame, resName);

#ifdef __WXMSW__
    SetWindowStyleFlag(GetWindowStyleFlag() & ~wxMAXIMIZE_BOX);
#endif

    nbNotebook = CTRL_NOTEBOOK("nbNotebook");
    if (!nbNotebook)
    {
        wxMessageBox(wxString::Format(_("Problem with resource %s: Notebook not found.\nPrepare to crash!"), resName.c_str()));
        return;
    }
    SetIcon(wxIcon(properties_xpm));

    txtName = CTRL_TEXT("txtName");
    txtOid = CTRL_TEXT("txtOID");
    txtComment = CTRL_TEXT("txtComment");
    cbOwner = CTRL_COMBOBOX2("cbOwner");

    wxNotebookPage *page=nbNotebook->GetPage(0);
    wxASSERT(page != NULL);
    page->GetClientSize(&width, &height);

    numericValidator.SetStyle(wxFILTER_NUMERIC);
    btnOK->Disable();
}


dlgProperty::~dlgProperty()
{
    wxString prop = wxT("Properties/") + wxString(typesList[objectType].typName);
	settings->Write(prop, GetPosition());

    if (GetWindowStyle() & wxTHICK_FRAME)
        settings->Write(prop, GetSize());
}


wxString dlgProperty::GetHelpPage() const
{
    wxString page;

    pgObject *obj=0; //GetObject();
    if (obj)
        page=obj->GetHelpPage(false);
    else
    {
        page=wxT("sql-create") + wxString(typesList[objectType].typName).Lower();
    }
    return page;
}


void dlgProperty::CheckValid(bool &enable, const bool condition, const wxString &msg)
{
    if (enable)
    {
        if (!condition)
        {
            if (statusBar)
                statusBar->SetStatusText(msg);
            enable=false;
        }
    }
}


void dlgProperty::SetDatabase(pgDatabase *db)
{
    database=db;
    if (db)
        connection=db->GetConnection();
}

    
void dlgProperty::EnableOK(bool enable)
{
    wxButton *btn=btnApply;
    if (btn)
        btn->Enable(enable);

    btnOK->Enable(enable);
    if (enable)
    {
        if (statusBar)
            statusBar->SetStatusText(wxEmptyString);
    }
}


int dlgProperty::Go(bool modal)
{
    // restore previous position and size, if applicable
    wxString prop = wxT("Properties/") + wxString(typesList[objectType].typName);

    if (GetWindowStyle() & wxTHICK_FRAME)
        SetSize(settings->Read(prop, GetSize()));

    wxPoint pos=settings->Read(prop, GetPosition());
    if (pos.x >= 0 && pos.y >= 0)
        Move(pos);

    wxComboBox *cbowner = cbOwner;

    if (cbowner && !cbowner->GetCount())
    {
        if (!GetObject())
            cbOwner->Append(wxEmptyString);
        AddUsers(cbowner);
    }
    if (txtOid)
        txtOid->Disable();

    if (GetObject())
    {
        if (txtName)
            txtName->SetValue(GetObject()->GetName());
        if (txtOid)
            txtOid->SetValue(NumToStr((long)GetObject()->GetOid()));
        if (cbOwner)
            cbOwner->SetValue(GetObject()->GetOwner());
        if (txtComment)
            txtComment->SetValue(GetObject()->GetComment());


        if (!readOnly && !GetObject()->CanCreate())
        {
            // users who can't create will usually not be allowed to change either.
            readOnly=false;
        }

        SetTitle(wxString(wxGetTranslation(typesList[objectType].typName)) + wxT(" ") + GetObject()->GetFullIdentifier());
    }
    else
    {
        wxButton *btn=btnApply;
        if (btn)
            btn->Hide();
        if (objectType >= 0)
            SetTitle(wxGetTranslation(typesList[objectType].newString));
    }
    if (modal)
        return ShowModal();
    else
        Show(true);
    return 0;
}

void dlgProperty::CreateAdditionalPages()
{
    sqlPane = new ctlSQLBox(nbNotebook, CTL_PROPSQL, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY | wxTE_RICH2);
    nbNotebook->AddPage(sqlPane, wxT("SQL"));
}


wxString dlgProperty::GetName()
{
    if (txtName)
        return txtName->GetValue().Strip(wxString::both);
    return wxEmptyString;
}


void dlgProperty::AppendNameChange(wxString &sql)
{
    if (GetObject()->GetName() != GetName())
        sql += wxT("ALTER ") + GetObject()->GetTypeName()
            +  wxT(" ") + GetObject()->GetQuotedFullIdentifier()
            +  wxT(" RENAME TO ") + qtIdent(GetName())
            +  wxT(";\n");
}


void dlgProperty::AppendOwnerChange(wxString &sql)
{
    if (GetObject()->GetOwner() != cbOwner->GetValue())
        sql += wxT("ALTER ") + GetObject()->GetTypeName()
            +  wxT(" ") + qtIdent(GetName())
            +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue()) 
            +  wxT(";\n");
}


void dlgProperty::AppendOwnerNew(wxString &sql, const wxString &objName)
{
    if (cbOwner->GetGuessedSelection() > 0)
        sql += wxT("ALTER ") + objName
            +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue())
            +  wxT(";\n");
}


void dlgProperty::AppendComment(wxString &sql, const wxString &objName, pgObject *obj)
{
    wxString comment=txtComment->GetValue();
    if ((!obj && !comment.IsEmpty()) ||(obj && obj->GetComment() != comment))
    {
        sql += wxT("COMMENT ON ") + objName
            + wxT(" IS ") + qtString(comment) + wxT(";\n");
    }
}


void dlgProperty::AppendComment(wxString &sql, const wxString &objType, pgSchema *schema, pgObject *obj)
{
    wxString comment=txtComment->GetValue();
    if ((!obj && !comment.IsEmpty()) ||(obj && obj->GetComment() != comment))
    {
        sql += wxT("COMMENT ON ") + objType + wxT(" ");
        if (schema)
           sql += schema->GetQuotedPrefix();
        sql += qtIdent(GetName()) + wxT(" IS ") + qtString(comment) + wxT(";\n");
    }
}


void dlgProperty::AppendQuoted(wxString &sql, const wxString &name)
{
    // quick and quite dirty:
    // !!! this is unsafe if the name itself contains a dot which isn't meant as separator between schema and object
    if (name.First('.') >= 0)
    {
        sql += qtIdent(name.BeforeFirst('.')) + wxT(".") + qtIdent(name.AfterFirst('.'));
    }
    else
        sql += qtIdent(name);
}


void dlgProperty::FillCombobox(const wxString &query, wxComboBox *cb1, wxComboBox *cb2)
{
    if (!cb1 && !cb2)
        return;

    pgSet *set=connection->ExecuteSet(query);
    if (set)
    {
        while (!set->Eof())
        {
            if (cb1)
                cb1->Append(set->GetVal(0));
            if (cb2)
                cb2->Append(set->GetVal(0));
            set->MoveNext();
        }
    }
}


void dlgProperty::AddUsers(wxComboBox *cb1, wxComboBox *cb2)
{
    FillCombobox(wxT("SELECT usename FROM pg_user ORDER BY usename"), cb1, cb2);
}


void dlgProperty::PrepareTablespace(wxComboBox *cb, const wxChar *current)
{
    wxASSERT(cb != 0);

    if (connection->BackendMinimumVersion(7, 5))
    {
        if (current)
        {
            cb->Append(current);
            cb->SetSelection(0);
            cb->Disable();
        }
        else
        {
            cb->Append(wxEmptyString);
            FillCombobox(wxT("SELECT spcname FROM pg_tablespace WHERE spcname <> 'global' ORDER BY spcname"), cb);
            cb->SetSelection(0);
        }
    }
    else
        cb->Disable();
}


void dlgProperty::OnChangeStc(wxStyledTextEvent &ev)
{
    CheckChange();
}


void dlgProperty::OnChange(wxCommandEvent &ev)
{
    CheckChange();
}


void dlgProperty::OnChangeOwner(wxCommandEvent &ev)
{
    ctlComboBox *cb=cbOwner;
    if (cb)
        cb->GuessSelection();
    CheckChange();
}


bool dlgProperty::tryUpdate(wxTreeItemId collectionItem)
{
    wxTreeCtrl *browser=mainForm->GetBrowser();
    pgCollection *collection = (pgCollection*)browser->GetItemData(collectionItem);
    if (collection && collection->IsCollection() && collection->IsCollectionForType(objectType))
    {
        pgObject *data = CreateObject(collection);
        if (data)
        {
            wxString nodeName=data->GetFullName();
            size_t pos=0;
            wxTreeItemId item;

            if (data->GetType() != PG_COLUMN)
            {
                // columns should be appended, not inserted alphabetically

                wxCookieType cookie;
                item=browser->GetFirstChild(collectionItem, cookie);
                while (item)
                {
                    if (browser->GetItemText(item) > nodeName)
                        break;
                    pos++;
                    item=browser->GetNextChild(collectionItem, cookie);
                }
            }

            if (item)
                browser->InsertItem(collectionItem, pos, nodeName, data->GetIcon(), -1, data);
            else    
                browser->AppendItem(collectionItem, nodeName, data->GetIcon(), -1, data);

            if (data->WantDummyChild())
                browser->AppendItem(data->GetId(), wxT("Dummy"));

            if (browser->GetSelection() == collectionItem)
                collection->ShowTreeDetail(browser, 0, mainForm->GetProperties());
            else
                collection->UpdateChildCount(browser);
        }
        else if (GetName().IsEmpty())
        {
            // CreateObject didn't return a new pgObject; refresh the complete collection
            mainForm->Refresh(collection);
        }
        return true;
    }
    return false;
}



void dlgProperty::ShowObject()
{
    pgObject *data=GetObject();
    if (data)
    {
        pgObject *newData=data->Refresh(mainForm->GetBrowser(), item);
        if (newData && newData != data)
        {
            mainForm->SetCurrentObject(newData);
            mainForm->GetBrowser()->SetItemData(item, newData);
            mainForm->GetBrowser()->SetItemImage(item, newData->GetIcon(), wxTreeItemIcon_Normal);
            mainForm->GetBrowser()->SetItemImage(item, newData->GetIcon(), wxTreeItemIcon_Selected);
            newData->SetId(item);
            delete data;
            SetObject(newData);
        }
        if (newData)
        {
            mainForm->GetBrowser()->DeleteChildren(newData->GetId());

            if (item == mainForm->GetBrowser()->GetSelection())
                newData->ShowTree(mainForm, mainForm->GetBrowser(), mainForm->GetProperties(), 0);
            mainForm->GetBrowser()->SetItemText(item, newData->GetFullName());
            mainForm->GetSqlPane()->SetReadOnly(false);
            mainForm->GetSqlPane()->SetText(newData->GetSql(mainForm->GetBrowser()));
            mainForm->GetSqlPane()->SetReadOnly(true);
        }
    }
    else
    {
        wxTreeItemId collectionItem=item;

        while (collectionItem)
        {
            // search up the tree for our collection
            if (tryUpdate(collectionItem))
                break;
            collectionItem=mainForm->GetBrowser()->GetItemParent(collectionItem);
        }
        /*
        if (!collectionItem)
        {
            // search leaves for our collection
            // shouldn't become necessary
            long cookie;
            collectionItem=mainForm->GetBrowser()->GetFirstChild(item, cookie);

            while (collectionItem)
            {
                if (tryUpdate(collectionItem))
                    break;
                collectionItem=mainForm->GetBrowser()->GetNextChild(item, cookie);
            }
        }
        */
    }
}


bool dlgProperty::apply(const wxString &sql)
{
    if (!connection->ExecuteVoid(sql))
    {
        // error message is displayed inside ExecuteVoid
        return false;
    }

    if (database)
        database->AppendSchemaChange(sql);

    ShowObject();

    return true;
}


void dlgProperty::OnApply(wxCommandEvent &ev)
{
    EnableOK(false);

    wxString sql=GetSql();

    if (!sql.IsEmpty())
        if (!apply(sql))
            return;

    if (statusBar)
        statusBar->SetStatusText(_("Changes applied."));
}


void dlgProperty::OnOK(wxCommandEvent &ev)
{
    EnableOK(false);

    if (IsModal())
    {
        EndModal(0);
        return;
    }

    wxString sql=GetSql();

    if (!sql.IsEmpty())
        if (!apply(sql))
        {
            EnableOK(true);
            return;
        }
    Destroy();
}


void dlgProperty::OnPageSelect(wxNotebookEvent& event)
{
    if (sqlPane && event.GetSelection() == (int)nbNotebook->GetPageCount()-1)
    {
        sqlPane->SetReadOnly(false);
        if (btnOK->IsEnabled())
            sqlPane->SetText(GetSql());
        else
        {
            if (GetObject())
                sqlPane->SetText(_("-- nothing to change"));
            else
                sqlPane->SetText(_("-- definition incomplete"));
        }
        sqlPane->SetReadOnly(true);
    }
}


dlgProperty *dlgProperty::CreateDlg(frmMain *frame, pgObject *node, bool asNew, int type)
{
    pgConn *conn=node->GetConnection();

    if (type < 0)
    {
        type=node->GetType();
        if (node->IsCollection())
            type++;
    }

    pgObject *currentNode, *parentNode;
    if (asNew)
        currentNode=0;
    else
        currentNode=node;

    if (type != node->GetType())
        parentNode = node;
    else
        parentNode = (pgObject*)frame->GetBrowser()->GetItemData(
                                frame->GetBrowser()->GetItemParent(
                                    node->GetId()));

    if (parentNode && parentNode->IsCollection() && parentNode->GetType() != PG_SERVERS)
        parentNode = (pgObject*)frame->GetBrowser()->GetItemData(
                                frame->GetBrowser()->GetItemParent(
                                    parentNode->GetId()));

    dlgProperty *dlg=0;

    switch (type)
    {
        case PG_SERVER:
        case PG_SERVERS:
            dlg = new dlgServer(frame, (pgServer*)currentNode);
            break;
        case PG_USER:
        case PG_USERS:
            dlg=new dlgUser(frame, (pgUser*)currentNode);
            break;
        case PG_GROUP:
        case PG_GROUPS:
            dlg=new dlgGroup(frame, (pgGroup*)currentNode);
            break;
        case PG_DATABASE:
        case PG_DATABASES:
        {
            if (asNew)
            {
                // use the server's connection to avoid "template1 in use"
                conn=parentNode->GetConnection();
            }
            dlg=new dlgDatabase(frame, (pgDatabase*)currentNode);
            break;
        }
        case PG_TABLESPACE:
        case PG_TABLESPACES:
            dlg=new dlgTablespace(frame, (pgTablespace*)currentNode);
            break;
        case PG_CAST:
        case PG_CASTS:
            dlg=new dlgCast(frame, (pgCast*)currentNode);
            break;
        case PG_SCHEMA:
        case PG_SCHEMAS:
            dlg=new dlgSchema(frame, (pgSchema*)currentNode);
            break;
        case PG_LANGUAGE:
        case PG_LANGUAGES:
            dlg=new dlgLanguage(frame, (pgLanguage*)currentNode);
            break;
        case PG_AGGREGATE:
        case PG_AGGREGATES:
            dlg=new dlgAggregate(frame, (pgAggregate*)currentNode, (pgSchema*)parentNode);
            break;
        case PG_CONVERSION:
        case PG_CONVERSIONS:
            dlg=new dlgConversion(frame, (pgConversion*)currentNode, (pgSchema*)parentNode);
            break;
        case PG_DOMAIN:
        case PG_DOMAINS:
            dlg=new dlgDomain(frame, (pgDomain*)currentNode, (pgSchema*)parentNode);
            break;
        case PG_TRIGGERFUNCTION:
            if (parentNode->GetType() == PG_TRIGGER)
                parentNode = ((pgTrigger*)parentNode)->GetSchema();
        case PG_FUNCTION:
        case PG_FUNCTIONS:
        case PG_TRIGGERFUNCTIONS:
            dlg=new dlgFunction(frame, (pgFunction*)currentNode, (pgSchema*)parentNode);
            break;
        case PG_TABLE:
        case PG_TABLES:
            dlg=new dlgTable(frame, (pgTable*)currentNode, (pgSchema*)parentNode);
            break;
        case PG_COLUMN:
        case PG_COLUMNS:
            dlg=new dlgColumn(frame, (pgColumn*)currentNode, (pgTable*)parentNode);
            break;
        case PG_INDEX:
        case PG_INDEXES:
            dlg=new dlgIndex(frame, (pgIndex*)currentNode, (pgTable*)parentNode);
            break;
        case PG_PRIMARYKEY:
            dlg=new dlgPrimaryKey(frame, (pgIndex*)currentNode, (pgTable*)parentNode);
            break;
        case PG_FOREIGNKEY:
            dlg=new dlgForeignKey(frame, (pgForeignKey*)currentNode, (pgTable*)parentNode);
            break;
        case PG_UNIQUE:
            dlg=new dlgUnique(frame, (pgIndex*)currentNode, (pgTable*)parentNode);
            break;
        case PG_CHECK:
            dlg=new dlgCheck(frame, (pgCheck*)currentNode, (pgTable*)parentNode);
            break;
        case PG_SEQUENCE:
        case PG_SEQUENCES:
            dlg=new dlgSequence(frame, (pgSequence*)currentNode, (pgSchema*)parentNode);
            break;
        case PG_TRIGGER:
        case PG_TRIGGERS:
            dlg=new dlgTrigger(frame, (pgTrigger*)currentNode, (pgTable*)parentNode);
            break;
        case PG_TYPE:
        case PG_TYPES:
            dlg=new dlgType(frame, (pgType*)currentNode, (pgSchema*)parentNode);
            break;
        case PG_OPERATOR:
        case PG_OPERATORS:
            dlg=new dlgOperator(frame, (pgOperator*)currentNode, (pgSchema*)parentNode);
            break;
        case PG_VIEW:
        case PG_VIEWS:
            dlg=new dlgView(frame, (pgView*)currentNode, (pgSchema*)parentNode);
            break;
        case PG_RULE:
        case PG_RULES:
            dlg=new dlgRule(frame, (pgRule*)currentNode, (pgTable*)parentNode);
            break;
        case PGA_JOB:
            dlg=new dlgJob(frame, (pgaJob*)currentNode);
            break;
        case PGA_STEP:
            dlg=new dlgStep(frame, (pgaStep*)currentNode, (pgaJob*)parentNode);
            break;
        case PGA_SCHEDULE:
            dlg=new dlgSchedule(frame, (pgaSchedule*)currentNode, (pgaJob*)parentNode);
            break;
        default:
            break;
    }

    if (dlg)
    {
        dlg->CenterOnParent();
        if (dlg->objectType < 0)
            dlg->objectType=type;
        dlg->connection=conn;
        dlg->database=node->GetDatabase();

        if (type != node->GetType() && !node->IsCollection())
        {
            wxCookieType cookie;
            wxTreeItemId collectionItem=frame->GetBrowser()->GetFirstChild(node->GetId(), cookie);
            while (collectionItem)
            {
                pgCollection *collection=(pgCollection*)frame->GetBrowser()->GetItemData(collectionItem);
                if (collection && collection->IsCollection() && collection->IsCollectionForType(type))
                    break;

                collectionItem=frame->GetBrowser()->GetNextChild(node->GetId(), cookie);
            }
            dlg->item=collectionItem;
        }
        else
            dlg->item=node->GetId();
    }
    return dlg;
}


bool dlgProperty::CreateObjectDialog(frmMain *frame, pgObject *node, int type)
{
    if (node->GetType() != PG_SERVER && node->GetType() != PG_SERVERS)
    {
        pgConn *conn=node->GetConnection();
        if (!conn || conn->GetStatus() != PGCONN_OK || !conn->IsAlive())
            return false;
    }
    dlgProperty *dlg=CreateDlg(frame, node, true, type);

    if (dlg)
    {
        dlg->SetTitle(wxGetTranslation(typesList[dlg->objectType].newString));

        dlg->CreateAdditionalPages();
        dlg->Go();
    }
    else
        wxMessageBox(_("Not implemented."));
    
    return true;
}


bool dlgProperty::EditObjectDialog(frmMain *frame, ctlSQLBox *sqlbox, pgObject *node)
{
    if (node->GetType() != PG_SERVER)
    {
        pgConn *conn=node->GetConnection();
        if (!conn || conn->GetStatus() != PGCONN_OK || !conn->IsAlive())
            return false;
    }
    dlgProperty *dlg=CreateDlg(frame, node, false);

    if (dlg)
    {
        dlg->SetTitle(wxString(wxGetTranslation(typesList[dlg->objectType].typName)) + wxT(" ") + node->GetFullIdentifier());

        dlg->CreateAdditionalPages();
        dlg->Go();
    }
    else
        wxMessageBox(_("Not implemented."));

    return true;
}



/////////////////////////////////////////////////////////////////////////////


dlgTypeProperty::dlgTypeProperty(frmMain *frame, const wxString &resName)
: dlgProperty(frame, resName)
{
    isVarLen=false;
    isVarPrec=false;
    if (wxWindow::FindWindow(XRCID("txtLength")))
    {
        txtLength = CTRL_TEXT("txtLength");
        txtLength->SetValidator(numericValidator);
        txtLength->Disable();
    }
    else
        txtLength = 0;
    if (wxWindow::FindWindow(XRCID("txtPrecision")))
    {
        txtPrecision= CTRL_TEXT("txtPrecision");
        txtPrecision->SetValidator(numericValidator);
        txtPrecision->Disable();
    }
    else
        txtPrecision=0;
}


void dlgTypeProperty::FillDatatype(ctlComboBox *cb, bool withDomains)
{
    FillDatatype(cb, 0, withDomains);
}


void dlgTypeProperty::FillDatatype(ctlComboBox *cb, ctlComboBox *cb2, bool withDomains)
{
    DatatypeReader tr(database, withDomains);
    while (tr.HasMore())
    {
        pgDatatype dt=tr.GetDatatype();

        AddType(wxT("?"), tr.GetOid(), tr.GetQuotedSchemaPrefix() + dt.QuotedFullName());
        cb->Append(tr.GetSchemaPrefix() + dt.FullName());
        if (cb2)
            cb2->Append(tr.GetSchemaPrefix() + dt.FullName());
        tr.MoveNext();
    }
}


int dlgTypeProperty::Go(bool modal)
{
    if (GetObject())
    {
        if (txtLength)
            txtLength->SetValidator(numericValidator);
        if (txtPrecision)
            txtPrecision->SetValidator(numericValidator);
    }
    return dlgProperty::Go(modal);
}



void dlgTypeProperty::AddType(const wxString &typ, const OID oid, const wxString quotedName)
{
    wxString vartyp;
    if (typ == wxT("?"))
    {
        switch ((long)oid)
        {
            case PGOID_TYPE_BIT:
            case PGOID_TYPE_BIT_ARRAY:
            case PGOID_TYPE_CHAR:
			case PGOID_TYPE_CHAR_ARRAY:
            case PGOID_TYPE_VARCHAR:
			case PGOID_TYPE_VARCHAR_ARRAY:
                vartyp=wxT("L");
                break;
            case PGOID_TYPE_TIME:
			case PGOID_TYPE_TIME_ARRAY:
            case PGOID_TYPE_TIMETZ:
			case PGOID_TYPE_TIMETZ_ARRAY:
            case PGOID_TYPE_TIMESTAMP:
			case PGOID_TYPE_TIMESTAMP_ARRAY:
            case PGOID_TYPE_TIMESTAMPTZ:
			case PGOID_TYPE_TIMESTAMPTZ_ARRAY:
            case PGOID_TYPE_INTERVAL:
			case PGOID_TYPE_INTERVAL_ARRAY:
                vartyp=wxT("D");
                break;
            case PGOID_TYPE_NUMERIC:
			case PGOID_TYPE_NUMERIC_ARRAY:
                vartyp=wxT("P");
                break;
            default:
                vartyp=wxT(" ");
                break;
        }
    }
    else
        vartyp=typ;

    types.Add(vartyp + NumToStr(oid) + wxT(":") + quotedName);
}

    
wxString dlgTypeProperty::GetTypeInfo(int sel)
{
    wxString str;
    if (sel >= 0)
        str = types.Item(sel);

    return str;
}


wxString dlgTypeProperty::GetTypeOid(int sel)
{
    wxString str;
    if (sel >= 0)
        str = types.Item(sel).Mid(1).BeforeFirst(':');

    return str;
}


wxString dlgTypeProperty::GetQuotedTypename(int sel)
{
    wxString sql;
	bool isArray = FALSE;

    if (sel >= 0)
    {
        sql = types.Item(sel).AfterFirst(':');
		if (sql.Contains(wxT("[]"))) {
			sql = sql.BeforeFirst('[');
			isArray = TRUE;
		}

        if (isVarLen && txtLength)
        {
            wxString varlen=txtLength->GetValue();
            if (!varlen.IsEmpty() && NumToStr(StrToLong(varlen))==varlen && StrToLong(varlen) >= minVarLen)
            {
                sql += wxT("(") + varlen;
                if (isVarPrec && txtPrecision)
                {
                    wxString varprec=txtPrecision->GetValue();
                    if (!varprec.IsEmpty())
                        sql += wxT(", ") + varprec;
                }
                sql += wxT(")");
            }
        }
    }

	if (isArray) sql += wxT("[]");
    return sql;
}


void dlgTypeProperty::CheckLenEnable()
{
    int sel=cbDatatype->GetGuessedSelection();
    if (sel >= 0)
    {
        wxString info=types.Item(sel);
        isVarPrec = info.StartsWith(wxT("P"));
        isVarLen =  isVarPrec || info.StartsWith(wxT("L")) || info.StartsWith(wxT("D"));
        minVarLen = (info.StartsWith(wxT("D")) ? 0 : 1);
        maxVarLen = isVarPrec ? 1000 : 
                    minVarLen ? 0x7fffffff : 10;
    }
}


/////////////////////////////////////////////////////////////////////////////


dlgCollistProperty::dlgCollistProperty(frmMain *frame, const wxString &resName, pgTable *parentNode)
: dlgProperty(frame, resName)
{
    columns=0;
    table=parentNode;
}


dlgCollistProperty::dlgCollistProperty(frmMain *frame, const wxString &resName, ctlListView *colList)
: dlgProperty(frame, resName)
{
    columns=colList;
    table=0;
}


int dlgCollistProperty::Go(bool modal)
{
    if (columns)
    {
        int pos;
        // iterate cols
        for (pos=0 ; pos < columns->GetItemCount() ; pos++)
        {
            wxString col=columns->GetItemText(pos);
            if (cbColumns->FindString(col) < 0)
                cbColumns->Append(col);
        }
    }
    if (table)
    {
        wxCookieType cookie;
        pgObject *data;
        wxTreeItemId columnsItem=mainForm->GetBrowser()->GetFirstChild(table->GetId(), cookie);
        while (columnsItem)
        {
            data=(pgObject*)mainForm->GetBrowser()->GetItemData(columnsItem);
            if (data->GetType() == PG_COLUMNS)
                break;
            columnsItem=mainForm->GetBrowser()->GetNextChild(table->GetId(), cookie);
        }

        if (columnsItem)
        {
            wxCookieType cookie;
            pgColumn *column;
            wxTreeItemId item=mainForm->GetBrowser()->GetFirstChild(columnsItem, cookie);

            // check columns
            while (item)
            {
                column=(pgColumn*)mainForm->GetBrowser()->GetItemData(item);
                if (column->GetType() == PG_COLUMN)
                {
                    if (column->GetColNumber() > 0)
                    {
                        cbColumns->Append(column->GetName());
                    }
                }
        
                item=mainForm->GetBrowser()->GetNextChild(columnsItem, cookie);
            }
        }
    }
    return dlgProperty::Go(modal);
}



/////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(dlgSecurityProperty, dlgProperty)
    EVT_BUTTON(CTL_ADDPRIV,             dlgSecurityProperty::OnAddPriv)
    EVT_BUTTON(CTL_DELPRIV,             dlgSecurityProperty::OnDelPriv)
END_EVENT_TABLE();


dlgSecurityProperty::dlgSecurityProperty(frmMain *frame, pgObject *obj, const wxString &resName, const wxString& privList, char *privChar)
        : dlgProperty(frame, resName)
{
    securityChanged=false;


    if (!privList.IsEmpty() && (!obj || obj->CanCreate()))
    {
        securityPage = new ctlSecurityPanel(nbNotebook, privList, privChar, frame->GetImageList());

        if (obj)
        {
            wxString str=obj->GetAcl();
            if (!str.IsEmpty())
            {
                str = str.Mid(1, str.Length()-2);
                wxStringTokenizer tokens(str, wxT(","));

                while (tokens.HasMoreTokens())
                {
                    wxString str=tokens.GetNextToken();
                    if (str[0U]== '"')
                        str = str.Mid(1, str.Length()-2);

                    wxString name=str.BeforeLast('=');
                    wxString value;

					connection = obj->GetConnection();
                    if (connection->BackendMinimumVersion(7, 4))
                        value=str.Mid(name.Length()+1).BeforeLast('/');
                    else
                        value=str.Mid(name.Length()+1);

                    int icon=PGICON_USER;

                    if (name.Left(6).IsSameAs(wxT("group "), false))
                    {
                        icon = PGICON_GROUP;
                        name = wxT("group ") + qtStrip(name.Mid(6));
                    }
                    else if (name.IsEmpty())
                    {
                        icon = PGICON_PUBLIC;
                        name=wxT("public");
                    }
                    else
                        name = qtStrip(name);

                    securityPage->lbPrivileges->AppendItem(icon, name, value);
                    currentAcl.Add(name + wxT("=") + value);
                }
            }
        }
    }else
	securityPage = NULL;
}


dlgSecurityProperty::~dlgSecurityProperty()
{
}



int dlgSecurityProperty::Go(bool modal)
{
    if (securityPage)
        securityPage->SetConnection(connection);
    
    return dlgProperty::Go(modal);
}


void dlgSecurityProperty::AddGroups(ctlComboBox *comboBox)
{
    if (!((securityPage && securityPage->cbGroups) || comboBox))
        return;

    pgSet *set=connection->ExecuteSet(wxT("SELECT groname FROM pg_group ORDER BY groname"));

    if (set)
    {
        while (!set->Eof())
        {
            if (securityPage && securityPage->cbGroups)
                securityPage->cbGroups->Append(wxT("group ") + set->GetVal(0));
            if (comboBox)
                comboBox->Append(set->GetVal(0));
            set->MoveNext();
        }
        delete set;
    }
}


void dlgSecurityProperty::AddUsers(ctlComboBox *combobox)
{
    if (securityPage && securityPage->cbGroups && settings->GetShowUsersForPrivileges())
    {
        securityPage->stGroup->SetLabel(_("Group/User"));
        dlgProperty::AddUsers(securityPage->cbGroups, combobox);
    }
    else
        dlgProperty::AddUsers(combobox);
}


void dlgSecurityProperty::OnAddPriv(wxCommandEvent &ev)
{
    securityChanged=true;
    EnableOK(btnOK->IsEnabled());
}


void dlgSecurityProperty::OnDelPriv(wxCommandEvent &ev)
{
    securityChanged=true;
    EnableOK(btnOK->IsEnabled());
}


wxString dlgSecurityProperty::GetHelpPage() const
{
    if (nbNotebook->GetSelection() == (int)nbNotebook->GetPageCount()-2)
        return wxT("sql-grant");
    else
        return dlgProperty::GetHelpPage();
}


void dlgSecurityProperty::EnableOK(bool enable)
{
    if (securityChanged)
    {
        wxString sql=GetSql();
        if (sql.IsEmpty())
        {
            enable=false;
            securityChanged=false;
        }
        else
            enable=true;
    }
    dlgProperty::EnableOK(enable);
}




wxString dlgSecurityProperty::GetGrant(const wxString &allPattern, const wxString &grantObject)
{
    if (securityPage)
        return securityPage->GetGrant(allPattern, grantObject, &currentAcl);
    else
	return wxString();
}




/////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(dlgOidProperty, dlgProperty)
    EVT_BUTTON (wxID_OK,                            dlgOidProperty::OnOK)
END_EVENT_TABLE();

dlgOidProperty::dlgOidProperty(frmMain *frame, const wxString &resName)
: dlgProperty(frame, resName)
{
    oid=0;
}


wxString dlgOidProperty::GetSql()
{
    wxString str=GetInsertSql();
    if (!str.IsEmpty())
        str += wxT("\n\n");
    return str + GetUpdateSql();
}



bool dlgOidProperty::executeSql()
{
    wxString sql;
    bool dataChanged=false;

    sql=GetInsertSql();
    if (!sql.IsEmpty())
    {
        pgSet *set=connection->ExecuteSet(sql);
        if (set)
        {
            oid = set->GetInsertedOid();
            delete set;
        }
        if (!set || !oid)
        {
            return false;
        }
        dataChanged=true;
    }

    sql=GetUpdateSql();
    if (!sql.IsEmpty())
    {
        int pos;
        while ((pos=sql.Find(wxT("<Oid>"))) >= 0)
            sql = sql.Left(pos) + NumToStr(oid) + wxT("::oid") + sql.Mid(pos+5);

        if (!connection->ExecuteVoid(sql))
        {
            // error message is displayed inside ExecuteVoid
            return false;
        }
        dataChanged=true;
    }

    return dataChanged;
}


void dlgOidProperty::OnOK(wxCommandEvent &ev)
{
    if (IsModal())
    {
        EndModal(0);
        return;
    }

    

    connection->ExecuteVoid(wxT("BEGIN TRANSACTION"));

    if (executeSql())
    {
        connection->ExecuteVoid(wxT("COMMIT TRANSACTION"));
        ShowObject();
    }
    else
    {
        connection->ExecuteVoid(wxT("ROLLBACK TRANSACTION"));
    }

    Destroy();
}
