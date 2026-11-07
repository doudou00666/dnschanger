# dnschanger

Application de bureau (KDE / Plasma) permettant de **changer les DNS de la
connexion active sans redémarrer la distribution**.

- **Noyau** : Rust (`rust/`, bibliothèque `dnslib` statique, C-ABI)
- **Interface** : Qt6 Widgets (`qt/`)
- **Fonctionnement** : `nmcli con modify` + `nmcli con up` sur la connexion
  active détectée via le route par défaut (`ip route show default`).

## Fonctionnalités

- **Cases à cocher** : chaque résolveur du catalogue est une case à cocher
  ; cocher plusieurs résolveurs puis valider combine leurs IP en une liste
  unique appliquée d'un seul coup.
- **DNS personnalisés** : saisir une liste d'IP IPv4/IPv6 (séparées par des
  virgules) pour un résolveur hors catalogue.
- **Validation** : chaque token est validé (IPv4/IPv6). Les listes
  invalides sont rejetées avec un message d'erreur.
- **Catalogue** : 28 résolveurs DNS issus de la page
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

## Tests

Les tests unitaires sont intégrés au noyau Rust :

```bash
cd rust && cargo test
```

12 tests couvrant : taille du catalogue, intégrité des entrées, parsing des
listes d'IP (IPv4/IPv6, multi, dédup, rejet des tokens invalides, etc.).

Les tests de régression FFI (`test/test_fi.c`) valident l'application
réelle des DNS via `nmcli`.

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

GPL-3.0 (voir `LICENSE`).
