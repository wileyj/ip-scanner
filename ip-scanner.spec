Name:		ipscan
Version:	0.0.1
Release:	1.mtvi
Summary:  	Network Scanner
License: 	GPLv2
Group: 		Utilities
BuildArch:	x86_64
Source0:	%{name}-%{version}.tar.gz
BuildRoot: 	%{_tmppath}/%{realname}-%{version}-%{release}-root-%(%{__id_u} -n)

%description 
Utility 
1) similar to nslookup to provide ip's of a specified host. 
2) performs network scans to determine if an ip is responding to icmp request
3) allows searching for a variable number of ips in a specified network/mask

BuildRequires: gcc-c++

%prep

%setup -q -n %{name}-%{version}
%configure 
make %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} install DESTDIR="%{buildroot}"

%clean
%{__rm} -rf %{buildroot}

%files 
%defattr(-, root, root, 0755)
%{_sbindir}/ipscan

%changelog
* Tue Dec 10 2013 Jesse Wiley<jesse.wiley@viacom.com> - 0.0.1-1
- initial build.
