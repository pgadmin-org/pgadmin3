%define pgadmin_version %(date +%Y%m%d)
%define pgadmin_release rh%(rpm -q --qf='%{VERSION}' redhat-release | sed 's/\\..*$//')

Summary: Graphical client for PostgreSQL.
Name: pgadmin3
Version: %{pgadmin_version}
Release: %{pgadmin_release}
License: Artistic
Group: Applications/Databases
Source: pgadmin3-%{pgadmin_version}.tar.gz
URL: http://www.pgadmin.org/
BuildRoot: %{_tmppath}/%{name}-root

#
# I am not sure of dependencies until we build wxGTK 2.5 cvs RPMs
#

Requires: gtk2 >= 2.2, glibc >= 2.3.2, libjpeg >= 6b, libpng >= 1.2.2, pango >= 1.2.1, libstdc++ >= 3.2.2, openssl >= 0.9.7, libtiff >= 3.5.7
  

#
# wxWindows 2.5 CVS version > 20030607 is needed
# Please install from source with the following options:
# ./configure --with-gtk --enable-gtk2 --enable-unicode --disable-shared --enable-debug
#
# Also, compile the following contributions in wxWindows contrib/src
# stc (Styled Text Control), xrc (xml dialogs description)
#
BuildRequires: autoconf >= 2.57, automake >= 1.7.5, gtk2-devel >= 2.2, postgresql >= 7.3.2, openssl-devel >= 0.9.7

%description
pgAdmin3 is the graphical administration interface of PostgreSQL.

%prep
%setup -q

%build
sh bootstrap
sh configure --enable-debug --enable-unicode --enable-gtk2
make all

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
/usr/local/pgadmin3

%changelog
* Mon Jun 10 2003 Jean-Michel POURE <jm.poure@freesurf.fr>
- Initial build

