//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: update.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
