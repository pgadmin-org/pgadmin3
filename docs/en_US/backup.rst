.. _backup:


******
Backup
******

.. image:: images/backup.png
.. image:: images/backup-2.png
.. image:: images/backup-3.png
.. image:: images/backup-4.png

The backup dialogue presents a somewhat simplified interface to the
PostgreSQL `pg_dump <http://www.postgresql.org/docs/current/interactive/app-pgdump.html>`_ tool. 
You can backup a single table, a schema or a complete database,
dependent on the object you select when starting the backup
tool. 

pg_dump does not support all options for all backup file
formats. Particularly, to backup blobs the PLAIN format can not be
used. Also, a PLAIN file can not be interpreted, and can not be
restored using pgAdmin. The PLAIN format will create an SQL script
that can be executed using the psql tool. For standard backup and
restore purposes, the COMPRESS and TAR options are recommended.

In order to use backup, the pg_dump tool must be accessible by
pgAdmin. This can be accomplished by having it locatable using the
path, or by copying it into the same directory where the pgadmin3
executable resides.
