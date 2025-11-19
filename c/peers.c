#include <stdio.h>
#include <string.h>

int peers_add(const char *path, const char *peer) {
    FILE *f = fopen(path, "a"); if (!f) return -1;
    fprintf(f, "%s\n", peer); fclose(f); return 0;
}
int peers_list(const char *path) {
    FILE *f = fopen(path, "r"); if (!f) return -1;
    char line[256]; while (fgets(line, sizeof(line), f)) printf("peer: %s", line);
    fclose(f); return 0;
}
int peers_remove(const char *path, const char *peer) {
    FILE *f = fopen(path, "r"); if (!f) return -1;
    FILE *tmp = fopen("/tmp/peers.tmp", "w"); if (!tmp) { fclose(f); return -2; }
    char line[256]; int removed = 0;
    while (fgets(line, sizeof(line), f)) {
        char *nl = strchr(line, '\n'); if (nl) *nl = '\0';
        if (strcmp(line, peer) != 0) fprintf(tmp, "%s\n", line); else removed = 1;
    }
    fclose(f); fclose(tmp); remove(path); rename("/tmp/peers.tmp", path);
    return removed ? 0 : 1;
}
