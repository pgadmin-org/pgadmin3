//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// sshTunnel.cpp - Used to create SSH Tunnels
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"
#include <wx/dynlib.h>

#undef ssize_t
#define ssize_t long
#include "libssh2/libssh2.h"
#include "utils/sshTunnel.h"
#include "frm/frmMain.h"

#pragma comment (lib, "Ws2_32.lib")

typedef const char *(*inet_ntop_t) (int af, const void *src, char *dst, socklen_t size);

#ifdef WIN32
const char *custom_inet_ntop(int af, const void *src, char *dst, int cnt)
{
	struct sockaddr_in srcaddr;

	memset(&srcaddr, 0, sizeof(struct sockaddr_in));
	memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));

	srcaddr.sin_family = af;
	if (WSAAddressToStringA((struct sockaddr *) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0)
	{

		wxLogInfo(wxT("SSH error: WSAAddressToStringA failed with error code %d"), WSAGetLastError());
		return NULL;
	}
	return dst;
}

static inet_ntop_t gs_fnPtr_inet_ntop = &custom_inet_ntop;
#else
static inet_ntop_t gs_fnPtr_inet_ntop = &inet_ntop;
#endif

char CSSHTunnelThread::m_keyboard_interactive_pwd[SSH_MAX_PASSWORD_LEN];

CSSHTunnelThread::CSSHTunnelThread(const wxString tunnelhost, const wxString remote_desthost, const unsigned int remote_destport,
                                   const wxString username, const wxString password, const wxString publickey, const wxString privatekey,
                                   const enAuthenticationMethod &enAuthMethod, const unsigned int tunnelPort)
	: m_tunnelhost(tunnelhost), m_remote_desthost(remote_desthost), m_remote_destport(remote_destport), m_username(username),
	  m_password(password), m_publickey(publickey), m_privatekey(privatekey), m_enAuthMethod(enAuthMethod), m_tunnelPort(tunnelPort)
{
	m_local_listenip = wxEmptyString;
	m_local_listenport = 0;
	m_listensock = -1, m_sock = -1;
	m_session = NULL;

	memset(m_keyboard_interactive_pwd, 0 , strlen(m_keyboard_interactive_pwd));
	strncpy(m_keyboard_interactive_pwd, (const char *)password.mb_str(wxConvUTF8), password.Length());
}

CSSHTunnelThread::~CSSHTunnelThread(void)
{
}

