.. _slony-execute:


*****************************************
`Execute DDL scripts with Slony-I`:index:
*****************************************

.. image:: images/slony-execute.png

Most schema changes that can be executed with pgAdmin III are replication-aware, i.e. 
you can advise pgAdmin III to apply the change (create/alter) using the replication facilities, 
simply by selecting a replication set that is subscribed to all nodes which should
receive the PostgreSQL object.
