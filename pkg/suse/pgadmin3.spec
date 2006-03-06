%define major_version 1.4.2
%define minor_version 1.st2
%define wxGTK2_version 2.6.2
%define wx_config /opt/wxGTK-static/bin/wx-config
%define wx_prefix /opt/wxGTK-static

Summary:      Graphical tools for PostgreSQL.
Name:         pgadmin3
Version:      %{major_version}
Release:      %{minor_version}
License:      Artistic
Group:        Applications/Databases
Source:       pgadmin3-%{major_version}.tar.bz2
Patch0:	      pgadmin3-AMD64.patch
URL:          http://www.pgadmin.org/
BuildRoot:    %{_tmppath}/%{name}-root


BuildRequires: gcc-c++
BuildRequires: glib2-devel
BuildRequires: gtk2-devel
BuildRequires: openssl-devel 
BuildRequires: libstdc++-devel
BuildRequires: postgresql-devel
BuildRequires: krb5-devel 
BuildRequires: krb5-devel-static
BuildRequires: wxGTK-static >= %{wxGTK2_version}

Requires:      xorg-x11

%description
pgAdmin III is a powerful administration and development platform for the 
PostgreSQL database, free for any use. The application is running under 
GNU/Linux, FreeBSD and Windows 2000/XP.
pgAdmin III is designed to answer the needs of all users, from writing simple 
SQL queries to developing complex databases. The graphical interface supports 
all PostgreSQL features and makes administration easy. The application also 
includes a query builder, an SQL editor, a server-side code editor and much more. 
pgAdmin III is released with an installer and does not require any additional 
driver to communicate with the database server.

%debug_package
%prep
%setup -q
%ifarch x86_64 amd64
%patch0
%endif

%build
export CFLAGS=""
export CXXFLAGS=""
%configure \
    --enable-static \
    --libdir=%{_libdir} \
    --with-wx=%{wx_prefix} \
    --with-ssl \
    LDFLAGS="-L/usr/%_lib -L/usr/X11R6/%_lib" 
make all LDFLAGS="-L/usr/%_lib -L/usr/X11R6/%_lib -lkrb5"

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
#strip --strip-debug %{buildroot}/%{_bindir}/%{name}

cp -f ./src/include/images/elephant48.xpm %{buildroot}/%{_datadir}/%{name}/%{name}.xpm
cp -f ./pkg/%{name}.desktop %{buildroot}/%{_datadir}/%{name}/%{name}.desktop

mkdir -p %{buildroot}/%{_datadir}/applications

#
# Fix for wxWidgets-2.6.x environment bug
#

mv %buildroot%{_bindir}/%{name} %buildroot%{_bindir}/%{name}-bin
cat > %buildroot%{_bindir}/%{name} << EOF
#!/bin/bash
#
# This is a wrapper for wxWidgets-2.6.x bug
#
G_FILENAME_ENCODING=UTF-8 /usr/bin/pgadmin3-bin

EOF

chmod a+x %buildroot%{_bindir}/%{name}

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%{_bindir}/%{name}
%{_bindir}/%{name}-bin
%{_datadir}/%{name}
%{_datadir}/applications

%changelog
* Tue Jun 21 2005 Vasilev Max <max@stranger-team.ru>
- minor spec fix
- build with PostgreSQL 8.0.3 header (current in 9.3)

* Sun Jun 18 2005 Vasilev Max <max@stranger-team.ru>
- up to date wx to 2.6.1 (use only GTK part)
- up to date version to 1.2.2

* Wed Apr 6 2005 Vasilev Max <max@stranger-team.ru>
- up to date wx to 2.6.0
- up to date version to 1.2.1

* Thu Feb 8 2005 Vasilev Max <max@stranger-team.ru>
- fix requieres and description
- build with PortgreSQL 8.0.1

* Thu Feb 8 2005 Vasilev Max <max@stranger-team.ru>
- fix spec for AMD64

* Mon Nov 29 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 Gold

* Wed Nov 17 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 RC2 

* Mon Nov 15 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 RC1 rebuilt
- Fixed spec file so that beta and rc tags will be considered. (Note: AFAICS my previous rpms were broken :( )

* Thu Nov 4 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 RC1

* Mon Jun 10 2003 Jean-Michel POURE <pgadmin-hackers@postgresql.org>
- Initial build

