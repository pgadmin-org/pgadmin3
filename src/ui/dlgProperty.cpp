//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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

// Images
#include "images/properties.xpm"

#include "frmMain.h"
#include "dlgProperty.h"
#include "dlgUser.h"
#include "dlgDatabase.h"
#include "dlgTable.h"
#include "dlgColumn.h"
#include "dlgIndex.h"
#include "dlgIndexConstraint.h"
#include "dlgForeignKey.h"
#include "dlgCheck.h"
#include "dlgSchema.h"

#include "pgTable.h"
#include "pgColumn.h"

enum
{
    CTL_PROPSQL=250,
    CTL_LBPRIV,
    CTL_STATICGROUP,
    CTL_CBGROUP,
    CTL_ADDPRIV,
    CTL_DELPRIV,
    CTL_ALLPRIV,
    CTL_ALLPRIVGRANT,
    CTL_PRIVCB          // base for all privilege checkboxes, must be last
};


BEGIN_EVENT_TABLE(dlgProperty, wxDialog)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),  dlgProperty::OnPageSelect)  
    EVT_BUTTON (XRCID("btnOK"),                     dlgProperty::OnOK)
    EVT_BUTTON (XRCID("btnCancel"),                 dlgProperty::OnCancel)
    EVT_CLOSE(                                      dlgProperty::OnClose)
END_EVENT_TABLE();


dlgProperty::dlgProperty(frmMain *frame, const wxString &resName) : wxDialog()
{
    objectType=PG_NONE;
    sqlPane=0;
    sqlPageNo=-1;
    mainForm = frame;
    wxXmlResource::Get()->LoadDialog(this, frame, resName);
    if (!nbNotebook)
    {
        wxMessageBox(wxT("Problem with resource ") + resName + wxT(": Notebook not found.\nPrepare to crash!"));
        return;
    }
    SetIcon(wxIcon(properties_xpm));

#ifdef __WIN32__
    wxNotebookPage *page=nbNotebook->GetPage(0);
    wxASSERT(page != NULL);
    page->GetClientSize(&width, &height);
#else
    nbNotebook->GetClientSize(&width, &height);
	height -= 35;   // sizes of tabs
#endif

    numericValidator.SetStyle(wxFILTER_NUMERIC);
}


int dlgProperty::Go(bool modal)
{
    if (GetObject())
        SetTitle(wxString("") + typeNameList[objectType] + wxT(" ") + GetObject()->GetFullIdentifier());
    else
        SetTitle(wxString("Creating new ") + typeNameList[objectType]);

    if (modal)
        return ShowModal();
    else
        Show();
    return 0;
}

void dlgProperty::CreateAdditionalPages()
{
    sqlPane = new ctlSQLBox(nbNotebook, CTL_PROPSQL, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_READONLY | wxTE_RICH2);
    sqlPageNo=nbNotebook->GetPageCount();
    nbNotebook->AddPage(sqlPane, wxT("SQL"));
}


wxString dlgProperty::GetName()
{
    return txtName->GetValue();
}


void dlgProperty::AppendComment(wxString &sql, const wxString &objName, pgObject *obj)
{
    wxString comment=txtComment->GetValue();
    if ((!obj && !comment.IsEmpty()) ||(obj && obj->GetComment() != comment))
        sql += wxT("COMMENT ON ") + objName + wxT(" ") + qtIdent(txtName->GetValue())
            +  wxT(" IS ") + qtString(comment) + wxT(";\n");
}


void dlgProperty::OnClose(wxCloseEvent &ev)
{
    if (IsModal())
        EndModal(-1);
    else
        Destroy();
}


void dlgProperty::OnOK(wxNotifyEvent &ev)
{

    if (IsModal())
    {
        EndModal(0);
        return;
    }

    wxString sql=GetSql();
    if (!sql.IsEmpty())
    {
        if (!connection->ExecuteVoid(sql))
        {
            // error message is displayed inside ExecuteVoid
            return;
        }

        pgObject *data=GetObject();
        if (data)
        {
            pgObject *newData=data->Refresh(mainForm->GetBrowser(), item);
            if (newData && newData != data)
            {
                mainForm->GetBrowser()->SetItemData(item, newData);
                newData->SetId(item);
                delete data;
            }
            if (newData)
            {
                newData->ShowTree(mainForm, mainForm->GetBrowser(), properties, statistics, 0);
                mainForm->GetSqlPane()->SetText(newData->GetSql(mainForm->GetBrowser()));
            }
        }
        else
        {
            pgCollection *collection=0;

            wxTreeItemId collectionItem=mainForm->GetBrowser()->GetItemParent(item);

            while (collectionItem)
            {
                collection = (pgCollection*)mainForm->GetBrowser()->GetItemData(collectionItem);
                if (collection && collection->IsCollectionForType(objectType))
                {
                    data = CreateObject(collection);
                    if (data)
                    {
                        mainForm->GetBrowser()->AppendItem(collectionItem, data->GetFullIdentifier(), data->GetIcon(), -1, data);

                        if (mainForm->GetBrowser()->GetSelection() == collectionItem)
                            collection->ShowTreeDetail(mainForm->GetBrowser(), 0, properties);
                        else
                            collection->UpdateChildCount(mainForm->GetBrowser());
                    }
                    break;
                }
                collectionItem=mainForm->GetBrowser()->GetItemParent(collectionItem);
            }
        }
    }

    Destroy();
}


