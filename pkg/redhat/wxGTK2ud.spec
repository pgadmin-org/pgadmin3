%define minor_version 20030707
%define major_version 2.5

Summary: The GTK+ 2 port of the wxWindows library
Name: wxGTK2ud
Version: %{major_version}
Release: %{minor_version}
Copyright: wxWindows Licence
Group: X11/Libraries
Source: %{name}-%{version}.tar.bz2
URL: http://www.wxwindows.org
Packager: pgAdmin project <pgadmin-hackers@postgresql.org>
BuildRoot: %{_tmppath}/%{name}-root

# The package advertises only for libwx_gtk-%{ver2}.a
# Therefore, there is no incompatibility with wxGTK 2.4

Provides: libwx_gtk2ud-%{version}.a

%description
wxWindows is a free C++ library for cross-platform GUI development.
With wxWindows, you can create applications for different GUIs (GTK+,
Motif/LessTif, MS Windows, Mac) from the same source code. These packages
are provided with Unicode and debug features and linked agains GTK2 staticly.
You can use them for the development of pgAdmin3. They will not conflict with
existing GTK 2.4 libraries.

%package devel
Summary: The GTK+ 2 port of the wxWindows library
Group: X11/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Header files for wxGTK, the GTK+ 2 port of the wxWindows library.

%package xrc
Summary: The XML-based resource system for the wxWindows library.
Group: System Environment/Libraries
Requires: %{name} = %{version}-%{release}

%description xrc
The XML-based resource system, known as XRC, allows user interface
elements such as dialogs, menu bars and toolbars, to be stored in
text files and loaded into the application at run-time.

%package stc
Summary: Styled text control.

Group: System Environment/Libraries
Requires: %{name} = %{version}-%{release}

%description stc
Styled text control based on the Scintillia project http://www.scintilla.org/.

%prep
%setup -q

%build
./configure --prefix=%{buildroot}/usr --with-gtk --enable-gtk2 --enable-unicode --enable-debug --disable-shared
make 

pushd contrib/src/xrc
  make
popd
pushd contrib/utils/wxrc
  make
popd

pushd contrib/src/stc
  make
popd

%install
rm -rf %{buildroot}
make install

pushd contrib/src/xrc
make install
popd
pushd contrib/utils/wxrc
cp -r wxrc %{buildroot}%{_bindir}/
popd

pushd contrib/src/stc
make install
popd

# RedHat does it by default. Needed for SuSE and Mandrake.
strip --strip-debug %{buildroot}%{_libdir}/libwx_gtk2ud-%{version}.a
strip --strip-debug %{buildroot}%{_libdir}/libwx_gtk2ud_xrc-%{version}.a
strip --strip-debug %{buildroot}%{_libdir}/libwx_gtk2ud_stc-%{version}.a

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig
ln -s /usr/lib/wx/include/gtk2ud-2.5/wx/setup.h  /usr/include/wx/setup.h
sed -e "s/var\/tmp\/wxGTK2ud-root\/usr/usr/g" /usr/bin/wxgtk2ud-2.5-config > /var/tmp/tmpfoo
mv -f /var/tmp/tmpfoo /usr/bin/wxgtk2ud-2.5-config

%postun -p sbin/ldconfig
rm -fr /usr/include/wx/setup.h

%post xrc -p /sbin/ldconfig
%postun xrc -p /sbin/ldconfig

%post stc -p /sbin/ldconfig
%postun stc -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_libdir}/libwx_gtk2ud-%{version}.a
%{_datadir}/wx

%files devel
%defattr(-,root,root)
%{_includedir}/wx
%{_libdir}/wx
%{_bindir}/wxgtk2ud-%{version}-config
%{_datadir}/aclocal/*.m4

%files xrc
%defattr(-, root, root)
%{_libdir}/libwx_gtk2ud_xrc-%{version}.a
%{_bindir}/wxrc

%files stc
%defattr(-, root, root)
%{_libdir}/libwx_gtk2ud_stc-%{version}.a

%changelog
* Mon Jun 10 2003 Jean-Michel POURE <jm.poure@freesurf.fr>
- Initial build.
