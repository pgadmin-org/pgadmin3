# This spec file and ancilliary files are licensed in accordance with
# The pgAdmin license.
# In this file you can find the default build package list macros.  These can be overridden
# by defining on the rpm command line:
# rpm --define 'macroname value' ... to change the value of the macro.

Summary:	Graphical client for PostgreSQL
Name:		pgadmin3
Version:        PGADMIN_LONG_VERSION
Release:        1
License:        Artistic
Group:          Applications/Databases
URL:		http://www.pgadmin.org/
Packager:	pgAdmin project <pgadmin-hackers@postgresql.org>
Source:		pgadmin3-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-root-%(%{__id_u} -n)
BuildRequires:	postgresql-devel
Requires:	postgresql

%define wxGTK2_version 2.6
%define withwxconfig wx-config
%define desktop_vendor pgadmin

%define beta 0
%{?beta:%define __os_install_post /usr/lib/rpm/brp-compress}

# When rebuilding, wxWindows source installation is needed. Currently we don't have
# a stable RPM for wxWindows.
#BuildRequires: wxGTK2ud >= 2.5-%{wxGTK2_version}, wxGTK2ud-devel >= 2.5-%{wxGTK2_version}, wxGTK2ud-stc >= 2.5-%{wxGTK2_version}, wxGTK2ud-xrc >= 2.5-%{wxGTK2_version}

%description
pgAdmin III is a powerful administration and development 
platform for the PostgreSQL database, free for any use.
It is designed to answer the needs of all users,
from writing simple SQL queries to developing complex 
databases. The graphical interface supports all PostgreSQL 
features and makes administration easy. 

The application also includes a query builder, an SQL 
editor, a server-side code editor and much more. 

%package docs
Summary: Documentation for pgAdmin3
Group: Applications/Databases
Requires: %{name} = %{version}
%description docs
This package contains documentation for various languages,
which are in html format.

%prep
%setup -q -n %{name}-%{version}

%build
%configure --disable-debug
make all
# Remove the following line for next release (Currently: 1.4.3)
 for i in `find docs -iname "*.*"`; do dos2unix -q $i; done

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
strip --strip-debug %{buildroot}/%{_bindir}/%{name}
chmod +x $RPM_BUILD_ROOT/%{_bindir}/pgagent
chmod +x $RPM_BUILD_ROOT/%{_bindir}/pgadmin3

cp -f ./src/include/images/elephant32.xpm %{buildroot}/%{_datadir}/%{name}/%{name}.xpm
cp -f ./pkg/%{name}.desktop %{buildroot}/%{_datadir}/%{name}/%{name}.desktop

mkdir -p %{buildroot}/%{_datadir}/applications

desktop-file-install --vendor %{desktop_vendor} --delete-original  --dir %{buildroot}/%{_datadir}/applications --add-category X-Red-Hat-Base --add-category Application --add-category Development %{buildroot}/%{_datadir}/%{name}/%{name}.desktop

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%{_bindir}/*
%doc LICENCE.txt BUGS.txt README.txt
%{_datadir}/%{name}
%{_datadir}/applications

%files docs
%defattr(-,root,root)
%doc docs/*


%changelog
* Thu Jul 20 2006 Devrim GUNDUZ <devrim@commandprompt.com> 1.4.3-1
- 1.4.3
- Fixed all rpmlint errors and some warnings
- Moved all html docs to a new -docs rpm

* Thu Mar 9 2006 David Fetter <david@fetter.org> 1.4.2-1
- 1.4.2

* Sat Dec 10 2005 Devrim GUNDUZ <devrim@commandprompt.com> 1.4.1-1
- 1.4.1

* Mon Nov 7 2005 Devrim GUNDUZ <devrim@gunduz.org> 1.4.0-1
- 1.4.0 Gold

* Tue Nov 1 2005 Devrim GUNDUZ <devrim@gunduz.org> 1.4.0-RC1
- 1.4.0 RC1 

* Fri Oct 21 2005 Devrim GUNDUZ <devrim@gunduz.org> 1.4.0-beta3
- 1.4.0 beta3

* Tue Oct 18 2005 Devrim GUNDUZ <devrim@gunduz.org> 1.4.0-beta2
- 1.4.0 beta2
- Changed configure parameters.
- Spec file makeup

* Tue Jun 28 2005 Devrim GUNDUZ <devrim@gunduz.org> 1.2.2
- 1.2.2

* Mon Nov 29 2004 Devrim GUNDUZ <devrim@gunduz.org> 1.2.0
- 1.2.0 Gold

* Wed Nov 17 2004 Devrim GUNDUZ <devrim@gunduz.org> 1.2.0-RC2
- 1.2.0 RC2 

* Mon Nov 15 2004 Devrim GUNDUZ <devrim@gunduz.org> 1.2.0-RC1
- 1.2.0 RC1 rebuilt
- Fixed spec file so that beta and rc tags will be considered. (Note: AFAICS my previous rpms were broken :( )

* Thu Nov 4 2004 Devrim GUNDUZ <devrim@gunduz.org> 1.2.0-RC1
- 1.2.0 RC1

* Mon Jun 10 2003 Jean-Michel POURE <pgadmin-hackers@postgresql.org> 1.1.0
- Initial build

