/////////////////////////////////   //////////////////////////////////////////////
// Name:        calbox.cpp
// Purpose:     date-picker control box
// Author:      Andreas Pflug
// Modified by:
// Created:     12.10.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Andreas Pflug <pgadmin@pse-consulting.de>
// Licence:     Artistic Licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>

#include "calbox.h"

#define CTRLID_TXT  101
#define CTRLID_CAL  102
#define CTRLID_BTN  103
#define CTRLID_PAN  104

BEGIN_EVENT_TABLE(wxCalendarBox, wxControl)
    EVT_BUTTON(CTRLID_BTN, wxCalendarBox::OnClick)
    EVT_TEXT(CTRLID_TXT, wxCalendarBox::OnText)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxCalendarBox, wxControl)



wxCalendarBox::wxCalendarBox(wxWindow *parent,
                            wxWindowID id,
                            const wxDateTime& date,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    Init();
    Create(parent, id, date, pos, size, style, name);
}


bool wxCalendarBox::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxDateTime& date,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    wxString txt;
    if (date.IsValid())
        txt = date.FormatDate();

    if ( !wxControl::Create(parent, id, pos, size,
                            style | wxCLIP_CHILDREN | wxWANTS_CHARS,
                            wxDefaultValidator, name) )

    {
        return false;
    }

    SetWindowStyle(style | wxWANTS_CHARS);
    SetFont(parent->GetFont());

    wxSize cs=GetClientSize();
    wxSize bs=ConvertDialogToPixels(wxSize(10, 0));

    wxBitmap bmp(8, 4);
    {
        wxMemoryDC dc;

        dc.SelectObject(bmp);
        dc.SetBrush(wxBrush(GetBackgroundColour()));
        dc.SetPen(wxPen(GetBackgroundColour()));
        dc.DrawRectangle(0,0, 8,4);

        dc.SetBrush(wxBrush(GetForegroundColour()));
        dc.SetPen(wxPen(GetForegroundColour()));
        wxPoint pt[3] = { wxPoint(0,0), wxPoint(6,0), wxPoint(3,3) };
        dc.DrawPolygon(3, pt);
        dc.SelectObject(wxNullBitmap);
    }
    
    m_txt=new wxTextCtrl(this, CTRLID_TXT, txt, wxPoint(0,0), wxSize(cs.x-bs.x-1, cs.y), wxNO_BORDER);
    m_txt->Connect(wxID_ANY, wxID_ANY, wxEVT_KEY_DOWN, (wxObjectEventFunction)&wxCalendarBox::OnEditKey, 0, this);
    m_txt->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, (wxObjectEventFunction)&wxCalendarBox::OnKillFocus, 0, this);
    SetFormat(wxT("%x"));

    m_btn = new wxBitmapButton(this, CTRLID_BTN, bmp, wxPoint(cs.x - bs.x, 0), wxSize(bs.x, cs.y));

    m_dlg = new wxDialog(this, CTRLID_CAL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    m_dlg->Connect(wxID_ANY, wxID_ANY, wxEVT_ACTIVATE, (wxObjectEventFunction)&wxCalendarBox::OnActivate, 0, this);
    m_dlg->SetFont(GetFont());

    wxPanel *panel=new wxPanel(m_dlg, CTRLID_PAN, wxPoint(0, 0), wxDefaultSize, wxSUNKEN_BORDER|wxCLIP_CHILDREN);
    m_cal = new wxCalendarCtrl(panel, CTRLID_CAL, wxDefaultDateTime, wxPoint(0,0), wxDefaultSize, wxSUNKEN_BORDER);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_SEL_CHANGED, (wxObjectEventFunction)&wxCalendarBox::OnSelChange, 0, this);
    m_cal->Connect(wxID_ANY, wxID_ANY, wxEVT_KEY_DOWN, (wxObjectEventFunction)&wxCalendarBox::OnCalKey, 0, this);

    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_DOUBLECLICKED, (wxObjectEventFunction)&wxCalendarBox::OnSelChange, 0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_DAY_CHANGED, (wxObjectEventFunction)&wxCalendarBox::OnSelChange, 0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_MONTH_CHANGED, (wxObjectEventFunction)&wxCalendarBox::OnSelChange, 0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_YEAR_CHANGED, (wxObjectEventFunction)&wxCalendarBox::OnSelChange, 0, this);
    Connect(wxID_ANY, wxID_ANY, wxEVT_SET_FOCUS, (wxObjectEventFunction)&wxCalendarBox::OnSetFocus);

    wxWindow *yearControl = m_cal->GetYearControl();

    wxClientDC dc(yearControl);
    dc.SetFont(m_font);
    wxCoord width, dummy;
    dc.GetTextExtent(wxT("2000"), &width, &dummy);
    width += ConvertDialogToPixels(wxSize(20,0)).x;

    wxSize calSize = m_cal->GetBestSize();
    wxSize yearSize = yearControl->GetSize();
    yearSize.x = width;

    wxPoint yearPosition = yearControl->GetPosition();

