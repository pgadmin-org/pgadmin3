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
#include "images/hint.xpm"
#include "images/hint2.xpm"



#define HINT_CANSUPPRESS 1
#define HINT_CANABORT    2
#define HINT_CANFIX      4
#define HINT_YESNO       8


struct make_dumb_compilers_happy        // unnamed structs don't make all compilers happy...
{
    const wxChar *hintPage;
    const wxChar *hintCaption;
    const wxChar *fixText;
    const wxChar *helpItem;
    int flags;
} 
hintArray[]=
{
    {   HINT_CONNECTSERVER,
        __("Server not listening"),
        0,
        wxT("runtime-config#runtime-config-connection"),
        HINT_CANSUPPRESS
    },
    {   HINT_MISSINGHBA,
        __("Server denies access"),
        0, 
        wxT("client-authentication#auth-pg-hba-conf"), 
        HINT_CANSUPPRESS
    },
    {   HINT_MISSINGIDENT,
        __("Ident authentication failed"),
        0, 
        wxT("auth-methods#auth-ident"), 
        HINT_CANSUPPRESS
    },
    {   HINT_PRIMARYKEY,
        __("Creation of primary key suggested"),
        0, 
        wxT("ddl-constraints"),
        HINT_CANSUPPRESS|HINT_CANABORT
    },
    {   HINT_FKINDEX,
        __("Creation of index in referencing table suggested"),
        0,
        wxT("ddl-constraints#ddl-constraints-fk"),
        HINT_CANSUPPRESS|HINT_CANABORT
    },
    {   HINT_VACUUM,
        __("Running VACUUM recommended"),
        __("VACUUM"),
        wxT("maintenance#routine-vacuuming"),
        HINT_CANSUPPRESS | HINT_CANFIX
    },
    {
        HINT_QUERYRUNTIME,
        __(""),
        0,
        wxT("pgadmin/query"),
        HINT_CANSUPPRESS|HINT_CANABORT|HINT_YESNO
    },
    { 0,0,0,0 }
};


BEGIN_EVENT_TABLE(frmHint, DialogWithHelp)
    EVT_BUTTON(XRCID("btnFix"), frmHint::OnFix)
END_EVENT_TABLE();




#define chkSuppress     CTRL_CHECKBOX("chkSuppress")
#define htmlHint        (XRCCTRL(*this, "htmlHint", wxHtmlWindow))
#define btnFix          CTRL_BUTTON("btnFix")
#define btnYes          CTRL_BUTTON("wxID_YES")
#define btnNo           CTRL_BUTTON("wxID_NO")


frmHint::frmHint(wxWindow *fr, bool _force) : DialogWithHelp(0)
{
    force=_force;
    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(fr, wxT("frmHint"));

    if (force)
        btnCancel->Disable();

    SetIcon(wxIcon(pgAdmin3_xpm));
}



wxString frmHint::GetPage(const wxChar *hintPage)
{
    wxString page;

    extern wxString docPath;
    wxString cn=settings->GetCanonicalLanguage();
    if (cn.IsEmpty())
        cn=wxT("en_US");

    wxString filename=docPath + wxT("/") + cn + wxT("/hints/") + hintPage + wxT(".html");

    if (!wxFile::Exists(filename))
        filename = docPath + wxT("/en_US/hints/") + hintPage + wxT(".html");
    if (wxFile::Exists(filename))
    {
        wxUtfFile file(filename);
        file.Read(page);
    }

    return page;
}


void frmHint::SetHint(int hintno, const wxString &info)
{
    wxArrayInt hintnos;
    hintnos.Add(hintno);
    SetHint(hintnos, info);
};



