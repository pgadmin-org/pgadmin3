//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// explainCanvas.cpp - Explain Canvas
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "ctl/explainCanvas.h"


BEGIN_EVENT_TABLE(ExplainCanvas, wxShapeCanvas)
	EVT_MOTION(ExplainCanvas::OnMouseMotion)
END_EVENT_TABLE()


ExplainCanvas::ExplainCanvas(wxWindow *parent)
	: wxShapeCanvas(parent), rootShape(NULL)
{
	SetDiagram(new wxDiagram);
	GetDiagram()->SetCanvas(this);
	SetBackgroundColour(*wxWHITE);
	popup = NULL;
}


ExplainCanvas::~ExplainCanvas()
{
}


void ExplainCanvas::Clear()
{
	GetDiagram()->DeleteAllShapes();
	rootShape = NULL;
}


void ExplainCanvas::SetExplainString(const wxString &str)
{
	Clear();

	// We can have multiple plans in a single explain string
	// Add a empty root shape, which will never get drawn, but it will help us
	// to keep track of all these plans
	rootShape = ExplainShape::Create(0, NULL, wxEmptyString);
	AddShape(rootShape);

	ExplainShape *last = rootShape;
	int maxLevel = 0;

	wxStringTokenizer lines(str, wxT("\n"));

	while (lines.HasMoreTokens())
	{
		wxString tmp = lines.GetNextToken();
		wxString line = tmp.Strip(wxString::both);

		int braceCount = 0;
		do
		{
			const wxChar *cp = line.c_str();
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
				wxString tmp = lines.GetNextToken();
				line += wxT(" ") + tmp.Strip(wxString::both);
				braceCount = 0;
			}
			else
				break;
		}
		while (lines.HasMoreTokens());

		long level = ((tmp.Length() - line.Length() + 4) / 6) + 1;

		if (last)
		{
			if (level != 1)
			{
				if (line.Left(4) == wxT("->  "))
					line = line.Mid(4);
				else
				{
					last->SetCondition(line);
					continue;
				}
			}

			while (last != rootShape && level <= last->GetLevel())
				last = last->GetUpper();
		}


		ExplainShape *s = ExplainShape::Create(level, last, line);
		if (!s)
			continue;
		s->SetCanvas(this);
		InsertShape(s);
		s->Show(true);

		if (level > maxLevel)
			maxLevel = level;

		last = s;
	}


	int x0 = (int)(rootShape->GetWidth() * 3);
	int y0 = (int)(rootShape->GetHeight() * 3 / 2);
	int xoffs = (int)(rootShape->GetWidth() * 3);
	int yoffs = (int)(rootShape->GetHeight() * 5 / 4);

	wxNode *current = GetDiagram()->GetShapeList()->GetFirst();
	while (current)
	{
		ExplainShape *s = (ExplainShape *)current->GetData();

		if (!s->totalShapes)
			s->totalShapes = 1;
		if (s->GetUpper())
			s->GetUpper()->totalShapes += s->totalShapes;
		current = current->GetNext();
	}

	current = GetDiagram()->GetShapeList()->GetLast();
	while (current)
	{
		ExplainShape *s = (ExplainShape *)current->GetData();

		s->SetX(y0 + (maxLevel - s->GetLevel()) * xoffs);
		ExplainShape *upper = s->GetUpper();

		if (upper)
		{
			s->SetY(upper->GetY() + upper->usedShapes * yoffs);
			upper->usedShapes += s->totalShapes;

			// We don't require to draw a line from the root shape to its
			// childrens
			if (upper != rootShape)
			{
				wxLineShape *l = new ExplainLine(s, upper);
				l->Show(true);
				AddShape(l);
			}
		}
		else
		{
			s->SetY(y0);
		}

		current = current->GetPrevious();
	}

#define PIXPERUNIT  20
	int w = (maxLevel * xoffs + x0 * 2 + PIXPERUNIT - 1) / PIXPERUNIT;
	int h = (rootShape->totalShapes * yoffs + y0 * 2 + PIXPERUNIT - 1) / PIXPERUNIT;

	SetScrollbars(PIXPERUNIT, PIXPERUNIT, w, h);
}


