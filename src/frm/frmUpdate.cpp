//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmUpdate.cpp - Online update
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"

#include "frmUpdate.h"
#include "update.h"
#include "version.h"


BEGIN_EVENT_TABLE(frmUpdate, DialogWithHelp)
    EVT_BUTTON(wxID_OK, frmUpdate::OnOk)
END_EVENT_TABLE()


static double GetBetaVersion(const wxString &ver)
{
    double version=0.;
    int offset=0;
    if (!ver.Left(5).CmpNoCase(wxT("Post-")))
    {
        version = .5;
        offset=5;
    }
    if (!ver.Mid(offset, 2).CmpNoCase(wxT("RC")))
        version += 1000. + StrToDouble(ver.Mid(offset+2));
    else    // Beta
        version += StrToDouble(ver.Mid(offset+4));

    return version;
}


frmUpdate::frmUpdate(frmMain *mainForm) : DialogWithHelp(mainForm)
{
    wxLogInfo(wxT("Creating an update dialogue"));

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource((wxWindow*)mainForm, wxT("frmUpdate"));
}


frmUpdate::~frmUpdate()
{
    wxLogInfo(wxT("Destroy an update dialogue"));
}

wxString frmUpdate::GetHelpPage() const
{
    return wxT("update");
}


void frmUpdate::OnOk(wxCommandEvent &ev)
{
}


bool frmUpdate::HasLanguageUpdate()
{
    extern wxLocale *locale;    // in pgAdmin3.cpp

    long translated = StrToLong(_("@@@pgAdmin internal: Translated string count"));
    long newTranslated=settings->Read(wxT("Updates/Current/") + locale->GetCanonicalName(), 0L);

    return newTranslated > translated;
}


bool frmUpdate::HasNewVersion()
{
    wxString release=settings->Read(wxT("Updates/Current/Release"), wxEmptyString);

    double thisVer=StrToDouble(VERSION_STR);
    double releaseVer=StrToDouble(release);

    if (releaseVer > thisVer)
        return true;
    else if (releaseVer == thisVer)
    {
        releaseVer = StrToDouble(release.AfterFirst('.').AfterFirst('.'));
        thisVer = StrToDouble(wxString(VERSION_STR).AfterFirst('.').AfterFirst('.'));
        if (releaseVer > thisVer)
            return true;
    }
    return false;
}


bool frmUpdate::HasNewBeta()
{
    wxString beta=settings->Read(wxT("Updates/Current/Beta"), wxEmptyString);

#if PRERELEASE    
    double thisVer=StrToDouble(VERSION_STR);
    double betaVer=StrToDouble(beta);

    // check if release is out
    if (betaVer > thisVer)
        return true;
    else if (betaVer == thisVer)
    {
        thisVer = GetBetaVersion(wxString(VERSION_STR).AfterFirst(' '));
        betaVer = GetBetaVersion(beta.AfterFirst(' '));
        if (betaVer > thisVer)
            return true;
    }
#else
    if (!beta.IsEmpty())
        return true;
#endif
    return false;
}


onlineUpdateFactory::onlineUpdateFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : actionFactory(list)
{
    mnu->Append(id, _("Online Update"), _("Check online for updates"));
}


wxWindow *onlineUpdateFactory::StartDialog(frmMain *form, pgObject *obj)
{
    frmUpdate *upd=new frmUpdate(form);
    upd->Show();
    return upd;
}
