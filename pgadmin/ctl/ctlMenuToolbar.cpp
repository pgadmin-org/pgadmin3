//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
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
static const char *pulldown_xpm[] =
{
	"16 16 2 1",
	". c Black",
	"  c None",
	/* pixels */
	"                ",
	"                ",
	"                ",
	"                ",
	"                ",
	"                ",
	"     .......    ",
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

//////////////////
// ctlMenuButton
//////////////////

void ctlMenuButton::Create(wxWindow *window, wxToolBar *toolBar, int ID, wxMenu *menu)
{
	wxBitmap bmpPulldown(pulldown_xpm);
	wxSize pulldownSize;

#ifdef __WXMSW__
	pulldownSize.Set(12, 16);
#else
#ifdef __WXGTK__
	pulldownSize.Set(18, 16);
#else
	pulldownSize.Set(10, 16);
#endif
#endif

	m_toolBar = toolBar;
	m_menu = menu;

	((wxBitmapButton *)this)->Create(window, ID, bmpPulldown, wxDefaultPosition, pulldownSize, wxNO_BORDER);
	Connect(ID, wxEVT_LEFT_DOWN, wxMouseEventHandler(ctlMenuButton::DoProcessLeftClick) );
}


void ctlMenuButton::DoProcessLeftClick(wxMouseEvent &event)
{
	wxPoint menu_pos;

	if(m_toolBar)
	{
		wxSize tool_size = m_toolBar->GetToolSize();
		wxSize button_size = GetSize();

		// ** Assume that pulldown is to the right of a standard toolbar button,
		//    so, move the x position back one standard toolbar button's width
		menu_pos.x = - tool_size.GetWidth();
		menu_pos.y =  button_size.GetHeight() / 2 + tool_size.GetHeight() / 2;

#ifdef __WXMAC__
		wxSize tbar_size = m_toolBar->GetSize();
		wxPoint button_pos = GetPosition();
		int iToolSep = m_toolBar->GetToolSeparation();
		if(iToolSep == 0) iToolSep = 5;

		menu_pos.x += - iToolSep;
		menu_pos.y = tbar_size.GetHeight() - button_pos.y / 2;
#endif
	}
	else
	{
		wxSize button_size;
		button_size = GetSize();
		menu_pos.x = 0;
		menu_pos.y = button_size.GetHeight();
	}

	DoPopupMenu(m_menu, menu_pos.x, menu_pos.y);
}



////////////////
// ctlMenuTool
////////////////

ctlMenuTool::ctlMenuTool(wxToolBarToolBase *new_tool, int toolId)
{
	m_tool = new_tool;
	m_menu = NULL;
	m_toolId = toolId;
}


////////////////////
// ctlMenuToolbar
////////////////////

ctlMenuToolbar::ctlMenuToolbar(wxFrame *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
	: wxToolBar(parent, id, pos, size, style, name)
{
	m_frame = parent;
	m_menuTools = NULL;

	Connect(id, wxEVT_LEFT_DOWN, wxMouseEventHandler(ctlMenuToolbar::DoProcessLeftClick) );
}


ctlMenuToolbar::~ctlMenuToolbar()
{
	if(m_menuTools)
		delete m_menuTools;
}

ctlMenuButton *ctlMenuToolbar::AddMenuPulldownTool(int toolId, const wxString &label, const wxString &shortHelpString, wxMenu *popupmenu)
{
	ctlMenuButton *menu_button = new ctlMenuButton(this, toolId, popupmenu);

	AddControl(menu_button);

	return menu_button;
}


void ctlMenuToolbar::DoProcessLeftClick(wxMouseEvent &event)
{
	ctlMenuToolList::Node *node = NULL;
	ctlMenuTool *menu_tool = NULL;
	wxToolBarToolBase *clickTool = FindToolForPosition(event.m_x, event.m_y);

	if(clickTool == NULL || m_menuTools == NULL)
	{
		event.Skip();
		return;
	}

	// search for clickTool in the list of menu tools
	node = m_menuTools->GetFirst();
	for( ; node ; node = node->GetNext())
	{
		menu_tool = node->GetData();
		if(menu_tool->m_tool == clickTool)
			break;
	}

	if(node == NULL)
	{
		event.Skip();
		return;
	}

	wxSize tbar_size = GetSize();
	wxPoint menu_pos;

	menu_pos.x = event.m_x - 20;
	menu_pos.y = tbar_size.GetHeight() - 2;

	PopupMenu(menu_tool->m_menu, menu_pos);
}
