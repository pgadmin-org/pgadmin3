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

#include "frmMain.h"
#include "dlgProperty.h"
#include "dlgUser.h"
#include "dlgDatabase.h"

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
END_EVENT_TABLE();


dlgProperty::dlgProperty(wxFrame *frame, const wxString &resName) : wxDialog()
{
    notebook=0;

    wxXmlResource::Get()->LoadDialog(this, frame, resName);
    notebook=CTRL("nbNotebook", wxNotebook);
    if (!notebook)
    {
        wxMessageBox(wxT("Problem with resource ") + resName + wxT(": Notebook not found.\nPrepare to crash!"));
        return;
    }
}


void dlgProperty::CreateAdditionalPages()
{
    sqlPane = new ctlSQLBox(notebook, CTL_PROPSQL, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_READONLY | wxTE_RICH2);
    sqlPageNo=notebook->GetPageCount();
    notebook->AddPage(sqlPane, wxT("SQL"));
}


void dlgProperty::OnOK(wxNotifyEvent &ev)
{
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
            pgObject *newData=data->Refresh(browser, item);
            if (newData && newData != data)
            {
                browser->SetItemData(item, newData);
                newData->SetId(item);
                delete data;
            }
            if (newData)
            {
                newData->ShowTree(mainForm, browser, properties, statistics, 0);
                mainForm->sqlPane->SetText(newData->GetSql(browser));
            }
        }
        else
        {
            int collectionType=objectType -1;

            pgCollection *collection=0;

            while (item)
            {
                collection = (pgCollection*)browser->GetItemData(item);
                if (collection && collection->GetType() == collectionType)
                {
                    data = CreateObject(collection);
                    if (data)
                    {
                        browser->AppendItem(item, data->GetFullIdentifier(), data->GetIcon(), -1, data);

                        if (browser->GetSelection() == item)
                            collection->ShowTreeDetail(browser, 0, properties);
                        else
                            collection->UpdateChildCount(browser);
                    }
                    break;
                }
                item=browser->GetItemParent(item);
            }
        }
    }
    Destroy();
}


void dlgProperty::OnCancel(wxNotifyEvent &ev)
{
    Destroy();
}



void dlgProperty::OnPageSelect(wxNotebookEvent& event)
{
    if (notebook && event.GetSelection() == sqlPageNo)
    {
        sqlPane->SetText(GetSql());
    }
}



dlgProperty *dlgProperty::CreateDlg(wxFrame *frame, pgObject *node, bool asNew)
{
    dlgProperty *dlg=0;
    pgObject *currentNode;
    if (asNew)
        currentNode=NULL;
    else
        currentNode=node;
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
        default:
            break;
    }

    if (dlg)
    {
        dlg->CenterOnParent();
    }
    return dlg;
}


void dlgProperty::CreateObjectDialog(frmMain *frame, wxTreeCtrl *browser, wxListCtrl *properties, pgObject *node, pgConn *conn)
{
    dlgProperty *dlg=CreateDlg((wxFrame*)frame, node, true);
    pgObject *obj=0;

    if (dlg)
    {
        dlg->connection=conn;
        dlg->browser=browser;
        dlg->properties = properties;
        dlg->item=node->GetId();
        dlg->objectType=node->GetType();
        if (node->IsCollection())
            dlg->objectType++;

        dlg->SetTitle(wxString("pgAdmin III - Creating new ") + typeNameList[dlg->objectType]);

        dlg->CreateAdditionalPages();
        dlg->Go();
        dlg->Show();
    }
    else
        wxMessageBox("Not implemented");
}


