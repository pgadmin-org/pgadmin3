//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// update.h - Online update check
//
//////////////////////////////////////////////////////////////////////////


#ifndef __UPDATE_H
#define __UPDATE_H

class frmMain;

bool CheckUpdates();

wxThread *BackgroundCheckUpdates(frmMain *mainForm);

#endif
