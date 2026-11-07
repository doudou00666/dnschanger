// dnslib — C-ABI FFI library
//  * Built-in catalogue of alternative DNS resolvers (from sebsauvage's wiki)
//  * Change the active connection's DNS (NetworkManager, no reboot required)
//  * Apply a custom / combined list of DNS servers (multi-select or manual)
//
// The FFI surface uses `const char*` (UTF-8) for all data.

use std::ffi::CString;
use std::os::raw::{c_char, c_int};

// ---------------------------------------------------------------------------
// Resolver catalogue
// ---------------------------------------------------------------------------

pub struct Resolver {
    /// Display name
    pub name: &'static str,
    /// Short tag for GUI (filter icons)
    pub tag: &'static str,
    /// IPv4 / IPv6 addresses, comma separated (empty if none)
    pub ip: &'static str,
    /// Human notes (filtering, jurisdiction, warnings)
    pub notes: &'static str,
    /// Whether usable for `nmcli` (has at least one IP address)
    pub usable: bool,
}

const RESOLVERS: &[Resolver] = &[
    Resolver {
        name: "FDN",
        tag: "☀️",
        ip: "80.67.169.12,80.67.169.40",
        notes: "Association à but non-lucratif, France. Aucun filtrage.",
        usable: true,
    },
    Resolver {
        name: "dns.sb",
        tag: "☀️",
        ip: "185.222.222.222,45.11.45.11",
        notes: "Entreprise (hébergeur), Allemagne. Aucun filtrage.",
        usable: true,
    },
    Resolver {
        name: "DNSForge.de",
        tag: "👿💵",
        ip: "176.9.93.198,176.9.1.117",
        notes: "Individu, Allemagne. Filtrage pubs/trackers/malware. 100 req/10 s.",
        usable: true,
    },
    Resolver {
        name: "DNSForge.de-Clean",
        tag: "👿💵💋",
        ip: "49.12.223.2,49.12.43.208",
        notes: "Individu, Allemagne. Filtrage pubs/trackers/adultes + SafeSearch.",
        usable: true,
    },
    Resolver {
        name: "Mullvad",
        tag: "☀️",
        ip: "194.242.2.2",
        notes: "Entreprise (VPN), Suède. Aucun filtrage. Service de Mozilla VPN.",
        usable: true,
    },
    Resolver {
        name: "Mullvad-AdBlock",
        tag: "💵",
        ip: "194.242.2.3",
        notes: "Suède. Filtrage publicités/trackers.",
        usable: true,
    },
    Resolver {
        name: "Mullvad-Base",
        tag: "👿💵",
        ip: "194.242.2.4",
        notes: "Suède. Filtrage pubs/trackers/malware.",
        usable: true,
    },
    Resolver {
        name: "Mullvad-Extended",
        tag: "👿💵",
        ip: "194.242.2.5",
        notes: "Suède. Filtrage pubs/trackers/malware + réseaux sociaux.",
        usable: true,
    },
    Resolver {
        name: "Mullvad-Family",
        tag: "👿💵💋",
        ip: "194.242.2.6",
        notes: "Suède. Filtrage pubs/trackers/malware + contenu adulte.",
        usable: true,
    },
    Resolver {
        name: "Mullvad-All",
        tag: "👿💵💋",
        ip: "194.242.2.9",
        notes: "Suède. Filtrage pubs/trackers/adultes + réseaux sociaux.",
        usable: true,
    },
    Resolver {
        name: "DNS4All.eu",
        tag: "☀️",
        ip: "194.0.5.3,194.0.5.64",
        notes: "Pays-Bas. Aucun filtrage.",
        usable: true,
    },
    Resolver {
        name: "LibreDNS.gr",
        tag: "☀️",
        ip: "88.198.92.222,192.71.166.92",
        notes: "Collectif. Aucun filtrage.",
        usable: true,
    },
    Resolver {
        name: "Quad9",
        tag: "👮‍♂️👿",
        ip: "9.9.9.9,149.112.112.112",
        notes: "Fondation, Suisse. Filtrage malveillants. Censure gouvernementale possible.",
        usable: true,
    },
    Resolver {
        name: "ControlD-Unfiltered",
        tag: "☀️",
        ip: "76.76.2.0,76.76.10.0",
        notes: "Entreprise, Canada. Aucun filtrage.",
        usable: true,
    },
    Resolver {
        name: "ControlD-Malware",
        tag: "👿",
        ip: "76.76.2.1,76.76.10.1",
        notes: "Canada. Filtrage domaines malveillants.",
        usable: true,
    },
    Resolver {
        name: "ControlD-Ads & Tracking",
        tag: "👿💵",
        ip: "76.76.2.2,76.76.10.2",
        notes: "Canada. Filtrage malveillants/pub/tracking.",
        usable: true,
    },
    Resolver {
        name: "ControlD-Family Friendly",
        tag: "👿💵💋",
        ip: "76.76.2.4,76.76.10.4",
        notes: "Canada. Filtrage malveillants/pub/adultes.",
        usable: true,
    },
    Resolver {
        name: "dns.watch",
        tag: "☀️",
        ip: "84.200.69.80,84.200.70.40",
        notes: "Allemagne. Aucun filtrage.",
        usable: true,
    },

    Resolver {
        name: "AdGuard-unfiltered",
        tag: "☀️",
        ip: "94.140.14.140,94.140.14.141",
        notes: "Entreprise, Chypre. Aucun filtrage.",
        usable: true,
    },
    Resolver {
        name: "AdGuard",
        tag: "💵",
        ip: "94.140.14.14,94.140.15.15",
        notes: "Chypre. Filtrage publicités/trackers.",
        usable: true,
    },
    Resolver {
        name: "AdGuard-famille",
        tag: "💵💋",
        ip: "94.140.14.15,94.140.15.16",
        notes: "Chypre. Filtrage pubs/adultes + SafeSearch.",
        usable: true,
    },
    Resolver {
        name: "CloudFlare DNS",
        tag: "👮‍♂️",
        ip: "1.1.1.1,1.0.0.1",
        notes: "USA. Non neutre. Censure pour ordres gouvernementaux.",
        usable: true,
    },
    Resolver {
        name: "CloudFlare-AntiMalware",
        tag: "👮‍♂️👿",
        ip: "1.1.1.2,1.0.0.2",
        notes: "USA. Non neutre. Malware/phishing.",
        usable: true,
    },
    Resolver {
        name: "CloudFlare-Family",
        tag: "👮‍♂️👿💋",
        ip: "1.1.1.3,1.0.0.3",
        notes: "USA. Non neutre. Malware + contenu adulte.",
        usable: true,
    },
    Resolver {
        name: "Google DNS",
        tag: "👮‍♂️",
        ip: "8.8.8.8,8.8.4.4",
        notes: "USA. Non neutre. Censure pour ordres gouvernementaux.",
        usable: true,
    },
    Resolver {
        name: "OpenDNS",
        tag: "☀️",
        ip: "208.67.222.222,208.67.220.220",
        notes: "Cisco, USA. Inaccessible depuis la France.",
        usable: true,
    },
    Resolver {
        name: "OpenDNS-FamilyShield",
        tag: "💋",
        ip: "208.67.222.123,208.67.220.123",
        notes: "Cisco, USA. Sites adultes. Inaccessible depuis la France.",
        usable: true,
    },
    Resolver {
        name: "NextDNS.io",
        tag: "👿💵",
        ip: "45.90.28.222,45.90.30.222",
        notes: "USA. Filtrage malveillants/pub/tracking. 300k req/mois (gratuit).",
        usable: true,
    },
];

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Thread-local store: we keep CStrings alive so the GUI doesn't free a
// pointer that Rust has already dropped.
thread_local! {
    static STORE: std::cell::RefCell<Vec<CString>> = std::cell::RefCell::new(Vec::new());
}

