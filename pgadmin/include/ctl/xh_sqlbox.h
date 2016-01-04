//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// xh_sqlbox.h - wxSqlBox handler
//
//////////////////////////////////////////////////////////////////////////


#ifndef _WX_XH_SQLBOX_H_
#define _WX_XH_SQLBOX_H_


#include "wx/xrc/xmlres.h"

//class WXDLLIMPEXP_XRC
class ctlSQLBoxXmlHandler : public wxXmlResourceHandler
{
	DECLARE_DYNAMIC_CLASS(ctlSQLBoxXmlHandler)
public:
	ctlSQLBoxXmlHandler();
	virtual wxObject *DoCreateResource();
	virtual bool CanHandle(wxXmlNode *node);
};


#endif // _WX_XH_SQLBOX_H_