void dlgProperty::OnCancel(wxNotifyEvent &ev)
{
    if (IsModal())
        EndModal(-1);
    else
        Destroy();
}



void dlgProperty::OnPageSelect(wxNotebookEvent& event)
{
    if (sqlPane && event.GetSelection() == sqlPageNo)
    {
        if (btnOK->IsEnabled())
            sqlPane->SetText(GetSql());
        else
        {
            if (GetObject())
                sqlPane->SetText(wxT("-- nothing to change"));
            else
                sqlPane->SetText(wxT("-- definition incomplete"));
        }
    }
}



dlgProperty *dlgProperty::CreateDlg(frmMain *frame, pgObject *node, bool asNew)
{
    dlgProperty *dlg=0;
    pgObject *currentNode, *parentNode=0;
    if (asNew)
        currentNode=NULL;
    else
        currentNode=node;
    parentNode = (pgObject*)frame->GetBrowser()->GetItemData(
                            frame->GetBrowser()->GetItemParent(
                                node->GetId()));

    if (parentNode && parentNode->IsCollection())
        parentNode = (pgObject*)frame->GetBrowser()->GetItemData(
                                frame->GetBrowser()->GetItemParent(
                                    parentNode->GetId()));


    switch (node->GetType())
    {
        case PG_USER:
        case PG_USERS:
            dlg=new dlgUser(frame, (pgUser*)currentNode);
            break;
        case PG_DATABASE:
        case PG_DATABASES:
            dlg=new dlgDatabase(frame, (pgDatabase*)currentNode);
            break;
        case PG_SCHEMA:
        case PG_SCHEMAS:
            dlg=new dlgSchema(frame, (pgSchema*)currentNode);
            break;
        case PG_TABLE:
        case PG_TABLES:
            dlg=new dlgTable(frame, (pgTable*)currentNode);
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
            
        default:
            break;
    }

    if (dlg)
    {
        dlg->CenterOnParent();
    }
    return dlg;
}


void dlgProperty::CreateObjectDialog(frmMain *frame, wxListCtrl *properties, pgObject *node, pgConn *conn)
{
    dlgProperty *dlg=CreateDlg(frame, node, true);
    pgObject *obj=0;

    if (dlg)
    {
        dlg->connection=conn;
        dlg->properties = properties;
        dlg->item=node->GetId();
        dlg->objectType=node->GetType();
        if (node->IsCollection())
            dlg->objectType++;

        dlg->SetTitle(wxString("Creating new ") + typeNameList[dlg->objectType]);

        dlg->CreateAdditionalPages();
        dlg->Go();
    }
    else
        wxMessageBox("Not implemented");
}


void dlgProperty::EditObjectDialog(frmMain *frame, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlbox, pgObject *node, pgConn *conn)
{
    dlgProperty *dlg=CreateDlg(frame, node, false);
    pgObject *obj=0;

    if (dlg)
    {
        dlg->connection=conn;
        dlg->properties = properties;
        dlg->statistics=statistics;
        dlg->sqlFormPane=sqlbox;
        dlg->item=node->GetId();
        dlg->objectType=node->GetType();
        if (node->IsCollection())
            dlg->objectType++;

        dlg->SetTitle(wxString("") + typeNameList[dlg->objectType] + wxT(" ") + node->GetFullIdentifier());

        dlg->CreateAdditionalPages();
        dlg->Go();
    }
    else
        wxMessageBox("Not implemented");
}