void dlgProperty::EditObjectDialog(frmMain *frame, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlbox, pgObject *node, pgConn *conn)
{
    dlgProperty *dlg=CreateDlg((wxFrame*)frame, node, false);
    pgObject *obj=0;

    if (dlg)
    {
        dlg->mainForm=frame;
        dlg->connection=conn;
        dlg->browser=browser;
        dlg->properties = properties;
        dlg->statistics=statistics;
        dlg->sqlFormPane=sqlbox;
        dlg->item=node->GetId();
        dlg->objectType=node->GetType();
        if (node->IsCollection())
            dlg->objectType++;

        dlg->SetTitle(wxString("pgAdmin III - Properties of ") + typeNameList[dlg->objectType] + wxT(" ") + node->GetFullIdentifier());

        dlg->CreateAdditionalPages();
        dlg->Go();
        dlg->Show();
    }
    else
        wxMessageBox("Not implemented");
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



dlgSecurityProperty::dlgSecurityProperty(wxFrame *frame, pgObject *obj, const wxString &resName, const wxString& privilegeList, char *_privChar)
        : dlgProperty(frame, resName)
{
    privCheckboxes=0;
    privilegeChars = _privChar;

    wxStringTokenizer privileges(privilegeList, ',');
    privilegeCount=privileges.CountTokens();

    if (privilegeCount)
    {
        wxWindow *page = new wxWindow(notebook, -1, wxDefaultPosition, wxDefaultSize);
        privCheckboxes = new wxCheckBox*[privilegeCount*2];
        int i=0;

        notebook->AddPage(page, wxT("Security"));
        int width, height;
        page->GetClientSize(&width, &height);

        lbPrivileges = new wxListView(page, CTL_LBPRIV, wxPoint(10,10), wxSize(width-20, height-120-20*privilegeCount));
        lbPrivileges->InsertColumn(0, wxT("User/Group"), wxLIST_FORMAT_LEFT, width/2-10);
        lbPrivileges->InsertColumn(1, wxT("Privileges"), wxLIST_FORMAT_LEFT, width/2-10);
        int y=height-105-20*privilegeCount;

        btnAddPriv = new wxButton(page, CTL_ADDPRIV, wxT("Add/Change"), wxPoint(10, y), wxSize(75, 25));
        btnDelPriv = new wxButton(page, CTL_DELPRIV, wxT("Remove"), wxPoint(95, y), wxSize(75, 25));
        y += 35;

        new wxStaticBox(page, -1, wxT("Privileges"), wxPoint(10, y), wxSize(width-20, 65+20*privilegeCount));
        y += 15;

        stGroup = new wxStaticText(page, CTL_STATICGROUP, wxT("Group"), wxPoint(20, y+3), wxSize(width/2-20, 20));
        cbGroups = new wxComboBox(page, CTL_CBGROUP, wxT(""), wxPoint(width/2, y), wxSize(width/2-20, 100));
        y += 25;

        allPrivileges = new wxCheckBox(page, CTL_ALLPRIV, wxT("ALL"), wxPoint(20, y), wxSize(width/2-20, 20));
        allPrivilegesGrant = new wxCheckBox(page, CTL_ALLPRIVGRANT, wxT("WITH GRANT"), wxPoint(width/2, y), wxSize(width/2-20, 20));
        y += 20;
        allPrivilegesGrant->Disable();

        cbGroups->Append(wxT("public"));
        cbGroups->SetSelection(0);

        while (privileges.HasMoreTokens())
        {
            wxString priv=privileges.GetNextToken();
            wxCheckBox *cb;
            cb=new wxCheckBox(page, CTL_PRIVCB+i, priv, wxPoint(20, y), wxSize(width/2-20, 20));
            privCheckboxes[i++] = cb;
            cb=new wxCheckBox(page, CTL_PRIVCB+i, wxT("WITH GRANT"), wxPoint(width/2, y), wxSize(width/2-20, 20));
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
                int pos=0;
                while (tokens.HasMoreTokens())
                {
                    wxString str=tokens.GetNextToken().BeforeLast('/');
                    if (str[0]== '"')
                        str = str.Mid(1);

                    wxString name=str.BeforeLast('=');
                    wxString value=str.Mid(name.Length()+1);

                    if (name.Left(6).IsSameAs(wxT("group "), false))
                        name = wxT("group ") + name.Mid(6);
                    else if (name.IsEmpty())
                        name=wxT("public");

                    lbPrivileges->InsertItem(pos, name, 0);
                    lbPrivileges->SetItem(pos, 1, value);
                    currentAcl.Add(name + wxT("=") + value);
                    pos++;
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


        wxListItem item;
        item.SetId(pos);
        item.SetColumn(1);
        item.SetMask(wxLIST_MASK_TEXT);
        lbPrivileges->GetItem(item);
        

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
            int index=item.GetText().Find(privilegeChars[i]);
            if (index >= 0)
            {
                privCheckboxes[i*2]->SetValue(true);
                privCheckboxes[i*2+1]->SetValue(item.GetText().Mid(index+1, 1) == wxT("*"));
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
        lbPrivileges->InsertItem(pos, name, 0);
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
}


void dlgSecurityProperty::OnDelPriv(wxNotifyEvent &ev)
{
    lbPrivileges->DeleteItem(lbPrivileges->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED));
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
        int nameLen=name.Length();

        wxListItem item;
        item.SetId(pos);
        item.SetColumn(1);
        item.SetMask(wxLIST_MASK_TEXT);
        lbPrivileges->GetItem(item);

        bool privWasAssigned=false;
        for (i=0 ; i < tmpAcl.GetCount() ; i++)
        {
            if (tmpAcl.Item(i).Left(nameLen) == name)
            {
                if (tmpAcl.Item(i).Mid(nameLen+1) == item.GetText())
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
            sql += pgObject::GetPrivileges(allPattern, item.GetText(), grantObject, name);
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
