%global ver 0.1.2
Name:    dnschanger
Version: %{ver}
Release: 1%{?dist}
Summary: Application KDE pour changer les DNS sans redémarrage (Rust + Qt6)
License: GPLv3
URL:     https://github.com/doudou00666/dnschanger
Source:  %{name}-%{version}.tar.gz

BuildRequires: cargo, rust, cmake, ninja-build, qt6-qtbase-devel
Requires:      qt6-qtbase, NetworkManager

%description
Application de bureau (KDE / Plasma) permettant de changer les DNS de la
connexion active sans redémarrer la distribution. Le noyau est écrit en
Rust, l'interface en Qt6. Le catalogue intégré contient 28 résolveurs
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
    %{buildroot}/usr/bin/dnschanger
install -D -m644 dnschanger.desktop \
    %{buildroot}/usr/share/applications/dnschanger.desktop

%files
/usr/bin/dnschanger
/usr/share/applications/dnschanger.desktop

%changelog
* Fri Nov 06 2026 doudou00666 <doudou00666@users.noreply.github.com> - 0.1.2-1
- GUI : sélection des résolveurs via cases à cocher (checkboxes)
- Suppression de 3 entrées sans IP listée (LibreDNS.gr-Ads, bortzmeyer.fr, ShaftInc.fr)
- Catalogue : 28 résolveurs
* Fri Nov 06 2026 doudou00666 <doudou00666@users.noreply.github.com> - 0.1.1-1
- Sélection multiple de résolveurs avant validation (liste combinée)
- DNS personnalisés (IPv4/IPv6, liste séparée par des virgules)
- Validation de la liste d'IP (rejet des tokens invalides)
- 12 tests unitaires + tests de régression FFI
* Thu Nov 06 2026 doudou00666 <doudou00666@users.noreply.github.com> - 0.1.0-1
- initial release