/// Convert a &str to a stable *const c_char, keeping the CString alive in
// thread-local storage.
fn store(s: &str) -> *const c_char {
    let c = match CString::new(s) {
        Ok(c) => c,
        Err(_) => return std::ptr::null(),
    };
    let ptr = c.as_ptr();
    STORE.with(|store| store.borrow_mut().push(c));
    ptr
}

/// Validate a comma-separated list of DNS servers (IPv4/IPv6).
/// Returns (ok, valid_list, error_message).
/// - Splits on commas. Trims whitespace.
/// - Each token must parse as an IPv4 or IPv6 address.
/// - Empty input -> error.
pub fn parse_ip_list(csv: &str) -> (bool, String, String) {
    let tokens: Vec<&str> = csv.split(',').collect();
    let mut out: Vec<String> = Vec::new();
    for t in tokens {
        let tok = t.trim();
        if tok.is_empty() {
            continue; // tolerate trailing commas
        }
        if tok.parse::<std::net::Ipv4Addr>().is_ok()
            || tok.parse::<std::net::Ipv6Addr>().is_ok()
        {
            out.push(tok.to_string());
        } else {
            return (false, String::new(), format!("'{}' n'est pas une adresse IP valide", tok));
        }
    }
    if out.is_empty() {
        return (false, String::new(), "aucune adresse IP valide".to_string());
    }
    // de-duplicate while preserving order
    let mut seen: Vec<String> = Vec::new();
    for s in out.clone() {
        if !seen.contains(&s) {
            seen.push(s);
        }
    }
    (true, seen.join(","), String::new())
}

