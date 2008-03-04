//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlMenuToolbar.h - Menu tool bar
//
// This code is essentially stolen (with the authors permission) from 
// Paul Nelson (http://www.pnelsoncomposer.com/)
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLMENUTOOLBAR_H
#define CTLMENUTOOLBAR_H

#include "wx/frame.h"

DECLARE_EVENT_TYPE(wxEVT_FILL_MENU, -1)

// ctlMenuButton - Can be used wherever you can use a standard wxBitmapButton
//
// Implements a small pull-down triangle (v), which, when clicked, will display
// a pop-up menu. 

class ctlMenuButton : public wxBitmapButton
{
public:
    ctlMenuButton(wxToolBar *toolBar, int ID, wxMenu *menu)  { Create(toolBar, toolBar, ID, menu); }
    void DoProcessLeftClick(wxMouseEvent& event);

    wxMenu *m_menu;

    void FillMenu();

private:
    void Create(wxWindow *window, wxToolBar *toolBar, int ID, wxMenu *menu);
    wxToolBar *m_toolBar;
};


// ctlMenuTool - is only used internal to the implementation of ctlMenuToolbar.
//
// You should never have to use it yourself

class ctlMenuTool {
public:
    ctlMenuTool(wxToolBarToolBase *new_tool, int toolId);

    wxToolBarToolBase *m_tool;
    int m_toolId;
    ctlMenuButton *m_button;
    wxMenu *m_menu;
};

WX_DECLARE_LIST(ctlMenuTool, ctlMenuToolList);


// *** ctlMenuToolbar  -  A replacement for wxToolBar which implements menu buttons
//                       and pull-down buttons
//
// A menu button is a standard looking toolbar tool which, when clicked, pops up a 
// menu which can be selected.
//
// A pull-down button presents a small black triangle which, when clicked, pops up
// a menu which can be selected. These buttons are typically used for a list of previous
// actions (for example, previous web pages visited).
//
// All menus can be filled by responding to the wxEVT_FILL_MENU event, as in:

class ctlMenuToolbar : public wxToolBar
{
public:
    ctlMenuToolbar();
    ctlMenuToolbar(wxFrame* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTB_HORIZONTAL | wxNO_BORDER, const wxString& name = wxPanelNameStr);
    ~ctlMenuToolbar();

    // NOTE:  label, shortHelpString, are not implemented on Macintosh (they work on MSW)
    void AddMenuButtonTool(int toolId, const wxString& label, const wxBitmap& bitmap1, const wxString& shortHelpString = wxEmptyString);

    // NOTE:  label, shortHelpString, are not implemented on all platforms and are only 
    //        included for possible future upgrades
    ctlMenuButton *AddMenuPulldownTool(int toolId, const wxString& label, const wxString& shortHelpString = wxEmptyString, wxMenu *popupmenu=0);

    void NewMenuTool(ctlMenuTool *menu_tool);

    void DoProcessLeftClick(wxMouseEvent& event);

    wxFrame *m_frame;
    ctlMenuToolList *m_menuTools;
};


#ifdef __WXMAC__
// On Macintosh, the toolbar does not transmit wxEVT_LEFT_DOWN events, so we need to
// replace MenuButton Tools with bitmap buttons
class ctlMacMenuTool : public wxBitmapButton
{
public:
    ctlMacMenuTool(ctlMenuToolbar* parent, wxWindowID id, const wxBitmap &bmp, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTB_HORIZONTAL | wxNO_BORDER);
    ~ctlMacMenuTool();

    void DoProcessLeftClick(wxMouseEvent& event);
    wxMenu *m_menu;
    ctlMenuToolbar *m_toolBar;
};

#endif

#endif

