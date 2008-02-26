//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// explainCanvas.cpp - Explain Canvas
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "ctl/explainCanvas.h"



ExplainCanvas::ExplainCanvas(wxWindow *parent)
: wxShapeCanvas(parent)
{
    SetDiagram(new wxDiagram);
    GetDiagram()->SetCanvas(this);
    SetBackgroundColour(*wxWHITE);
    lastShape=0;
    popup = new ExplainPopup(this);
}


ExplainCanvas::~ExplainCanvas()
{
}


void ExplainCanvas::Clear()
{
    GetDiagram()->DeleteAllShapes();
    lastShape=0;
}


void ExplainCanvas::SetExplainString(const wxString &str)
{
    Clear();

    ExplainShape *last=0;
    int maxLevel=0;

    wxStringTokenizer lines(str, wxT("\n"));

    while (lines.HasMoreTokens())
    {
        wxString tmp=lines.GetNextToken();
        wxString line=tmp.Strip(wxString::both);

		int braceCount=0;
		do
		{
			const wxChar *cp=line.c_str();
			while (*cp)
			{
				if (*cp == '(')
					braceCount++;
				else if (*cp == ')')
					braceCount--;
				cp++;
			}
			if (braceCount > 0)
			{
		        wxString tmp=lines.GetNextToken();
				line += wxT(" ") + tmp.Strip(wxString::both);
				braceCount=0;
			}
			else
				break;
		}
		while (lines.HasMoreTokens());

        long level = (tmp.Length() - line.Length() +4) / 6;

        if (last)
        {
            if (level)
            {
                if (line.Left(4) == wxT("->  "))
                    line = line.Mid(4);
                else
                {
                    last->SetCondition(line);
                    continue;
                }
            }

            while (last && level <= last->GetLevel())
                last = last->GetUpper();
        }


        ExplainShape *s=ExplainShape::Create(level, last, line);
        if (!s)
            continue;
        s->SetCanvas(this);
        InsertShape(s);
        s->Show(true);

        if (level > maxLevel)
            maxLevel = level;
        
        if (!last)
            rootShape = s;
        last=s;
    }


    int x0 = (int)(rootShape->GetWidth()*3);
    int y0 = (int)(rootShape->GetHeight()*3/2);
    int xoffs = (int)(rootShape->GetWidth()*3);
    int yoffs = (int)(rootShape->GetHeight()*5/4);

    wxNode *current = GetDiagram()->GetShapeList()->GetFirst();
    while (current)
    {
        ExplainShape *s = (ExplainShape*)current->GetData();

        if (!s->totalShapes)
            s->totalShapes = 1;
        if (s->GetUpper())
            s->GetUpper()->totalShapes += s->totalShapes;
        current = current->GetNext();
    }

    current = GetDiagram()->GetShapeList()->GetLast();
    while (current)
    {
        ExplainShape *s = (ExplainShape*)current->GetData();

        s->SetX(y0 + (maxLevel - s->GetLevel()) * xoffs);
        ExplainShape *upper = s->GetUpper();

        if (upper)
        {
            s->SetY(upper->GetY() + upper->usedShapes * yoffs);
            upper->usedShapes += s->totalShapes;

            wxLineShape *l=new ExplainLine(s, upper);
            l->Show(true);
            AddShape(l);
        }
        else
        {
            s->SetY(y0);
        }

        current = current->GetPrevious();
    }

#define PIXPERUNIT  20
    int w=(maxLevel * xoffs + x0*2 + PIXPERUNIT - 1) / PIXPERUNIT;
    int h=(rootShape->totalShapes * yoffs + y0*2 + PIXPERUNIT - 1) / PIXPERUNIT;

    SetScrollbars(PIXPERUNIT, PIXPERUNIT, w, h);
}


void ExplainCanvas::ShowPopup(ExplainShape *s)
{
    int sx, sy;
    CalcScrolledPosition((int)s->GetX(), (int)s->GetY(), &sx, &sy);

    popup->SetShape(s);

    if (sy > GetClientSize().y*2/3)
        sy -= popup->GetSize().y;
    sx -= popup->GetSize().x / 2;
    if (sx < 0) sx=0;

    popup->Popup();
    popup->Move(ClientToScreen(wxPoint(sx, sy)));
}