/// Get the default-route network device name (e.g. `enp1s0`), if any.
fn default_route_device() -> Option<String> {
    let out = std::process::Command::new("ip")
        .args(["route", "show", "default"])
        .output();
    let out = out.ok()?;
    let s = String::from_utf8(out.stdout).unwrap_or_default();
    for line in s.lines() {
        if line.contains("default") {
            if let Some(dev) = line.rsplit(' ').next() {
                if dev.contains(':') {
                    let d = dev.split(':').next().unwrap_or("").to_string();
                    return Some(d);
                } else {
                    return Some(dev.to_string());
                }
            }
        }
    }
    None
}

/// Best connection currently active. Returns the nmcli connection NAME.
fn active_connection_id() -> Option<String> {
    let out = std::process::Command::new("nmcli")
        .args(["--terse", "con", "show"])
        .output();
    let out = out.ok()?;
    if !out.status.success() {
        return None;
    }
    let s = String::from_utf8(out.stdout).unwrap_or_default();
    let mut lines: Vec<&str> = s.lines().collect();
    lines.retain(|l| !l.to_lowercase().contains("name"));

    if let Some(dev) = default_route_device() {
        for line in &lines {
            let fields: Vec<&str> = line.split(':').collect();
            if fields.len() >= 4 && fields[3] == dev {
                return Some(fields[0].to_string());
            }
        }
    }
    for line in &lines {
        let fields: Vec<&str> = line.split(':').collect();
        if fields.len() >= 4 && fields[2] != "loopback" {
            return Some(fields[0].to_string());
        }
    }
    None
}

/// Run nmcli and return the stdout+stderr as a single string.
fn nmcli(args: &[&str]) -> (bool, String) {
    let result = std::process::Command::new("nmcli")
        .args(args)
        .output();
    match result {
        Ok(out) => {
            let mut msg = String::from_utf8(out.stdout).unwrap_or_default();
            let err = String::from_utf8(out.stderr).unwrap_or_default();
            if !err.is_empty() {
                msg.push('\n');
                msg.push_str(&err);
            }
            (out.status.success(), msg.trim().to_string())
        }
        Err(e) => (false, format!("Erreur Command::nmcli: {:?}", e)),
    }
}