bool CSSHTunnelThread::Initialize()
{
	int rc, auth = AUTH_NONE;
	const char *fingerprint;
	char *userauthlist;

#ifdef WIN32
	char sockopt;
	WSADATA wsadata;
	int err;

	err = WSAStartup(MAKEWORD(2, 0), &wsadata);
	if(err != 0)
	{
		wxLogInfo(wxT("WSAStartup failed with error: %d"), err);
		return false;
	}
#else
	int sockopt;
#endif

	wxArrayString arrTunnelHostIP;

	if (resolveDNS(m_tunnelhost.mb_str(), arrTunnelHostIP))
	{
		rc = libssh2_init (0);

		if (rc != 0)
		{
			LogSSHTunnelErrors(wxString::Format(_("libssh2 initialization failed with error code %d"), rc), GetId());
			return false;
		}

		/* Connect to SSH server */
		m_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		m_sin.sin_family = AF_INET;
		if (INADDR_NONE == (m_sin.sin_addr.s_addr = inet_addr(arrTunnelHostIP.Item(0).mb_str())))
		{
			LogSSHTunnelErrors(wxString::Format(_("SSH error: Error in inet address with error code %d"), wxSysErrorCode()), GetId());
			return false;
		}
		m_sin.sin_port = htons(m_tunnelPort);
		if (connect(m_sock, (struct sockaddr *)(&m_sin),
		            sizeof(struct sockaddr_in)) != 0)
		{
			LogSSHTunnelErrors(wxString::Format(_("SSH error: Could not connect to socket with error code %d"), wxSysErrorCode()), GetId());
			return false;
		}

		/* Create a session instance */
		m_session = libssh2_session_init();

		if (!m_session)
		{
			LogSSHTunnelErrors(_("SSH error: Could not initialize SSH session!"), GetId());
			return false;
		}

		/* ... start it up. This will trade welcome banners, exchange keys,
		* and setup crypto, compression, and MAC layers
		*/
		rc = libssh2_session_handshake(m_session, m_sock);
		if (rc)
		{
			LogSSHTunnelErrors(wxString::Format(_("SSH error: Error when starting up SSH session with error code %d"), rc), GetId(), m_session);
			return false;
		}

		/* At this point we havn't yet authenticated.  The first thing to do
		* is check the hostkey's fingerprint against our known hosts Your app
		* may have it hard coded, may go to a file, may present it to the
		* user, that's your call
		*/
		fingerprint = libssh2_hostkey_hash(m_session, LIBSSH2_HOSTKEY_HASH_SHA1);
		wxString newHostKey = wxEmptyString;
		for(int i = 0; i < 20; i++)
		{
			newHostKey += wxString::Format(wxT("%02X "), (unsigned char)fingerprint[i]);
		}

		// Check if the SSH Host Key is verified
		if(!IsHostKeyVerified(newHostKey))
		{
			Cleanup();
			return false;
		}


		/* check what authentication methods are available */
		userauthlist = libssh2_userauth_list(m_session, m_username.mb_str(), strlen(m_username.mb_str()));

		if (strstr(userauthlist, "password"))
			auth |= AUTH_PASSWORD;
		if(strstr(userauthlist, "keyboard-interactive"))
			auth |= AUTH_KEYBOARD_INTERACTIVE;
		if (strstr(userauthlist, "publickey"))
			auth |= AUTH_PUBLICKEY;

		if ((auth & AUTH_PASSWORD) && (m_enAuthMethod == AUTH_PASSWORD))
			auth = AUTH_PASSWORD;
		else if ((auth & AUTH_KEYBOARD_INTERACTIVE) && (m_enAuthMethod == AUTH_PASSWORD))
			auth = AUTH_KEYBOARD_INTERACTIVE;
		if ((auth & AUTH_PUBLICKEY) && (m_enAuthMethod == AUTH_PUBLICKEY))
			auth = AUTH_PUBLICKEY;

		if (auth & AUTH_PASSWORD)
		{
			rc = libssh2_userauth_password(m_session, m_username.mb_str(), m_password.mb_str());
			if (rc)
			{
				LogSSHTunnelErrors(wxString::Format(_("SSH error: Authentication by password failed with error code %d"), rc), GetId(), m_session);
				Cleanup();
				return false;
			}
		}
		else if (auth & AUTH_KEYBOARD_INTERACTIVE)
		{
			rc = libssh2_userauth_keyboard_interactive(m_session, m_username.mb_str(), &CSSHTunnelThread::keyboard_interactive);
			if (rc)
			{
				LogSSHTunnelErrors(wxString::Format(_("SSH error: Authentication by password failed with error code %d"), rc), GetId(), m_session);
				Cleanup();
				return false;
			}
		}
		else if (auth & AUTH_PUBLICKEY)
		{
#ifdef HAVE_GCRYPT
			rc = libssh2_userauth_publickey_fromfile(m_session, m_username.mb_str(), m_publickey.mb_str(), m_privatekey.mb_str(), m_password.mb_str());
#else
			rc = libssh2_userauth_publickey_fromfile(m_session, m_username.mb_str(), NULL, m_privatekey.mb_str(), m_password.mb_str());
#endif
			if (rc)
			{
				LogSSHTunnelErrors(wxString::Format(_("SSH error: Authentication by identity file failed with error code %d"), rc), GetId(), m_session);
				Cleanup();
				return false;
			}
		}
		else
		{
			LogSSHTunnelErrors(_("SSH error: No supported authentication methods found!"), GetId());
			Cleanup();
			return false;
		}

		// Get the IP Address of local machine
		wxArrayString arrLocalIP;
		if(resolveDNS("localhost", arrLocalIP))
		{
			m_listensock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			memset(&m_sin, 0 , sizeof(m_sin));
			m_sin.sin_family = AF_INET;

			// Give port no to 0 so that bind will automatically select the available port.
			m_sin.sin_port = htons(0);
			if (INADDR_NONE == (m_sin.sin_addr.s_addr = inet_addr(arrLocalIP.Item(0).mb_str())))
			{
				Cleanup();
				return false;
			}

			sockopt = 1;
			setsockopt(m_listensock, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));
			m_sinlen = sizeof(m_sin);
			if (-1 == bind(m_listensock, (struct sockaddr *)&m_sin, m_sinlen))
			{
				LogSSHTunnelErrors(wxString::Format(_("SSH error: bind failed with error code %d"), wxSysErrorCode()), GetId());
				Cleanup();
				return false;
			}

			if (getsockname(m_listensock, (struct sockaddr *)&m_sin, &m_sinlen) == -1)
			{
				LogSSHTunnelErrors(wxString::Format(_("SSH error: getsockname() failed with error code %d"), wxSysErrorCode()), GetId());
				Cleanup();
				return false;
			}

			if (-1 == listen(m_listensock, 2))
			{
				LogSSHTunnelErrors(wxString::Format(_("SSH error: listen failed with error code %d"), wxSysErrorCode()), GetId());
				Cleanup();
				return false;
			}

			m_local_listenip = wxString(inet_ntoa(m_sin.sin_addr), wxConvLibc);
			m_local_listenport = ntohs(m_sin.sin_port);

			wxLogInfo(wxT("Waiting for TCP connection on %s:%d..."), m_local_listenip.c_str(), m_local_listenport);
			return true;
		}
		else
		{
			LogSSHTunnelErrors(_("SSH error: Unable to resolve localhost"), GetId());
		}
	}
	else
	{
		LogSSHTunnelErrors(wxString::Format(_("SSH error: Unable to resolve host: %s"), m_tunnelhost.c_str()), GetId());
	}

	return false;
}

