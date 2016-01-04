//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// calbox.cpp - Date-picker control box
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#include <wx/wx.h>

#include "ctl/calbox.h"

#if !defined(wxUSE_DATEPICKCTRL) || !wxUSE_DATEPICKCTRL

#if defined(__WXMSW__)
#define TXTCTRL_FLAGS     wxNO_BORDER
#define CALBORDER         0
#define TXTPOSX           0
#define TXTPOSY           1
#elif defined(__WXGTK__)
#define TXTCTRL_FLAGS     0
#define CALBORDER         4
#define TXTPOSX           0
#define TXTPOSY           0
#else
#define TXTCTRL_FLAGS     0
#define CALBORDER         4
#define TXTPOSX           0
#define TXTPOSY           0
#endif


#define CTRLID_TXT  101
#define CTRLID_CAL  102
#define CTRLID_BTN  103
#define CTRLID_PAN  104

BEGIN_EVENT_TABLE(wxCalendarBox, wxControl)
	EVT_BUTTON(CTRLID_BTN, wxCalendarBox::OnClick)
	EVT_TEXT(CTRLID_TXT, wxCalendarBox::OnText)
	EVT_CHILD_FOCUS(wxCalendarBox::OnChildSetFocus)
	EVT_SIZE(wxCalendarBox::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxCalendarBox, wxControl)



wxCalendarBox::wxCalendarBox(wxWindow *parent,
                             wxWindowID id,
                             const wxDateTime &date,
                             const wxPoint &pos,
                             const wxSize &size,
                             long style,
                             const wxString &name)
{
	Init();
	Create(parent, id, date, pos, size, style, name);
}


bool wxCalendarBox::Create(wxWindow *parent,
                           wxWindowID id,
                           const wxDateTime &date,
                           const wxPoint &pos,
                           const wxSize &size,
                           long style,
                           const wxString &name)
{
	wxString txt;
	if (date.IsValid())
		txt = date.FormatISODate();

	if ( !wxControl::Create(parent, id, pos, size,
	                        style | wxCLIP_CHILDREN | wxWANTS_CHARS,
	                        wxDefaultValidator, name) )

	{
		return false;
	}

	InheritAttributes();

	wxSize cs = GetClientSize();
	wxSize bs = ConvertDialogToPixels(wxSize(10, 0));

	wxBitmap bmp(8, 4);
	{
		wxMemoryDC dc;

		dc.SelectObject(bmp);
		dc.SetBrush(wxBrush(GetBackgroundColour()));
		dc.SetPen(wxPen(GetBackgroundColour()));
		dc.DrawRectangle(0, 0, 8, 4);

		dc.SetBrush(wxBrush(GetForegroundColour()));
		dc.SetPen(wxPen(GetForegroundColour()));
		wxPoint pt[3] = { wxPoint(0, 0), wxPoint(6, 0), wxPoint(3, 3) };
		dc.DrawPolygon(3, pt);
		dc.SelectObject(wxNullBitmap);
	}

	m_txt = new wxTextCtrl(this, CTRLID_TXT, txt, wxDefaultPosition, size, TXTCTRL_FLAGS);
	m_txt->Connect(wxID_ANY, wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(wxCalendarBox::OnEditKey), 0, this);
	m_txt->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(wxCalendarBox::OnKillFocus), 0, this);
	SetFormat(wxT("%Y-%m-%d"));

	m_btn = new wxBitmapButton(this, CTRLID_BTN, bmp, wxDefaultPosition, bs);

	m_dlg = new wxDialog(this, CTRLID_CAL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
	m_dlg->SetFont(GetFont());

	wxPanel *panel = new wxPanel(m_dlg, CTRLID_PAN, wxPoint(0, 0), wxDefaultSize, wxSUNKEN_BORDER | wxCLIP_CHILDREN);
	m_cal = new pgCompatCalendarCtrl(panel, CTRLID_CAL, wxDefaultDateTime, wxPoint(0, 0), wxDefaultSize, wxSUNKEN_BORDER);
	m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_SEL_CHANGED, wxCalendarEventHandler(wxCalendarBox::OnSelChange), 0, this);
	m_cal->Connect(wxID_ANY, wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(wxCalendarBox::OnCalKey), 0, this);
	m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_DOUBLECLICKED, wxCalendarEventHandler(wxCalendarBox::OnSelChange), 0, this);
	m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_DAY_CHANGED, wxCalendarEventHandler(wxCalendarBox::OnSelChange), 0, this);
	m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_MONTH_CHANGED, wxCalendarEventHandler(wxCalendarBox::OnSelChange), 0, this);
	m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_YEAR_CHANGED, wxCalendarEventHandler(wxCalendarBox::OnSelChange), 0, this);

	wxWindow *yearControl = m_cal->GetYearControl();

	Connect(wxID_ANY, wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(wxCalendarBox::OnSetFocus));

	wxClientDC dc(yearControl);
	dc.SetFont(m_font);
	wxCoord width, dummy;
	dc.GetTextExtent(wxT("2000"), &width, &dummy);
	width += ConvertDialogToPixels(wxSize(20, 0)).x;

	wxSize calSize = m_cal->GetBestSize();
	wxSize yearSize = yearControl->GetSize();
	yearSize.x = width;

	wxPoint yearPosition = yearControl->GetPosition();

	width = yearPosition.x + yearSize.x + 2 + CALBORDER / 2;
	if (width < calSize.x - 4)
		width = calSize.x - 4;

	int calPos = (width - calSize.x) / 2;
	if (calPos == -1)
	{
		calPos = 0;
		width += 2;
	}
	m_cal->SetSize(calPos, 0, calSize.x, calSize.y);
	yearControl->SetSize(width - yearSize.x - CALBORDER / 2, yearPosition.y, yearSize.x, yearSize.y);
	m_cal->GetMonthControl()->Move(0, 0);

	SetInitialSize(size);

	panel->SetClientSize(width + CALBORDER / 2, calSize.y - 2 + CALBORDER);
	m_dlg->SetClientSize(panel->GetSize());

	return true;
}


