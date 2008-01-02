//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id: misc.cpp 5013 2006-02-21 09:39:15Z dpage $
// Copyright (C) 2002 - 2008 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.cpp - misc functions
//
//////////////////////////////////////////////////////////////////////////

#include "pgAgent.h"

#ifndef __WXMSW__
#include <unistd.h>
#endif

// In unix.c or win32.c
void usage(const wxString& executable);

wxString getArg(int &argc, char** &argv)
{
    wxString s;
    if (argv[0][2])
        s = wxString::FromAscii(argv[0] +2);
    else
    {
        if (argc >= 1)
        {
            argc--;
            argv++;
            s = wxString::FromAscii(argv[0]);
        }
        else
        {
            // very bad!
            LogMessage(_("Invalid command line argument"), LOG_ERROR);
        }
    }

    return s;
}


void setOptions(int argc, char **argv, const wxString& executable)
{
    while (argc-- > 0)
    {
        if (argv[0][0] == '-')
        {
            switch (argv[0][1])
            {
                case 't':
                {
                    int val = atoi(getArg(argc, argv).mb_str(wxConvUTF8));
                    if (val > 0)
                        shortWait = val;
                    break;
                }
                case 'r':
                {
                    int val = atoi(getArg(argc, argv).mb_str(wxConvUTF8));
                    if (val >= 10)
                        longWait = val;
                    break;
                }
                case 'l':
                {
                    int val = atoi(getArg(argc, argv).mb_str(wxConvUTF8));
                    if (val >= 0 && val <= 2)
                        minLogLevel = val;
                    break;
                }
#ifndef __WXMSW__
                case 'f':
                {
                    runInForeground = true;
                    break;
                }
                case 's':
                {
                    logFile = getArg(argc, argv);
                    break;
                }
#endif
                default:
                {
                    usage(executable);
                    exit(1);
                }
            }
        }
        else
        {
            if (connectString != wxT(""))
                connectString += wxT(" ");
            connectString += wxString::FromAscii(*argv);
            if (**argv == '"')
                connectString = connectString.substr(1, connectString.length()-2);
        }
        argv++;
    }
}


void WaitAWhile(const bool waitLong)
{
    int count;
    if (waitLong)
        count=longWait;
    else
        count=shortWait;

    while (count--)
    {
#ifdef WIN32
        CheckForInterrupt();
        Sleep(1000);
#else
    sleep(1);
#endif
    }
}



wxString NumToStr(const long l)
{
    wxString buf;
    buf.Printf(wxT("%ld"), l);
    return buf;
}


