.. _restore:


*******
Restore
*******

.. image:: images/restore.png
.. image:: images/restore-2.png
.. image:: images/restore-3.png
.. image:: images/restore-4.png

The restore dialogue presents a somewhat simplified interface to the
PostgreSQL `pg_restore <http://www.postgresql.org/docs/current/interactive/app-pgrestore.html>`_
tool. 

After you selected a valid backup file, pgAdmin will read the
contents of the file when you press the View button. This may take
some time, dependend on the size of the file. You can watch the
progress on the Messages tab. After the examination has completed,
pgAdmin will display the contents on the Contents tab.

Intentionally, the tool will not create the database to be
restored. In order to restore a database, first create it using
pgAdmin, and then start the restore tool with that fresh database
selected. The data will be restored to that database.

You can restore a single object, when in restore database mode (this
is displayed in the title bar, as in this example). After
examining the backup file, you can select the desired table from the
Contents tab, and check the Single Object box. Only that table will be
restored.

You can select a table as restore destination. pgAdmin will
restore its data if it is present in the backup file.

In order to use backup, the pg_restore tool must be accessible by
pgAdmin. This can be accomplished by having it locatable using the
path, or by copying it into the same directory where the pgadmin3
executable resides.
