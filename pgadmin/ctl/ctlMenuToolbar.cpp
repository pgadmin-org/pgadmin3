//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlMenuToolbar.cpp - Menu tool bar
//
// This code is essentially stolen (with the authors permission) from 
// Paul Nelson (http://www.pnelsoncomposer.com/)
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "ctl/ctlMenuToolbar.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ctlMenuToolList);

/* XPM */
static char *pulldown_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 15 2 1",
". c Black",
"  c None",
/* pixels */
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"      .....     ",
"       ...      ",
"        .       ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                "
};


DEFINE_EVENT_TYPE(wxEVT_FILL_MENU)


/**************************/
/*** THE WX-MENU BUTTON ***/
/**************************/

/******************************************/
/*** Creation, Destruction, and Cleanup ***/
/******************************************/

void 
ctlMenuButton::Create(wxWindow *window, wxToolBar *toolBar, int ID, wxMenu *menu)
{
  wxBitmap bmpPulldown(pulldown_xpm);
  wxSize pulldownSize;

#ifdef __WXMSW__
  pulldownSize.Set(12,15);
#else
#ifdef __WXGTK__
  pulldownSize.Set(18,15);
#else
  pulldownSize.Set(16,15);
#endif
#endif

  m_toolBar = toolBar;
  m_menu = menu;

  ((wxBitmapButton *)this)->Create(window, ID, bmpPulldown, wxDefaultPosition, 
         pulldownSize, wxNO_BORDER);
  Connect(ID, wxEVT_LEFT_DOWN, wxMouseEventHandler(ctlMenuButton::DoProcessLeftClick) );
}

/********************************/
/* ctlMenuButton::DoProcessClick */
/********************************/
void 
ctlMenuButton::DoProcessLeftClick(wxMouseEvent& event)
{
  wxPoint menu_pos;

  if(m_toolBar) {
    wxSize tool_size = m_toolBar->GetToolSize();
    wxSize button_size = GetSize();

    // ** Assume that pulldown is to the right of a standard toolbar button,
    //    so, move the x position back one standard toolbar button's width
    menu_pos.x = - tool_size.GetWidth();
    menu_pos.y =  button_size.GetHeight()/2 + tool_size.GetHeight()/2;

#ifdef __WXMAC__
    wxSize tbar_size = m_toolBar->GetSize();
    wxPoint button_pos = GetPosition();
    int iToolSep = m_toolBar->GetToolSeparation();
    if(iToolSep == 0) iToolSep = 5;

    menu_pos.x += - iToolSep;
    menu_pos.y = tbar_size.GetHeight() - button_pos.y/2;
#endif
  }
  else {
    wxSize button_size;
    button_size = GetSize();
    menu_pos.x = 0;
    menu_pos.y = button_size.GetHeight();
  }

  PopupMenu(m_menu, menu_pos);
}



/************************/
/*** THE WX-MENU-TOOL ***/
/************************/

ctlMenuTool::ctlMenuTool(wxToolBarToolBase *new_tool, int toolId)
{
  m_tool = new_tool;
  m_menu = NULL;
  m_toolId = toolId;
}

ctlMenuTool::~ctlMenuTool()
{
  if(m_menu)  delete m_menu;
}



/****************************/
/*** THE WX-MENU TOOL BAR ***/
/****************************/

/****************************/
/* Creation and Destruction */
/****************************/

ctlMenuToolbar::ctlMenuToolbar(wxFrame* parent, 
                      wxWindowID id, 
                      const wxPoint& pos, 
                      const wxSize& size, 
                      long style, 
                      const wxString& name)

  : wxToolBar(parent, id, pos, size, style, name)
{
  m_frame = parent;
  m_menuTools = NULL;

  Connect(id, wxEVT_LEFT_DOWN, wxMouseEventHandler(ctlMenuToolbar::DoProcessLeftClick) );
}


ctlMenuToolbar::~ctlMenuToolbar()
{
  if(m_menuTools)  delete m_menuTools;
}


/****************/
/* Adding Tools */
/****************/

