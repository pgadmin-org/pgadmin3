//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlSecurityPanel.cpp - Panel with security information
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>

// App headers
#include "pgAdmin3.h"
#include "utils/sysLogger.h"
#include "ctl/ctlSecurityPanel.h"
#include "db/pgConn.h"
#include "schema/pgObject.h"
#include "schema/pgGroup.h"
#include "schema/pgUser.h"



BEGIN_EVENT_TABLE(ctlSecurityPanel, wxPanel)
    EVT_LIST_ITEM_SELECTED(CTL_LBPRIV,  ctlSecurityPanel::OnPrivSelChange)
    EVT_BUTTON(CTL_ADDPRIV,             ctlSecurityPanel::OnAddPriv)
    EVT_BUTTON(CTL_DELPRIV,             ctlSecurityPanel::OnDelPriv)
    EVT_TEXT(CTL_CBGROUP,               ctlSecurityPanel::OnGroupChange)
    EVT_COMBOBOX(CTL_CBGROUP,           ctlSecurityPanel::OnGroupChange)
    EVT_CHECKBOX(CTL_ALLPRIV,           ctlSecurityPanel::OnPrivCheckAll)
    EVT_CHECKBOX(CTL_ALLPRIVGRANT,      ctlSecurityPanel::OnPrivCheckAllGrant)
    EVT_CHECKBOX(CTL_PRIVCB,            ctlSecurityPanel::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+2,          ctlSecurityPanel::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+4,          ctlSecurityPanel::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+6,          ctlSecurityPanel::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+8,          ctlSecurityPanel::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+10,         ctlSecurityPanel::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+12,         ctlSecurityPanel::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+14,         ctlSecurityPanel::OnPrivCheck)
    EVT_CHECKBOX(CTL_PRIVCB+16,         ctlSecurityPanel::OnPrivCheck)
END_EVENT_TABLE();

DEFINE_EVENT_TYPE(EVT_SECURITYPANEL_CHANGE)

ctlSecurityPanel::ctlSecurityPanel(wxNotebook *nb, const wxString &privList, char *privChars, wxImageList *imgList)
: wxPanel(nb, -1, wxDefaultPosition, wxDefaultSize)
{
    nbNotebook = nb;
    nbNotebook->AddPage(this, _("Privileges"));

    connection = 0;
    privilegeChars=privChars;
    allPrivileges=0;

    privCheckboxes=0;

    wxStringTokenizer privileges(privList, wxT(","));
    privilegeCount=privileges.CountTokens();


    if (privilegeCount)
    {
        bool needAll=(privilegeCount > 1);
        privCheckboxes = new wxCheckBox*[privilegeCount*2];
        int i=0;

        int width, height;

#ifdef __WIN32__
		GetClientSize(&width, &height);
#else
		nbNotebook->GetClientSize(&width, &height);
		height -= ConvertDialogToPixels(wxPoint(0, 20)).y;   // sizes of tabs
#endif

        wxPoint zeroPos=ConvertDialogToPixels(wxPoint(5, 5));

        btnAddPriv = new wxButton(this, CTL_ADDPRIV, _("Add/Change"));
        wxSize btnSize=btnAddPriv->GetSize();
        wxSize chkSize=ConvertDialogToPixels(wxSize(65,12));
        wxSize spcSize=ConvertDialogToPixels(wxSize(2, 2));

        {
            int y = height 
                - spcSize.GetHeight()
                - zeroPos.y * 2
			    - btnSize.GetHeight()
                - chkSize.GetHeight() * (3 + privilegeCount + (needAll ? 0 : 1));

            lbPrivileges = new ctlListView(this, CTL_LBPRIV, 
                zeroPos, 
                wxSize(width - zeroPos.x * 2, y - zeroPos.y - spcSize.GetHeight()), wxSUNKEN_BORDER|wxLC_REPORT);

            lbPrivileges->CreateColumns(imgList, _("User/Group"), _("Privileges"), -1);

			btnAddPriv->Move(wxPoint(zeroPos.x, y)); 
            btnDelPriv = new wxButton(this, CTL_DELPRIV, _("Remove"), 
                wxPoint(zeroPos.x * 2 + btnSize.GetWidth(), y), btnSize);
            y += zeroPos.y + btnSize.GetHeight();

            new wxStaticBox(this, -1, _("Privileges"), 
                wxPoint(zeroPos.x, y), 
                wxSize(width - zeroPos.x*2, btnSize.GetHeight() + chkSize.GetHeight() * (2 + privilegeCount-(needAll?0:1))));
            y += zeroPos.y + spcSize.GetHeight();

            stGroup = new wxStaticText(this, CTL_STATICGROUP, _("Group"), wxPoint(zeroPos.x * 2, y+3), chkSize);
            cbGroups = new ctlComboBox(this, CTL_CBGROUP, 
                wxPoint(zeroPos.x * 3 + chkSize.GetWidth(), y), 
                wxSize(width - zeroPos.x * 4 - chkSize.GetWidth() - spcSize.GetWidth(), chkSize.GetHeight()));
            y += btnSize.GetHeight();
            cbGroups->Append(wxT("public"));
            cbGroups->SetSelection(0);


            if (needAll)
            {
                allPrivileges = new wxCheckBox(this, CTL_ALLPRIV, wxT("ALL"), 
                    wxPoint(zeroPos.x * 2, y), 
                    chkSize);
                allPrivilegesGrant = new wxCheckBox(this, CTL_ALLPRIVGRANT, wxT("WITH GRANT OPTION"), 
                    wxPoint(zeroPos.x * 3 + chkSize.GetWidth(), y), 
                    wxSize(width - zeroPos.x * 4 - chkSize.GetWidth() - spcSize.GetWidth(), chkSize.GetHeight()));
                y += chkSize.GetHeight();
                allPrivilegesGrant->Disable();
            }

            while (privileges.HasMoreTokens())
            {
                wxString priv=privileges.GetNextToken();
                wxCheckBox *cb;
                cb=new wxCheckBox(this, CTL_PRIVCB+i, priv, 
                    wxPoint(zeroPos.x * 2, y), 
                    chkSize);
                privCheckboxes[i++] = cb;
                cb=new wxCheckBox(this, CTL_PRIVCB+i, wxT("WITH GRANT OPTION"), 
                    wxPoint(zeroPos.x * 3 + chkSize.GetWidth(), y), 
                    wxSize(width - zeroPos.x * 4 - chkSize.GetWidth() - spcSize.GetWidth(), chkSize.GetHeight()));
                cb->Disable();
                privCheckboxes[i++] = cb;

                y += chkSize.GetHeight();
            }
        }

    }
}


