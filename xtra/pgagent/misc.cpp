//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.cpp - misc functions
//
//////////////////////////////////////////////////////////////////////////

#include "pgAgent.h"

#ifndef WIN32
#include <unistd.h>
#endif


wxString getArg(int &argc, char** &argv)
{
    wxString s;
    if (argv[0][2])
        s = wxString::FromAscii(argv[0] +2);
    else
    {
        if (argc > 1)
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


void setOptions(int argc, char **argv)
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
#ifndef _WIN32_
                case 'd':
                {
					runInForeground = true;
                    break;
                }
#endif
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
