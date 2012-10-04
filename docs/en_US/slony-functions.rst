.. _slony-functions:


**********************
`Slony-I tasks`:index:
**********************

In the appropriate context, pgAdmin III can trigger several maintenance tasks.

.. _restart:

* **Restart node** - Under some conditions, it may be necessary to
  restart a slon process on a node, initialize as if freshly started and
  make it reload its configuration. This can be triggered remotely with
  the restart node function.

.. _lock:

* **Lock set** - Disables updates to a set to be replicated. This is 
  necessary for clean switch over the source of a replication from one
  node to another.

.. _unlock:

* **Unlock set** - Re-enables updates to a set to be replicated after a
  previous lock.

.. _merge:

* **Merge set** - Joins two sets, originating from the same node and
  subscribed by the same nodes, into one. This can be used as a
  workaround to the fact that a subscribed set can't be extended.

.. _move:

* **Move set** - Moves a replication set from one node to another, i.e.
  making the target node the new source. The old node will become
  subscriber to the new provider node. This is the normal way how to
  switch over gracefully the master role from one node to another.