void *CSSHTunnelThread::Entry()
{
	while (1)
	{
		int forwardsock = accept(m_listensock, (struct sockaddr *) & m_sin, &m_sinlen);
		if (-1 == forwardsock)
		{
#ifdef WIN32
			if(wxSysErrorCode() != WSAEINTR && wxSysErrorCode() != WSAEBADF)
#else
			if(wxSysErrorCode() != EINTR && wxSysErrorCode() != EBADF && wxSysErrorCode() != ECONNABORTED)
#endif
			{
				LogSSHTunnelErrors(wxString::Format(_("SSH error: accept failed with error code %d"), wxSysErrorCode()), GetId());
				Cleanup();
			}
			break;
		}

		// Create thread for read/write.
		CSubThread *subThread = new CSubThread(m_sin, m_remote_desthost, m_remote_destport, m_session, forwardsock);
		if ( subThread->Create() != wxTHREAD_NO_ERROR )
		{
			delete subThread;
			subThread = NULL;
		}
		else
		{
			if (subThread->Run() != wxTHREAD_NO_ERROR )
			{
				delete subThread;
				subThread = NULL;
			}
			else
			{
				g_SSHThreadMutex.Lock();
				g_setSocketDescriptor.insert(forwardsock);
				g_SSHThreadMutex.Unlock();
			}
		}
	}

	return NULL;
}

void CSSHTunnelThread::Cleanup()
{
	// Close all the sockets
	g_SSHThreadMutex.Lock();
	subThreadSDSet::iterator it;
	for( it = g_setSocketDescriptor.begin(); it != g_setSocketDescriptor.end(); ++it )
	{
		int socketDescriptor = *it;
#ifdef WIN32
		closesocket(socketDescriptor);
#else
		close(socketDescriptor);
#endif
	}
	g_SSHThreadMutex.Unlock();

	Sleep(1000);

	if(m_session)
	{
		libssh2_session_disconnect(m_session, "Client disconnecting normally");
		libssh2_session_free(m_session);
		m_session = NULL;
	}

	if (m_listensock)
	{
#ifdef WIN32
		closesocket(m_listensock);
#else
		close(m_listensock);
#endif
		m_listensock = 0;
	}

	if (m_sock)
	{
#ifdef WIN32
		closesocket(m_sock);
#else
		close(m_sock);
#endif
		m_sock = 0;
	}

	libssh2_exit();
}

