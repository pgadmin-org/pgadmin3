//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// plugins.cpp - Plugin management for frmMain
//
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/file.h>

// Application headers
#include "frm/frmMain.h"
#include "ctl/ctlMenuToolbar.h"
#include "schema/pgObject.h"
#include "schema/pgDatabase.h"
#include "schema/pgSchema.h"
#include "schema/pgTable.h"

extern wxString utilitiesIni;

void frmMain::LoadPluginUtilities()
{
    if (utilitiesIni.IsEmpty())
        return;

    PluginUtility *util = new PluginUtility;
    ClearPluginUtility(util);

    // Load the config file
	wxFileName utilIni(utilitiesIni);
	if (!utilIni.FileExists())
        return;

    wxLogDebug(wxT("Loading plugin utilities from %s"), utilIni.GetFullPath().c_str());
    wxString brCfg = FileRead(utilIni.GetFullPath());

	wxStringTokenizer tkz(brCfg, wxT("\r\n"));

    // Loop round the lines in the file. Everytime we find a new 'Title' value
    // we create the current plugin and start a new one 
	while(tkz.HasMoreTokens())
	{
		wxString token = tkz.GetNextToken();

		if (token.Trim() == wxEmptyString || token.StartsWith(wxT(";")))
			continue;

        // Separator
		if (token.Lower().StartsWith(wxT("[separator]")))
		{
            // Add the previous app if required.
            if (!util->title.IsEmpty() && !util->command.IsEmpty())
            {
                CreatePluginUtility(util);
                ClearPluginUtility(util);
                pluginUtilityCount++;
            }

            pluginsMenu->AppendSeparator();
        }

        // Title
		if (token.Lower().StartsWith(wxT("title=")))
		{
            // Add the previous app if required.
            if (!util->title.IsEmpty() && !util->command.IsEmpty())
            {
                CreatePluginUtility(util);
                ClearPluginUtility(util);
                pluginUtilityCount++;
            }

            util->title = token.AfterFirst('=').Trim();
        }

        // Command
		if (token.Lower().StartsWith(wxT("command=")))
            util->command = token.AfterFirst('=').Trim();

        // Description
		if (token.Lower().StartsWith(wxT("description=")))
            util->description = token.AfterFirst('=').Trim();

        // Database
		if (token.Lower().StartsWith(wxT("database=")))
        {
            if (token.AfterFirst('=').Trim().Lower() == wxT("yes"))
                util->database = true;
            else
                util->database = false;
        }

        // Applies to
		if (token.Lower().StartsWith(wxT("appliesto=")))
        {
            util->applies_to.Clear();

            // This is a comma delimited list of values going into an array.
            wxStringTokenizer valueTkz(token.AfterFirst('='), wxT(","));

           	while(valueTkz.HasMoreTokens())
		        util->applies_to.Add(valueTkz.GetNextToken());
        }

        // Set password
		if (token.Lower().StartsWith(wxT("setpassword=")))
        {
            if (token.AfterFirst('=').Trim().Lower() == wxT("yes"))
                util->set_password = true;
            else
                util->set_password = false;
        }
    }

	// Add the last app if required.
    if (!util->title.IsEmpty() && !util->command.IsEmpty())
    {
        CreatePluginUtility(util);
        ClearPluginUtility(util);
        pluginUtilityCount++;
    }

    if (util)
        delete util;
}

// Create a new Plugin utility factory
void frmMain::CreatePluginUtility(PluginUtility *util)
{
    wxLogDebug(wxT("Adding plugin utility: %s"), util->title.c_str());
    wxLogDebug(wxT("              Command: %s"), util->command.c_str());
    wxLogDebug(wxT("          Description: %s"), util->description.c_str());
    wxLogDebug(wxT("            Database?: %s"), util->database ? wxT("Yes") : wxT("No"));
    wxLogDebug(wxT("        Set Password?: %s"), util->set_password ? wxT("Yes") : wxT("No"));

    new pluginUtilityFactory(menuFactories, pluginsMenu, util);
}

// Clear a PluginUtility struct
void frmMain::ClearPluginUtility(PluginUtility *util)
{
    util->title = wxEmptyString;
    util->command = wxEmptyString;
    util->description = wxEmptyString;
    util->database = false;
    util->applies_to.Clear();
    util->set_password = false;
}

// The actionFactory for the plugin utilities
pluginUtilityFactory::pluginUtilityFactory(menuFactoryList *list, wxMenu *menu, PluginUtility *util) : actionFactory(list)
{
    title = util->title;
    command = util->command;
    description = util->description;
    database = util->database;
    applies_to = util->applies_to;
    set_password = util->set_password;

    menu->Append(id, title, description);
}


