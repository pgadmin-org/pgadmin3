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

BEGIN_EVENT_TABLE(wxCalendarBox, wxControl)
//    EVT_PAINT(wxCalendarBox::OnPaint)
    EVT_BUTTON(CTRLID_BTN, wxCalendarBox::OnClick)
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

    wxSize cs=GetClientSize();
    wxSize bs=ConvertDialogToPixels(wxSize(10, 0));


    m_txt=new wxTextCtrl(this, CTRLID_TXT, txt, wxPoint(0,0), wxSize(cs.x-bs.x-1, cs.y), wxNO_BORDER); //WS_VISIBLE);

    wxBitmap bmp(8, 4);
    {
        wxMemoryDC dc;

        dc.SelectObject(bmp);
        dc.SetBrush(wxBrush(GetBackgroundColour()));
        dc.SetPen(wxPen(GetBackgroundColour()));
        dc.DrawRectangle(0,0, 8,4);

        dc.SetBrush(*wxBLACK_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
        wxPoint pt[3] = { wxPoint(0,0), wxPoint(6,0), wxPoint(3,3) };
        dc.DrawPolygon(3, pt);
        dc.SelectObject(wxNullBitmap);
    }
    
    m_btn = new wxBitmapButton(this, CTRLID_BTN, bmp, wxPoint(cs.x - bs.x, 0), wxSize(bs.x, cs.y));

    m_dlg = new wxDialog(this, CTRLID_CAL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    m_dlg->Connect(wxID_ANY, wxID_ANY, wxEVT_ACTIVATE, (wxObjectEventFunction)(wxCalendarEventFunction)&wxCalendarBox::OnActivate, 0, this);

    m_cal = new wxCalendarCtrl(m_dlg, CTRLID_CAL, wxDefaultDateTime, wxPoint(0,0), wxDefaultSize, wxSUNKEN_BORDER);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_SEL_CHANGED, (wxObjectEventFunction)&wxCalendarBox::OnSelChange, 0, this);

    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_DOUBLECLICKED, (wxObjectEventFunction)&wxCalendarBox::OnSelChange, 0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_DAY_CHANGED, (wxObjectEventFunction)&wxCalendarBox::OnSelChange, 0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_MONTH_CHANGED, (wxObjectEventFunction)&wxCalendarBox::OnSelChange, 0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_YEAR_CHANGED, (wxObjectEventFunction)&wxCalendarBox::OnSelChange, 0, this);
    //Connect(wxID_ANY, wxID_ANY, wxEVT_SET_FOCUS, (wxObjectEventFunction)wxCalendarBox::OnSetFocus);

#if 1
    // GetBestSize doesn't work correctly
    // until corrected, we calculate it ourselves
    wxSize siz=m_cal->GetBestSize();
    wxClientDC dc(this);
    dc.SetFont(m_font);

    wxCoord width, dummy, width2=0;

    dc.GetTextExtent(wxT("2000"), &width, &dummy);

    wxDateTime::Month month;
    for ( month = wxDateTime::Jan; month <= wxDateTime::Dec; wxNextMonth(month) )
    {
        wxCoord tmpwidth;
        dc.GetTextExtent(wxDateTime::GetMonthName(month), &tmpwidth, &dummy);
        if (tmpwidth > width2)
            width2 = tmpwidth;
    }


    width += width2 + 15 + 2*ConvertDialogToPixels(wxSize(18,0)).x;

    if (width > siz.x)
        siz.x = width;

    m_cal->SetSize(siz);
#endif


    m_dlg->SetClientSize(m_cal->GetSize());

    return TRUE;
}


void wxCalendarBox::Init()
{
    m_dlg = NULL;
    m_txt = NULL;
    m_cal = NULL;
    m_btn = NULL;

    SetFormat(wxT("%x"));

    m_dropped = false;
    m_processing = false;
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


void wxCalendarBox::OnClick(wxMouseEvent& event)
{
    if (m_dlg)
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
}


void wxCalendarBox::OnSetFocus(wxFocusEvent &ev)
{
    if (m_txt)
        m_txt->SetFocus();
}


void wxCalendarBox::OnKillFocus(wxFocusEvent &ev)
{
}


void wxCalendarBox::OnActivate(wxActivateEvent &ev)
{
    if (!ev.GetActive())
    {
        m_dlg->Hide();
        m_dropped = false;
    }
}


void wxCalendarBox::OnSelChange(wxCalendarEvent &ev)
{
    if (m_cal)
    {
        m_txt->SetValue(m_cal->GetDate().FormatDate());
        if (ev.GetEventType() == wxEVT_CALENDAR_DOUBLECLICKED)
        {
            m_dlg->Hide();
            m_dropped = false;
            m_txt->SetFocus();
        }
    }
    GetParent()->ProcessEvent(ev);
}


void wxCalendarBox::OnPaintButton(wxPaintEvent &ev)
{
    if (m_processing)
    {
        ev.Skip();
        return;
    }
    m_processing = true;
    m_btn->ProcessEvent(ev);
    m_processing = false;

    wxPaintDC dc(m_btn);
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.SetPen(*wxBLACK_PEN);

    wxSize cs=m_btn->GetClientSize();
    wxPoint pt[3];


    pt[0].x = cs.x/2 - 3;
    pt[0].y = cs.y/2 - 2;
    pt[1].x = cs.x/2 + 3;
    pt[1].y = cs.y/2 - 2;
    pt[2].x = cs.x/2;
    pt[2].y = cs.y/2 + 1;

    dc.SetClippingRegion(pt[0].x, pt[0].y, pt[1].x+1, pt[2].y+1);
    dc.DrawPolygon(3, pt);
    dc.DestroyClippingRegion();
    ev.Skip();
}
