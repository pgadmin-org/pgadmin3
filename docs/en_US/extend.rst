.. _extend:


**************************
`Extended features`:index:
**************************

pgAdmin III's functionality can be further enhanced using the *adminpack* contrib
module for PostgreSQL. This is a library of additional functions that may be installed 
on your server which allows pgAdmin to offer additional features, such as:

* The sizes on disk of tablespaces, databases, tables and indexes are
  displayed on the Statistics tab in the :ref:`main window <main>`.

* The :ref:`Status dialogue <status>` should be able to display the
  server logfiles (if logging is appropriately configured).

* Running queries from other users may be cancelled from the 
  :ref:`Status dialogue <status>`.

* The server's configuration files postgresql.conf and pg_hba.conf
  may be editted, and reloaded remotely.

The adminpack is installed and activated by default if you are running the 
'official' pgInstaller distribution of PostgreSQL for Windows and is included as a
contrib module with all versions of PostgreSQL 8.2 and above. However, if you are 
running any other version of PostgreSQL you will need to manually install it. To do so,
simply copy the appropriate adminpack source code to the /contrib directory of your
pre-configured PostgreSQL source tree. You can download the adminpacks
`here <http://www.pgadmin.org/download/adminpacks.php>`_.

Run the following commands (substituting *admin* for *admin81* 
if required)::

  # cd $PGSRC/contrib/admin
  # make all
  # make install

The module should now be built, and installed into your existing PostgreSQL installation.
To be used, you must now install the module into the database you use as pgAdmin's *Maintenance
DB* (normally *postgres*, though you may wish to use a different database). To do so, 
locate the *admin.sql* or *admin81.sql* file installed in the previous step - normally 
this may be found in /usr/local/pgsql/share/contrib/ and run it against your database. You will 
normally need to do this as the PostgreSQL superuser::

  $ psql postgres < /usr/local/pgsql/share/contrib/admin.sql

The next time you connect to the server with pgAdmin, it will detect the presence of the adminpack
and offer full functionality.

Since PostgreSQL 9.1, you don't need to use psql. You can manually add
adminpack extension or make pgAdmin do the same by clicking on the "Fix It!"
button of the guru hint.
