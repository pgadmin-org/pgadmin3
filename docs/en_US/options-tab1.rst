.. _options-tab1:


*********************************
pgAdmin Options - Tab 1 (General)
*********************************

.. image:: images/options-general.png

* **User language** - This option allows you to change the language
   used in the pgAdmin user interface.

* **PG help path** - This option is used to specify the path
   to the PostgreSQL help files. This may be a URL or directory containing
   the HTML format help files (note that some browsers may require 
   file:///path/to/local/files/ notation, whilst others require /path/to/local/files/
   notation), or the path to a Compiled HTML Help (.chm) file (on
   Windows), an HTML Help project (.hhp) file, or a Zip archive containing the HTML
   files and the HTML Help project file. The PostgreSQL Help is used when selected
   from the Help menu, or when working with many database objects on PostgreSQL
   servers.

* **EDB help path** - This option is used to specify the path
   to the EnterpriseDB help files, and may be configured in the same way as the PG
   help path. The EnterpriseDB Help is used when selected from the Help menu, 
   or when working with many database objects on EnterpriseDB servers.

* **GP help path** - This option is used to specify the path
   to the Greenplum help files, and may be configured in the same way as the PG
   help path. The Greenplum Help is used when selected from the Help menu,
   or when working with many database objects on Greenplum servers.

* **Slony help path** - This option is used to specify the path
   to the Slony help files, and may be configured in the same way as the PG
   help path. The Slony Help is used when selected from the Help menu.

* **System schemas** - You can suppress the display of additional schemas,
   by declaring them a system schema. This field takes a list of comma separated strings.
   
* **Slony-I path** - To enable pgAdmin III to create Slony-I replication clusters
   from scratch, you need to enter the path to the Slony-I script installation directory here. If no
   valid scripts can be found, only joining existing clusters is possible.

* **PG bin path** - This should be set to the path to the standard PostgreSQL pg_dump, pg_restore
   and pg_dumpall utilities. pgAdmin will use these for backing up and restoring your PostgreSQL databases.
   If this path is not set, pgAdmin will attempt to find the utilities in its installation directory,
   the 'hint' directory set by a local PostgreSQL installation (on Windows) and in the system path.

* **EDB bin path** - This should be set to the path to the EnterpriseDB pg_dump, pg_restore
   and pg_dumpall utilities. pgAdmin will use these for backing up and restoring your EnterpriseDB databases.
   If this path is not set, pgAdmin will attempt to find the utilities in standard locations used by
   EnterpriseDB.

* **GP bin path** - This should be set to the path to the Greenplum pg_dump, pg_restore
   and pg_dumpall utilities. pgAdmin will use these for backing up and restoring your Greenplum databases.
   If this path is not set, pgAdmin will attempt to find the utilities in standard locations used by
   Greenplum.

* **Ignore server/utility version mismatches** - When checked, pgAdmin will pass the '-i' option 
   to utilities such as <i>pg_dump</i> which will allow utilities from one version of the database to 
   be used with another version. This option should be use with extreme care as it may cause errors to
   occur during backup or restore options.
