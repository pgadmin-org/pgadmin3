%define major_version 0.1.1
%define minor_version %(date +%Y%m%d)

Summary: Graphical client for PostgreSQL.
Name: pgadmin3
Version: %{major_version}
Release: %{minor_version}
License: Artistic
Group: Applications/Databases
Source: pgadmin3-%{version}.tar.gz
URL: http://www.pgadmin.org/
Packager: pgAdmin project <pgadmin-hackers@postgresql.org>
BuildRoot: %{_tmppath}/%{name}-root

#
# When rebuilding, wxWindows 2.5 CVS version > 20030607 is needed.
# wxWindows 2.5 CVS (RPM and SRPMs) snapshots can be downloaded from
# http://snake.pgadmin.org in the directory of your distribution.
# 

BuildRequires: autoconf >= 2.57, automake >= 1.7.2, wxGTK2ud-cvs >= 2.5-20030607, wxGTK2ud-cvs-devel, wxGTK2ud-cvs-stc, wxGTK2ud-cvs-xrc

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