void
ctlMenuToolbar::NewMenuTool(ctlMenuTool *menu_tool)
{
  if(m_menuTools == NULL) {
    m_menuTools = new ctlMenuToolList();
    m_menuTools->DeleteContents(true);
  }

  m_menuTools->Append(menu_tool);
}


void
ctlMenuToolbar::AddMenuButtonTool(int toolId, 
                         const wxString& label, 
                         const wxBitmap& bitmap1, 
                         const wxString& shortHelpString)
{
#ifndef __WXMAC__
  wxToolBarToolBase *tb_base;
  tb_base = AddTool(toolId, label, bitmap1, shortHelpString, wxITEM_NORMAL);
  NewMenuTool(new ctlMenuTool(tb_base, toolId));
#else
  wxSize tool_size = GetToolSize();
  ctlMacMenuTool *menu_button = new ctlMacMenuTool((ctlMenuToolbar *)this, toolId, bitmap1, 
                       wxDefaultPosition, tool_size, (long int)wxNO_BORDER);
  AddControl(menu_button);
#endif
}


ctlMenuButton *
ctlMenuToolbar::AddMenuPulldownTool(int toolId, 
                         const wxString& label, 
                         const wxString& shortHelpString,
                         wxMenu *popupmenu)
{
  ctlMenuButton *menu_button = new ctlMenuButton(this, toolId, popupmenu);
  AddControl(menu_button);

  return menu_button;
}


void 
ctlMenuToolbar::DoProcessLeftClick(wxMouseEvent& event)
{
  ctlMenuToolList::Node *node = NULL;
  ctlMenuTool *menu_tool = NULL;
  wxToolBarToolBase *clickTool = FindToolForPosition(event.m_x, event.m_y);

  if(clickTool == NULL || m_menuTools == NULL) {  event.Skip();  return;  }

  // search for clickTool in the list of menu tools
  node = m_menuTools->GetFirst();
  for( ; node ; node = node->GetNext()) {
    menu_tool = node->GetData();
    if(menu_tool->m_tool == clickTool) break;
  }
  if(node == NULL) {  event.Skip();  return;  }

  // *** Give others the opportunity to create and/or change the menu
  wxCommandEvent eventFill( wxEVT_FILL_MENU, menu_tool->m_toolId );
  if(menu_tool->m_menu == NULL)  menu_tool->m_menu = new wxMenu;
  eventFill.SetClientData( menu_tool->m_menu );
  GetEventHandler()->ProcessEvent( eventFill );

  wxSize tbar_size = GetSize();
  wxPoint menu_pos;

  menu_pos.x = event.m_x - 20;
  menu_pos.y = tbar_size.GetHeight() - 2;

  PopupMenu(menu_tool->m_menu, menu_pos);
}


#ifdef __WXMAC__

//
// *** Macintosh Implementation
//

ctlMacMenuTool::ctlMacMenuTool(ctlMenuToolbar* parent, 
                   wxWindowID id, 
                   const wxBitmap &bmp, 
                   const wxPoint& pos, 
                   const wxSize& size, 
                   long style) : wxBitmapButton(parent, id, bmp, pos, size, style)
{
  m_toolBar = parent;
  m_menu = NULL;
  Connect(id, wxEVT_LEFT_DOWN, wxMouseEventHandler(ctlMacMenuTool::DoProcessLeftClick) );
}

ctlMacMenuTool::~ctlMacMenuTool()
{
  if(m_menu)  delete m_menu;
}

void 
ctlMacMenuTool::DoProcessLeftClick(wxMouseEvent& event)
{
  wxPoint menu_pos;

  if(!m_menu) {
    m_menu = new wxMenu;
  }

  // *** Give others the opportunity to create and/or change the menu
  wxCommandEvent eventFill( wxEVT_FILL_MENU, GetId() );
  eventFill.SetClientData( m_menu );
  GetEventHandler()->ProcessEvent( eventFill );

  wxSize tool_size = m_toolBar->GetToolSize();
  wxSize tbar_size = m_toolBar->GetSize();

  menu_pos.x = event.m_x - 20;  // Just move it back a nice amount from the mouse click position
  menu_pos.y =  tbar_size.GetHeight() - 1;

  PopupMenu(m_menu, menu_pos);
}

#endif