void wxCalendarBox::Init()
{
	m_dlg = NULL;
	m_txt = NULL;
	m_cal = NULL;
	m_btn = NULL;

	m_dropped = false;
	m_ignoreDrop = false;
}


bool wxCalendarBox::Destroy()
{
	if (m_cal)
		m_cal->Destroy();
	if (m_dlg)
		m_dlg->Destroy();
	if (m_txt)
		m_txt->Destroy();
	if (m_btn)
		m_btn->Destroy();

	m_dlg = NULL;
	m_txt = NULL;
	m_cal = NULL;
	m_btn = NULL;

	return wxControl::Destroy();
}


void wxCalendarBox::DoMoveWindow(int x, int y, int w, int h)
{
	wxControl::DoMoveWindow(x, y, w, h);
	if (m_dropped)
	{
		DropDown();
	}
}


wxSize wxCalendarBox::DoGetBestSize() const
{
	int bh = m_btn->GetBestSize().y;
	int eh = m_txt->GetBestSize().y;
	return wxSize(100, bh > eh ? bh : eh);
}


void wxCalendarBox::OnSize(wxSizeEvent &event)
{
	if ( m_btn )
	{
		wxSize sz = GetClientSize();

		wxSize bs = m_btn->GetSize();
		int eh = m_txt->GetBestSize().y;

		m_txt->SetSize(TXTPOSX, TXTPOSY, sz.x - bs.x - TXTPOSX, sz.y > eh ? eh - TXTPOSY : sz.y - TXTPOSY);
		m_btn->SetSize(sz.x - bs.x, 0, bs.x, sz.y);
	}

	event.Skip();
}


bool wxCalendarBox::Show(bool show)
{
	if ( !wxControl::Show(show) )
	{
		return false;
	}

	if (!show)
	{
		if (m_dlg)
		{
			m_dlg->Hide();
			m_dropped = false;
		}
	}

	return true;
}


bool wxCalendarBox::Enable(bool enable)
{
	if ( !wxControl::Enable(enable) )
	{
		return false;
	}

	if (m_cal)
	{
		if (enable)
			m_cal->Show();
		else
			m_cal->Hide();
	}

	if (m_btn)
		m_btn->Enable(enable);
	return true;
}


bool wxCalendarBox::SetFormat(const wxChar *fmt)
{
	wxDateTime dt;
	dt.ParseFormat(wxT("2003-10-13"), wxT("%Y-%m-%d"));
	wxString str = dt.Format(fmt);
	const wxChar *p = (const wxChar *) str;

	m_format = wxEmptyString;

	while (*p)
	{
		int n = wxAtoi(p);
		if (n == dt.GetDay())
		{
			m_format.Append(wxT("%d"));
			p += 2;
		}
		else if (n == (int)dt.GetMonth() + 1)
		{
			m_format.Append(wxT("%m"));
			p += 2;
		}
		else if (n == dt.GetYear())
		{
			m_format.Append(wxT("%Y"));
			p += 4;
		}
		else
			m_format.Append(*p++);
	}

	if (m_txt)
	{
		wxArrayString valArray;
		wxChar c;
		for (c = '0'; c <= '9'; c++)
			valArray.Add(wxString(c, 1));
		const wxChar *p = (const wxChar *) m_format;
		while (*p)
		{
			if (*p == '%')
				p += 2;
			else
				valArray.Add(wxString(*p++, 1));
		}
		wxTextValidator tv(wxFILTER_INCLUDE_CHAR_LIST);
		tv.SetIncludes(valArray);

		m_txt->SetValidator(tv);
	}
	return true;
}