bool CSSHTunnelThread::resolveDNS(const char *host, wxArrayString &arrIPAddress)
{
	struct addrinfo hints, *res, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	memset(&ipstr, 0, sizeof ipstr);
	hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;

	if ((status = getaddrinfo(host, NULL, &hints, &res)) != 0)
	{
		wxLogInfo(wxT("getaddrinfo failed with error code: %d"), status);
		return false;
	}

	for(p = res; p != NULL; p = p->ai_next)
	{
		void *addr;
		if (p->ai_family == AF_INET)
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);

			/* convert the IP to a string*/
			if(NULL != gs_fnPtr_inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr))
				arrIPAddress.Add(wxString(ipstr, wxConvLocal));
		}
	}

	if(res)
	{
		freeaddrinfo(res); // free the linked list
	}

	if(arrIPAddress.Count() > 0)
	{
		return true;
	}

	return false;
}

void CSSHTunnelThread::keyboard_interactive(const char *name, int name_len, const char *instr, int instr_len,
        int num_prompts,  const struct _LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts, struct _LIBSSH2_USERAUTH_KBDINT_RESPONSE *res,
        void **abstract)
{
	if (num_prompts == 1)
	{
		res[0].text = strdup(m_keyboard_interactive_pwd);
		res[0].length = strlen(m_keyboard_interactive_pwd);
	}
}

bool CSSHTunnelThread::IsHostKeyVerified(const wxString &newHostKey)
{
	bool bIsVerified = false;
	wxString cachedHostKey = settings->Read(wxT("HostKeys/") + m_tunnelhost, wxEmptyString);

	// If cached host key is empty then ask user to accept or reject
	if (cachedHostKey == wxEmptyString)
	{
		// Prompt User to accept or reject
		wxString msg = wxString::Format(wxT("Host key received for the SSH server \"%s\" is \n\n%s\n\nWould you like to accept it and continue with the connection?"), m_tunnelhost.c_str(), newHostKey.c_str());
		int answer = wxMessageBox(msg, wxT("Host key verification"), wxYES_NO | wxNO_DEFAULT);
		if (answer == wxYES)
		{
			// Write the host key with respect to tunnel host
			settings->Write(wxT("HostKeys/") + m_tunnelhost, newHostKey);
			bIsVerified = true;
		}
	}
	else
	{
		// Compare the cached host key with the new key
		if (cachedHostKey.compare(newHostKey) == 0)
			bIsVerified = true;
		else
		{
			// Prompt user to accept or reject the new host key received for the tunnel host
			wxString msg = wxString::Format(wxT("The host key received from the server \"%s\" is\n\n%s\n\nbut the stored key is\n\n%s\n\nThis may indicate that this is not the same server that was previously used.\n\nDo you wish to accept and store the new key, and continue with the connection?"), m_tunnelhost.c_str(), newHostKey.c_str(), cachedHostKey.c_str());
			int answer = wxMessageBox(msg, wxT("Host key verification - WARNING"), wxYES_NO | wxNO_DEFAULT);
			if (answer == wxYES)
			{
				// Write the host key with respect to tunnel host
				settings->Write(wxT("HostKeys/") + m_tunnelhost, newHostKey);
				bIsVerified = true;
			}
		}
	}

	return bIsVerified;
}

CSubThread::CSubThread(const struct sockaddr_in sin, const wxString remote_desthost, const unsigned int remote_destport,
                       LIBSSH2_SESSION *session, int forwardsock)
	: m_sin(sin), m_remote_desthost(remote_desthost), m_remote_destport(remote_destport),
	  m_subThreadSession(session), m_forwardsock(forwardsock)
{
}

CSubThread::~CSubThread(void)
{
	g_SSHThreadMutex.Lock();
	g_setSocketDescriptor.erase(m_forwardsock);
	g_SSHThreadMutex.Unlock();
}

