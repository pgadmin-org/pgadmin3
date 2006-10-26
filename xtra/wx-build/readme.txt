This directory contains scripts for building a suitable installation of 
wxWidgets for your platform to allow you to build pgAdmin.

GTK
===

Download wxGTK to a suitable directory and unpack it. Then run the following 
command as root:

./build-wxgtk /path/to/wxgtk

wxGTK will be built in shared and static library versions, with and without
debug symbols. it will be installed in the default location, /usr/local/

Mac
===

Download wxMAC to a suitable directory and unpack it. Then run the following
command as root:

./build-wxmac /path/to/wxmac

wxMAC will be built in shared and static library versions, with and without
debug symbols. it will be installed in the default location, /usr/local/

MSW
===

Download wxMSW to ..\..\..\pgadmin3-deps\wxWidgets

Run "build-wxmsw.bat" to from a Visual Studio 2005 commandprompt to build the 
required parts of wxWidgets in both release and debug builds (unicode, non-DLL).


Additional tips 
---------------

To view wxString and other objects in the VC++ debugger:

1.  Locate and open a file called AUTOEXP.DAT. Its normally in the 
    \Program Files\Microsoft Visual Studio 8\Common7\Packages\Debugger under 
    Visual Studio 2005.

2.  Add the following line to the [autoexpand] section - be careful not to put
    it in the [Visualizer] section which makes up the bulk of the file!:

;-- wxWidgets Options ------------------------
wxString =<m_pchData,st>
wxPoint =x=<x> y=<y>
wxSize =w=<x> h=<y>
wxRect =x=<x> y=<y> w=<width> h=<height>
wxWindow =<,t> hWnd=<m_hWnd>
;---------------------------------------------


3. Restart Visual Studio.

To prevent the debugger stepping into trivial functions, add entries similar to
the following to AUTOEXP.DAT, in the [ExecutionControl] section (create it if
needed):


;-- wxWidgets Options ------------------------
wxString\:\:c_str.*=NoStepInto
operator new=NoStepInto
wxString\:\:wxString=NoStepInto
wxString\:\:~wxString=NoStepInto
;---------------------------------------------


Note that the function name match is a regexp, hence the escaping of the colons.

Originally from: 
  http://www.litwindow.com/Knowhow/wxHowto/wxhowto.html
  http://lists.wxwidgets.org/archive/wx-users/msg16845.html

