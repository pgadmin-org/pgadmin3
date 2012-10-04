.. _slony-example:


************************
`Slony-I example`:index:
************************

In this example, a master server is setup with two direct slaves. This example was
written and tested using Slony-I v1.2.11 and PostgreSQL 8.2.5, running on a single
Windows XP machine. The PostgresSQL pgbench utility is used to generate the test 
schema and workload.

#. Create 3 databases, master, slave1 and slave2 and ensure pl/pgsql is
   setup in each.

#. Create a pgbench schema in the master database::

      > pgbench -i -U postgres master
   
#. Add a primary key called history_pkey to the history table on the tid,
   bid and aid columns

#. Create a schema-only dump of the master database, and load it into
   slave1 and slave2::

      > pg_dump -s -U postgres master > schema.sql
      > psql -U postgres slave1 < schema.sql
      > psql -U postgres slave2 < schema.sql

#. Create Slony config files for each slon engine (daemon on Unix). The
   files should contain just the following two lines::

      cluster_name='pgbench'
      conn_info='host=127.0.0.1 port=5432 user=postgres dbname=master'

   Create a file for each database, adjusting the dbname parameter as
   required and adding any other connection options that may be
   needed.

#. (Windows only) Install the Slony-I service::

      > slon -regservice Slony-I

#. Register each of the engines (this is only necessary on Windows - on
   Unix the slon daemons may be started individually and given the path
   to the config file on the command line using the -f option)::

      > slon -addengine Slony-I C:\slony\master.conf
      > slon -addengine Slony-I C:\slony\slave1.conf
      > slon -addengine Slony-I C:\slony\slave2.conf

#. In pgAdmin under the Slony Replication node in the master database,
   create a new Slony-I cluster using the following options::

      Join existing cluster: Unchecked
      Cluster name:          pgbench
      Local node:            1        Master node
      Admin node:            99       Admin node

#. Under the Slony Replication node, create a Slony-I cluster in each of
   the slave databases using the following options::

      Join existing cluster: Checked
      Server:                <Select the server containing the master database>
      Database:              master
      Cluster name:          pgbench
      Local node:            10       Slave node 1
      Admin node:            99 - Admin node
      
      Join existing cluster: Checked
      Server:                <Select the server containing the master database>
      Database:              master
      Cluster name:          pgbench
      Local node:            20       Slave node 2
      Admin node:            99 - Admin node

#. Create Paths on the master to both slaves, and on each slave back to
   the master. Create the paths under each node on the master, using the
   connection strings specified in the slon config files. Note that future
   restructuring of the cluster may require additional paths to be defined.

#. Create a Replication Set on the master using the following settings::

      ID:                  1
      Comment:             pgbench set

#. Add the tables to the replication set using the following settings::

      Table:               public.accounts
      ID:                  1
      Index:               accounts_pkey
      
      Table:               public.branches
      ID:                  2
      Index:               branches_pkey
      
      Table:               public.history
      ID:                  3
      Index:               history_pkey
      
      Table:               public.tellers
      ID:                  4
      Index:               tellers_pkey

#. On the master node, create a new subscription for each slave using the
   following options::

      Origin:              1
      Provider:            1 - Master node
      Receiver:            10 - Slave node 1
      
      Origin:              1
      Provider:            1 - Master node
      Receiver:            20 - Slave node 2

#. Start the slon service (or daemons on Unix)::

      > net start Slony-I

Initial replication should begin and can be monitored on the statistics
tab in pgAdmin for each node. The pgbench utility may be run against the
master database to generate a test workload.