void ExplainCanvas::OnMouseMotion(wxMouseEvent &ev)
{
	ev.Skip(true);

	if (ev.Dragging())
		return;

	wxClientDC dc(this);
	PrepareDC(dc);

	wxPoint logPos(ev.GetLogicalPosition(dc));

	double x, y;
	x = (double) logPos.x;
	y = (double) logPos.y;

	// Find the nearest object
	int attachment = 0;
	ExplainShape *nearestObj = dynamic_cast<ExplainShape *>(FindShape(x, y, &attachment));

	if (nearestObj)
	{
		ShowPopup(nearestObj);
	}
}


void ExplainCanvas::ShowPopup(ExplainShape *s)
{
	if (popup || s == NULL)
		return;

	popup = new ExplainPopup(this, s, &popup);

}


void ExplainCanvas::SaveAsImage(const wxString &fileName, wxBitmapType imageType)
{
	if (GetDiagram()->GetCount() == 0)
	{
		wxMessageBox(_("Nothing to be saved!"), _("Save As an image"), wxOK | wxICON_INFORMATION);
		return;
	}

	int width = 0, height = 0;
	GetVirtualSize(&width, &height);

	/*
	* Create the bitmap from the Explain window
	*/
	wxMemoryDC memDC;
	wxBitmap tempBitmap(width, height);

	memDC.SelectObject(tempBitmap);
	memDC.Clear();

	// Draw the diagram on the bitmap (Memory Device Context)
	GetDiagram()->Redraw(memDC);

	memDC.SelectObject(wxNullBitmap);

	if (!tempBitmap.SaveFile(fileName, imageType))
	{
		wxLogError(_("Could not write file \"%s\": error code %d."), fileName.c_str(), wxSysErrorCode());
	}
}

class ExplainText : public wxWindow
{
public:
	ExplainText(ExplainPopup *parent, ExplainShape *s);

protected:
	void OnMouseMove(wxMouseEvent &ev);
#ifdef wxUSE_POPUPWIN
	void OnMouseLost(wxMouseCaptureLostEvent &ev);
#endif

private:
	ExplainPopup *popup;
	void OnPaint(wxPaintEvent &ev);

	wxString m_desc, m_detail, m_condition, m_cost, m_actual;

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ExplainText, wxWindow)
	EVT_PAINT(ExplainText::OnPaint)
	EVT_MOTION(ExplainText::OnMouseMove)
#ifdef wxUSE_POPUPWIN
	EVT_MOUSE_CAPTURE_LOST(ExplainText::OnMouseLost)
#endif
END_EVENT_TABLE()

ExplainText::ExplainText(ExplainPopup *parent, ExplainShape *s) : wxWindow(parent, -1)
{
	SetBackgroundColour(wxColour(255, 255, 224));

	popup = parent;

	wxWindowDC dc(this);
	dc.SetFont(settings->GetSystemFont());

	m_desc = s->description;
	m_detail = s->detail;
	m_condition = s->condition;
	m_cost = s->cost;
	m_actual = s->actual;

	int w1, w2, h;
	dc.GetTextExtent(m_desc, &w1, &h);

	dc.GetTextExtent(m_detail, &w2, &h);
	if (w1 < w2)    w1 = w2;
	dc.GetTextExtent(m_condition, &w2, &h);
	if (w1 < w2)    w1 = w2;
	dc.GetTextExtent(m_cost, &w2, &h);
	if (w1 < w2)    w1 = w2;
	dc.GetTextExtent(m_actual, &w2, &h);
	if (w1 < w2)    w1 = w2;

	int n = 2;
	if (!m_detail.IsEmpty())
		n++;
	if (!m_condition.IsEmpty())
		n++;
	if (!m_cost.IsEmpty())
		n++;
	if (!m_actual.IsEmpty())
		n++;

	if (!h)
		h = GetCharHeight();

	SetSize(GetCharHeight() + w1, GetCharHeight() + h * n + h / 3);
}

