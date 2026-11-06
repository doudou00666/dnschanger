// test_fi — small C test for the FFI
#include <stdio.h>
#include <stdint.h>

extern uint32_t      dnslib_count();
extern const char*   dnslib_name(uint32_t);
extern const char*   dnslib_tag(uint32_t);
extern const char*   dnslib_ip(uint32_t);
extern const char*   dnslib_notes(uint32_t);
extern int           dnslib_usable(uint32_t);
extern void          dnslib_reset_store();
extern const char*   dnslib_active_connection();
extern const char*   dnslib_current_dns();
extern const char*   dnslib_set_dns(uint32_t);

int main() {
    printf("=== Catalogue ===\n");
    uint32_t n = dnslib_count();
    printf("Count: %u\n", n);
    for (uint32_t i = 0; i < n; ++i) {
        printf("[%u] %s | %s | %s | usable=%d\n",
            i, dnslib_name(i), dnslib_tag(i), dnslib_ip(i), dnslib_usable(i));
    }
    printf("\n=== Active connection ===\n");
    printf("Conn: %s\n", dnslib_active_connection());
    printf("Current DNS: %s\n", dnslib_current_dns());
    printf("\n=== Apply index 0 (FDN) ===\n");
    printf("Result: %s\n", dnslib_set_dns(0));
    printf("\n=== After apply ===\n");
    printf("Conn: %s\n", dnslib_active_connection());
    printf("Current DNS: %s\n", dnslib_current_dns());
    dnslib_reset_store();
    printf("\nOK\n");
    return 0;
}
