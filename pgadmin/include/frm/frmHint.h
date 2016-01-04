//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmHint.h - PostgreSQL Guru hints
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRMHINT
#define __FRMHINT


#define HINT_CONNECTSERVER               wxT("conn-listen")
#define HINT_MISSINGHBA                  wxT("conn-hba")
#define HINT_MISSINGIDENT                wxT("conn-ident")
#define HINT_PRIMARYKEY                  wxT("pk")
#define HINT_FKINDEX                     wxT("fki")
#define HINT_VACUUM                      wxT("vacuum")
#define HINT_VACUUM_FULL                 wxT("vacuum-full")
#define HINT_QUERYRUNTIME                wxT("query-runtime")
#define HINT_INSTRUMENTATION             wxT("instrumentation")
#define HINT_INSTRUMENTATION_91_WITH     wxT("instrumentation91_with")
#define HINT_INSTRUMENTATION_91_WITHOUT  wxT("instrumentation91_without")
#define HINT_ENCODING_ASCII              wxT("encoding-ascii")
#define HINT_ENCODING_UNICODE            wxT("encoding-unicode")
#define HINT_READONLY_NOPK               wxT("view-without-pk")
#define HINT_AUTOVACUUM                  wxT("autovacuum")
#define HINT_OBJECT_EDITING              wxT("object-editing")
#define HINT_SAVING_PASSWORDS            wxT("saving-passwords")

#define HINT_RC_FIX             42

#include "dlg/dlgClasses.h"
#include "utils/factory.h"

class frmMain;
class frmHint : public DialogWithHelp
{
public:
	static int ShowHint(wxWindow *fr,  const wxString &hint, const wxString &info = wxEmptyString, bool force = false);
	static int ShowHint(wxWindow *fr,  const wxArrayString &hints, const wxString &info = wxEmptyString, bool force = false);
	static bool WantHint(const wxString &hint);
	static void ResetHints();

	void SetHint(int hint, const wxString &info);
	void SetHint(const wxArrayInt &hintnos, const wxString &info);

private:
	frmHint(wxWindow *fr, bool force);
	~frmHint();

	void SetHint(const wxString &info);
	void OnFix(wxCommandEvent &ev);
	static int GetHintNo(const wxString &hint);
	static bool WantHint(int hintno);
	wxString GetPage(const wxChar *hintpage);
	wxString GetHelpPage() const;

	DECLARE_EVENT_TABLE()

	wxArrayInt hintnos;
	int currentHint;
	bool force;
};


class hintFactory : public actionFactory
{
public:
	hintFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar, bool bigTool);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


#endif