wxWindow *pluginUtilityFactory::StartDialog(frmMain *form, pgObject *obj)
{
    wxString execCmd = command;
    wxArrayString environment;

    // Remember this as the last plugin used
    form->SetLastPluginUtility(this);

    // Replace all the place holders with appropriate values
    if (HaveDatabase(obj))
    {
        execCmd.Replace(wxT("$$HOSTNAME"), obj->GetConnection()->GetHostName());
        execCmd.Replace(wxT("$$HOSTADDR"), obj->GetConnection()->GetHostAddress());
        execCmd.Replace(wxT("$$PORT"), NumToStr((long)obj->GetConnection()->GetPort()));

        switch (obj->GetConnection()->GetSslMode())
        {
            case 1: execCmd.Replace(wxT("$$SSLMODE"), wxT("require"));   break;
            case 2: execCmd.Replace(wxT("$$SSLMODE"), wxT("prefer"));    break;
            case 3: execCmd.Replace(wxT("$$SSLMODE"), wxT("allow"));     break;
            case 4: execCmd.Replace(wxT("$$SSLMODE"), wxT("disable"));   break;
            default: execCmd.Replace(wxT("$$SSLMODE"), wxT("prefer"));   break;
        }
        execCmd.Replace(wxT("$$DATABASE"), obj->GetConnection()->GetDbname());
        execCmd.Replace(wxT("$$USERNAME"), obj->GetConnection()->GetUser());
        execCmd.Replace(wxT("$$PASSWORD"), obj->GetConnection()->GetPassword());



        // Set the PGPASSWORD variable if required.
        if (set_password && !obj->GetConnection()->GetPassword().IsEmpty())
            wxSetEnv(wxT("PGPASSWORD="), obj->GetConnection()->GetPassword());
    }
    else
    {
        // Blank the rest
        execCmd.Replace(wxT("$$HOSTNAME"), wxEmptyString);
        execCmd.Replace(wxT("$$HOSTADDR"), wxEmptyString);
        execCmd.Replace(wxT("$$PORT"), wxEmptyString);
        execCmd.Replace(wxT("$$SSLMODE"), wxEmptyString);
        execCmd.Replace(wxT("$$DATABASE"), wxEmptyString);
        execCmd.Replace(wxT("$$USERNAME"), wxEmptyString);
        execCmd.Replace(wxT("$$PASSWORD"), wxEmptyString);
    }

    // Name
    if (obj && obj->IsCollection())
        execCmd.Replace(wxT("$$OBJECTNAME"), wxT("*"));
    else if (obj)
        execCmd.Replace(wxT("$$OBJECTNAME"), obj->GetName());
    else
        execCmd.Replace(wxT("$$OBJECTNAME"), wxEmptyString);

    // Object type
    if (obj && obj->GetFactory())
        execCmd.Replace(wxT("$$OBJECTTYPE"), wxString(obj->GetFactory()->GetTypeName()).Upper());
    else
        execCmd.Replace(wxT("$$OBJECTTYPE"), wxEmptyString);

    // Schema
    if (obj && obj->GetSchema())
    {
        if (obj->GetMetaType() == PGM_SCHEMA)
            execCmd.Replace(wxT("$$SCHEMA"), obj->GetName());
        else
            execCmd.Replace(wxT("$$SCHEMA"), obj->GetSchema()->GetName());
    }
    else
        execCmd.Replace(wxT("$$SCHEMA"), wxEmptyString);

    // Table
    if (obj && obj->GetTable())
    {
        if (obj->GetMetaType() == PGM_TABLE)
            execCmd.Replace(wxT("$$TABLE"), obj->GetName());
        else
            execCmd.Replace(wxT("$$TABLE"), obj->GetTable()->GetName());
    }
    else
        execCmd.Replace(wxT("$$TABLE"), wxEmptyString);

    // Let's go!!
    if (wxExecute(execCmd) == 0)
        wxLogError(_("Failed to execute plugin %s (%s)"), title.c_str(), command.c_str());

    return 0;
}

bool pluginUtilityFactory::CheckEnable(pgObject *obj)
{
    // First check that this is one of the supported object types
    // for this plugin. If none are specified, then anything goes
    if (obj && applies_to.Count() > 0)
    {
        if (applies_to.Index(wxString(obj->GetFactory()->GetTypeName()).Lower()) == wxNOT_FOUND)
            return false;
    }

    // If we don't need a database, we're always OK.
    if (!database)
        return true;
   
    return HaveDatabase(obj);
}

bool pluginUtilityFactory::HaveDatabase(pgObject *obj)
{
    // We need a good connection and database.
    if (!obj)
        return false;

    if (!obj->GetDatabase())
        return false;

    if (!obj->GetDatabase()->GetConnection())
        return false;
        
    if (!obj->GetDatabase()->GetConnection()->GetStatus() == PGCONN_OK)
        return false;

    return true;
}

// The pluginButtonMenuFactory class manages the toolbar menu button
// for the plugins. 

#include "images/plugins.xpm"
pluginButtonMenuFactory::pluginButtonMenuFactory(menuFactoryList *list, wxMenu *popupmenu, ctlMenuToolbar *toolbar, int pluginCount) : actionFactory(list)
{
    if (pluginCount)
        enableButton = true;
    else
        enableButton = false;

    if (toolbar)
    {
        toolbar->AddTool(id, _("Plugins"), wxBitmap(plugins_xpm), _("Execute the last used plugin."));
        pulldownButton = toolbar->AddMenuPulldownTool(MNU_PLUGINBUTTONLIST, wxT("Execute Plugin"), wxT("Select a plugin."), popupmenu); 
    }
}

// Call the last plugin used, or popup the menu if this is the first time
wxWindow *pluginButtonMenuFactory::StartDialog(frmMain *form, pgObject *obj)
{
    if (form->GetLastPluginUtility())
        return form->GetLastPluginUtility()->StartDialog(form, obj);
    else
    {
        wxMouseEvent evt;
        pulldownButton->DoProcessLeftClick(evt);
    }

    return 0;
}

bool pluginButtonMenuFactory::CheckEnable(pgObject *obj)
{
    return enableButton;
}
