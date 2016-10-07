Summary: PAM module for executing scripts.
Name: libpam-script
Version: 0.1.11
Release: tis1
License: GPL
BuildRoot: /var/tmp/%{name}-%{version}
Group: System Environment/Base
Source: %{name}-%{version}.tar.gz
Packager: Jacob Rief <jacob.rief@tiscover.com>

%description
PAM-scripts is a module which allows to execute scripts after opening and/or closing
a session using pam.

%prep
%setup

%build
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/lib/security
install --mode 755 pam_script.so $RPM_BUILD_ROOT/lib/security

%files
%doc README
%defattr(-,root,root)
/lib/security/pam_script.so

%changelog
* Thu Jul 31 2007 Jacob Rief <jacob.rief@tiscover.com>
- rebuilding for 0.1.11

* Thu Feb 27 2003 Jacob Rief <jacob.rief@tiscover.com>
- Initial revision