void dlgProperty::CreateListColumns(wxListCtrl *list, const wxString &left, const wxString &right, int leftSize)
{
    int rightSize;
    if (leftSize < 0)
    {
        leftSize = rightSize = width/2-10;
    }
    else
    {
        rightSize = width-leftSize-20;
    }
    if (!leftSize)
    {
        list->InsertColumn(0, left, wxLIST_FORMAT_LEFT, width);
    }
    else
    {
        list->InsertColumn(0, left, wxLIST_FORMAT_LEFT, leftSize);
        list->InsertColumn(1, right, wxLIST_FORMAT_LEFT, rightSize);
    }
    list->SetImageList(mainForm->GetImageList(), wxIMAGE_LIST_SMALL);
}


void dlgProperty::AppendListItem(wxListCtrl *list, const wxString& str1, const wxString& str2, int icon)
{
    int pos=list->GetItemCount();
    list->InsertItem(pos, str1, icon);
    if (str2 != wxT(""))
        list->SetItem(pos, 1, str2);
}



dlgCollistProperty::dlgCollistProperty(frmMain *frame, const wxString &resName, pgTable *parentNode)
: dlgProperty(frame, resName)
{
    columns=0;
    table=parentNode;
}


dlgCollistProperty::dlgCollistProperty(frmMain *frame, const wxString &resName, wxListCtrl *colList)
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
            cbColumns->Append(columns->GetItemText(pos));
    }
    if (table)
    {
        long cookie;
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
            long cookie;
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


BEGIN_EVENT_TABLE(dlgSecurityProperty, dlgProperty)
    EVT_LIST_ITEM_SELECTED(CTL_LBPRIV,  dlgSecurityProperty::OnPrivSelChange)
    EVT_BUTTON(CTL_ADDPRIV,             dlgSecurityProperty::OnAddPriv)
    EVT_BUTTON(CTL_DELPRIV,             dlgSecurityProperty::OnDelPriv)
    EVT_CHECKBOX(CTL_ALLPRIV,           dlgSecurityProperty::OnPrivCheckAll)
    EVT_CHECKBOX(CTL_ALLPRIVGRANT,      dlgSecurityProperty::OnPrivCheckAllGrant)
    EVT_CHECKBOX(CTL_PRIVCB,            dlgSecurityProperty::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+2,          dlgSecurityProperty::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+4,          dlgSecurityProperty::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+6,          dlgSecurityProperty::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+8,          dlgSecurityProperty::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+10,         dlgSecurityProperty::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+12,         dlgSecurityProperty::OnPrivCheck)
END_EVENT_TABLE();



dlgSecurityProperty::dlgSecurityProperty(frmMain *frame, pgObject *obj, const wxString &resName, const wxString& privilegeList, char *_privChar)
        : dlgProperty(frame, resName)
{
    privCheckboxes=0;
    privilegeChars = _privChar;
    securityChanged=false;

    wxStringTokenizer privileges(privilegeList, ',');
    privilegeCount=privileges.CountTokens();

    if (privilegeCount)
    {
        wxWindow *page = new wxWindow(nbNotebook, -1, wxDefaultPosition, wxDefaultSize);
        privCheckboxes = new wxCheckBox*[privilegeCount*2];
        int i=0;

        nbNotebook->AddPage(page, wxT("Security"));

        lbPrivileges = new wxListView(page, CTL_LBPRIV, wxPoint(10,10), wxSize(width-20, height-120-20*privilegeCount));
        CreateListColumns(lbPrivileges, wxT("User/Group"), wxT("Privileges"), -1);
        int y=height-105-20*privilegeCount;

        btnAddPriv = new wxButton(page, CTL_ADDPRIV, wxT("Add/Change"), wxPoint(10, y), wxSize(75, 25));
        btnDelPriv = new wxButton(page, CTL_DELPRIV, wxT("Remove"), wxPoint(95, y), wxSize(75, 25));
        y += 35;

        new wxStaticBox(page, -1, wxT("Privileges"), wxPoint(10, y), wxSize(width-20, 65+20*privilegeCount));
        y += 15;

        stGroup = new wxStaticText(page, CTL_STATICGROUP, wxT("Group"), wxPoint(20, y+3), wxSize(100, 20));
        cbGroups = new wxComboBox(page, CTL_CBGROUP, wxT(""), wxPoint(130, y), wxSize(width-145, 100), 0, 0, wxCB_DROPDOWN|wxCB_READONLY);
        y += 25;

        allPrivileges = new wxCheckBox(page, CTL_ALLPRIV, wxT("ALL"), wxPoint(20, y), wxSize(100, 20));
        allPrivilegesGrant = new wxCheckBox(page, CTL_ALLPRIVGRANT, wxT("WITH GRANT OPTION"), wxPoint(130, y), wxSize(width-145, 20));
        y += 20;
        allPrivilegesGrant->Disable();

        cbGroups->Append(wxT("public"));
        cbGroups->SetSelection(0);

        while (privileges.HasMoreTokens())
        {
            wxString priv=privileges.GetNextToken();
            wxCheckBox *cb;
            cb=new wxCheckBox(page, CTL_PRIVCB+i, priv, wxPoint(20, y), wxSize(100, 20));
            privCheckboxes[i++] = cb;
            cb=new wxCheckBox(page, CTL_PRIVCB+i, wxT("WITH GRANT OPTION"), wxPoint(130, y), wxSize(width-145, 20));
            cb->Disable();
            privCheckboxes[i++] = cb;

            y += 20;
        }

        if (obj)
        {
            wxString str=obj->GetAcl();
            if (!str.IsEmpty())
            {
                str = str.Mid(1, str.Length()-2);
                wxStringTokenizer tokens(str, ',');

                while (tokens.HasMoreTokens())
                {
                    wxString str=tokens.GetNextToken().BeforeLast('/');
                    if (str[0U]== '"')
                        str = str.Mid(1);

                    wxString name=str.BeforeLast('=');
                    wxString value=str.Mid(name.Length()+1);

                    int icon=PGICON_USER;

                    if (name.Left(6).IsSameAs(wxT("group "), false))
                    {
                        icon = PGICON_GROUP;
                        name = wxT("group ") + name.Mid(6);
                    }
                    else if (name.IsEmpty())
                    {
                        icon = PGICON_PUBLIC;
                        name=wxT("public");
                    }

                    AppendListItem(lbPrivileges, name, value, icon);
                    currentAcl.Add(name + wxT("=") + value);
                }
            }
        }
    }
}


dlgSecurityProperty::~dlgSecurityProperty()
{
    if (privCheckboxes)
        delete[] privCheckboxes;
}



void dlgSecurityProperty::AddGroups(wxComboBox *comboBox)
{
    pgSet *set=connection->ExecuteSet(wxT("SELECT groname FROM pg_group ORDER BY groname"));

    if (set)
    {
        while (!set->Eof())
        {
            cbGroups->Append(wxT("group ") + set->GetVal(0));
            if (comboBox)
                comboBox->Append(set->GetVal(0));
            set->MoveNext();
        }
        delete set;
    }
}


void dlgSecurityProperty::AddUsers(wxComboBox *comboBox)
{
    if (!settings->GetShowUsersForPrivileges() && !comboBox)
        return;

    pgSet *set=connection->ExecuteSet(wxT("SELECT usename FROM pg_user ORDER BY usename"));

    if (set)
    {
        if (settings->GetShowUsersForPrivileges())
            stGroup->SetLabel(wxT("Group/User"));

        while (!set->Eof())
        {
            if (settings->GetShowUsersForPrivileges())
                cbGroups->Append(set->GetVal(0));

            if (comboBox)
                comboBox->Append(set->GetVal(0));

            set->MoveNext();
        }
        delete set;
    }
}



void dlgSecurityProperty::OnPrivCheckAll(wxCommandEvent& ev)
{
    bool all=allPrivileges->GetValue();
    int i;
    for (i=0 ; i < privilegeCount ; i++)
    {
        if (all)
        {
            privCheckboxes[i*2]->SetValue(true);
            privCheckboxes[i*2]->Disable();
            privCheckboxes[i*2+1]->Disable();
            allPrivilegesGrant->Enable(GrantAllowed());
        }
        else
        {
            allPrivilegesGrant->Disable();
            allPrivilegesGrant->SetValue(false);
            privCheckboxes[i*2]->Enable();
            ExecPrivCheck(i);
        }
    }
}


void dlgSecurityProperty::OnPrivSelChange(wxListEvent &ev)
{
    allPrivileges->SetValue(false);
    allPrivilegesGrant->SetValue(false);
    allPrivilegesGrant->Disable();

    long pos=lbPrivileges->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (pos >= 0)
    {
        wxString name=lbPrivileges->GetItemText(pos);
        wxString value=GetListText(lbPrivileges, pos, 1);

        pos=cbGroups->FindString(name);
        if (pos < 0)
        {
            cbGroups->Append(name);
            pos=cbGroups->GetCount()-1;
        }
        cbGroups->SetSelection(pos);

        int i;
        for (i=0 ; i < privilegeCount ; i++)
        {
            privCheckboxes[i*2]->Enable();
            int index=value.Find(privilegeChars[i]);
            if (index >= 0)
            {
                privCheckboxes[i*2]->SetValue(true);
                privCheckboxes[i*2+1]->SetValue(value.Mid(index+1, 1) == wxT("*"));
            }
            else
                privCheckboxes[i*2]->SetValue(false);
            ExecPrivCheck(i);
        }
    }
}


void dlgSecurityProperty::OnPrivCheckAllGrant(wxCommandEvent& ev)
{
    bool grant=allPrivilegesGrant->GetValue();
    int i;
    for (i=0 ; i < privilegeCount ; i++)
        privCheckboxes[i*2+1]->SetValue(grant);
}


void dlgSecurityProperty::OnPrivCheck(wxCommandEvent& ev)
{
    int id=(ev.GetId()-CTL_PRIVCB) /2;
    ExecPrivCheck(id);
}


void dlgSecurityProperty::ExecPrivCheck(int id)
{
    bool canGrant=(GrantAllowed() && privCheckboxes[id*2]->GetValue());
    if (canGrant)
        privCheckboxes[id*2+1]->Enable();
    else
    {
        privCheckboxes[id*2+1]->SetValue(false);
        privCheckboxes[id*2+1]->Disable();
    }
}


void dlgSecurityProperty::OnAddPriv(wxNotifyEvent &ev)
{
    wxString name=cbGroups->GetValue();

    long pos=lbPrivileges->FindItem(-1, name);
    if (pos < 0)
    {
        pos = lbPrivileges->GetItemCount();
        int icon=PGICON_USER;

        if (name.Left(6).IsSameAs(wxT("group "), false))
            icon = PGICON_GROUP;
        else if (name.IsSameAs(wxT("public"), false))
            icon = PGICON_PUBLIC;

        lbPrivileges->InsertItem(pos, name, icon);
    }
    wxString value;
    int i;
    for (i=0 ; i < privilegeCount ; i++)
    {
        if (privCheckboxes[i*2]->GetValue())
        {
            value += privilegeChars[i];
            if (privCheckboxes[i*2+1]->GetValue())
                value += '*';
        }
    }
    lbPrivileges->SetItem(pos, 1, value);
    securityChanged=true;
    EnableOK(btnOK->IsEnabled());
}


void dlgSecurityProperty::OnDelPriv(wxNotifyEvent &ev)
{
    lbPrivileges->DeleteItem(GetListSelected(lbPrivileges));
    securityChanged=true;
    EnableOK(btnOK->IsEnabled());
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
    btnOK->Enable(enable);
}


bool dlgSecurityProperty::GrantAllowed() const
{
    if (connection->GetVersionNumber() < 7.4)
        return false;

    wxString user=cbGroups->GetValue();
    if (user.Left(6).IsSameAs(wxT("group "), false) || user.IsSameAs(wxT("public"), false))
        return false;

    return true;
}


wxString dlgSecurityProperty::GetGrant(const wxString &allPattern, const wxString &grantObject)
{
    wxString sql;

    wxArrayString tmpAcl=currentAcl;

    int cnt=lbPrivileges->GetItemCount();
    int pos;
    unsigned int i;

    for (pos=0 ; pos < cnt ; pos++)
    {
        wxString name=lbPrivileges->GetItemText(pos);
        wxString value=GetListText(lbPrivileges, pos, 1);

        int nameLen=name.Length();

        bool privWasAssigned=false;
        for (i=0 ; i < tmpAcl.GetCount() ; i++)
        {
            if (tmpAcl.Item(i).Left(nameLen) == name)
            {
                if (tmpAcl.Item(i).Mid(nameLen+1) == value)
                    privWasAssigned=true;
                tmpAcl.RemoveAt(i);
                break;
            }
        }
        if (!privWasAssigned)
        {
            if (name.Left(6).IsSameAs("group ", false))
                name = wxT("GROUP ") + qtIdent(name.Mid(6));
            else
                name=qtIdent(name);
            sql += pgObject::GetPrivileges(allPattern, value, grantObject, name);
        }
    }

    for (i=0 ; i < tmpAcl.GetCount() ; i++)
    {
        wxString name=tmpAcl.Item(i).BeforeLast('=');

        if (name.Left(6).IsSameAs("group ", false))
            name = wxT("GROUP ") + qtIdent(name.Mid(6));
        else
            name=qtIdent(name);
        sql += pgObject::GetPrivileges(allPattern, wxT(""), grantObject, name);
    }
    return sql;
}