ctlSecurityPanel::~ctlSecurityPanel()
{
    if (privCheckboxes)
        delete[] privCheckboxes;
}


void ctlSecurityPanel::SetConnection(pgConn *conn)
{
    connection=conn;
    if (connection && stGroup && connection->BackendMinimumVersion(8, 1))
        stGroup->SetLabel(_("Role"));
}


wxString ctlSecurityPanel::GetGrant(const wxString &allPattern, const wxString &grantObject, wxArrayString *currentAcl)
{
    wxArrayString tmpAcl;
    if (currentAcl)
        tmpAcl = *currentAcl;

    wxString sql;
    int cnt=lbPrivileges->GetItemCount();
    int pos;
    unsigned int i;

    for (pos=0 ; pos < cnt ; pos++)
    {
        wxString name=lbPrivileges->GetText(pos);
        wxString value=lbPrivileges->GetText(pos, 1);

        int nameLen=name.Length();

        bool privWasAssigned=false;
        bool privPartiallyAssigned=false;
        for (i=0 ; i < tmpAcl.GetCount() ; i++)
        {
            if (tmpAcl.Item(i).Left(nameLen) == name)
            {
                privPartiallyAssigned=true;
                if (tmpAcl.Item(i).Mid(nameLen+1) == value)
                    privWasAssigned=true;
                tmpAcl.RemoveAt(i);
                break;
            }
        }

        if (name.Left(6).IsSameAs(wxT("group "), false))
            name = wxT("GROUP ") + qtIdent(name.Mid(6));
        else
            name=qtIdent(name);

        if (!privWasAssigned)
        {
            if (privPartiallyAssigned)
                sql += pgObject::GetPrivileges(allPattern, wxT(""), grantObject, name);
            sql += pgObject::GetPrivileges(allPattern, value, grantObject, name);
        }
    }

    for (i=0 ; i < tmpAcl.GetCount() ; i++)
    {
        wxString name=tmpAcl.Item(i).BeforeLast('=');

        if (name.Left(6).IsSameAs(wxT("group "), false))
            name = wxT("GROUP ") + qtIdent(name.Mid(6));
        else
            name=qtIdent(name);
        sql += pgObject::GetPrivileges(allPattern, wxT(""), grantObject, name);
    }
    return sql;
}