class ExplainText : public wxWindow
{
public:
    ExplainText(wxWindow *parent, ExplainShape *s);

private:
    ExplainShape *shape;
    void OnPaint(wxPaintEvent &ev);

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ExplainText, wxWindow)
    EVT_PAINT(ExplainText::OnPaint)
END_EVENT_TABLE()


ExplainText::ExplainText(wxWindow *parent, ExplainShape *s) : wxWindow(parent, -1)
{
    SetBackgroundColour(wxColour(255,255,224));

    shape=s;

    wxWindowDC dc(this);
    dc.SetFont(settings->GetSystemFont());

    int w1, w2, h;
    dc.GetTextExtent(shape->description, &w1, &h);

    dc.GetTextExtent(shape->detail, &w2, &h);
    if (w1 < w2)    w1=w2;
    dc.GetTextExtent(shape->condition, &w2, &h);
    if (w1 < w2)    w1=w2;
    dc.GetTextExtent(shape->cost, &w2, &h);
    if (w1 < w2)    w1=w2;
    dc.GetTextExtent(shape->actual, &w2, &h);
    if (w1 < w2)    w1=w2;

    int n=2;
    if (!shape->detail.IsEmpty())
        n++;
    if (!shape->condition.IsEmpty())
        n++;
    if (!shape->cost.IsEmpty())
        n++;
    if (!shape->actual.IsEmpty())
        n++;

    if (!h)
        h = GetCharHeight();

    SetSize(GetCharHeight() + w1, GetCharHeight() + h * n + h/3);
}

    
void ExplainText::OnPaint(wxPaintEvent &ev)
{
    wxPaintDC dc(this);

    wxFont stdFont = settings->GetSystemFont();
    wxFont boldFont = stdFont;
    boldFont.SetWeight(wxBOLD);

    int x = GetCharHeight() / 2;
    int y = GetCharHeight() / 2;
    int w, yoffs;
    dc.GetTextExtent(wxT("Dummy"), &w, &yoffs);

    dc.SetFont(boldFont);
    dc.DrawText(shape->description, x, y);

    dc.SetFont(stdFont);

    if (!shape->detail.IsEmpty())
    {
        y += yoffs;
        dc.DrawText(shape->detail, x, y);

    }
    y += yoffs/3;

    if (!shape->condition.IsEmpty())
    {
        y += yoffs;
        dc.DrawText(shape->condition, x, y);
    }
    if (!shape->cost.IsEmpty())
    {
        y += yoffs;
        dc.DrawText(shape->cost, x, y);
    }
    if (!shape->actual.IsEmpty())
    {
        y += yoffs;
        dc.DrawText(shape->actual, x, y);
    }
}


BEGIN_EVENT_TABLE(ExplainPopup, wxDialog)
EVT_MOTION(ExplainPopup::OnMouseMove)
END_EVENT_TABLE()

ExplainPopup::ExplainPopup(wxWindow *w) : wxDialog(w, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER)
{
    explainText=0;
}


void ExplainPopup::SetShape(ExplainShape *s)
{
    if (explainText)
        delete explainText;
    explainText = new ExplainText(this, s);
    SetSize(explainText->GetSize());
}


void ExplainPopup::Popup()
{
    Show();
	Raise();
    wxTheApp->Yield(true);

    popupPoint = wxDefaultPosition;
    CaptureMouse();
}


#define POPUP_HISTERESIS 5

void ExplainPopup::OnMouseMove(wxMouseEvent &ev)
{
    if (popupPoint == wxDefaultPosition)
    {
        popupPoint = ev.GetPosition();
    }
    else
    {
        if (abs(popupPoint.x - ev.GetX()) > POPUP_HISTERESIS || abs(popupPoint.y - ev.GetY()) > POPUP_HISTERESIS)
        {
            ReleaseMouse();
            delete explainText;
            explainText=0;
            wxDialog::Hide();
        }
    }
}
