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

// App headers
#include "pgAdmin3.h"
#include "ctlSQLBox.h"

#include "dlgProperty.h"
#include "dlgUser.h"
#include "pgCollection.h"


#define CTL_PROPSQL 295

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


    sqlPane = new ctlSQLBox(notebook, CTL_PROPSQL, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSIMPLE_BORDER | wxTE_READONLY | wxTE_RICH2);
//    sqlPane->Show();

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
                newData->ShowTree(mainForm, browser, properties, statistics, sqlPane);
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

        dlg->Go();
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
        dlg->Go();
    }
    else
        wxMessageBox("Not implemented");
}
