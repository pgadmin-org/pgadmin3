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

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif


string getArg(int &argc, char** &argv)
{
    string s;
    if (argv[0][2])
        s = argv[0] +2;
    else
    {
        if (argc > 1)
        {
            argc--;
            argv++;
            s = argv[0];
        }
        else
        {
            // very bad!
            LogMessage("bad argument.", LOG_ERROR);
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
                    int val = atoi(getArg(argc, argv).c_str());
                    if (val > 0)
                        shortWait = val;
                    break;
                }
                case 'r':
                {
                    int val = atoi(getArg(argc, argv).c_str());
                    if (val >= 10)
                        longWait = val;
                    break;
                }
                case 'l':
                {
                    int val = atoi(getArg(argc, argv).c_str());
                    if (val >= 0 && val <= 2)
                        minLogLevel = val;
                    break;
                }
            }
        }
        else
        {
            if (connectString != "")
                connectString += " ";
            connectString += *argv;
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



string NumToStr(const long l)
{
    char buf[40];
    sprintf(buf, "%ld", l);
    return buf;
}
