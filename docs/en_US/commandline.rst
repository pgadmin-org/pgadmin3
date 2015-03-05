.. _commandline:


********************************
`Command Line Parameters`:index:
********************************

There are a number of command line options that may be passed to the pgAdmin executable
when starting it.

* **-h** Display command line option help.

* **-s <server description>** Auto connect to the specified server.
  <server description> is the description entered when you added the server to
  the treeview in pgAdmin, quoted if required. If multiple servers are
  registered with the same description, the first found will be auto-connected.

* **-S** Open a :ref:`Server Status <status>` window upon connection. If used
  together with the **-s <server description>** option, the server status window
  will automatically be connected to this server.
  If used alone, a dialog box will appear prompting for the connection information.
  This switch cannot be used together with the **-Sc <connection string>** option.

* **-Sc <connection string>** Open a :ref:`Server Status <status>` window upon
  connection. The server status window will automatically be connected to the
  server specified in the connection string. This option cannot be used together
  with the **-S** switch.
  
  **NOTE:** The "password" connection string option will not be recognised for
  security reasons. Please use a 
  `PGPASSFILE <http://www.postgresql.org/docs/current/static/libpq.html#libpq-connect>`_ 
  if you need to store the connection password.

* **-q** Open a :ref:`Query Tool <query>` window upon connection. If used
  together with the  **-s <server description>** option, the query tool
  will automatically be connected to this server.
  If used alone, a dialog box will appear prompting for the connection
  information. This switch cannot be used together with the **-qc
  <connection string>** option.

* **-qc <connection string>** Open a :ref:`Query Tool <query>` window upon
  connection. The query tool will automatically be connected to the server
  specified in the connection string. This option cannot be used together with
  the **-q** switch.
  
  **NOTE:** The "password" connection string option will not be recognised for
  security reasons. Please use a 
  `PGPASSFILE <http://www.postgresql.org/docs/current/static/libpq.html#libpq-connect>`_ 
  if you need to store the connection password.

* **-f <SQL script>** Automatically open the specified SQL script in the
  :ref:`Query Tool <query>`. Only useful with -q or -qc.

* **-ch <hba file>** Edit the specified pg_hba.conf config file.

* **-cm <config file>** Edit the specified postgresql.conf config file.
  
* **-cp <pgpass file>** Edit the specified .pgpass/pgpass.conf config file.

* **-c <dir>** Edit the config files in specified cluster directory.

* **-t** Start in dialogue translation test mode. This mode allows each dialogue
  to be viewed without running the  application to assist with string translation.

