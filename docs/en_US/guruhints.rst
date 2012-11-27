.. _guruhints:


*******************
`Guru Hints`:index:
*******************

pgAdmin III has some functions included which should help to implement healthy 
and good performing databases. They are the result of the long experience of the
developers worldwide, and the design problems commonly observed.

If pgAdmin III thinks it has a valuable hint for you, it will pop up a guru hint unless
you have chosen to suppress that individual hint or all hints (see 
:ref:`options <options-miscellaneous>`). You can bring up the hint by pressing
the hint button in the toolbar at any time, if enabled.

.. image:: images/guru-connect.png

This is an example of a guru hint, as it appears when trying to connect to a 
non-running or misconfigured PostgreSQL server.

If you check 'do not show this hint again', the automatic popup of the individual hint will be 
suppressed in the future, until it is reset in the 
:ref:`options <options-miscellaneous>`. Error messages will still appear, but without
the extended information as provided by the hints.
