#include <stdio.h>
#include <string.h>
#include <stdint.h>
extern uint32_t       dnslib_count();
extern const char*    dnslib_name(uint32_t);
extern const char*    dnslib_tag(uint32_t);
extern const char*    dnslib_ip(uint32_t);
extern const char*    dnslib_notes(uint32_t);
extern int            dnslib_usable(uint32_t);
extern const char*    dnslib_apply_custom(const char* ip);
extern const char*    dnslib_current_dns();
extern const char*    dnslib_active_connection();
extern void           dnslib_reset_store();

int main() {
    printf("== Tests regression FFI ==\n");
    uint32_t n = dnslib_count();
    printf("Catalogue: %u entries\n", n);

    // R1: multi-select combo FDN(0) + Quad9(13)
    const char* fdn = dnslib_ip(0);
    const char* q9  = dnslib_ip(13);
    char combined[256];
    snprintf(combined, sizeof(combined), "%s,%s", fdn, q9);
    printf("R1 - Combinaison FDN+Quad9: %s\n", combined);
    const char* res = dnslib_apply_custom(combined);
    printf("R1 - Resultat: %s\n", res);

    // R2: custom DNS single
    const char* r2 = dnslib_apply_custom("9.9.9.9");
    printf("R2 - Custom single 9.9.9.9: %s\n", r2);

    // R3: invalid custom
    const char* r3 = dnslib_apply_custom("blabla,foo");
    printf("R3 - Invalid list: %s\n", r3);

    // R4: current DNS readback
    const char* cur = dnslib_current_dns();
    printf("R4 - Current DNS: %s\n", cur);

    // R5: active connection
    const char* act = dnslib_active_connection();
    printf("R5 - Active connection: %s\n", act);

    // R6: catalogue integrity — 31 entries, all names non-null
    int bad = 0;
    for (uint32_t i = 0; i < n; i++) {
        if (!dnslib_name(i) || !dnslib_ip(i)) bad++;
    }
    printf("R6 - Catalogue integrity: %d bad entries\n", bad);

    dnslib_reset_store();
    printf("== Fin ==\n");
    return bad > 0 ? 1 : 0;
}
