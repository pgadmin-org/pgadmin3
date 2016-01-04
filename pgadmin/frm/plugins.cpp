//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
#include "db/pgConn.h"
#include "ctl/ctlMenuToolbar.h"
#include "schema/pgObject.h"
#include "schema/pgDatabase.h"
#include "schema/pgSchema.h"
#include "schema/pgTable.h"
#include "utils/sysSettings.h"

void frmMain::LoadPluginUtilities()
{
	if (pluginsDir.IsEmpty())
		return;

	PluginUtility *util = new PluginUtility;
	ClearPluginUtility(util);

	// Loop through all the ini files we find in the directory.
	wxString iniFile;
	wxDir iniDir(pluginsDir);

	if (!iniDir.IsOpened())
		return;

	wxLogInfo(wxT("Loading plugin ini files from %s"), pluginsDir.c_str());

	bool cont = iniDir.GetFirst(&iniFile, wxT("*.ini"), wxDIR_FILES);

	while(cont)
	{
		// Load the config file
		wxFileName utilIni(pluginsDir + wxT("/") + iniFile);
		if (!utilIni.FileExists())
		{
			cont = iniDir.GetNext(&iniFile);
			continue;
		}

		wxLogInfo(wxT("Loading plugin utilities from %s"), utilIni.GetFullPath().c_str());
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
				AddPluginUtility(util);
				pluginsMenu->AppendSeparator();
			}

			// Title
			if (token.Lower().StartsWith(wxT("title=")))
			{
				// Add the previous app if required.
				AddPluginUtility(util);
				util->title = token.AfterFirst('=').Trim();
			}

			// Command
			if (token.Lower().StartsWith(wxT("command=")))
				util->command = token.AfterFirst('=').Trim();

			// Description
			if (token.Lower().StartsWith(wxT("description=")))
				util->description = token.AfterFirst('=').Trim();

			// KeyFile
			if (token.Lower().StartsWith(wxT("keyfile=")))
			{
				wxString keyfile = token.AfterFirst('=').Trim();

				// Substitute path placeholders
				keyfile.Replace(wxT("$$BINDIR"), loadPath);
				keyfile.Replace(wxT("$$WORKINGDIR"), wxGetCwd());
				keyfile.Replace(wxT("$$PGBINDIR"), settings->GetPostgresqlPath());
				keyfile.Replace(wxT("$$EDBBINDIR"), settings->GetEnterprisedbPath());
				keyfile.Replace(wxT("$$SLONYBINDIR"), settings->GetSlonyPath());

				util->keyfile = keyfile;
			}

			// Platform
			if (token.Lower().StartsWith(wxT("platform=")))
				util->platform = token.AfterFirst('=').Trim();

			// Server types
			if (token.Lower().StartsWith(wxT("servertype=")))
			{
				util->server_types.Clear();

				// This is a comma delimited list of values going into an array.
				wxStringTokenizer valueTkz(token.AfterFirst('='), wxT(","));

				while(valueTkz.HasMoreTokens())
					util->server_types.Add(valueTkz.GetNextToken());
			}

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
			// Environment
			if (token.Lower().StartsWith(wxT("environment=")))
			{
				util->set_env.Clear();

				// This is a comma delimited list of values going into an array.
				wxStringTokenizer valueTkz(token.AfterFirst('='), wxT(","));

				while(valueTkz.HasMoreTokens())
					util->set_env.Add(valueTkz.GetNextToken());
			}
		}

		// Add the last app if required.
		AddPluginUtility(util);

		// Get the next file
		cont = iniDir.GetNext(&iniFile);
	}

	if (util)
		delete util;
}

// Add a new plugin to the collection.
void frmMain::AddPluginUtility(PluginUtility *util)
{
	// Platform name
#ifdef __WXMSW__
	wxString thisPlatform = wxT("windows");
#else
#ifdef __WXGTK__
	wxString thisPlatform = wxT("unix");
#else
	wxString thisPlatform = wxT("osx");
#endif
#endif

	// Only add apps targeted to this, or any platform
	if (util->platform.Lower() == thisPlatform || util->platform == wxEmptyString)
	{
		// Only add an app with a title and command
		if (!util->title.IsEmpty() && !util->command.IsEmpty())
		{
			// We're only going to add this if the keyfile exists or isn't specified
			if (util->keyfile.IsEmpty() || wxFileExists(util->keyfile))
			{
				CreatePluginUtility(util);
				ClearPluginUtility(util);
				pluginUtilityCount++;
			}
		}
	}
}

