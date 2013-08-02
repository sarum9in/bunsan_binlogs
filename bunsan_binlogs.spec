%define _builddir   .
%define _sourcedir  .
%define _specdir    .
%define _rpmdir     .

Name:       bunsan_binlogs
Version:    %{yandex_mail_version}
Release:    %{yandex_mail_release}
Url:        %{yandex_mail_url}

Summary:    bunsan binary logging library
License:    GPLv3
Group:      System Environment/Libraries
Packager:   Aleksey Filippov <sarum9in@yandex-team.ru>
Distribution:   Red Hat Enterprise Linux

Requires:       boost >= 1.53.0
BuildRequires:  boost-devel >= 1.53.0
BuildRequires:  bunsan_cmake = %{version}-%{release}

BuildRoot:  %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)


%description
Library provides classes for binary logging using google::protobuf::Message.
Package also provides program for reading such logs.


%package        devel
Summary:        bunsan_binlogs development files
Requires:       %{name} = %{version}-%{release}


%description    devel
Development files.


%build
cmake . -DCMAKE_INSTALL_PREFIX=/usr \
        -DENABLE_TESTS=NO \
        -DCMAKE_BUILD_TYPE=Release
%{__make} %{?_smp_mflags}


%install
rm -rf %{buildroot}
%{__make} install DESTDIR="%{buildroot}"


%clean
%{__rm} -rf %{buildroot}


%files
%defattr (-,root,root,-)
%{_libdir}/libbunsan_binlogs.so
%{_bindir}/bunsan_binlogs_read


%files devel
%defattr (-,root,root,-)
%{_includedir}/bunsan/binlogs/*
%{_includedir}/bunsan/binlogs/detail/*
%{_includedir}/bunsan/binlogs/io/*
%{_includedir}/bunsan/binlogs/io/file/*
%{_includedir}/bunsan/binlogs/io/filter/*
%{_includedir}/bunsan/binlogs/v1/*


%changelog
