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

// App headers
#include "copyright.h"
#include "frmHint.h"
#include "frmMain.h"
#include "menu.h"

// Icons
#include "images/pgAdmin3.xpm"



#define HINT_CANSUPPRESS 1
#define HINT_CANABORT    2

struct
{
    const wxChar *helpItem;
    int flags;
    const wxChar *helpCaption;
    const wxChar *helpText;
} hintArray[]=
{
    { wxT("pk"), HINT_CANSUPPRESS, 
        __("Primary key suggested"),
        __("In general, every table should have a primary key which uniquely identifies each row.")
    },
    { wxT("fk"), HINT_CANSUPPRESS, 
        __("Index in referenced table suggested"), 
        __("Creating a foreign key on a table also affects access to the referenced table, because on each update and delete all foreign keys have to be checked for referencings rows in dependent tables.")
        __("This effectively means that an implicit select access to those tables is performed, which should be supported by indexes in most cases.\n")
        __("To create an index on the referencing table automatically, you can select the \"Covering Index\" option.")
    },
    { 0,0,0,0 }
};


BEGIN_EVENT_TABLE(frmHint, DialogWithHelp)
END_EVENT_TABLE();


#define stBold          CTRL_STATIC("stBold")
#define stNormal        CTRL_STATIC("stNormal")
#define chkSuppress     CTRL_CHECKBOX("chkSuppress")


frmHint::frmHint(wxWindow *fr, Hint hintno) : DialogWithHelp(0)
{
    SetIcon(wxIcon(pgAdmin3_xpm));
    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(fr, wxT("frmHint"));

    wxWindowBase::SetFont(settings->GetSystemFont());

    wxFont fntLabel = stBold->GetFont();
    fntLabel.SetWeight(wxBOLD);
    stBold->SetFont(fntLabel);

    stBold->SetLabel(wxGetTranslation(hintArray[hintno].helpCaption));
    stNormal->SetLabel(wxGetTranslation(hintArray[hintno].helpText));
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
    return hintArray[currentHint].helpItem;
}


int frmHint::ShowHint(wxWindow *fr, Hint hintno)
{
    int rc=wxID_OK;
    if (hintno < 0 || hintno >= HintLast)
        return rc;

    if (!StrToBool(settings->Read(wxString::Format(wxT("Hints/Suppress%d"), hintno),wxT("No"))))
    {
        frmHint *frm=new frmHint(fr, hintno);

        rc = frm->ShowModal() != wxID_CANCEL;
        delete frm;
    }
    return rc;
}
