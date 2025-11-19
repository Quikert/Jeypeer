#include <stdio.h>
#include <string.h>

static int run_sha256sum(const char *input, char *out_hex64) {
    // 
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "printf %%s \"%s\" | sha256sum | awk '{print $1}'", input);
    FILE *p = fopen(cmd, "r");
    if (!p) return -1;
    if (!fgets(out_hex64, 65, p)) { fclose(p); return -2; }
    fclose(p);
    out_hex64[64] = '\0';
    return 0;
}

void jp_sha256_hex(const char *payload, char out_hex64[65]) {
    if (run_sha256sum(payload, out_hex64) != 0) {
        // 
        strcpy(out_hex64, "0000000000000000000000000000000000000000000000000000000000000000");
    }
}
