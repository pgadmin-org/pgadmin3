//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmGrantWizard.cpp - Grant Wizard dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frmGrantWizard.h"
#include "sysLogger.h"
#include "pgFunction.h"
#include "ctlSecurityPanel.h"

// Icons
#include "images/index.xpm"


#define chkList     CTRL_CHECKLISTBOX("chkList")



BEGIN_EVENT_TABLE(frmGrantWizard, ExecutionDialog)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),  frmGrantWizard::OnPageSelect) 
    EVT_BUTTON(XRCID("btnChkAll"), frmGrantWizard::OnCheckAll)
    EVT_BUTTON(XRCID("btnUnchkAll"), frmGrantWizard::OnUncheckAll)
END_EVENT_TABLE()


frmGrantWizard::frmGrantWizard(frmMain *form, pgObject *obj) : ExecutionDialog(form, obj)
{
    wxLogInfo(wxT("Creating a grant wizard dialogue for %s %s"), object->GetTypeName().c_str(), object->GetFullName().c_str());

    nbNotebook = 0;

    wxWindowBase::SetFont(settings->GetSystemFont());
    wxXmlResource::Get()->LoadDialog(this, form, wxT("frmGrantWizard"));
    SetTitle(wxString::Format(_("Privileges for %s %s"), object->GetTypeName().c_str(), object->GetFullIdentifier().c_str()));

    // Icon
    SetIcon(wxIcon(index_xpm));
    nbNotebook = CTRL_NOTEBOOK("nbNotebook");
    sqlPane = 0;

    CenterOnParent();
}


frmGrantWizard::~frmGrantWizard()
{
    wxLogInfo(wxT("Destroying a grant wizard dialogue"));
    Abort();
}


wxString frmGrantWizard::GetHelpPage() const
{
    wxString page=wxT("sql-grant");

    return page;
}



void frmGrantWizard::OnUncheckAll(wxCommandEvent& event)
{
    int i;
    for (i=0 ; i < chkList->GetCount() ; i++)
        chkList->Check(i, false);
}



void frmGrantWizard::OnCheckAll(wxCommandEvent& event)
{
    int i;
    for (i=0 ; i < chkList->GetCount() ; i++)
        chkList->Check(i, true);
}


void frmGrantWizard::OnPageSelect(wxNotebookEvent& event)
{
    if (nbNotebook && sqlPane && event.GetSelection() == (int)nbNotebook->GetPageCount()-2)
    {
        sqlPane->SetReadOnly(false);
        sqlPane->SetText(GetSql());
        sqlPane->SetReadOnly(true);
    }
}


void frmGrantWizard::AddObjects(pgCollection *collection)
{
    bool traverseKids = (!collection->IsCollection() || collection->GetType() == PG_SCHEMAS);

    if (!traverseKids)
    {
        if (!collection->IsCollectionForType(PG_TABLE) &&
            !collection->IsCollectionForType(PG_FUNCTION) &&
            !collection->IsCollectionForType(PG_TRIGGERFUNCTION) &&
            !collection->IsCollectionForType(PG_VIEW) &&
            !collection->IsCollectionForType(PG_SEQUENCE))
            return;
    }
    wxCookieType cookie;
    wxTreeItemId item=mainForm->GetBrowser()->GetFirstChild(collection->GetId(), cookie);

    while (item)
    {
        pgObject *obj=(pgObject*)mainForm->GetBrowser()->GetItemData(item);
        if (obj)
        {
            if (traverseKids)
                AddObjects((pgCollection*)obj);
            else
            {
                objectArray.Add(obj);
                chkList->Append(obj->GetTypeName() + wxT(" ") + obj->GetFullIdentifier());
            }
        }
        item=mainForm->GetBrowser()->GetNextChild(collection->GetId(), cookie);
    }

}

void frmGrantWizard::Go()
{
    chkList->SetFocus();

    wxString privList = wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCES,TRIGGER");
    char *privChar="arwdRxt";
    switch (object->GetType())
    {
        case PG_DATABASE:
        case PG_SCHEMAS:
        case PG_SCHEMA:
            privList.Append(wxT(",EXECUTE"));
            privChar = "arwdRxtX";
            break;
        case PG_FUNCTIONS:
        case PG_TRIGGERFUNCTIONS:
            privList = wxT(",EXECUTE");
            privChar = "X";
            break;
        default:
            break;
    }


    securityPage = new ctlSecurityPanel(nbNotebook, privList, privChar, mainForm->GetImageList());
    securityPage->SetConnection(object->GetConnection());

    sqlPane = new ctlSQLBox(nbNotebook, CTL_PROPSQL, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY | wxTE_RICH2);
    nbNotebook->AddPage(sqlPane, wxT("SQL"));

    txtMessages = new wxTextCtrl(nbNotebook, CTL_MSG, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL);
    nbNotebook->AddPage(txtMessages, _("Messages"));

    AddObjects((pgCollection*)object);


    if (securityPage->cbGroups)
    {
        pgSet *set=object->GetConnection()->ExecuteSet(wxT("SELECT groname FROM pg_group ORDER BY groname"));

        if (set)
        {
            while (!set->Eof())
            {
                securityPage->cbGroups->Append(wxT("group ") + set->GetVal(0));
                set->MoveNext();
            }
            delete set;
        }

        if (settings->GetShowUsersForPrivileges())
        {
            set=object->GetConnection()->ExecuteSet(wxT("SELECT usename FROM pg_user ORDER BY usename"));

            if (set)
            {
                securityPage->stGroup->SetLabel(_("Group/User"));

                while (!set->Eof())
                {
                    securityPage->cbGroups->Append(set->GetVal(0));
                    set->MoveNext();
                }
                delete set;
            }
        }
    }

    Show(true);
}


wxString frmGrantWizard::GetSql()
{
    wxString sql;

    int i;
    for (i=0 ; i < chkList->GetCount() ; i++)
    {
        if (chkList->IsChecked(i))
        {
            wxString tmp;

            pgObject *obj=(pgObject*)objectArray.Item(i);

            if (obj->GetType() == PG_FUNCTION || obj->GetType() == PG_TRIGGERFUNCTION)
            {
                tmp = securityPage->GetGrant(wxT("X"), wxT("FUNCTION ") 
                    + obj->GetQuotedFullIdentifier() + wxT("(")
                    + ((pgFunction*)obj)->GetArgTypes() + wxT(")"));
            }
            else
                tmp = securityPage->GetGrant(wxT("arwdRxt"), obj->GetTypeName().Upper() + wxT(" ") + obj->GetQuotedFullIdentifier());

            if (!tmp.IsEmpty())
                sql.Append(tmp);
        }
    }
    return sql;
}