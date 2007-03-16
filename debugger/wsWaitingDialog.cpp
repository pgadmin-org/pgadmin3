//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wswaitingdialog.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wx/utils.h"
#include "wx/frame.h"
#include "wx/button.h"
#include "wx/stattext.h"
#include "wx/sizer.h"
#include "wx/event.h"
#include "wx/intl.h"
#include "wx/dcclient.h"
#include "wx/timer.h"
#include "wx/settings.h"

#include "wsWaitingDialog.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define LAYOUT_MARGIN 8

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// update the label to show the given time ( in seconds )
static void SetTimeLabel( unsigned long val, wxStaticText *label );

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( wsWaitingDialog, wxDialog )
    EVT_BUTTON( wxID_CANCEL, wsWaitingDialog::OnCancel )
    EVT_CLOSE( wsWaitingDialog::OnClose )
END_EVENT_TABLE( )

IMPLEMENT_CLASS( wsWaitingDialog, wxDialog )

// ============================================================================ 
// wsWaitingDialog implementation
// ============================================================================ 

// ----------------------------------------------------------------------------
// wsWaitingDialog creation
// ----------------------------------------------------------------------------

wsWaitingDialog::wsWaitingDialog( wxString const &title, 
                                   wxString const &message, 
                                   int maximum, 
                                   wxWindow *parent, 
                                   int style )
                : wxDialog( parent, wxID_ANY, title ), 
                  m_delay( 3 ), 
                  m_hasAbortButton( false )
{
    // we may disappear at any moment, let the others know about it
    SetExtraStyle( GetExtraStyle( ) | wxWS_EX_TRANSIENT );
    m_windowStyle |= style;

    m_hasAbortButton = ( style & wxPD_CAN_ABORT ) != 0;

#if defined( __WXMSW__ ) && !defined( __WXUNIVERSAL__ )
    // we have to remove the "Close" button from the title bar then as it is
    // confusing to have it - it doesn't work anyhow
    //
    // FIXME: should probably have a ( extended? ) window style for this
    if( !m_hasAbortButton )
    {
        EnableCloseButton( false );
    }
#endif // wxMSW

    m_state = m_hasAbortButton ? Continue : Uncancelable;
    m_maximum = maximum;

#if defined( __WXMSW__ ) || defined( __WXPM__ )
    // we can't have values > 65, 536 in the progress control under Windows, so
    // scale everything down
    m_factor = m_maximum / 65536 + 1;
    m_maximum /= m_factor;
#endif // __WXMSW__

    m_parentTop = wxGetTopLevelParent( parent );

    wxClientDC dc( this );
    dc.SetFont( wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT ));

    long widthText;
    dc.GetTextExtent( message, &widthText, NULL, NULL, NULL, NULL );

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    m_msg = new wxStaticText( this, wxID_ANY, message );
    sizer->Add( m_msg, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 2*LAYOUT_MARGIN );

    wxSize sizeDlg;
    wxSize sizeLabel = m_msg->GetSize( );

    sizeDlg.y = 2*LAYOUT_MARGIN + sizeLabel.y;

    // create the estimated/remaining/total time zones if requested
    m_elapsed = NULL;
    m_break   = 0;
    m_ctdelay = 0;

    // if we are going to have at least one label, remmeber it in this var
    wxStaticText *label = NULL;

    // also count how many labels we really have
    size_t nTimeLabels = 0;

    if( style & wxPD_ELAPSED_TIME )
    {
        nTimeLabels++;

        label = m_elapsed = CreateLabel( _( "Elapsed time : " ), sizer );
    }

    if( nTimeLabels > 0 )
    {
        // set it to the current time
        m_timeStart = wxGetCurrentTime( );
        sizeDlg.y  += nTimeLabels * ( label->GetSize( ).y + LAYOUT_MARGIN );
    }

    m_btnAbort = NULL;
    bool sizeDlgModified = false;
    wxBoxSizer *buttonSizer = new wxBoxSizer( wxHORIZONTAL );

    const int sizerFlags = 
#if defined( __WXMSW__ ) || defined( __WXPM__ )
                           wxALIGN_RIGHT | wxALL
#else // !MSW
                           wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxTOP
#endif // MSW/!MSW
                           ;
    if( m_hasAbortButton )
    {
        m_btnAbort = new wxButton( this, wxID_CANCEL );

        // Windows dialogs usually have buttons in the lower right corner
        buttonSizer->Add( m_btnAbort, 0, sizerFlags, LAYOUT_MARGIN );
        if( !sizeDlgModified )
            sizeDlg.y += 2*LAYOUT_MARGIN + wxButton::GetDefaultSize( ).y;
    }

    sizer->Add( buttonSizer, 0, sizerFlags, LAYOUT_MARGIN );

    SetSizerAndFit( sizer );

	sizeDlg.y += 2*LAYOUT_MARGIN;

	// try to make the dialog not square but rectangular of reasonable width
	sizeDlg.x  = ( wxCoord )wxMax( widthText, 4*sizeDlg.y/3 );
	sizeDlg.x *= 3;
	sizeDlg.x /= 2;
	SetClientSize( sizeDlg );

    Centre( wxCENTER_FRAME | wxBOTH );

    if( style & wxPD_APP_MODAL )
    {
        m_winDisabler = new wxWindowDisabler( this );
    }
    else
    {
        if( m_parentTop )
            m_parentTop->Disable( );
        m_winDisabler = NULL;
    }

    Show( );
    Enable( );

    // this one can be initialized even if the others are unknown for now
    //
    // NB: do it after calling Layout( ) to keep the labels correctly aligned
    if( m_elapsed )
    {
        SetTimeLabel( 0, m_elapsed );
    }

    Update( );
}

wxStaticText *wsWaitingDialog::CreateLabel( const wxString& text, 
                                            wxSizer *sizer )
{
    wxBoxSizer *locsizer = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *dummy = new wxStaticText( this, wxID_ANY, text );
    wxStaticText *label = new wxStaticText( this, wxID_ANY, _( "unknown" ));

    // select placement most native or nice on target GUI
#if defined( __WXMSW__ ) || defined( __WXPM__ ) || defined( __WXMAC__ )
    // label and time centered in one row
    locsizer->Add( dummy, 1, wxALIGN_RIGHT );
    locsizer->Add( label, 1, wxALIGN_LEFT | wxLEFT, LAYOUT_MARGIN );
    sizer->Add( locsizer, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, LAYOUT_MARGIN );
#else
    // label and time to the right in one row
    sizer->Add( locsizer, 0, wxALIGN_CENTER_HORIZONTAL | wxRIGHT | wxTOP, LAYOUT_MARGIN );
    locsizer->Add( dummy );
    locsizer->Add( label, 0, wxLEFT, LAYOUT_MARGIN );
#endif

    return label;
}

// ----------------------------------------------------------------------------
// wsWaitingDialog operations
// ----------------------------------------------------------------------------

bool
wsWaitingDialog::Update( int value, const wxString& newmsg )
{

#ifdef __WXMSW__
    value /= m_factor;
#endif // __WXMSW__

    wxASSERT_MSG( value <= m_maximum, _( "invalid progress value" ));

    if( !newmsg.empty( ) && newmsg != m_msg->GetLabel( ) )
    {
        m_msg->SetLabel( newmsg );

        wxYieldIfNeeded( ) ;
    }

    if( m_elapsed && ( value != 0 ) )
    {
        unsigned long elapsed = wxGetCurrentTime( ) - m_timeStart;

        if( m_last_timeupdate < elapsed || value == m_maximum )
        {
            m_last_timeupdate = elapsed;

            unsigned long estimated = m_break + ( unsigned long )(( ( double ) ( elapsed-m_break ) * m_maximum ) / (( double )value )) ;

            if( estimated > m_display_estimated && m_ctdelay >= 0 )
            {
                ++m_ctdelay;
            }
            else if( estimated < m_display_estimated && m_ctdelay <= 0 )
            {
                --m_ctdelay;
            }
            else
            {
                m_ctdelay = 0;
            }
            if(    m_ctdelay >= m_delay          // enough confirmations for a higher value
                 || m_ctdelay <= ( m_delay*-1 )     // enough confirmations for a lower value
                 || value == m_maximum            // to stay consistent
                 || elapsed > m_display_estimated // to stay consistent
                 || ( elapsed > 0 && elapsed < 4 ) // additional updates in the beginning
               )
            {
                m_display_estimated = estimated;
                m_ctdelay = 0;
            }
        }

        long display_remaining = m_display_estimated - elapsed;

        if( display_remaining < 0 )
            display_remaining = 0;

        SetTimeLabel( elapsed, m_elapsed );
    }

    if( value == m_maximum )
    {
        if( m_state == Finished )
        {
            // ignore multiple calls to Update( m_maximum ): it may sometimes be
            // troublesome to ensure that Update( ) is not called twice with the
            // same value ( e.g. because of the rounding errors ) and if we don't
            // return now we're going to generate asserts below
            return true;
        }

        // so that we return true below and that out [Cancel] handler knew what
        // to do
        m_state = Finished;
        if( !( GetWindowStyle( ) & wxPD_AUTO_HIDE ) )
        {
            EnableClose( );
#if defined( __WXMSW__ ) && !defined( __WXUNIVERSAL__ )
            EnableCloseButton( );
#endif // __WXMSW__

            if( newmsg.empty( ) )
            {
                // also provide the finishing message if the application didn't
                m_msg->SetLabel( _( "Done." ));
            }

            wxYieldIfNeeded( ) ;

            ( void )ShowModal( );
        }
        else // auto hide
        {
            // reenable other windows before hiding this one because otherwise
            // Windows wouldn't give the focus back to the window which had
            // been previously focused because it would still be disabled
            ReenableOtherWindows( );

            Hide( );
        }
    }
    else
    {
        // we have to yield because not only we want to update the display but
        // also to process the clicks on the cancel button
        wxYieldIfNeeded( );
    }

    // update the display in case yielding above didn't do it
    Update( );

    return m_state != Canceled;
}

