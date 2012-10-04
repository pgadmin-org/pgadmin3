.. _connect-error:


**************************
`Connection errors`:index:
**************************

When connecting to a PostgreSQL server, you might get error messages
that need some more explanations. If you encounter them, please read the
following information carefully, we're sure they will help you.

* `notrunning`_
* `no-hba`_

.. _notrunning:

could not connect to Server: Connection refused
===============================================

.. image:: images/not-running.png

If this message appears, there are two possible reasons for this: either
the server isn't running at all. Simply start it.

The other non-trivial cause for this message is that the server isn't
configured to accept TCP/IP requests on the address shown.

For security reasons, a PostgreSQL server "out of the box" doesn't
listen on TCP/IP ports. Instead, it has to be enabled to listen for
TCP/IP requests. This can be done by adding **tcpip = true** to the
postgresql.conf file for Versions 7.3.x and 7.4.x, or 
**listen_addresses='*'** for Version 8.0.x and above; this will make the
server accept connections on any IP interface.

For further information, please refer to the PostgreSQL documentation
about `runtime configuration <http://www.postgresql.org/docs/current/interactive/runtime-config.html>`_.

.. _no-hba:

FATAL: no pg_hba.conf entry
===========================

.. image:: images/no-hba.png

If this message appears, your server can be contacted correctly over the
network, but isn't configured to accept your connection. Your client
isn't detected as a legal user for the database.

You will have to add an entry in the form
**host template1 postgres 192.168.0.0/24 md5** for IPV4 or 
**host template1 postgres ::ffff:192.168.0.0/120 md5** for IPV6 networks
to the  pg_hba.conf file.

For further information, please refer to the PostgreSQL documentation
about `client authentication <http://www.postgresql.org/docs/current/interactive/client-authentication.html>`_.
