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

#ifdef WIN32
#error this file is for unix only!
#endif

void usage()
{
    printf(
        "Usage:\n"
        "pgagent [options] <connect-string>\n"
        "options:\n"
        "-t <poll time interval>\n"
        "-r <retry period after connection abort (>=10s)>\n"
        "-c <connection pool size (>=5)>\n"
        );
}



int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage();
        return 1;
    }

    setOptions(argc, argv);

    DBconn *conn=DBconn::InitConnection(connectString);
    if (!conn->IsValid())
        fatal("connection not valid: " + conn->GetLastError());

    return 0;
}