void wsWaitingDialog::Resume( )
{
    m_state    = Continue;
    m_ctdelay  = m_delay; 		// force an update of the elapsed/estimated/remaining time
    m_break   += wxGetCurrentTime( ) - m_timeStop;

    EnableAbort( );
}

bool wsWaitingDialog::Show( bool show )
{
#ifdef __WIN32__
    // We call ReenableOtherWindows() to make the main window 
    // responsive even if Wait dialog is active. This will 
    // allow the user move or resize the main application 
	// window on Windows hosts
    ReenableOtherWindows();
#else
    // reenable other windows before hiding this one because otherwise
    // Windows wouldn't give the focus back to the window which had
    // been previously focused because it would still be disabled

    if( !show )
        ReenableOtherWindows();
#endif

    return wxDialog::Show( show );
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wsWaitingDialog::OnCancel( wxCommandEvent& event )
{
    if( m_state == Finished )
    {
        // this means that the count down is already finished and we're being
        // shown as a modal dialog - so just let the default handler do the job
        event.Skip( );
    }
    else
    {
        // request to cancel was received, the next time Update( ) is called we
        // will handle it
        m_state = Canceled;

        // update the buttons state immediately so that the user knows that the
        // request has been noticed
        DisableAbort( );

        // save the time when the dialog was stopped
        m_timeStop = wxGetCurrentTime( );
    }
}

void wsWaitingDialog::OnClose( wxCloseEvent& event )
{
    if( m_state == Uncancelable )
    {
        // can't close this dialog
        event.Veto( );
    }
    else if( m_state == Finished )
    {
        // let the default handler close the window as we already terminated
        event.Skip( );
    }
    else
    {
        // next Update( ) will notice it
        m_state = Canceled;
        DisableAbort( );

        m_timeStop = wxGetCurrentTime( );
    }
}

// ----------------------------------------------------------------------------
// destruction
// ----------------------------------------------------------------------------

wsWaitingDialog::~wsWaitingDialog( )
{
    // normally this should have been already done, but just in case
    ReenableOtherWindows( );
}

void wsWaitingDialog::ReenableOtherWindows( )
{
    if( GetWindowStyle( ) & wxPD_APP_MODAL )
    {
        delete m_winDisabler;
        m_winDisabler = ( wxWindowDisabler * )NULL;
    }
    else
    {
        if( m_parentTop )
            m_parentTop->Enable( );
    }
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static void SetTimeLabel( unsigned long val, wxStaticText *label )
{
    if( label )
    {
        wxString s;
        unsigned long hours   = val / 3600;
        unsigned long minutes = ( val % 3600 ) / 60;
        unsigned long seconds = val % 60;

        s.Printf( wxT( "%lu:%02lu:%02lu" ), hours, minutes, seconds );

        if( s != label->GetLabel( ) )
            label->SetLabel( s );
    }
}

void wsWaitingDialog::EnableAbort( bool enable )
{
    if( m_hasAbortButton )
    {
        if( m_btnAbort )
            m_btnAbort->Enable( enable );
    }
}

void wsWaitingDialog::EnableClose( )
{
    if( m_hasAbortButton )
    {
        if( m_btnAbort )
        {
            m_btnAbort->Enable( );
            m_btnAbort->SetLabel( _( "Close" ));
        }
    }
}
