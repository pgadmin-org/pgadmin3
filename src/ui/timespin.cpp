///////////////////////////////////////////////////////////////////////////////
// Name:        timespin.cpp
// Purpose:     timeSpan SpinCtrl
// Author:      Andreas Pflug
// Modified by:
// Created:     19.10.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Andreas Pflug <pgadmin@pse-consulting.de>
// Licence:     Artistic Licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include "timespin.h"


#define CTRLID_TXT  101
#define CTRLID_SPN  102


BEGIN_EVENT_TABLE(wxTimeSpinCtrl, wxControl)
    EVT_TEXT(CTRLID_TXT, wxTimeSpinCtrl::OnText)
    EVT_NAVIGATION_KEY(wxTimeSpinCtrl::OnNavigate)
    EVT_SPIN_UP(CTRLID_SPN, wxTimeSpinCtrl::OnSpinUp)
    EVT_SPIN_DOWN(CTRLID_SPN, wxTimeSpinCtrl::OnSpinDown)
    EVT_SET_FOCUS(wxTimeSpinCtrl::OnSetFocus)
END_EVENT_TABLE()


IMPLEMENT_DYNAMIC_CLASS(wxTimeSpinCtrl, wxControl)


wxTimeSpinCtrl::wxTimeSpinCtrl(wxWindow *parent,
                            wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    Init();
    Create(parent, id, pos, size, style, name);
}


bool wxTimeSpinCtrl::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    wxControl::Create(parent, id, pos, size, style & ~(wxSP_WRAP|wxSP_ARROW_KEYS), wxDefaultValidator, name);
    SetFont(parent->GetFont());

    m_spn=new wxSpinButton(this, CTRLID_SPN, wxDefaultPosition, wxDefaultSize, wxSP_WRAP | wxSP_VERTICAL | (style & wxSP_ARROW_KEYS));

    wxSize cs=GetClientSize();
    wxSize ss=m_spn->GetBestSize();

    m_txt=new wxTextCtrl(this, CTRLID_TXT, wxEmptyString, wxPoint(0,0), wxSize(cs.x-ss.x, cs.y), wxNO_BORDER|wxWANTS_CHARS);
    m_txt->Connect(wxID_ANY, wxID_ANY, wxEVT_KEY_DOWN, (wxObjectEventFunction)&wxTimeSpinCtrl::OnEditKey, 0, this);
    m_txt->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, (wxObjectEventFunction)&wxTimeSpinCtrl::OnKillFocus, 0, this);

    wxStringList valList;
    wxChar c;
    for (c='0'; c <= '9'; c++)
        valList.Add(wxString(c, 1));
    valList.Add(wxT(":"));
    wxTextValidator tv(wxFILTER_INCLUDE_CHAR_LIST);
    tv.SetIncludeList(valList);
    m_txt->SetValidator(tv);

    m_spn->SetSize(ss.x, cs.y);
    m_spn->SetPosition(wxPoint(cs.x-ss.x, 0));

    canWrap = (style & wxSP_WRAP) != 0;
    SetMax(24*60*60 -1);

    return true;
}


bool wxTimeSpinCtrl::Destroy()
{
    if (m_txt)
    {
        m_txt->Destroy();
        m_txt = NULL;
    }
    if (m_spn)
    {
        m_spn->Destroy();
        m_spn = NULL;
    }
    return true;
}


void wxTimeSpinCtrl::Init()
{
    m_txt = NULL;
    m_spn = NULL;
    spinValue = 0;
}


bool wxTimeSpinCtrl::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
    {
        return FALSE;
    }

    m_txt->Enable(enable);
    m_spn->Enable(enable);

    return TRUE;
}

void wxTimeSpinCtrl::SetMax(long seconds, bool useDay)
{
    hasDay = useDay;
    if (hasDay)
        m_format = wxT("%D:%H:%M:%S");
    else
        m_format = wxT("%H:%M:%S");
    maxSpinValue = seconds+1;
    if (maxSpinValue < 2)
        maxSpinValue = 2;
    m_spn->SetRange(0, 32767);
}


bool wxTimeSpinCtrl::SetTime(const wxDateTime& time)
{
    if (time.IsValid())
    {
        wxDateTime tt(time);
        tt.ResetTime();
        return SetValue(time - tt);
    }
    else
    {
        m_txt->SetValue(wxEmptyString);
        return false;
    }
}


bool wxTimeSpinCtrl::SetValue(const wxTimeSpan& span)
{
    m_txt->SetValue(span.Format(m_format));
    spinValue = span.GetSeconds().GetLo();
    return true;
}



wxTimeSpan wxTimeSpinCtrl::GetValue()
{
    return wxTimeSpan(0, 0, spinValue);
}


int wxTimeSpinCtrl::GetTimePart()
{
    long from, to;
    m_txt->GetSelection(&from, &to);
    wxString strAfter=m_txt->GetRange(to, 9999);
    int cnt=0;
    wxChar *p=(wxChar*)strAfter.c_str();
    while (*p)
    {
        if (*p++ == ':')
            cnt++;
    }
    return cnt;
}


