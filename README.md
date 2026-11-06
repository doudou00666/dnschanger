# dnschanger

Application de bureau (KDE / Plasma) permettant de **changer les DNS de la
connexion active sans redémarrer la distribution**.

- **Noyau** : Rust (`rust/`, bibliothèque `dnslib` statique, C-ABI)
- **Interface** : Qt6 Widgets (`qt/`)
- **Fonctionnement** : `nmcli con modify` + `nmcli con up` sur la connexion
  active détectée via le route par défaut (`ip route show default`).

## Catalogue intégrée

31 résolveurs DNS issus de la page
[« Résolveurs DNS Alternatifs »](https://sebsauvage.net/wiki/doku.php?id=dns-alternatifs)
(FDN, dns.sb, DNSForge, Mullvad, DNS4All, LibreDNS, Quad9, ControlD,
AdGuard, CloudFlare, Google, OpenDNS, NextDNS…), avec indication du type
de filtrage (☀️ aucun, 💵 pubs, 👿 malware, 💋 adulte, 👮 non-neutre).

## Compilation

```bash
# 1. bibliothèque Rust
cd rust && cargo build --release

# 2. application Qt6
cd ../qt && mkdir build && cd build
cmake .. && make
./dnschanger
```

Dépendances : `rust`, `cargo`, `cmake`, `ninja`, `qt6-qtbase-devel`.

## RPM

Le spécificateur est fourni dans `rpm/dnschanger.spec`.

```bash
rpmbuild --define 'dist .%{release}' rpm/dnschanger.spec
```

ou utiliser `rpm/` comme dossier de build standard :

```bash
mkdir -p ~/rpmbuild/{BUILD,SOURCES,SPECS}
tar czf ~/rpmbuild/SOURCES/dnschanger-%version.tar.gz -C . .
rpmbuild -D 'dist ' rpm/dnschanger.spec   # le tarball doit correspondre au Source:
```

## Licence

MIT (le code est libre de redistribution).