// Create a new Plugin utility factory
void frmMain::CreatePluginUtility(PluginUtility *util)
{
	wxLogInfo(wxT("Adding plugin utility: %s"), util->title.c_str());
	wxLogInfo(wxT("              Command: %s"), util->command.c_str());
	wxLogInfo(wxT("          Description: %s"), util->description.c_str());
	wxLogInfo(wxT("            Database?: %s"), util->database ? wxT("Yes") : wxT("No"));
	wxLogInfo(wxT("        Set Password?: %s"), util->set_password ? wxT("Yes") : wxT("No"));

	new pluginUtilityFactory(menuFactories, pluginsMenu, util);
}

// Clear a PluginUtility struct
void frmMain::ClearPluginUtility(PluginUtility *util)
{
	util->title = wxEmptyString;
	util->command = wxEmptyString;
	util->description = wxEmptyString;
	util->keyfile = wxEmptyString;
	util->platform = wxEmptyString;
	util->server_types.Clear();
	util->database = false;
	util->applies_to.Clear();
	util->set_password = false;
	util->set_env.Clear();
}

// The actionFactory for the plugin utilities
pluginUtilityFactory::pluginUtilityFactory(menuFactoryList *list, wxMenu *menu, PluginUtility *util) : actionFactory(list)
{
	title = util->title;
	command = util->command;
	description = util->description;
	server_types = util->server_types;
	database = util->database;
	applies_to = util->applies_to;
	set_password = util->set_password;
	set_env = util->set_env;

	menu->Append(id, title, description);
}


