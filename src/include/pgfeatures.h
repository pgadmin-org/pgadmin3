//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgfeatures.h PostgreSQL features constants
//
//////////////////////////////////////////////////////////////////////////

enum
{
    FEATURE_INITIALIZED=0,
    FEATURE_SIZE,
    FEATURE_FILEREAD,
    FEATURE_ROTATELOG,
    FEATURE_POSTMASTER_STARTTIME,
    FEATURE_TERMINATE_BACKEND,
    FEATURE_RELOAD_CONF,
    FEATURE_LAST
};
