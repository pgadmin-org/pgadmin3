//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmMainConfig.h - Backend configuration tool
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMMAINCONFIG_H
#define FRMMAINCONFIG_H

#include "pgconfig.h"
#include "frmConfig.h"
#include "dlgClasses.h"
#include "base/factory.h"

class ctlListView;
class pgServer;

WX_DECLARE_OBJARRAY(pgConfigOrgLine, pgConfigOrgLineArray);

class frmMainConfig : public frmConfig
{
public:
    frmMainConfig(const wxString& title, const wxString &configFile);
    frmMainConfig(frmMain *parent, pgServer *server=0);

    ~frmMainConfig();

protected:
    void DisplayFile(const wxString &str);
    void WriteFile(pgConn *conn=0);
    wxString GetHintString();
    wxString GetHelpPage() const;

private:
    void Init();
    void Init(pgSettingReader *reader);

    void FillList(const wxString &categoryMember);
    void FillList(wxArrayString *category);

    void OnContents(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnEditSetting(wxListEvent& event);

    void UpdateLine(int line);


    ctlListView *cfgList;

    pgSettingItemHashmap options;
    pgCategoryHashmap categories;
    pgConfigOrgLineArray lines;

    DECLARE_EVENT_TABLE()
};


class mainConfigFactory : public actionFactory
{
public:
    mainConfigFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class mainConfigFileFactory : public actionFactory
{
public:
    mainConfigFileFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

#endif
