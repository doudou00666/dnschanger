%global ver 0.1.0
Name:    dnschanger
Version: %{ver}
Release: 1%{?dist}
Summary: Application KDE pour changer les DNS sans redémarrage (Rust + Qt6)
License: MIT
URL:     https://github.com/doudou00666/dnschanger
Source:  %{name}-%{version}.tar.gz

BuildRequires: cargo, rust, cmake, ninja, qt6-qtbase-devel
Requires:      qt6-qtbase, networkmanager

%description
Application de bureau (KDE / Plasma) permettant de changer les DNS de la
connexion active sans redémarrer la distribution. Le noyau est écrit en
Rust, l'interface en Qt6. Le catalogue intégré contient 31 résolveurs
DNS alternatifs issus de la page « Résolveurs DNS Alternatifs » de
sebsauvage.net.

%prep
%autosetup -n %{name}

%build
pushd rust
cargo build --release
popd
pushd qt
mkdir build
cd build
cmake ..
make
popd

%install
install -D -m755 qt/build/dnschanger \
    %{buildroot}/%{_localbase}/bin/dnschanger
install -D -m644 dnschanger.desktop \
    %{buildroot}/%{_localbase}/share/applications/dnschanger.desktop

%files
%{_localbase}/bin/dnschanger
%{_localbase}/share/applications/dnschanger.desktop

%changelog
* Thu Nov 06 2023 doudou00666 <doudou00666@users.noreply.github.com> - 0.1.0-1
- initial release
