# This spec file and ancilliary files are licensed in accordance with
# The pgAdmin license.
# In this file you can find the default build package list macros.  These can be overridden
# by defining on the rpm command line:
# rpm --define 'macroname value' ... to change the value of the macro.

Summary:	Graphical client for PostgreSQL.
Name:		pgadmin3
Version:        1.4.2
Release:        
License:        Artistic
Group:          Applications/Databases
URL:		http://www.pgadmin.org/
Packager:	pgAdmin project <pgadmin-hackers@postgresql.org>
Source:		pgadmin3-%{version}-%{release}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%define wxGTK2_version 2.6
%define withwxconfig wx-config
%define desktop_vendor pgadmin
%define debug_package %{nil}

%define beta 1
%{?beta:%define __os_install_post /usr/lib/rpm/brp-compress}

# When rebuilding, wxWindows source installation is needed. Currently we don't have
# a stable RPM for wxWindows.
#BuildRequires: wxGTK2ud >= 2.5-%{wxGTK2_version}, wxGTK2ud-devel >= 2.5-%{wxGTK2_version}, wxGTK2ud-stc >= 2.5-%{wxGTK2_version}, wxGTK2ud-xrc >= 2.5-%{wxGTK2_version}

%description
 pgAdmin III is a powerful administration and development platform for the PostgreSQL 
database, free for any use. The application is running under GNU/Linux, FreeBSD and Windows 
2000/XP.

 pgAdmin III is designed to answer the needs of all users, from writing simple SQL 
queries to developing complex databases. The graphical interface supports all PostgreSQL 
features and makes administration easy. The application also includes a query builder, an 
SQL editor, a server-side code editor and much more. pgAdmin III is released with an 
installer and does not require any additional driver to communicate with the database 
server.

 pgAdmin III is developed by a community of database specialists around the world and is 
available in more than 30 languages. It is Free Software released under the Artistic 
License.

%prep
%setup -q -n %{name}-%{version}-%{release}

%build
%configure --enable-static --disable-debug
make all

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
strip --strip-debug %{buildroot}/%{_bindir}/%{name}

cp -f ./src/include/images/elephant32.xpm %{buildroot}/%{_datadir}/%{name}/%{name}.xpm
cp -f ./pkg/%{name}.desktop %{buildroot}/%{_datadir}/%{name}/%{name}.desktop

mkdir -p %{buildroot}/%{_datadir}/applications

desktop-file-install --vendor %{desktop_vendor} --delete-original  --dir %{buildroot}/%{_datadir}/applications --add-category X-Red-Hat-Base --add-category Application --add-category Development %{buildroot}/%{_datadir}/%{name}/%{name}.desktop

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%{_bindir}/*
%{_datadir}/%{name}
%{_datadir}/applications

%changelog
* Tue Oct 18 2005 Devrim GUNDUZ <devrim@gunduz.org>
- 1.4.0 beta2
- Changed configure parameters.
- Spec file makeup

* Tue Jun 28 2005 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.2

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