/// Apply an arbitrary, validated DNS server list (comma separated) to the
/// active connection, re-activating it so the change applies without reboot.
/// Returns a human-readable status string as UTF-8.
fn apply_ip_list(ip_csv: &str) -> String {
    let conn = match active_connection_id() {
        Some(c) => c,
        None => return "aucune connexion active".to_string(),
    };
    let (ok, msg) = nmcli(&["con", "modify", &conn, "ipv4.dns", ip_csv]);
    if !ok {
        return format!("échec nmcli con modify: {}", msg);
    }
    let (ok2, msg2) = nmcli(&["con", "up", &conn]);
    if !ok2 {
        return format!("échec nmcli con up: {}", msg2);
    }
    std::thread::sleep(std::time::Duration::from_millis(500));
    let (ok3, cur) = nmcli(&["--terse", "con", "show", &conn]);
    if ok3 {
        format!("OK — DNS appliqué. Actuel: {}", cur)
    } else {
        format!("OK — DNS appliqué.")
    }
}

// ---------------------------------------------------------------------------
// FFI surface
// ---------------------------------------------------------------------------

#[no_mangle]
pub extern "C" fn dnslib_count() -> u32 {
    RESOLVERS.len() as u32
}

#[no_mangle]
pub extern "C" fn dnslib_name(index: u32) -> *const c_char {
    match RESOLVERS.get(index as usize) {
        Some(r) => store(r.name),
        None => std::ptr::null(),
    }
}

#[no_mangle]
pub extern "C" fn dnslib_tag(index: u32) -> *const c_char {
    match RESOLVERS.get(index as usize) {
        Some(r) => store(r.tag),
        None => std::ptr::null(),
    }
}

#[no_mangle]
pub extern "C" fn dnslib_ip(index: u32) -> *const c_char {
    match RESOLVERS.get(index as usize) {
        Some(r) => store(r.ip),
        None => std::ptr::null(),
    }
}

#[no_mangle]
pub extern "C" fn dnslib_notes(index: u32) -> *const c_char {
    match RESOLVERS.get(index as usize) {
        Some(r) => store(r.notes),
        None => std::ptr::null(),
    }
}

#[no_mangle]
pub extern "C" fn dnslib_usable(index: u32) -> c_int {
    match RESOLVERS.get(index as usize) {
        Some(r) => r.usable as c_int,
        None => 0,
    }
}

#[no_mangle]
pub extern "C" fn dnslib_reset_store() {
    STORE.with(|store| {
        store.borrow_mut().clear();
    });
}

#[no_mangle]
pub extern "C" fn dnslib_active_connection() -> *const c_char {
    match active_connection_id() {
        Some(s) => store(&s),
        None => std::ptr::null(),
    }
}

/// Current DNS servers for the active connection (comma separated), read via
/// `resolvectl status` (the actual runtime DNS). Returns "" if none.
#[no_mangle]
pub extern "C" fn dnslib_current_dns() -> *const c_char {
    let out = std::process::Command::new("resolvectl")
        .arg("status")
        .output();
    let out = match out {
        Ok(o) if o.status.success() => o,
        _ => return store(""),
    };
    let s = String::from_utf8(out.stdout).unwrap_or_default();
    let mut dns = String::new();
    for line in s.lines() {
        if line.contains("DNS Servers") {
            let val = line.split("DNS Servers:").nth(1).unwrap_or("");
            let cleaned: Vec<&str> = val.split_whitespace().collect();
            if !cleaned.is_empty() {
                if !dns.is_empty() {
                    dns.push(',');
                }
                dns.push_str(&cleaned.join(","));
            }
        }
    }
    store(&dns)
}

