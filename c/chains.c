#include <stdio.h>
#include <string.h>

int verify_chain(const char *chain_path) {
    FILE *f = fopen(chain_path, "r");
    if (!f) return -1;
    char line[1024];
    char prev_hash[65] = "GENESIS";
    unsigned long expected_index = 0;
    while (fgets(line, sizeof(line), f)) {
        unsigned long idx, ts;
        char ph[65], h[65], dat[256];
        if (sscanf(line, "%lu,%lu,%64[^,],%64[^,],%255[^\n]", &idx, &ts, ph, h, dat) != 5) {
            fclose(f);
            return -2;
        }
        if (idx != expected_index) { fclose(f); return -3; }
        if (expected_index == 0 && strcmp(ph, "GENESIS") != 0) { fclose(f); return -4; }
        if (expected_index > 0 && strcmp(ph, prev_hash) != 0) { fclose(f); return -5; }
        strcpy(prev_hash, h);
        expected_index++;
    }
    fclose(f);
    return 0;
}