wxWindow *pluginUtilityFactory::StartDialog(frmMain *form, pgObject *obj)
{
	wxString execCmd = command;
	wxArrayString environment = set_env;

	// Remember this as the last plugin used
	form->SetLastPluginUtility(this);

	// Replace all the place holders with appropriate values
	if (HaveDatabase(obj))
	{
		execCmd.Replace(wxT("$$HOSTNAME"), obj->GetConnection()->GetHostName());
		execCmd.Replace(wxT("$$HOSTADDR"), obj->GetConnection()->GetHostName());
		execCmd.Replace(wxT("$$PORT"), NumToStr((long)obj->GetConnection()->GetPort()));
		execCmd.Replace(wxT("$$SSLMODE"), obj->GetConnection()->GetSslModeName());
		execCmd.Replace(wxT("$$DATABASE"), obj->GetConnection()->GetDbname());
		execCmd.Replace(wxT("$$USERNAME"), obj->GetConnection()->GetUser());
		execCmd.Replace(wxT("$$PASSWORD"), obj->GetConnection()->GetPassword());

		// Set the PGPASSWORD variable if required.
		if (set_password && !obj->GetConnection()->GetPassword().IsEmpty())
			wxSetEnv(wxT("PGPASSWORD"), obj->GetConnection()->GetPassword());

		// Pass the SSL settings via the environment
		switch (obj->GetConnection()->GetSslMode())
		{
			case 1:
				wxSetEnv(wxT("PGREQUIRESSL"), wxT("1"));
				break;
			case 2:
				wxSetEnv(wxT("PGREQUIRESSL"), wxT("0"));
				break;
		}

		wxSetEnv(wxT("PGSSLMODE"), obj->GetConnection()->GetSslModeName());
		wxSetEnv(wxT("PGSSLCERT"), obj->GetConnection()->GetSSLCert());
		wxSetEnv(wxT("PGSSLKEY"), obj->GetConnection()->GetSSLKey());
		wxSetEnv(wxT("PGSSLROOTCERT"), obj->GetConnection()->GetSSLRootCert());
		wxSetEnv(wxT("PGSSLCRL"), obj->GetConnection()->GetSSLCrl());
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
	if (obj)
	{
		if (obj->GetMetaType() == PGM_SCHEMA)
			execCmd.Replace(wxT("$$SCHEMA"), obj->GetName());
		else if (obj->GetSchema())
			execCmd.Replace(wxT("$$SCHEMA"), obj->GetSchema()->GetName());
	}
	else
		execCmd.Replace(wxT("$$SCHEMA"), wxEmptyString);

	// Table
	if (obj)
	{
		if (obj->GetMetaType() == PGM_TABLE || obj->GetMetaType() == GP_PARTITION)
			execCmd.Replace(wxT("$$TABLE"), obj->GetName());
		else if (obj->GetTable())
			execCmd.Replace(wxT("$$TABLE"), obj->GetTable()->GetName());
	}
	else
		execCmd.Replace(wxT("$$TABLE"), wxEmptyString);

	// Directory substitutions
	execCmd.Replace(wxT("$$BINDIR"), loadPath);
	execCmd.Replace(wxT("$$WORKINGDIR"), wxGetCwd());
	execCmd.Replace(wxT("$$PGBINDIR"), settings->GetPostgresqlPath());
	execCmd.Replace(wxT("$$EDBBINDIR"), settings->GetEnterprisedbPath());
	execCmd.Replace(wxT("$$SLONYBINDIR"), settings->GetSlonyPath());

	// set Environment variable.
	for (size_t i = 0 ; i < environment.GetCount() ; i++)
	{
		wxString str = environment.Item(i);
		wxSetEnv(str.BeforeFirst('='), str.AfterFirst('='));
	}

	// Let's go!!
	if (wxExecute(execCmd) == 0)
	{
		wxLogError(_("Failed to execute plugin %s (%s)"), title.c_str(), command.c_str());
	}

	// Reset the environment variables set by us
	wxUnsetEnv(wxT("PGPASSWORD"));
	wxUnsetEnv(wxT("PGSSLMODE"));
	wxUnsetEnv(wxT("PGREQUIRESSL"));
	wxUnsetEnv(wxT("PGSSLCERT"));
	wxUnsetEnv(wxT("PGSSLKEY"));
	wxUnsetEnv(wxT("PGSSLROOTCERT"));
	wxUnsetEnv(wxT("PGSSLCRL"));

	return 0;
}

bool pluginUtilityFactory::CheckEnable(pgObject *obj)
{
	// First check that this is one of the supported server types
	// for this plugin. If none are specified, then anything goes
	if (database && server_types.Count() > 0)
	{
		// If we need a specific server type, we can't enable unless
		// we have a connection.
		if (!obj || !(obj->GetConnection()->GetStatus() == PGCONN_OK))
			return false;

		// Get the server type.
		wxString serverType = wxT("postgresql");
		if (obj->GetConnection()->GetIsEdb())
			serverType = wxT("enterprisedb");

		// Check if it's in the list.
		if (server_types.Index(serverType) == wxNOT_FOUND)
			return false;
	}

	// Now check that this is one of the supported object types
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

	if (!(obj->GetDatabase()->GetConnection()->GetStatus() == PGCONN_OK))
		return false;

	return true;
}

// The pluginButtonMenuFactory class manages the toolbar menu button
// for the plugins.

#include "images/plugins.pngc"
pluginButtonMenuFactory::pluginButtonMenuFactory(menuFactoryList *list, wxMenu *popupmenu, ctlMenuToolbar *toolbar, int pluginCount) : actionFactory(list)
{
	if (pluginCount)
		enableButton = true;
	else
		enableButton = false;

	if (toolbar)
	{
		toolbar->AddTool(id, wxEmptyString, *plugins_png_bmp, _("Execute the last used plugin."));
		pulldownButton = toolbar->AddMenuPulldownTool(MNU_PLUGINBUTTONLIST, wxT("Execute Plugin"), wxT("Select a plugin."), popupmenu);
	}
}

// Call the last plugin used, or popup the menu if this is the first time
wxWindow *pluginButtonMenuFactory::StartDialog(frmMain *form, pgObject *obj)
{
	if (form->GetLastPluginUtility() && form->GetLastPluginUtility()->CheckEnable(obj))
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