void ExplainText::OnMouseMove(wxMouseEvent &ev)
{
	popup->OnMouseMove(ev);
}

#ifdef wxUSE_POPUPWIN
void ExplainText::OnMouseLost(wxMouseCaptureLostEvent &ev)
{
	/*
	 * We will not do anything here.
	 * But - in order to resolve a weird bug on window, when using
	 * wxPopupTransientWindow, related to loosing the mouse control,
	 * was not taken care properly, we have to introduce this function
	 * to avoid the crash.
	 *
	 * Please refer:
	 * http://article.gmane.org/gmane.comp.lib.wxwidgets.devel/82376
	 */
}
#endif

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
	dc.DrawText(m_desc, x, y);

	dc.SetFont(stdFont);

	if (!m_detail.IsEmpty())
	{
		y += yoffs;
		dc.DrawText(m_detail, x, y);

	}
	y += yoffs / 3;

	if (!m_condition.IsEmpty())
	{
		y += yoffs;
		dc.DrawText(m_condition, x, y);
	}
	if (!m_cost.IsEmpty())
	{
		y += yoffs;
		dc.DrawText(m_cost, x, y);
	}
	if (!m_actual.IsEmpty())
	{
		y += yoffs;
		dc.DrawText(m_actual, x, y);
	}

#if wxUSE_POPUPWIN

	wxPen pen1 = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)),
	      pen2 = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW)),
	      pen3 = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT)),
	      pen4 = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));

	wxRect rect(wxPoint(0, 0), GetSize());

	// draw the rectangle
	dc.SetPen(pen1);
	dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetLeft(), rect.GetBottom());
	dc.DrawLine(rect.GetLeft() + 1, rect.GetTop(), rect.GetRight(), rect.GetTop());
	dc.SetPen(pen2);
	dc.DrawLine(rect.GetRight(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
	dc.DrawLine(rect.GetLeft(), rect.GetBottom(), rect.GetRight() + 1, rect.GetBottom());

	// adjust the rect
	rect.Inflate(-1);

	// draw the rectangle
	dc.SetPen(pen3);
	dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetLeft(), rect.GetBottom());
	dc.DrawLine(rect.GetLeft() + 1, rect.GetTop(), rect.GetRight(), rect.GetTop());
	dc.SetPen(pen4);
	dc.DrawLine(rect.GetRight(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
	dc.DrawLine(rect.GetLeft(), rect.GetBottom(), rect.GetRight() + 1, rect.GetBottom());

	// adjust the rect
	rect.Inflate(-1);

	dc.SetPen(pen1);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawRectangle(rect);

#endif

}


BEGIN_EVENT_TABLE(ExplainPopup, pgTipWindowBase)
	EVT_MOTION(ExplainPopup::OnMouseMove)
	EVT_LEFT_DOWN(ExplainPopup::OnMouseClick)
	EVT_RIGHT_DOWN(ExplainPopup::OnMouseClick)
	EVT_MIDDLE_DOWN(ExplainPopup::OnMouseClick)
#if wxUSE_POPUPWIN
	EVT_MOUSE_CAPTURE_LOST(ExplainPopup::OnMouseLost)
#else
	EVT_KILL_FOCUS(ExplainPopup::OnKillFocus)
	EVT_ACTIVATE(ExplainPopup::OnActivate)
#endif // !wxUSE_POPUPWIN
END_EVENT_TABLE()

ExplainPopup::ExplainPopup(ExplainCanvas *parent, ExplainShape *shape, ExplainPopup **popup)
#if wxUSE_POPUPWIN
	: wxPopupTransientWindow(parent, wxNO_BORDER)
#else
	: wxFrame(parent, wxID_ANY, wxEmptyString,
	          wxDefaultPosition, wxDefaultSize,
	          wxNO_BORDER | wxFRAME_NO_TASKBAR )
#endif
{
	wxASSERT(parent != NULL);
	wxASSERT(shape != NULL);

	if (popup)
		m_ptr = popup;
	else
		m_ptr = NULL;

	m_explainText = new ExplainText(this, shape);
#if !wxUSE_POPUPWIN
	m_creationTime = wxGetLocalTime();
#endif
	double width = 0.0, height = 0.0;

	shape->GetBoundingBoxMin(&width, &height);
	if (fabs(width) < 4.0) width = 4.0;
	if (fabs(height) < 4.0) height = 4.0;

	width += (double)4.0;
	height += (double)4.0; // Allowance for inaccurate mousing

	int x = (int)(shape->GetX() - (width / 2.0));
	int y = (int)(shape->GetY() - (height / 2.0));

	int sx, sy;
	parent->CalcScrolledPosition(x, y, &sx, &sy);
	parent->ClientToScreen(&sx, &sy);

	m_rectBound.x = sx;
	m_rectBound.y = sy;
	m_rectBound.width = WXROUND(width);
	m_rectBound.height = WXROUND(height);

	wxSize popupSize;
	popupSize = m_explainText->GetSize();
	popupSize.DecTo(wxGetDisplaySize());
	SetSize(popupSize);

	if (sy > GetClientSize().y * 2 / 3)
		sy -= GetSize().y;
	sx -= GetSize().x / 2;

	if (sx < 5) sx = 5;

	if (sx < 0) x = 0;

#if wxUSE_POPUPWIN
	Position(wxPoint(sx, sy), wxSize(0, 0));
	Popup(m_explainText);
	m_explainText->SetFocus();
#ifdef __WXGTK__
	m_explainText->CaptureMouse();
#endif
#else
	Move(sx, sy);
	Show(true);

	m_explainText->SetFocus();
	m_explainText->CaptureMouse();
#endif
}


void ExplainPopup::OnMouseMove(wxMouseEvent &ev)
{
	if ( m_rectBound.width &&
	        !m_rectBound.Contains(ClientToScreen(ev.GetPosition())) )
	{
		// mouse left the bounding rect, disappear
		Close();
	}
	else
	{
		ev.Skip();
	}
}


void ExplainPopup::OnMouseClick(wxMouseEvent &ev)
{
	Close();
}


ExplainPopup::~ExplainPopup()
{
	if (m_ptr)
	{
		*m_ptr = NULL;
	}
#ifdef wxUSE_POPUPWIN
#ifdef __WXGTK__
	if (m_explainText->HasCapture() )
		m_explainText->ReleaseMouse();
#endif
#endif
	if(HasCapture())
		ReleaseMouse();
}

void ExplainPopup::Close()
{
	if (m_ptr)
	{
		*m_ptr = NULL;
		m_ptr = NULL;
	}

	if (m_explainText->HasCapture())
		m_explainText->ReleaseMouse();

	if(HasCapture())
		ReleaseMouse();

#if wxUSE_POPUPWIN
	Show(false);
#ifdef __WXGTK__
#endif
	Destroy();
#else
	wxFrame::Close();
#endif
}


#if wxUSE_POPUPWIN
void ExplainPopup::OnDismiss()
{
	Close();
}

void ExplainPopup::OnMouseLost(wxMouseCaptureLostEvent &ev)
{
	/* Do Nothing */
}
#else
void ExplainPopup::OnKillFocus(wxFocusEvent &event)
{
	// Workaround the kill focus event happening just after creation in wxGTK
	if (wxGetLocalTime() > m_creationTime + 1 &&
	        m_rectBound.width &&
	        !m_rectBound.Contains(::wxGetMousePosition()))
	{
		Close();
		return;
	}
	m_explainText->SetFocus();
	m_explainText->CaptureMouse();
}


void ExplainPopup::OnActivate(wxActivateEvent &event)
{
	if (!event.GetActive())
	{
		if ( m_rectBound.width &&
		        !m_rectBound.Contains(::wxGetMousePosition()) )
		{
			Close();
			return;
		}
		m_explainText->SetFocus();
		m_explainText->CaptureMouse();
	}
}
#endif // !wxUSE_POPUPWIN
