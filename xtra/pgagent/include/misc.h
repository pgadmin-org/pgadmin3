//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.h - misc functions
//
//////////////////////////////////////////////////////////////////////////


#ifndef MISC_H
#define MISC_H


void WaitAWhile(const bool waitLong=false);
void setOptions(int argc, char **argv, const wxString& executable);
wxString getArg(int &argc, char** &argv);
wxString NumToStr(const long l);
wxString qtDbString(const wxString& value);

#endif // MISC_H