void *
CSubThread::Entry()
{
	fd_set fds;
	struct timeval tv;
	ssize_t len, wr;
	char buf[20480];
	int rc, i = 0;

	const char *shost = inet_ntoa(m_sin.sin_addr);
	unsigned int sport = ntohs(m_sin.sin_port);

	wxLogInfo(wxT("Forwarding connection from %s:%d to %s:%d"), wxString(inet_ntoa(m_sin.sin_addr), wxConvLibc).c_str(),
	          sport, m_remote_desthost.c_str(), m_remote_destport);

	/* Must use blocking here to avoid connect errors */
	//libssh2_session_set_blocking(m_subThreadSession, 1);

	while((m_channel = libssh2_channel_direct_tcpip_ex(m_subThreadSession, m_remote_desthost.mb_str(),
	                   m_remote_destport, shost, sport)) == NULL)
	{
		rc = libssh2_session_last_error(m_subThreadSession, NULL, NULL, 0);
		if (rc == LIBSSH2_ERROR_EAGAIN)
			Sleep(10);
		else
			break;
	}

	/* Must use non-blocking IO hereafter due to the current libssh2 API */
	libssh2_session_set_blocking(m_subThreadSession, 0);

	if (!m_channel)
	{
		wxLogInfo(_("SSH error: Could not open a direct-tcpip channel!"));
		goto shutdown;
	}

	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(m_forwardsock, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		rc = select(m_forwardsock + 1, &fds, NULL, NULL, &tv);
		memset(buf, 0, sizeof(buf));
		if (-1 == rc)
		{
			wxLogInfo(_("SSH error: select failed with error code %d"), wxSysErrorCode());
			goto shutdown;
		}
		if (rc && FD_ISSET(m_forwardsock, &fds))
		{
			len = recv(m_forwardsock, buf, sizeof(buf), 0);
			if (len < 0)
			{
				wxLogInfo(_("SSH error: read failed with error code %d"), wxSysErrorCode());
				goto shutdown;
			}
			else if (0 == len)
			{
				wxLogInfo(_("The client at %s:%d disconnected!"), wxString(inet_ntoa(m_sin.sin_addr), wxConvLibc).c_str(), sport);
				goto shutdown;
			}
			wr = 0;
			do
			{
				i = libssh2_channel_write(m_channel, buf, len);

				if (i < 0)
				{
					wxLogInfo(_("SSH error: libssh2_channel_write with error code %d"), i);
					goto shutdown;
				}
				wr += i;
			}
			while (i > 0 && wr < len);
		}
		while (1)
		{
			len = libssh2_channel_read(m_channel, buf, sizeof(buf));

			if (LIBSSH2_ERROR_EAGAIN == len)
				break;
			else if (len < 0)
			{
				wxLogInfo(_("SSH error: libssh2_channel_read with error code %d"), (int)len);
				goto shutdown;
			}
			wr = 0;
			while (wr < len)
			{
				i = send(m_forwardsock, buf + wr, len - wr, 0);
				if (i <= 0)
				{
					wxLogInfo(_("SSH error: write failed with error code %d"), wxSysErrorCode());
					goto shutdown;
				}
				wr += i;
			}
			if (libssh2_channel_eof(m_channel))
			{
				wxLogInfo(_("Connection at %s:%d disconnected by server"),
				          wxString(inet_ntoa(m_sin.sin_addr), wxConvLibc).c_str(), sport);
				goto shutdown;
			}
		}
	}
shutdown:
#ifdef WIN32
	closesocket(m_forwardsock);
#else
	close(m_forwardsock);
#endif

	if (m_channel)
	{
		libssh2_channel_close(m_channel);
		libssh2_channel_free(m_channel);
		m_channel = NULL;
	}

	return NULL;
}

void LogSSHTunnelErrors(const wxString &msg, const int &id, struct _LIBSSH2_SESSION *session)
{
	g_SSHThreadMutex.TryLock();

	wxString errorMsg = msg;
	// If session is not NULL then fetch the last error on that session
	if (session)
	{
		char *errmsg;
		int errmsg_len;
		libssh2_session_last_error(session, &errmsg, &errmsg_len, 0);
		if (errmsg_len > 0)
		{
			wxString errmsg_s(errmsg, wxConvLibc);
			errorMsg += wxString::Format(_(" [%s]"), errmsg_s.c_str());
		}
	}

	wxCommandEvent event(SSH_TUNNEL_ERROR_EVENT, id);
	// Give it some contents
	event.SetString(errorMsg);

	// Do send it
	wxPostEvent(winMain, event);

	g_SSHThreadMutex.Unlock();
}