void wxTimeSpinCtrl::Highlight(int tp)
{
    wxString txt=m_txt->GetValue();
    long from=0, to;
    int i;

    for (i=3-tp - (hasDay ? 0 : 1) ; i > 0 ; i--)
        from += txt.Mid(from).Find(':') +1;

    to=txt.Mid(from).Find(':') + from;
    if (to < from)
        to = 999;

    m_txt->SetSelection(from, to);
}

        
void wxTimeSpinCtrl::OnSpinUp(wxSpinEvent &ev)
{
    m_txt->SetFocus();
    DoSpin(1);
}

void wxTimeSpinCtrl::OnSpinDown(wxSpinEvent &ev)
{
    m_txt->SetFocus();
    DoSpin(-1);
}

void wxTimeSpinCtrl::DoSpin(int diff)
{
    int tp=GetTimePart();
    long oldValue, maxValue, mult;

    switch (tp)
    {
        case 0: 
            oldValue = spinValue;
            maxValue = 60;
            mult = 1;
            break;
        case 1:
            oldValue = spinValue/60;
            maxValue = 60;
            mult = 60;
            break;

        case 2:
            oldValue = spinValue/60/60;
            if (hasDay)
                maxValue = 24;
            else
                maxValue = maxSpinValue/60/60;
            mult = 60*60;
            break;
        case 3:
            oldValue = spinValue/60/60/24;
            maxValue = maxSpinValue/60/60/24;
            mult = 60*60*24;
            break;
        default:
            // can't happen
            break;
    }
    oldValue %= maxValue;
    int newValue = oldValue + diff;

    if (!canWrap)
    {
        if (newValue < 0)
            diff += maxValue;
        else if (newValue >= maxValue)
            diff -= maxValue;
    }

    long newSpinValue = spinValue + diff*mult;
    if (newSpinValue < 0)
        newSpinValue = maxSpinValue-1;
    else if (newSpinValue > maxSpinValue)
        newSpinValue = 0;

    if (spinValue != newSpinValue)
    {
        spinValue = newSpinValue;
        wxTimeSpan span(0, 0, spinValue);
        wxString txt=span.Format(m_format);
        m_txt->SetValue(txt);
        Highlight(tp);

        wxSpinEvent ev;
        ev.SetEventObject(this);
        ev.SetId(GetId());
        GetParent()->ProcessEvent(ev);
    }
}


void wxTimeSpinCtrl::OnText(wxNotifyEvent &ev)
{
    long time=GetTextTime();
    if (time >= 0)
    {
        spinValue = time;
        ev.SetEventObject(this);
        ev.SetId(GetId());
        GetParent()->ProcessEvent(ev);
    }
}


void wxTimeSpinCtrl::OnNavigate(wxNavigationKeyEvent &ev)
{
    if (wxWindow::FindFocus() == m_txt)
    {
        int tp=GetTimePart();
        if (ev.GetDirection())
            tp++;
        else
            tp--;
        if (tp >= 0 && tp < 3 || (hasDay && tp == 3))
        {
            Highlight(tp);
            return;
        }
    }
    ev.Skip();
}


void wxTimeSpinCtrl::OnEditKey(wxKeyEvent &ev)
{
    if (!ev.HasModifiers())
    {
        switch(ev.GetKeyCode())
        {
            case WXK_LEFT:
            {
                if (!ev.ShiftDown())
                {
                    int tp=GetTimePart()+1;
                    if (tp < 3 || (hasDay && tp == 3))
                    {
                        Highlight(tp);
                        return;
                    }
                }
                break;
            }
            case WXK_RIGHT:
            {
                if (!ev.ShiftDown())
                {
                    int tp=GetTimePart()-1;
                    if (tp >= 0)
                    {
                        Highlight(tp);
                        return;
                    }
                }
                break;
            }
            case WXK_UP:
                DoSpin(1);
                return;
                break;
            case WXK_DOWN:
                DoSpin(-1);
                return;
                break;
            default:
                break;
        }
    }
    ev.Skip();
}


long wxTimeSpinCtrl::GetTextTime()
{
    int t1, t2, t3, t4;
    int scanned=wxSscanf(m_txt->GetValue(), wxT("%d:%d:%d:%d"), &t1, &t2, &t3, &t4);

    long time=0;

    switch (scanned)
    {
        case 1:
            if (t1 >= 0)
                return t1;
            break;
        case 2:
            if (t1 >= 0 && t2 >= 0 && t2 < 60)
                return t1*60+t2;
            break;
        case 3:
            if (t1 >= 0 && t2 >= 0 && t2 < 60 && t3 >= 0 && t3 < 60)
                return (t1*60+t2)*60+t3;
            break;
        case 4:
            if (t1 >= 0 && t2 >= 0 && t2 < 24 && t3 >= 0 && t3 < 60 && t4 >= 0 && t4 < 60)
                return ((t1*24+t2)*60+t3)*60+t4;
            break;
        default:
            break;
    }
    return -1;
}


void wxTimeSpinCtrl::OnKillFocus(wxFocusEvent &ev)
{
    long time=GetTextTime();

    if (time < 0)
    {
        m_txt->SetValue(wxEmptyString);
        spinValue = 0;
        m_spn->SetValue(0);
    }

    SetValue(wxTimeSpan(0,0,time));
}


void wxTimeSpinCtrl::OnSetFocus(wxFocusEvent &ev)
{
    m_txt->SetFocus();
    Highlight(hasDay ? 3 : 2);
}
