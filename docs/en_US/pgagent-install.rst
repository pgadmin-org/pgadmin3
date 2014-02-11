.. _pgagent-install:


*****************************
`pgAgent Installation`:index:
*****************************

pgAgent runs as a daemon on Unix systems, and a service on Windows systems.
In the majority of cases it will run on the database server itself - it is 
for this reason that pgAgent is not automatically setup when pgAdmin is 
installed. In some cases however, it may be preferable to run pgAgent on
multiple systems, against the same database - individual jobs may be targetted
at a particular host, or left for execution by any host. Locking prevents 
execution of the same instance of a job by multiple hosts.

Database setup
==============

pgAgent stores its configuration in the 'postgres' database in your cluster. This 
database exists by default in PostgreSQL 8.1 versions, for earlier versions
you need to create the database yourself. The database
must have the pl/pgsql procedural language installed - PostgreSQL's 
'createlang' program can do this if required.

Connect to the "postgres" database (once created if needed), and open the SQL tool.
If the server is 9.1 or later, and pgAgent 3.4.0 or later, simply paste in this 
query and click the 'Run' button:

    CREATE EXTENSION pgagent;

This command will create a number of tables and other objects in a schema called
'pgagent'.

For earlier versions of PostgreSQL or pgAgent, select the File -> Open option from the
menu and find the 'pgagent.sql' script installed with pgAdmin. The
installation location for this file varies from operating system to operating
system, however it will normally be found under 'C:\Program files\pgAdmin III'
on Windows systems (or
'C:\Program files\PostgreSQL\8.x\pgAdmin III' if installed with the PostgreSQL server installer),
or '/usr/local/pgadmin3/share/pgadmin3' or '/usr/share/pgadmin3' on Unix 
systems. Once the file is loaded, click the 'Run' button to execute the script.

The script will create a number of tables and other objects in a schema called
'pgagent'.

Daemon installation on Unix
===========================

To install the pgAgent daemon on a Unix system, you will normally need to have root
privileges to modify the system startup scripts - doing so is quite system specific
so you should consult your system documentation for further information.

The program itself takes few command line options - most of which are only
needed for debugging or specialised configurations::

  Usage:
    /path/to/pgagent [options] <connect-string>
  
  options:
    -f run in the foreground (do not detach from the terminal)
    -t <poll time interval in seconds (default 10)>
    -r <retry period after connection abort in seconds (>=10, default 30)>
    -s <log file (messages are logged to STDOUT if not specified)>
    -l <logging verbosity (ERROR=0, WARNING=1, DEBUG=2, default 0)>

The connect string required is a standard PostgreSQL libpq connection
string (see the `PostgreSQL documentation on the connection string
<http://www.postgresql.org/docs/current/static/libpq.html#libpq-connect>`_
for further details). For example, the following
command lilne will run pgAgent against a server listening on the localhost,
using a database called 'pgadmin', connecting as the user 'postgres'::

  /path/to/pgagent hostaddr=127.0.0.1 dbname=postgres user=postgres

Service installation on Windows
===============================

pgAgent is able to self-install itself as a service on Windows systems.
The command line options available are similar to those on Unix systems, but
include an additional parameter to tell the service what to do::

  Usage:
    pgAgent REMOVE <serviceName>
    pgAgent INSTALL <serviceName> [options] <connect-string>
    pgAgent DEBUG [options] <connect-string>

    options:
      -u <user or DOMAIN\user>
      -p <password>
      -d <displayname>
      -t <poll time interval in seconds (default 10)>
      -r <retry period after connection abort in seconds (>=10, default 30)>
      -l <logging verbosity (ERROR=0, WARNING=1, DEBUG=2, default 0)>

The service may be quite simply installed from the command line as follows
(adjusting the path as required)::

  "C:\Program Files\pgAdmin III\pgAgent" INSTALL pgAgent -u postgres -p secret hostaddr=127.0.0.1 dbname=postgres user=postgres

The service may then be started from the command line using *net start pgAgent*,
or from the *Services* control panel applet. Any logging output or errors will be
reported in the Application event log. The DEBUG mode may be used to run pgAgent from a
command prompt. When run this way, log messages will output to the command window.

Security concerns
=================

pgAgent is a very powerful tool, but does have some security considerations
that you should be aware of:

**Database password** - *DO NOT* be tempted to include a password in
the pgAgent connection string - on Unix systems it may be visible to all users
in 'ps' output, and on Windows systems it will be stored in the registry in
plain text. Instead, use a libpq *~/.pgpass* file to store the passwords for
every database that pgAgent must access. Details of this technique may be
found in the `PostgreSQL documentation on .pgpass file
<http://www.postgresql.org/docs/current/static/libpq-pgpass.html>`_.

**System/database access** - all jobs run by pgAgent will run with the
security privileges of the pgAgent user. SQL steps will run as the user that
pgAgent connects to the database as, and batch/shell scripts will run as the
operating system user that the pgAgent service or daemon is running under.
Because of this, it is essential to maintain control over the users that are
able to create and modify jobs. By default, only the user that created the
pgAgent database objects will be able to do this - this will normally be the
PostgreSQL superuser.

