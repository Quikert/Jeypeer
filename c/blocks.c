#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void jp_sha256_hex(const char *payload, char out_hex64[65]);

#define MAX_DATA 256

typedef struct {
    unsigned long index;
    unsigned long timestamp;
    char prev_hash[65];
    char hash[65];
    char data[MAX_DATA];
} Block;

static void compute_block_hash(const Block *b, char out[65]) {
    char payload[768];
    snprintf(payload, sizeof(payload), "%lu|%lu|%s|%s",
             b->index, b->timestamp, b->prev_hash, b->data);
    jp_sha256_hex(payload, out);
}

int block_to_file(const char *path, const Block *b) {
    FILE *f = fopen(path, "a");
    if (!f) return -1;
    fprintf(f, "%lu,%lu,%s,%s,%s\n", b->index, b->timestamp, b->prev_hash, b->hash, b->data);
    fclose(f);
    return 0;
}

int block_genesis(const char *chain_path, const char *log_path) {
    Block b; memset(&b, 0, sizeof(b));
    b.index = 0;
    b.timestamp = (unsigned long)time(NULL);
    strcpy(b.prev_hash, "GENESIS");
    strcpy(b.data, "jeypeer-genesis");
    compute_block_hash(&b, b.hash);

    if (block_to_file(chain_path, &b) != 0) return -1;

    FILE *l = fopen(log_path, "a");
    if (l) { fprintf(l, "[Genesis] idx=%lu hash=%s\n", b.index, b.hash); fclose(l); }
    return 0;
}

int block_append(const char *chain_path, const char *log_path, const char *data) {
    FILE *f = fopen(chain_path, "r");
    if (!f) return -1;
    Block last; memset(&last, 0, sizeof(last));
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        Block tmp;
        char prev[65], hash[65], dat[MAX_DATA];
        unsigned long idx, ts;
        if (sscanf(line, "%lu,%lu,%64[^,],%64[^,],%255[^\n]", &idx, &ts, prev, hash, dat) == 5) {
            tmp.index = idx; tmp.timestamp = ts;
            strcpy(tmp.prev_hash, prev); strcpy(tmp.hash, hash); strcpy(tmp.data, dat);
            last = tmp;
        }
    }
    fclose(f);

    Block b;
    b.index = last.index + 1;
    b.timestamp = (unsigned long)time(NULL);
    strncpy(b.prev_hash, last.hash, sizeof(b.prev_hash));
    strncpy(b.data, data, sizeof(b.data)-1);
    b.data[sizeof(b.data)-1] = '\0';
    compute_block_hash(&b, b.hash);

    if (block_to_file(chain_path, &b) != 0) return -1;

    FILE *l = fopen(log_path, "a");
    if (l) { fprintf(l, "[Append] idx=%lu prev=%s hash=%s data=%s\n", b.index, b.prev_hash, b.hash, b.data); fclose(l); }
    return 0;
}