/// Apply the resolver at `index` to the active connection (single). Returns a
/// human-readable status string as UTF-8.
#[no_mangle]
pub extern "C" fn dnslib_set_dns(index: u32) -> *const c_char {
    let idx = index as usize;
    let Some(r) = RESOLVERS.get(idx) else {
        return store("index invalide");
    };
    if !r.usable {
        return store("ce résolveur n'a pas d'IP (DOT/IPv6 only)");
    }
    store(&apply_ip_list(r.ip))
}

/// Apply a CUSTOM, user-provided, comma-separated list of DNS servers to the
/// active connection. The list is validated (each token must be a valid IPv4
/// or IPv6 address). Returns a human-readable status string as UTF-8.
#[no_mangle]
pub extern "C" fn dnslib_apply_custom(ip_csv: *const c_char) -> *const c_char {
    let s = unsafe {
        if ip_csv.is_null() {
            return store("aucune liste fournie");
        }
        std::ffi::CStr::from_ptr(ip_csv)
            .to_str()
            .map(|s| s.to_string())
            .unwrap_or_default()
    };
    let (ok, valid, err) = parse_ip_list(&s);
    if !ok {
        return store(&format!("Liste personnalisée invalide: {}", err));
    }
    store(&apply_ip_list(&valid))
}

// ---------------------------------------------------------------------------
// tests
// ---------------------------------------------------------------------------

#[test]
fn catalogue_size() {
    assert_eq!(RESOLVERS.len(), 28);
}

#[test]
fn all_have_names() {
    for r in RESOLVERS {
        assert!(!r.name.is_empty());
    }
}

#[test]
fn all_usable_resolvers_have_ips() {
    for r in RESOLVERS {
        if r.usable {
            let (_, _, err) = parse_ip_list(r.ip);
            assert!(err.is_empty(), "IP non valide pour {}: {}", r.name, err);
        }
    }
}

#[test]
fn parse_single_ipv4() {
    let (ok, list, err) = parse_ip_list("8.8.8.8");
    assert!(ok);
    assert_eq!(list, "8.8.8.8");
    assert!(err.is_empty());
}

#[test]
fn parse_multi_ipv4() {
    let (ok, list, _) = parse_ip_list("8.8.8.8, 8.8.4.4,1.1.1.1");
    assert!(ok);
    assert_eq!(list, "8.8.8.8,8.8.4.4,1.1.1.1");
}

#[test]
fn parse_trailing_comma() {
    let (ok, list, _) = parse_ip_list("1.1.1.1,");
    assert!(ok);
    assert_eq!(list, "1.1.1.1");
}

#[test]
fn parse_invalid_token() {
    let (ok, _, err) = parse_ip_list("bananafds");
    assert!(!ok);
    assert!(!err.is_empty());
}

#[test]
fn parse_empty() {
    let (ok, _, _) = parse_ip_list("");
    assert!(!ok);
    let (ok2, _, _) = parse_ip_list(", ,,");
    assert!(!ok2);
}

#[test]
fn parse_ipv6() {
    let (ok, list, _) = parse_ip_list("262d:22:70e7::1");
    assert!(ok);
    assert_eq!(list, "262d:22:70e7::1");
}

#[test]
fn parse_dedup() {
    let (_, list, _) = parse_ip_list("1.1.1.1,1.1.1.1,1.1.1.1");
    assert_eq!(list, "1.1.1.1");
}

#[test]
fn parse_mixed_ipv4_ipv6() {
    let (ok, list, _) = parse_ip_list("1.1.1.1,262d:22:70e7::1");
    assert!(ok);
    assert_eq!(list, "1.1.1.1,262d:22:70e7::1");
}

#[test]
fn combined_multi_select_ip() {
    // Simulate combining two catalogue resolvers' IP lists
    let a = "80.67.169.12,80.67.169.40";
    let b = "9.9.9.9,149.112.112.112";
    let combined = format!("{},{}", a, b);
    let (ok, list, _) = parse_ip_list(&combined);
    assert!(ok);
    assert_eq!(list, "80.67.169.12,80.67.169.40,9.9.9.9,149.112.112.112");
}
