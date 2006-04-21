These files are used to create the wx library.

Download wxWidgets to ..\..\..\pgadmin3-deps\wxWidgets

Copy setup-msw-xxx to ..\..\..\pgadmin3-deps\wxWidgets\include\wx\setup0.h (and
to wx\setup.h and wx\msw\setup.h)

Use the wx-pgadmin.dsw workspace for making all contained modules (using the
unicode non-dll configuration)

Additional tips
===============

To view wxString objects in the VC++ debugger:

1.  Locate and open a file called AUTOEXP.DAT. Its in the 
    \Program Files\Microsoft Visual Studio\Common\MSDev98\Bin directory, if you
    are using Version 6. Search for it if you are using .NET Studio.
2.  Add the following line to the [autoexpand] section (or the bottom of the
    file). With .NET 2003 be careful not to put it in the [hresult] section!:

      wxString=<m_pchData,st>

3. Restart Visual Studio.

To prevent the debugger stepping into trivial functions, add entries similar to
the following to AUTOEXP.DAT:

[ExecutionControl]
wxString\:\:c_str.*=NoStepInto
operator new=NoStepInto
wxString\:\:wxString=NoStepInto
wxString\:\:~wxString=NoStepInto

Note that the function name match is a regexp, hence the escaping of the colons.

(Originally from: http://www.litwindow.com/Knowhow/wxHowto/wxhowto.html)



