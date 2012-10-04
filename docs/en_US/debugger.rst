.. _debugger:


*************************
`pgAdmin Debugger`:index:
*************************

.. image:: images/debugger.png

The debugger may be used to debug pl/pgsql functions in PostgreSQL,
as well as EDB-SPL functions, stored procedures and packages in 
EnterpriseDB.

**Note:** The debugger may only be used by roles with 'superuser' 
privileges.

In order to use the debugger, a plugin is required on your server. This
is included by default with EnterpriseDB, and is available for download on
`pgFoundry <http://pgfoundry.org/projects/edb-debugger/>`_. It is
installed as a contrib module with the Windows distribution of PostgreSQL
8.3 and above.

The debugger may be used for both in-context and direct debugging. To
debug an object in-context, right click it in the pgAdmin browser treeview,
and select the "Global breakpoint" option. The debugger will then wait for 
the next session to execute the object, and break on the first line of 
executable code. To directly debug an object, right click it and select
the "Debug" option. The debugger will prompt you for any parameter values
that may be required, invoke the object, and break on the first line
of executable code.

When entering parameter values, type the value into the appropriate cell
on the grid, or, leave the cell empty to represent NULL, enter '' (two single 
quotes) to represent an empty string, or to enter a literal string consisting 
of just two single quotes, enter \'\'. PostgreSQL 8.4 and above supports
variadic function parameters. These may be entered as a comma-delimited list
of values, quoted and/or cast as required.

Once the debugger session has started, you can step through the code using
the menu options, keyboard shortcuts or toolbar buttons. Breakpoints may be 
set or cleared by clicking in the margin of the source window, or by clicking
on the desired code line and using the "Toggle breakpoint" button or menu
option. If you step into other functions, the Stack pane may be used to navigate
to different stack frames - simply select the frame you wish to view.

When the debugger has reached the end of the executable code, if running in-context
it will wait for the next call to the function, otherwise it will prompt for 
parameter values again and restart execution. You may exit the debugger at any
time.
