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

struct
{
    int flags;
    const wxChar *helpItem;
    const wxChar *hintPage;
    const wxChar *hintCaption;
} hintArray[]=
{
    {   HINT_CANSUPPRESS, 
        wxT("runtime-config#runtime-config-connection"),
        wxT("conn-listen"), __("Server not listening")
    },
    {   HINT_CANSUPPRESS, wxT("client-authentication#auth-pg-hba-conf"), 
        wxT("conn-hba"), __("Server denies access")
    },
    {   HINT_CANSUPPRESS|HINT_CANABORT, 0, 
        wxT("pk"), __("Primary key suggested")
    },
    {   HINT_CANSUPPRESS|HINT_CANABORT, 0, 
        wxT("fki"),     __("Index in referenced table suggested")
    },
    { 0, 0,0,0 }
};


BEGIN_EVENT_TABLE(frmHint, DialogWithHelp)
END_EVENT_TABLE();




#define chkSuppress     CTRL_CHECKBOX("chkSuppress")
#define htmlHint        (XRCCTRL(*this, "htmlHint", wxHtmlWindow))

frmHint::frmHint(wxWindow *fr, Hint hintno, const wxString &info) : DialogWithHelp(0)
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

    currentHint = hintno;
};


frmHint::~frmHint()
{
    if (chkSuppress->GetValue())
        settings->Write(wxString::Format(wxT("Hints/Suppress%d"), currentHint),wxT("Yes"));
}


wxString frmHint::GetHelpPage() const
{
    const wxChar *helpItem=hintArray[currentHint].helpItem;
    if (helpItem)
        return helpItem;

    return wxT("guruhints");
}



bool frmHint::WantHint(Hint hintno)
{
    if (hintno < 0 || hintno >= HintLast)
        return false;
    if ((hintArray[hintno].flags & HINT_CANSUPPRESS) && settings->GetSuppressGuruHints())
        return false;

    if (!StrToBool(settings->Read(wxString::Format(wxT("Hints/Suppress%d"), hintno),wxT("No"))))
        return true;

    return false;
}

int frmHint::ShowHint(wxWindow *fr, Hint hintno, const wxString &info)
{
    int rc=wxID_OK;

    if (WantHint(hintno))
    {
        frmHint *frm=new frmHint(fr, hintno, info);

        rc = frm->ShowModal();
        delete frm;

        if (!(hintArray[hintno].flags & HINT_CANABORT))
            rc = wxID_OK;
    }
    return rc;
}