void ctlSecurityPanel::OnGroupChange(wxCommandEvent &ev)
{
    cbGroups->GuessSelection(ev);
    wxString name=cbGroups->GetGuessedStringSelection();

    btnAddPriv->Enable(!name.Strip(wxString::both).IsEmpty());
}

    
void ctlSecurityPanel::OnPrivCheckAll(wxCommandEvent& ev)
{
    bool all=allPrivileges->GetValue();
    int i;
    for (i=0 ; i < privilegeCount ; i++)
    {
        if (all)
        {
            // We use the Name property of the checkboxes as a flag to note if that
            // box should remain disabled (eg. CONNECT for a DB on PG < 8.2
            if (privCheckboxes[i*2]->GetName() != wxT("DISABLE"))
            {
                privCheckboxes[i*2]->SetValue(true);
                privCheckboxes[i*2]->Disable();
                privCheckboxes[i*2+1]->Disable();
                allPrivilegesGrant->Enable(GrantAllowed());
            }
        }
        else
        {
            if (privCheckboxes[i*2]->GetName() != wxT("DISABLE"))
            {
                allPrivilegesGrant->Disable();
                allPrivilegesGrant->SetValue(false);
                privCheckboxes[i*2]->Enable();
                CheckGrantOpt(i);
            }
        }
    }
}



void ctlSecurityPanel::OnPrivCheckAllGrant(wxCommandEvent& ev)
{
    bool grant=allPrivilegesGrant->GetValue();
    int i;
    for (i=0 ; i < privilegeCount ; i++)
        privCheckboxes[i*2+1]->SetValue(grant);
}


void ctlSecurityPanel::OnPrivCheck(wxCommandEvent& ev)
{
    int id=(ev.GetId()-CTL_PRIVCB) /2;
    CheckGrantOpt(id);
}


void ctlSecurityPanel::CheckGrantOpt(int id)
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


void ctlSecurityPanel::OnDelPriv(wxCommandEvent &ev)
{
    lbPrivileges->DeleteCurrentItem();
    
    wxCommandEvent event( EVT_SECURITYPANEL_CHANGE, GetId() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( event );

    ev.Skip();
}


void ctlSecurityPanel::OnAddPriv(wxCommandEvent &ev)
{
    wxString name=cbGroups->GetGuessedStringSelection();

    long pos=lbPrivileges->FindItem(-1, name);
    if (pos < 0)
    {
        pos = lbPrivileges->GetItemCount();
        int icon=userFactory.GetIconId();

        if (name.Left(6).IsSameAs(wxT("group "), false))
            icon = groupFactory.GetIconId();
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

    wxCommandEvent event( EVT_SECURITYPANEL_CHANGE, GetId() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( event );

    ev.Skip();
}


void ctlSecurityPanel::OnPrivSelChange(wxListEvent &ev)
{
    if (!cbGroups)
        return;
    if (allPrivileges)
    {
        allPrivileges->SetValue(false);
        allPrivilegesGrant->SetValue(false);
        allPrivilegesGrant->Disable();
    }
    long pos=lbPrivileges->GetSelection();
    if (pos >= 0)
    {
        wxString name=lbPrivileges->GetText(pos);
        wxString value=lbPrivileges->GetText(pos, 1);

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
            if (privCheckboxes[i*2]->GetName() != wxT("DISABLE"))
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
            }
            CheckGrantOpt(i);
        }
    }
    btnAddPriv->Enable();
}



bool ctlSecurityPanel::GrantAllowed() const
{
    if (!connection->BackendMinimumVersion(7, 4))
        return false;

    wxString user=cbGroups->GetValue();
    if (user.IsSameAs(wxT("public"), false))
        return false;

    if (!connection->BackendMinimumVersion(8, 1) &&
        user.Left(6).IsSameAs(wxT("group "), false))
        return false;

    return true;
}

bool ctlSecurityPanel::DisablePrivilege(const wxString &priv)
{
    for (int i=0; i < privilegeCount; i++)
    {
        if (privCheckboxes[i*2]->GetLabel() == priv)
        {
            // Use the Name property as a flag so we don't accidently reenable the control later
            privCheckboxes[i*2]->SetName(wxT("DISABLE"));
            privCheckboxes[i*2]->Disable();
            return true;
        }
    }
    return false;
}
