//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAgent.h - main include
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGAGENT_H
#define PGAGENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

using namespace std;

#include "misc.h"
#include "connection.h"
#include "job.h"

extern long longWait;
extern long shortWait;
extern string connectString;
extern string serviceDBname;

// to be implemented platform dependent
void CheckForInterrupt();


void MainLoop();

#endif // PGAGENT_H

