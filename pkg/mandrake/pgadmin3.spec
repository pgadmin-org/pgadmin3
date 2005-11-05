%define name		pgadmin3
%define major_version 	1.2.0
%define minor_version 	1mdk
%define wxGTK2_version 	2.5.3
%define withwxconfig 	wx-config
%define desktop_vendor 	pgadmin
%define debug_package 	%{nil}
%define __os_install_post /usr/lib/rpm/brp-compress

Summary:	Graphical client for PostgreSQL.
Name: 		%{name}
Version:	%{major_version}
Release:	%{minor_version}
License: 	Artistic
Group: 		Databases
Source: 	%{name}-%{major_version}.tar.bz2
URL: 		http://www.pgadmin.org/
Packager: 	pgAdmin project <pgadmin-hackers@postgresql.org>
BuildRoot: 	%{_tmppath}/%{name}-root
BuildRequires:  postgresql postgresql-devel

#
# When rebuilding, wxWindows 2.5 CVS version is needed.
# wxWindows 2.5 CVS (RPM and SRPMs) snapshots can be downloaded from
# http://www.pgadmin.org/pgadmin3/development.php page in the Snapshots section
#

#BuildRequires: wxGTK2ud >= 2.5-%{wxGTK2_version}, wxGTK2ud-devel >= 2.5-%{wxGTK2_version}, wxGTK2ud-stc >= 2.5-%{wxGTK2_version}, wxGTK2ud-xrc >= 2.5-%{wxGTK2_version}

%description
PostgreSQL Tools.

%prep
%setup -q

%build
export CFLAGS=""
export CXXFLAGS=""
%ifarch x86_64 amd64
%configure --with-wx-config=%{withwxconfig} --with-wx=/usr --with-ssl LDFLAGS=-L/usr/X11R6/lib64
%else
%configure --with-wx-config=%{withwxconfig} --with-wx=/usr --with-ssl
%endif
make all

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
#strip --strip-debug %{buildroot}/%{_bindir}/%{name}

cp -f ./src/include/images/elephant48.xpm %{buildroot}/%{_datadir}/%{name}/%{name}.xpm
cp -f ./pkg/%{name}.desktop %{buildroot}/%{_datadir}/%{name}/%{name}.desktop

mkdir -p %{buildroot}/%{_datadir}/applications

desktop-file-install --vendor %{desktop_vendor} --delete-original  --dir %{buildroot}/%{_datadir}/applications --add-category X-Red-Hat-Base --add-category Application --add-category Development %{buildroot}/%{_datadir}/%{name}/%{name}.desktop

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root, 755 )
%doc LICENCE.txt README.txt BUGS.txt
%doc docs/en_US/*
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications

%changelog
* Thur Feb 10 2005 Dave Page <dpage@pgadmin.org>
- 1.2.0
- Add 64 bit libs per Vasilev Max

* Tue Dec 3 2004 Hugo Ferreira <hmf at inescporto.pt>
- 1.2.0 
- Some changes for MDK 10.1 (name, BuildRequires, minor_version to 1mdk  and 
  Group to Databases). Checked w/ rpmlint. Still has problems.

* Wed Nov 17 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 RC2 

* Mon Nov 15 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 RC1 rebuilt
- Fixed spec file so that beta and rc tags will be considered. (Note: AFAICS my previous rpms were broken :( )

* Thu Nov 4 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 RC1

* Mon Jun 10 2003 Jean-Michel POURE <pgadmin-hackers@postgresql.org>
- 1.2.0 Initial build

