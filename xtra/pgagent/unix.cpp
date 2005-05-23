//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003-2005 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// unix.cpp - pgAgent unix specific functions
//
//////////////////////////////////////////////////////////////////////////

#include "pgAgent.h"

#include <fcntl.h>

#ifdef WIN32
#error this file is for unix only!
#endif

void usage(const wxString &executable)
{
	wxFileName *fn = new wxFileName(executable);

    wxPrintf(_("Usage:\n"));
    wxPrintf(fn->GetName() + _(" [options] <connect-string>\n"));
    wxPrintf(_("options:\n"));
    wxPrintf(_("-t <poll time interval in seconds (default 10)>\n"));
    wxPrintf(_("-r <retry period after connection abort in seconds (>=10, default 30)>\n"));
    wxPrintf(_("-l <logging verbosity (ERROR=0, WARNING=1, DEBUG=2, default 0)>\n"));
}

void LogMessage(wxString msg, int level)
{
    switch (level)
    {
        case LOG_DEBUG:
            if (minLogLevel >= LOG_DEBUG)
                wxPrintf(_("DEBUG: %s\n"), msg.c_str());
            break;
        case LOG_WARNING:
            if (minLogLevel >= LOG_WARNING)
                wxPrintf(_("WARNING: %s\n"), msg.c_str());
            break;
        case LOG_ERROR:
            wxPrintf(_("ERROR: %s\n"), msg.c_str());
            exit(1);
            break;
    }

}

// Shamelessly lifted from pg_autovacuum...
static void daemonize(void)
{
    pid_t pid;

    pid = fork();
    if (pid == (pid_t) -1)
    {
        LogMessage(_("Cannot disassociate from controlling TTY"), LOG_ERROR);
        exit(1);
    }
    else if (pid)
        exit(0);

#ifdef HAVE_SETSID
    if (setsid() < 0)
    {
        LogMessage(_("Cannot disassociate from controlling TTY"), LOG_ERROR);
        exit(1);
    }
#endif

}

int main(int argc, char **argv)
{
	// Statup wx
	wxInitialize();
    
    wxString executable;
	executable = wxString::FromAscii(argv[0]);

    if (argc < 2)
    {
        usage(executable);
        return 1;
    }

    argc--;
    argv++;

    setOptions(argc, argv);

    daemonize();

    MainLoop();

    return 0;
}
