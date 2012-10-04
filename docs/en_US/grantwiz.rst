.. _grantwiz:


*********************
`Grant Wizard`:index:
*********************

.. image:: images/grantwiz.png

The Grant Wizard allows assignment of a set of privileges to groups and users to
multiple objects (tables, sequences, views and functions) in a convenient way.

The Selection tab will show all applicable objects under the currently selected object.
Check all objects you like to grant privileges on.

The security tab defines the privileges and groups/users that should be granted. Assigning 
no privileges to a group or user will result in revoking all rights for them.

The SQL tab shows the sql commands that are generated from the selected grant targets.

When clicking OK, the sql commands will be executed; any result will be displayed in the 
Messages tab.
