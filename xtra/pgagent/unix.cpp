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

void usage()
{
    printf(
        "Usage:\n"
        "pgagent [options] <connect-string>\n"
        "options:\n"
        "-t <poll time interval in seconds (default 10)>\n"
        "-r <retry period after connection abort in seconds (>=10, default 30)>\n"
        "-c <connection pool size (>=5, default 5)>\n"
        "-l <logging verbosity (ERROR=0, WARNING=1, DEBUG=2, default 0)>\n"
        );
}

void LogMessage(string msg, int level)
{
    switch (level)
    {
        case LOG_DEBUG:
            if (minLogLevel >= LOG_DEBUG)
                fprintf(stderr, "DEBUG: %s\n", msg.c_str());
            break;
        case LOG_WARNING:
            if (minLogLevel >= LOG_WARNING)
                fprintf(stderr, "WARNING: %s\n", msg.c_str());
            break;
        case LOG_ERROR:
            fprintf(stderr, "ERROR: %s\n", msg.c_str());
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
        LogMessage("Cannot disassociate from controlling TTY", LOG_ERROR);
        exit(1);
    }
    else if (pid)
        exit(0);

#ifdef HAVE_SETSID
    if (setsid() < 0)
    {
        LogMessage("Cannot disassociate from controlling TTY", LOG_ERROR);
        exit(1);
    }
#endif

}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage();
        return 1;
    }

    argc--;
    argv++;

    setOptions(argc, argv);

    DBconn *conn=DBconn::InitConnection(connectString);
    if (!conn->IsValid())
        LogMessage("Invalid connection: " + conn->GetLastError(), LOG_ERROR);

    serviceDBname = conn->GetDBname();

    daemonize();

    MainLoop();

    return 0;
}