wxDateTime wxCalendarBox::GetValue()
{
	wxDateTime dt;
	wxString txt = m_txt->GetValue();

	if (!txt.IsEmpty())
		dt.ParseFormat(txt, m_format);

	return dt;
}


bool wxCalendarBox::SetValue(const wxDateTime &date)
{
	bool retval = false;

	if (m_cal)
	{
		if (date.IsValid())
			m_txt->SetValue(date.FormatISODate());
		else
			m_txt->SetValue(wxEmptyString);
	}
	return retval;
}


void wxCalendarBox::DropDown(bool down)
{
	if (m_dlg)
	{
		if (down)
		{
			if (m_txt->GetValue().IsEmpty())
				m_cal->SetDate(wxDateTime::Today());
			else
			{
				wxDateTime dt;
				dt.ParseFormat(m_txt->GetValue(), m_format);
				m_cal->SetDate(dt);
			}
			wxPoint pos = GetParent()->ClientToScreen(GetPosition());

			m_dlg->Move(pos.x, pos.y + GetSize().y);
			m_dlg->Show();
			m_dropped = true;
		}
		else
		{
			if (m_dropped)
				m_dlg->Hide();
			m_dropped = false;
		}
	}
}


void wxCalendarBox::OnChildSetFocus(wxChildFocusEvent &ev)
{
	ev.Skip();
	m_ignoreDrop = false;

	wxWindow  *w = (wxWindow *)ev.GetEventObject();
	while (w)
	{
		if (w == m_dlg)
			return;
		w = w->GetParent();
	}

	if (m_dropped)
	{
		DropDown(false);
		if (ev.GetEventObject() == m_btn)
			m_ignoreDrop = true;
	}
}


void wxCalendarBox::OnClick(wxCommandEvent &event)
{
	if (m_ignoreDrop)
	{
		m_ignoreDrop = false;
		m_txt->SetFocus();
	}
	else
	{
		DropDown();
		m_cal->SetFocus();
	}
}


void wxCalendarBox::OnSetFocus(wxFocusEvent &ev)
{
	if (m_txt)
	{
		m_txt->SetFocus();
		m_txt->SetSelection(0, 100);
	}
}


void wxCalendarBox::OnKillFocus(wxFocusEvent &ev)
{
	wxDateTime dt;
	dt.ParseFormat(m_txt->GetValue(), m_format);
	if (!dt.IsValid())
		m_txt->SetValue(wxEmptyString);
	else
		m_txt->SetValue(dt.Format(m_format));
}


void wxCalendarBox::OnSelChange(wxCalendarEvent &ev)
{
	if (m_cal)
	{
		m_txt->SetValue(m_cal->GetDate().FormatISODate());
		if (ev.GetEventType() == wxEVT_CALENDAR_DOUBLECLICKED)
		{
			DropDown(false);
			m_txt->SetFocus();
		}
	}
	ev.SetEventObject(this);
	ev.SetId(GetId());

#if wxCHECK_VERSION(2, 9, 0)
	GetParent()->GetEventHandler()->ProcessEvent(ev);
#else
	GetParent()->ProcessEvent(ev);
#endif

}


void wxCalendarBox::OnText(wxCommandEvent &ev)
{
	ev.SetEventObject(this);
	ev.SetId(GetId());

#if wxCHECK_VERSION(2, 9, 0)
	GetParent()->GetEventHandler()->ProcessEvent(ev);
#else
	GetParent()->ProcessEvent(ev);
#endif

	// We'll create an additional event if the date is valid.
	// If the date isn't valid, the user's probably in the middle of typing
	wxString txt = m_txt->GetValue();
	wxDateTime dt;
	if (!txt.IsEmpty())
		dt.ParseFormat(txt, m_format);

#if wxCHECK_VERSION(2, 9, 0)
	wxCalendarEvent cev(m_cal, dt, wxEVT_CALENDAR_SEL_CHANGED);
#else
	wxCalendarEvent cev(m_cal, wxEVT_CALENDAR_SEL_CHANGED);
	cev.SetDate(dt);
#endif
	cev.SetEventObject(this);
	cev.SetId(GetId());

#if wxCHECK_VERSION(2, 9, 0)
	GetParent()->GetEventHandler()->ProcessEvent(cev);
#else
	GetParent()->ProcessEvent(cev);
#endif

}


void wxCalendarBox::OnEditKey(wxKeyEvent &ev)
{
	if (ev.GetKeyCode() == WXK_DOWN && !ev.HasModifiers())
		DropDown();
	else
		ev.Skip();
}


void wxCalendarBox::OnCalKey(wxKeyEvent &ev)
{
	if (ev.GetKeyCode() == WXK_ESCAPE && !ev.HasModifiers())
		DropDown(false);
	else
		ev.Skip();
}

#endif // !defined(wxUSE_DATEPICKCTRL) || !wxUSE_DATEPICKCTRL
