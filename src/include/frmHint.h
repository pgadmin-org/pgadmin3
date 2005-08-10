//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmHint.h - PostgreSQL Guru hints
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRMHINT
#define __FRMHINT


#define HINT_CONNECTSERVER  wxT("conn-listen")
#define HINT_MISSINGHBA     wxT("conn-hba")
#define HINT_MISSINGIDENT   wxT("conn-ident")
#define HINT_PRIMARYKEY     wxT("pk")
#define HINT_FKINDEX        wxT("fki")
#define HINT_VACUUM         wxT("vacuum")
#define HINT_QUERYRUNTIME   wxT("query-runtime")
#define HINT_RC_FIX         42


class frmMain;
class frmHint : public DialogWithHelp
{
public:
    static int ShowHint(wxWindow *fr,  const wxString &hint, const wxString &info=wxEmptyString, bool force=false);
    static int ShowHint(wxWindow *fr,  const wxArrayString &hints, const wxString &info=wxEmptyString);
    static bool WantHint(const wxString &hint);
    static void ResetHints();

    void SetHint(int hint, const wxString &info);
    void SetHint(const wxArrayInt &hintnos, const wxString &info);

private:
    frmHint(wxWindow *fr, bool force);
    ~frmHint();

    void OnFix(wxCommandEvent &ev);
    static int GetHintNo(const wxString &hint);
    static bool WantHint(int hintno);
    wxString GetPage(const wxChar *hintpage);
    wxString GetHelpPage() const;

    DECLARE_EVENT_TABLE();

    int currentHint;
    bool force;
};


class hintFactory : public contextActionFactory
{
public:
    hintFactory(wxMenu *mnu, wxToolBar *toolbar, bool bigTool=true);
    wxWindow *StartDialog(pgFrame *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


#endif
