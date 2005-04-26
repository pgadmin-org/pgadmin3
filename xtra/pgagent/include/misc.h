//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.h - misc functions
//
//////////////////////////////////////////////////////////////////////////


#ifndef MISC_H
#define MISC_H

void fatal(const string &description);

void WaitAWhile(const bool waitLong=false);
void setOptions(int argc, char **argv);
string getArg(int &argc, char** &argv);
string NumToStr(const long l);

#endif // MISC_H