void frmHint::SetHint(const wxArrayInt &hintnos, const wxString &info)
{
    currentHint = hintnos.Item(0);
    if (hintnos.GetCount() == 1)
    {
        SetTitle(_("pgAdmin III Guru Hint") + wxString(wxT(" - ")) + wxGetTranslation(hintArray[currentHint].hintCaption));

        wxString page=GetPage(hintArray[currentHint].hintPage);
        page.Replace(wxT("<INFO>"), info);

        htmlHint->SetPage(page);
    }
    else
    {
        SetTitle(_("pgAdmin III Guru Hints"));

        wxString header = GetPage(wxT("multiple"));
        wxString pages;

        if (header.IsEmpty())
        {
            header = 
                wxT("<html><head>\n")
                wxT("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\">\n")
                wxT("<link rel=\"STYLESHEET\" type=\"text/css\" href=\"../pgadmin3.css\">\n")
                wxT("<title>Guru Hints</title></head>\n")
                wxT("<body><p>\n");
        }
        else
        {
            int o=header.Find(wxT("</body>"));
            if (o > 0)
                header = header.Left(o) + wxT("<p>");
        }

        size_t i;
        for (i=0 ; i < hintnos.GetCount() ; i++)
        {
            int hintno=hintnos.Item(i);
            wxString page=GetPage(hintArray[hintno].hintPage);
            int a=page.Find(wxT("<body>"));
            int o=page.Find(wxT("</body>"));
            if (a<0)
                a=0;
            if (o < 0)
                o=wxSTRING_MAXLEN;


            int ha=page.Find(wxT("<H3>"));
            int ho=page.Find(wxT("</H3>"));
            if (ha < 0)
                ha=page.Find(wxT("<h3>"));
            if (ho < 0)
                ho=page.Find(wxT("</h3>"));

            if (ha > a && ho > ha)
            {
                wxString hintTitle=page.Mid(ha+4, ho-ha-4);

                pages  += page.Mid(a, ha-a)
                       + wxT("<H3><A Name=\"") + hintArray[hintno].hintPage + wxT("\">")
                       + hintTitle + wxT("</A>")
                       + page.Mid(ho, o-ho);
                header += wxString(wxT("<A HREF=\"#")) + hintArray[hintno].hintPage + wxT("\">")
                       + hintTitle + wxT("</A><BR>");
            }
            else
                pages += page.Mid(a, o-a);
        }

        pages.Replace(wxT("<INFO>"), info);

        htmlHint->SetPage(header + wxT("</p>") + pages + wxT("</body></html>\n"));
    }

    chkSuppress->SetValue(false);

    if (force || !(hintArray[currentHint].flags & HINT_CANSUPPRESS))
        chkSuppress->Disable();
    if (force || !(hintArray[currentHint].flags & HINT_CANABORT))
        btnCancel->Disable();
    if (!(hintArray[currentHint].flags & HINT_CANFIX))
        btnFix->Hide();

    if (!(hintArray[currentHint].flags & HINT_CANFIX))
        btnFix->Hide();
    else if (hintArray[currentHint].fixText)
        btnFix->SetLabel(wxGetTranslation(hintArray[currentHint].fixText));

    if (hintArray[currentHint].flags & HINT_YESNO)
        btnOK->Hide();
    else
    {
        btnYes->Hide();
        btnNo->Hide();
    }
};


frmHint::~frmHint()
{
    if (!force && chkSuppress->GetValue())
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



int frmHint::ShowHint(wxWindow *fr, const wxArrayString &hints, const wxString &info)
{
    // force implied
    wxArrayInt hintnos;
    size_t i;

    if(!hints.GetCount())
        return wxID_OK;
    for (i=0 ; i < hints.GetCount() ; i++)
        hintnos.Add(GetHintNo(hints.Item(i)));

    frmHint *frm=new frmHint(fr, true);
    frm->SetHint(hintnos, info);

    frm->CenterOnParent();
    int rc = frm->ShowModal();
    delete frm;

    if ((rc == wxID_CANCEL || rc == -1))
        rc = wxID_OK;

    return rc;
}


int frmHint::ShowHint(wxWindow *fr, const wxString &hint, const wxString &info, bool force)
{
    int rc=wxID_OK;
    int hintno = GetHintNo(hint);

    if (WantHint(hintno))
    {
        frmHint *frm=new frmHint(fr, force);
        frm->SetHint(hintno, info);

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



hintFactory::hintFactory(wxMenu *mnu, wxToolBar *toolbar, bool bigTool)
{
    mnu->Append(id, _("Hints"), _("Display helpful hints on current object."));
    if (toolbar)
    {
        char **img=(bigTool ? hint2_xpm : hint_xpm);
        toolbar->AddTool(id, _("Hints"), wxBitmap(img), _("Display helpful hints on current object."));
    }
}


wxWindow *hintFactory::StartDialog(pgFrame *form, pgObject *obj)
{
    obj->ShowHint((frmMain*)form, true);
    return 0;
}


bool hintFactory::CheckEnable(pgObject *obj)
{
    return obj && obj->GetCanHint();
}
