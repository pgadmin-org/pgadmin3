.. _slony-path:


***********************************
`Creating paths and listens`:index:
***********************************

.. _path:

Create paths to other nodes
===========================

.. image:: images/slony-path.png

Slony-I needs path information, that defines how a slon process can
communicate to other nodes. The conninfo string takes a connect string
as described in the `libpq connection <http://www.postgresql.org/docs/current/static/libpq.html#LIBPQ-CONNECT>`_
documentation. Usually, you will need to specify host, dbname and
username, while the password should be stored in the
`.pgpass file <http://www.postgresql.org/docs/current/static/libpq-pgpass.html>`_.

You must create a path to every other node, on each node. For example, in a two node cluster
you need to create a path to the slave on the master, and one to the master on the slave.

.. _listen:

Create listens to other nodes
=============================

.. image:: images/slony-listen.png

After the communication path has been defined, the slon processes need
to be advised to listen to events from other nodes.

**Note:** This step is not necessary for Slony-I v1.1 and later, because
listen information is generated automatically when paths are defined.
