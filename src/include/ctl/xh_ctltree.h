//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// xh_ctltree.h - ctlTree handler
//
//////////////////////////////////////////////////////////////////////////


#ifndef _WX_XH_CTLTREE_H_
#define _WX_XH_CTLTREE_H_

#include "wx/xrc/xmlres.h"
#include "wx/xrc/xh_tree.h"

//class WXDLLIMPEXP_XRC 
class ctlTreeXmlHandler : public wxTreeCtrlXmlHandler
{
DECLARE_DYNAMIC_CLASS(ctlTreeXmlHandler)
public:
    ctlTreeXmlHandler() : wxTreeCtrlXmlHandler() {}
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};


#endif 
