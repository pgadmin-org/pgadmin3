//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmIndexcheck.cpp - Index check dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "frmIndexcheck.h"
#include "sysLogger.h"
#include "pgIndex.h"

// Icons
#include "images/index.xpm"


BEGIN_EVENT_TABLE(frmIndexcheck, ExecutionDialog)
END_EVENT_TABLE()

#define rbxAction               CTRL_RADIOBOX("rbxAction")
#define sbxOptions              CTRL_STATICBOX("sbxOptions")
#define chkFull                 CTRL_CHECKBOX("chkFull")
#define chkFreeze               CTRL_CHECKBOX("chkFreeze")
#define chkAnalyze              CTRL_CHECKBOX("chkAnalyze")
#define sbxReindexOptions       CTRL_STATICBOX("sbxReindexOptions")
#define chkForce                CTRL_CHECKBOX("chkForce")
#define chkRecreate             CTRL_CHECKBOX("chkRecreate")
#define chkVerbose              CTRL_CHECKBOX("chkVerbose")

#define stBitmap                CTRL("stBitmap", wxStaticBitmap)



frmIndexcheck::frmIndexcheck(frmMain *form, pgObject *obj) : ExecutionDialog(form, obj)
{
    wxLogInfo(wxT("Creating a Index Check dialogue for %s %s"), object->GetTypeName().c_str(), object->GetFullName().c_str());

    wxWindowBase::SetFont(settings->GetSystemFont());
    wxXmlResource::Get()->LoadDialog(this, form, wxT("frmIndexcheck"));
    SetTitle(wxString::Format(_("Check indexes on %s %s"), object->GetTypeName().c_str(), object->GetFullIdentifier().c_str()));

    // Icon
    SetIcon(wxIcon(index_xpm));
    txtMessages = CTRL_TEXT("txtMessages");

    // Bitmap
//    txtMessages->SetMaxLength(0L);

//    wxCommandEvent ev;
//    OnAction(ev);
    CenterOnParent();
}


frmIndexcheck::~frmIndexcheck()
{
    wxLogInfo(wxT("Destroying a Index Check dialogue"));
    Abort();
}


wxString frmIndexcheck::GetHelpPage() const
{
    wxString page;

    return page;
}



wxString frmIndexcheck::GetSql()
{
    wxString sql;

    return sql;
}


void frmIndexcheck::Go()
{
//    chkFull->SetFocus();
    Show(true);
}
