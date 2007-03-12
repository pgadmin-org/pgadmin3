//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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
#include "frm/frmGrantWizard.h"
#include "frm/frmMain.h"
#include "utils/sysLogger.h"
#include "ctl/ctlSecurityPanel.h"
#include "schema/pgFunction.h"
#include "schema/pgSequence.h"
#include "schema/pgTable.h"
#include "schema/pgView.h"

// Icons
#include "images/index.xpm"


#define chkList     CTRL_CHECKLISTBOX("chkList")



BEGIN_EVENT_TABLE(frmGrantWizard, ExecutionDialog)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),  frmGrantWizard::OnPageSelect) 
    EVT_BUTTON(XRCID("btnChkAll"),                  frmGrantWizard::OnCheckAll)
    EVT_BUTTON(XRCID("btnUnchkAll"),                frmGrantWizard::OnUncheckAll)
    EVT_CHECKLISTBOX(XRCID("chkList"),              frmGrantWizard::OnChange)
END_EVENT_TABLE()


frmGrantWizard::frmGrantWizard(frmMain *form, pgObject *obj) : ExecutionDialog(form, obj)
{
    nbNotebook = 0;

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(form, wxT("frmGrantWizard"));
    RestorePosition();

    if (object->IsCollection())
        SetTitle(wxString::Format(_("Privileges for %s"), object->GetTranslatedTypeName().c_str(), ""));
    else
        SetTitle(wxString::Format(_("Privileges for %s %s"), object->GetTranslatedTypeName().c_str(), object->GetFullIdentifier().c_str()));

    // Icon
    SetIcon(wxIcon(index_xpm));
    nbNotebook = CTRL_NOTEBOOK("nbNotebook");
    sqlPane = 0;

    Restore();
    EnableOK(false);
}


frmGrantWizard::~frmGrantWizard()
{
    SavePosition();
    Abort();
}


wxString frmGrantWizard::GetHelpPage() const
{
    wxString page=wxT("pg/sql-grant");

    return page;
}

void frmGrantWizard::OnChange(wxCommandEvent& event)
{
    sqlPane->SetReadOnly(false);
    sqlPane->SetText(GetSql());
    sqlPane->SetReadOnly(true);

    if (sqlPane->GetText().IsEmpty())
        EnableOK(false);
    else
        EnableOK(true);
}


void frmGrantWizard::OnUncheckAll(wxCommandEvent& event)
{
    unsigned int i;
    for (i=0 ; i < chkList->GetCount() ; i++)
        chkList->Check(i, false);

     OnChange(event);
}



void frmGrantWizard::OnCheckAll(wxCommandEvent& event)
{
    unsigned int i;
    for (i=0 ; i < chkList->GetCount() ; i++)
        chkList->Check(i, true);

     OnChange(event);
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
    bool traverseKids = (!collection->IsCollection() || collection->GetMetaType() == PGM_SCHEMA);

    if (!traverseKids)
    {
        pgaFactory *factory=collection->GetFactory();
        if (!factory->IsCollectionFor(tableFactory) &&
            !factory->IsCollectionFor(functionFactory) &&
            !factory->IsCollectionFor(triggerFunctionFactory) &&
            !factory->IsCollectionFor(procedureFactory) &&
            !factory->IsCollectionFor(viewFactory) &&
            !factory->IsCollectionFor(sequenceFactory))
            return;
    }
    wxCookieType cookie;
    wxTreeItemId item=mainForm->GetBrowser()->GetFirstChild(collection->GetId(), cookie);

    while (item)
    {
        pgObject *obj=mainForm->GetBrowser()->GetObject(item);
        if (obj)
        {
            if (traverseKids)
                AddObjects((pgCollection*)obj);
            else
            {
                objectArray.Add(obj);
                chkList->Append(obj->GetTypeName() + wxT(" ") + obj->GetFullIdentifier()); // no translation!
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
    
    switch (object->GetMetaType())
    {
        case PGM_DATABASE:
        case PGM_SCHEMA:
            privList.Append(wxT(",EXECUTE"));
            privChar = "arwdRxtX";
            break;
        case PGM_FUNCTION:
            privList = wxT("EXECUTE");
            privChar = "X";
            break;
        default:
            break;
    }

    securityPage = new ctlSecurityPanel(nbNotebook, privList, privChar, mainForm->GetImageList());
    securityPage->SetConnection(object->GetConnection());
    this->Connect(EVT_SECURITYPANEL_CHANGE, wxCommandEventHandler(frmGrantWizard::OnChange));

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

    // Work around a wierd display bug in wx2.7
    this->Refresh();
}


wxString frmGrantWizard::GetSql()
{
    wxString sql;

    unsigned int i;
    for (i=0 ; i < chkList->GetCount() ; i++)
    {
        if (chkList->IsChecked(i))
        {
            wxString tmp;

            pgObject *obj=(pgObject*)objectArray.Item(i);

            switch (obj->GetMetaType())
            {
                case PGM_FUNCTION:
                {
                    tmp = securityPage->GetGrant(wxT("X"), wxT("FUNCTION ") 
                        + obj->GetQuotedFullIdentifier() + wxT("(")
                        + ((pgFunction*)obj)->GetArgTypes() + wxT(")"));
                    break;
                }
                case PGM_VIEW:
                case PGM_SEQUENCE:
                    tmp = securityPage->GetGrant(wxT("arwdRxt"), wxT("TABLE ") + obj->GetQuotedFullIdentifier());
                    break;
                default:
                    tmp = securityPage->GetGrant(wxT("arwdRxt"), obj->GetTypeName().Upper() + wxT(" ") + obj->GetQuotedFullIdentifier());
                    break;
            }

            if (!tmp.IsEmpty())
                sql.Append(tmp);
        }
    }
    return sql;
}


grantWizardFactory::grantWizardFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("&Grant Wizard"), _("Grants rights to multiple objects"));
}


wxWindow *grantWizardFactory::StartDialog(frmMain *form, pgObject *obj)
{
    frmGrantWizard *frm=new frmGrantWizard(form, obj);
    frm->Go();
    return 0;
}


bool grantWizardFactory::CheckEnable(pgObject *obj)
{
    if (obj)
    {
        switch (obj->GetMetaType())
        {
            case PGM_SCHEMA:
            case PGM_TABLE:
            case PGM_FUNCTION:
            case PGM_SEQUENCE:
            case PGM_VIEW:
                return obj->CanEdit();
            default:
                break;
        }
    }
    return false;
}
