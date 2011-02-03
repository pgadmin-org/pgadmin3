//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////

// Comand-line parser
#include <wx/cmdline.h>
#include <wx/msgout.h>

// Output stream
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

// pgScript
#include "pgscript/pgsApplication.h"

// If defined then messages are logged into debug.log
#undef USER_DEFINED_LOGGER

int main(int argc, char *argv[])
{
	// Initilize application
	wxInitializer initializer;
	if (!initializer.IsOk())
	{
		wxLogError(wxT("Cannot initialize the application"));
		return EXIT_FAILURE;
	}

	// Logging options
	sysLogger::logLevel = LOG_ERRORS;
#if defined(USER_DEFINED_LOGGER)
	wxLog *logger = new sysLogger();
	delete wxLog::SetActiveTarget(logger);
	wxLog::Resume();
#endif

	wxMessageOutput::Set(new wxMessageOutputStderr);
	wxCmdLineParser parser(argc, argv);

	wxString logo(wxT("                            ___         _      _\n")
	              wxT("                 _ __  __ _/ __| __ _ _(_)_ __| |_\n")
	              wxT("                | '_ \\/ _` \\__ \\/ _| '_| | '_ \\  _|\n")
	              wxT("                | .__/\\__, |___/\\__|_| |_| .__/\\__|\n")
	              wxT("                |_|   |___/              |_|\n\n"));

	logo << wxT("Runs enhanced SQL queries on a PostgreSQL database.") << wxT("\n");
	logo << wxT("See manual for the input file syntax.") << wxT("\n");

	parser.SetLogo(logo);

	wxString host, db, user, pass, enc, file;
	long port = pgsApplication::default_port;

	parser.AddOption(wxT('h'), wxT("host"), wxT("database server host"),
	                 wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY);
	parser.AddOption(wxT('p'), wxT("port"), wxT("database server port"),
	                 wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
	parser.AddOption(wxT('d'), wxT("db"), wxT("database name"),
	                 wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY);
	parser.AddOption(wxT('U'), wxT("user"), wxT("database user name"),
	                 wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY);
	parser.AddOption(wxT('W'), wxT("pass"), wxT("database user password"),
	                 wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
	parser.AddOption(wxT('e'), wxT("enc"), wxT("input file encoding"),
	                 wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
	parser.AddParam(wxT("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY);

	bool success = (parser.Parse(true) == 0);
	if (success)
	{
		// Mandatory parameters
		parser.Found(wxT("h"), &host);
		parser.Found(wxT("d"), &db);
		parser.Found(wxT("U"), &user);

		// Optional parameters
		if (parser.Found(wxT("p")))
			parser.Found(wxT("p"), &port);
		if (parser.Found(wxT("W")))
			parser.Found(wxT("W"), &pass);
		if (parser.Found(wxT("e")))
			parser.Found(wxT("e"), &enc);

		// Create application
		pgsApplication app(host, db, user, pass, port);

		if (app.IsConnectionValid())
		{
			for (size_t i = 0; i < parser.GetParamCount(); i++)
			{
				file = parser.GetParam(i);
			}

			// Output stream
			wxFileOutputStream output(2);
			wxTextOutputStream out(output, wxEOL_UNIX);

			// Encoding
			wxMBConv *conv = 0;
			if (enc.IsEmpty())
				conv = &wxConvLocal; // pnew wxConvAuto(); in the future
			else
				conv = pnew wxCSConv(enc);

			// Launch parser
			app.ParseFile(file, out, conv);
			app.Wait();

			// Delete encoding
			if (!enc.IsEmpty())
				pdelete(conv);
		}
	}

#if defined(PGSDEBUG)
	pgsAlloc::instance().dump();
#endif

#if defined(USER_DEFINED_LOGGER)
	delete wxLog::SetActiveTarget(NULL);
#endif

	return EXIT_SUCCESS;
}
