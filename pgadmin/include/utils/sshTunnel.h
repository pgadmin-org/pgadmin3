//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// sshTunnel.h - Used to create SSH Tunnels
//
//////////////////////////////////////////////////////////////////////////


#ifndef __SSH_TUNNELING
#define __SSH_TUNNELING

#include <wx/wx.h>
#include <wx/defs.h>

#ifdef WIN32
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#endif

#include <wx/thread.h>
#include <wx/arrstr.h>
#include <wx/hashset.h>
#include "utils/misc.h"

struct _LIBSSH2_CHANNEL;
struct _LIBSSH2_SESSION;
struct _LIBSSH2_USERAUTH_KBDINT_PROMPT;
struct _LIBSSH2_USERAUTH_KBDINT_RESPONSE;

enum enAuthenticationMethod
{
	AUTH_NONE = 0,
	AUTH_PASSWORD,
	AUTH_KEYBOARD_INTERACTIVE,
	AUTH_PUBLICKEY = 4
};

void LogSSHTunnelErrors(const wxString &msg, const int &id, struct _LIBSSH2_SESSION *session = NULL);
static wxMutex g_SSHThreadMutex;

WX_DECLARE_HASH_SET( int, wxIntegerHash, wxIntegerEqual, subThreadSDSet);
static subThreadSDSet g_setSocketDescriptor;

class CSSHTunnelThread :
	public wxThread
{
public:
	CSSHTunnelThread(const wxString tunnelhost, const wxString remote_desthost, const unsigned int remote_destport,
	                 const wxString username, const wxString password, const wxString publickey, const wxString privatekey,
	                 const enAuthenticationMethod &enAuthMethod, const unsigned int tunnelPort = 22);
	virtual ~CSSHTunnelThread(void);
	virtual void *Entry();
	bool Initialize();
	void Cleanup();

	wxString GetLocalListenIP() const
	{
		return m_local_listenip;
	}

	unsigned int GetLocalListenPort() const
	{
		return m_local_listenport;
	}

private:
	bool resolveDNS(const char *host, wxArrayString &arrIPAddress);
	static void keyboard_interactive(const char *name, int name_len, const char *instr, int instr_len,
	                                 int num_prompts, const struct _LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts, struct _LIBSSH2_USERAUTH_KBDINT_RESPONSE *res, void **abstract);
	bool IsHostKeyVerified(const wxString &newHostKey);

	int m_listensock, m_sock;
	struct sockaddr_in m_sin;
	socklen_t m_sinlen;
	struct _LIBSSH2_SESSION *m_session;
	static char m_keyboard_interactive_pwd[SSH_MAX_PASSWORD_LEN];

	wxString m_publickey;
	wxString m_privatekey;
	wxString m_username;
	wxString m_password;
	wxString m_tunnelhost;
	wxString m_local_listenip;
	wxString m_remote_desthost;
	unsigned int m_local_listenport;
	unsigned int m_remote_destport;
	unsigned int m_tunnelPort;
	enAuthenticationMethod m_enAuthMethod;
};

class CSubThread :
	public wxThread
{
public:
	CSubThread(const struct sockaddr_in sin, const wxString remote_desthost, const unsigned int remote_destport,
	           struct _LIBSSH2_SESSION *session, int forwardsock);
	virtual ~CSubThread(void);
	virtual void *Entry();

private:
	struct sockaddr_in m_sin;
	wxString m_remote_desthost;
	unsigned int m_remote_destport;
	struct _LIBSSH2_SESSION *m_subThreadSession;
	struct _LIBSSH2_CHANNEL *m_channel;
	int m_forwardsock;
};

#endif