#ifdef __WXMSW__
#define CALBORDER   0
#else
#define CALBORDER   4
#endif

    width = yearPosition.x + yearSize.x+2+CALBORDER/2;
    if (width < calSize.x-4)
        width = calSize.x-4;

	int calPos = (width-calSize.x)/2;
	if (calPos == -1)
	{
		calPos = 0;
		width += 2;
	}
    m_cal->SetSize(calPos, 0, calSize.x, calSize.y);
    yearControl->SetSize(width-yearSize.x-CALBORDER, yearPosition.y, yearSize.x, yearSize.y);
	m_cal->GetMonthControl()->Move(0, 0);



    panel->SetClientSize(width, calSize.y-2+CALBORDER);
    m_dlg->SetClientSize(panel->GetSize());

    return TRUE;
}


void wxCalendarBox::Init()
{
    m_dlg = NULL;
    m_txt = NULL;
    m_cal = NULL;
    m_btn = NULL;

    m_dropped = false;
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
        wxMouseEvent ev;
        OnClick(ev);
    }
}


bool wxCalendarBox::Show(bool show)
{
    if ( !wxControl::Show(show) )
    {
        return FALSE;
    }

    if (!show)
    {
        if (m_dlg)
        {
            m_dlg->Hide();
            m_dropped = false;
        }
    }

    return TRUE;
}


bool wxCalendarBox::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
    {
        return FALSE;
    }

    if (!enable)
    {
        if (m_cal)
            m_cal->Hide();
    }
    if (m_btn)
        m_btn->Enable(enable);
    return TRUE;
}


bool wxCalendarBox::SetFormat(const wxChar *fmt)
{
    wxDateTime dt;
    dt.ParseFormat(wxT("2003-10-13"), wxT("%Y-%m-%d"));
    wxString str=dt.Format(fmt);
    wxChar *p=(wxChar*)str.c_str();

    m_format=wxEmptyString;

    while (*p)
    {
        int n=wxAtoi(p);
        if (n == dt.GetDay())
        {
            m_format.Append(wxT("%d"));
            p += 2;
        }
        else if (n == (int)dt.GetMonth()+1)
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
        wxStringList valList;
        wxChar c;
        for (c='0'; c <= '9'; c++)
            valList.Add(wxString(c, 1));
        wxChar *p=(wxChar*)m_format.c_str();
        while (*p)
        {
            if (*p == '%')
                p += 2;
            else
                valList.Add(wxString(*p++, 1));
        }
        wxTextValidator tv(wxFILTER_INCLUDE_CHAR_LIST);
        tv.SetIncludeList(valList);
        
        m_txt->SetValidator(tv);
    }
    return true;
}


wxDateTime wxCalendarBox::GetDate()
{
    wxDateTime dt;
    wxString txt=m_txt->GetValue();

    if (!txt.IsEmpty())
        dt.ParseFormat(txt, m_format);

    return dt;
}


bool wxCalendarBox::SetDate(const wxDateTime& date)
{
    bool retval=false;

    if (m_cal)
    {
        if (date.IsValid())
            m_txt->SetValue(date.FormatDate());
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
            wxPoint pos=GetParent()->ClientToScreen(GetPosition());

            m_dlg->Move(pos.x, pos.y + GetSize().y);
            m_dlg->Show();
            m_dropped = true;
        }
        else
        {
            m_dlg->Hide();
            m_dropped = false;
        }
    }
}

    
void wxCalendarBox::OnClick(wxMouseEvent& event)
{
    DropDown();
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


void wxCalendarBox::OnActivate(wxActivateEvent &ev)
{
    if (!ev.GetActive())
    {
        DropDown(false);
    }
}


void wxCalendarBox::OnSelChange(wxCalendarEvent &ev)
{
    if (m_cal)
    {
        m_txt->SetValue(m_cal->GetDate().FormatDate());
        if (ev.GetEventType() == wxEVT_CALENDAR_DOUBLECLICKED)
        {
            DropDown(false);
            m_txt->SetFocus();
        }
    }
    ev.SetEventObject(this);
    ev.SetId(GetId());
    GetParent()->ProcessEvent(ev);
}


void wxCalendarBox::OnText(wxCommandEvent &ev)
{
    ev.SetEventObject(this);
    ev.SetId(GetId());
    GetParent()->ProcessEvent(ev);

    // We'll create an additional event if the date is valid.
    // If the date isn't valid, the user's probable in the middle of typing
    wxString txt=m_txt->GetValue();
    wxDateTime dt;
    if (!txt.IsEmpty())
    {
        dt.ParseFormat(txt, m_format);
        if (!dt.IsValid())
            return;
    }

    wxCalendarEvent cev(m_cal, wxEVT_CALENDAR_SEL_CHANGED);
    cev.SetEventObject(this);
    cev.SetId(GetId());
    cev.SetDate(dt);

    GetParent()->ProcessEvent(cev);
}


void wxCalendarBox::OnEditKey(wxKeyEvent & ev)
{
    if (ev.GetKeyCode() == WXK_DOWN && !ev.HasModifiers())
        DropDown();
    else
        ev.Skip();
}


void wxCalendarBox::OnCalKey(wxKeyEvent & ev)
{
    if (ev.GetKeyCode() == WXK_ESCAPE && !ev.HasModifiers())
        DropDown(false);
    else
        ev.Skip();
}
