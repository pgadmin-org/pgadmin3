.. _connect:


**************************
`Connect to server`:index:
**************************

.. image:: images/server.png

Use this dialog to add a new server connection to the pgAdmin tree.

If you're experiencing connection problems, check the :ref:`connection problems
<connect-error>`.

The name is a text to identify the server in the pgAdmin tree.

The host is the IP address of the machine to contact, or the fully
qualified domain name. On Unix based systems, the address field may 
be left blank to use the default PostgreSQL Unix Domain Socket on
the local machine, or be set to an alternate path containing a PostgreSQL
socket. If a path is entered, it must begin with a "/". The port number
may also be specified.

The service field is the name of a service configured in the
pg_service.conf file. For details, see the `pg_service
documentation <http://www.postgresql.org/docs/9.0/interactive/libpq-pgservice.html>`_.

The maintenance DB field is used to specify the initial database that 
pgAdmin connects to, and that will be expected to have the :ref:`pgAgent <pgagent>`
schema and :ref:`adminpack <extend>` objects
installed (both optional). On PostgreSQL 8.1 and above, the maintenance
DB is normally called 'postgres', and on earlier versions 'template1' is
often used, though it is preferrable to create a 'postgres' database for 
this purpose to avoid cluttering the template database.

If you select "Store password", pgAdmin stores passwords you enter in the ~/.pgpass
file under Unix or :file:%APPDATA%\postgresql\pgpass.conf
under Win32 for later reuse. For details, see `pgpass documentation
<http://www.postgresql.org/docs/current/interactive/libpq-pgpass.html>`_. It
will be used for all libpq based tools. If you want the password removed, you
can select the server's properties and uncheck the selection any time.

The colour field allows you to set a specific colour for this server. This
colour will be used in the background of the tree where each object of
this server is displayed.

The Group field is used to push your server in a specific group. You
can have a production group, and a test group. Or LAN specific groups.
It's completely up to you. But it helps when you have lors of server to
register.

The second tab has all the SSL specific options: what kind of SSL connection
you want, your root certificate file, your server CRL, your client
certificate file, and finally your client key file.

You can use the "SSH Tunneling" tab to connect pgAdmin (through an intermediary 
proxy host) to a server that resides on a network to which the client may 
not be able to connect directly.

*  Check the box next to "Use SSH tunneling?" to specify thatpgAdmin should use 
   an SSH tunnel when connecting to the specified server.
*  Specify the name or IP address of the SSH host (through which client 
   connections will be forwarded) in the *Tunnel host* field.
*  Specify the name of a user with connection privileges for the SSH host 
   in the *Username* field.
*  Specify the type of authentication that will be used when connecting to the 
   SSH host in the *Authentication* field.

   *  Select the radio button next to *Password* to specify that pgAdmin will 
      use a password for authentication by the SSH host.  This is the default.
   *  Select the radio button next to *Identity file* to specify that pgAdmin
      will use a private key file when connecting.
* If the SSH host is expecting a private key file for authentication, use the 
  *Identity file* field to specify the location of the key file.
* If the SSH host is expecting a password, use the *Password/Passphrase* field
  to specify a the password, or if an identity file is being used, specify the
  passphrase.

The fourth tab contains some advanced options that are seldomly used.

The "Connect now?" field makes pgAdmin attempt a connection as soon as
you hit the OK button.

You can unckeck the "Connect now" checkbox if you don't want the connection
to the server being established immediately, but only registered for later use. In this
case, the connection parameters won't be validated.

The "Restore env?" option determines whether or not pgAdmin will attempt to restore the 
browser environment when you reconnect to this server. If you regularly use different
databases on the same server you might want to turn this option off.

The Rolename field allows you to connect as a role, and then get the
permissions of another one (the one you specified in this field). The
connection role must be a member of the rolename.

The DB restriction field allows you to enter an SQL restriction that 
will be used against the `pg_database
<http://www.postgresql.org/docs/current/interactive/catalog-pg-database.html>`_
table to limit the databases that you see. For example, you might enter: 
*'live_db', 'test_db'* so that only live_db and test_db
are shown in the pgAdmin browser. Note that you can also limit the schemas
shown in the database from the Database properties dialogue by entering a
restriction against `pg_namespace
<http://www.postgresql.org/docs/current/interactive/catalog-pg-namespace.html>`_.

The "Service ID" field specifies parameters to control the database
service process. Its meaning is operating system dependent.

If pgAdmin is running on a Windows machine, it can control the
postmaster service if you have enough access rights. Enter the name of
the service. In case of a remote server, it must be prepended by the
machine name (e.g. PSE1\pgsql-8.0). pgAdmin will automatically
discover services running on your local machine.

If pgAdmin is running on a Unix machine, it can control processes
running on the local machine if you have enough access rights. Enter a
full path and needed options to access the pg_ctl program. When
executing service control functions, pgAdmin will append
status/start/stop keywords to this. Example::

  sudo /usr/local/pgsql/bin/pg_ctl -D /data/pgsql

This dialog can be launched at a later time to correct or add
parameters by executing "properties" when the server is selected. You should
not be connected to it if you want to make changes on its properties.

Contents:

.. toctree::
   :maxdepth: 2

   connect-error
