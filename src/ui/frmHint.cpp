//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmHint.cpp - PostgreSQL Guru hints
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/file.h>
#include "wx/wxhtml.h"

// App headers
#include "copyright.h"
#include "frmHint.h"
#include "frmMain.h"
#include "menu.h"
#include "utffile.h"

// Icons
#include "images/pgAdmin3.xpm"



#define HINT_CANSUPPRESS 1
#define HINT_CANABORT    2
#define HINT_CANFIX      4

struct
{
    const wxChar *hintPage;
    const wxChar *hintCaption;
    const wxChar *helpItem;
    int flags;
} 
hintArray[]=
{
    {   HINT_CONNECTSERVER,
        __("Server not listening"),
        wxT("runtime-config#runtime-config-connection"),
        HINT_CANSUPPRESS
    },
    {   HINT_MISSINGHBA,
        __("Server denies access"),
        wxT("client-authentication#auth-pg-hba-conf"), 
        HINT_CANSUPPRESS
    },
    {   HINT_MISSINGIDENT,
        __("Ident authentication failed"),
        wxT("auth-methods#auth-ident"), 
        HINT_CANSUPPRESS
    },
    {   HINT_PRIMARYKEY,
        __("Creation of primary key suggested"),
        wxT("ddl-constraints"),
        HINT_CANSUPPRESS|HINT_CANABORT
    },
    {   HINT_FKINDEX,
        __("Creation of index in referencing table suggested"),
        wxT("ddl-constraints#ddl-constraints-fk"),
        HINT_CANSUPPRESS|HINT_CANABORT
    },
    {   HINT_VACUUM,
        __("Running VACUUM recommended"),
        wxT("maintenance#routine-vacuuming"),
        HINT_CANSUPPRESS | HINT_CANFIX
    },
    { 0,0,0,0 }
};


BEGIN_EVENT_TABLE(frmHint, DialogWithHelp)
    EVT_BUTTON(XRCID("btnFix"), frmHint::OnFix)
END_EVENT_TABLE();




#define chkSuppress     CTRL_CHECKBOX("chkSuppress")
#define htmlHint        (XRCCTRL(*this, "htmlHint", wxHtmlWindow))
#define btnFix          CTRL_BUTTON("btnFix")


frmHint::frmHint(wxWindow *fr, int hintno, const wxString &info) : DialogWithHelp(0)
{
    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(fr, wxT("frmHint"));

    SetIcon(wxIcon(pgAdmin3_xpm));
    SetTitle(_("pgAdmin III Guru Hint") + wxString(wxT(" - ")) + wxGetTranslation(hintArray[hintno].hintCaption));

    extern wxString docPath;
    wxString cn=settings->GetCanonicalLanguage();
    if (cn.IsEmpty())
        cn=wxT("en_US");

    wxString filename=docPath + wxT("/") + cn + wxT("/hints/") + hintArray[hintno].hintPage + wxT(".html");

    if (!wxFile::Exists(filename))
        filename = docPath + wxT("/en_US/hints/") + hintArray[hintno].hintPage + wxT(".html");
    if (wxFile::Exists(filename))
    {
        if (info.IsEmpty())
            htmlHint->LoadPage(filename);
        else
        {
            wxString page;
            wxUtfFile file(filename);
            file.Read(page);
            page.Replace(wxT("<INFO>"), info);
            htmlHint->SetPage(page);
        }
    }
    chkSuppress->SetValue(false);
    if (!(hintArray[hintno].flags & HINT_CANSUPPRESS))
        chkSuppress->Disable();
    if (!(hintArray[hintno].flags & HINT_CANABORT))
        btnCancel->Disable();
    if (!(hintArray[hintno].flags & HINT_CANFIX))
        btnFix->Hide();
    currentHint = hintno;
};


frmHint::~frmHint()
{
    if (chkSuppress->GetValue())
        settings->Write(wxString(wxT("Hints/")) + hintArray[currentHint].hintPage, wxT("Suppress"));
}


wxString frmHint::GetHelpPage() const
{
    const wxChar *helpItem=hintArray[currentHint].helpItem;
    if (helpItem)
        return helpItem;

    return wxT("guruhints");
}


void frmHint::ResetHints()
{
    int hintno=0;
    while (hintArray[hintno].hintPage)
    {
        settings->Write(wxString(wxT("Hints/")) + hintArray[hintno].hintPage, wxEmptyString);
        hintno++;
    }
}


int frmHint::GetHintNo(const wxString &hint)
{
    int hintno=0;
    while (hintArray[hintno].hintPage)
    {
        if (hintArray[hintno].hintPage == hint)
            return hintno;
        hintno++;
    }
    return -1;
}


bool frmHint::WantHint(const wxString &hint)
{
    int hintno = GetHintNo(hint);
    if (hintno < 0)
        return false;

    return WantHint(hintno);
}


bool frmHint::WantHint(int hintno)
{
    if (hintno < 0)
        return false;

    if ((hintArray[hintno].flags & HINT_CANSUPPRESS) && settings->GetSuppressGuruHints())
        return false;

    if (settings->Read(wxString(wxT("Hints/")) + hintArray[hintno].hintPage, wxEmptyString) != wxT("Suppress"))
        return true;

    return false;
}


int frmHint::ShowHint(wxWindow *fr, const wxString &hint, const wxString &info)
{
    int rc=wxID_OK;
    int hintno = GetHintNo(hint);

    if (WantHint(hintno))
    {
        frmHint *frm=new frmHint(fr, hintno, info);

        frm->CenterOnParent();
        rc = frm->ShowModal();
        delete frm;

        if ((rc == wxID_CANCEL || rc == -1) && !(hintArray[hintno].flags & HINT_CANABORT))
            rc = wxID_OK;
    }
    return rc;
}


void frmHint::OnFix(wxCommandEvent &ev)
{
    EndModal(HINT_RC_FIX);
}
