//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// xh_ctlcombo.h - ctlComboBox handler
//
//////////////////////////////////////////////////////////////////////////


#ifndef _WX_XH_CTLCOMBO_H_
#define _WX_XH_CTLCOMBO_H_


#include "wx/xrc/xmlres.h"
#include "wx/xrc/xh_combo.h"

//class WXDLLIMPEXP_XRC 
class ctlComboBoxXmlHandler : public wxComboBoxXmlHandler
{
DECLARE_DYNAMIC_CLASS(ctlComboBoxXmlHandler)
public:
    ctlComboBoxXmlHandler() : wxComboBoxXmlHandler() {}
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};


#endif 
