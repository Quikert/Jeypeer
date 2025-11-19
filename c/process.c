#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int block_genesis(const char *chain_path, const char *log_path);
int block_append(const char *chain_path, const char *log_path, const char *data);
int verify_chain(const char *chain_path);
int peers_add(const char *path, const char *peer);
int peers_list(const char *path);
int peers_remove(const char *path, const char *peer);
int tokenize_to_log(const char *str, const char *log_path);
int p2p_server(unsigned short port);
int p2p_client(const char *host, unsigned short port, const char *payload);

static const char *CHAIN_FILE = "logs/chain.csv";
static const char *BLOCK_LOG  = "logs/blocks.log";
static const char *PROC_LOG   = "logs/process.log";
static const char *TOK_LOG    = "logs/tokens.log";
static const char *PEERS_FILE = "logs/peers.txt";

static void usage() {
    printf("+ Jeypeer\n");
    printf("│ \n");
    printf("└─ Welcome help menu \n\n");
    printf("+ Generics\n");
    printf("  help\n");
    printf("  init\n");
    printf("  add-block \"DATA\"\n");
    printf("  list\n");
    printf("  verify\n");
    printf("+ Peers\n");
    printf("  peers add <addr>\n");
    printf("  peers rm <addr>\n");
    printf("  peers ls\n");
    printf("+ Token and server\n");
    printf("  tokens \"EVENT\"\n");
    printf("  p2p server <port>\n");
    printf("  p2p send <host> <port> \"DATA\"\n");
    printf("  clean\n");
    printf("│ \n");
    printf("└─ End.\n");
}

static void list_chain() {
    FILE *f = fopen(CHAIN_FILE, "r");
    if (!f) { printf("No chain\n"); return; }
    char line[1024];
    while (fgets(line, sizeof(line), f)) { printf("%s", line); }
    fclose(f);
}

static void log_proc(const char *msg) {
    FILE *l = fopen(PROC_LOG, "a");
    if (l) { fprintf(l, "%s\n", msg); fclose(l); }
}

static int overwrite_file(const char *path) {
    FILE *f = fopen(path, "r+");
    if (!f) return -1;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -2; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return -3; }
    rewind(f);
    for (long i = 0; i < sz; i++) fputc(0x00, f);
    fflush(f);
    fclose(f);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) { usage(); return 0; }
    if (strcmp(argv[1], "help") == 0) { usage(); return 0; }

    if (strcmp(argv[1], "init") == 0) {
        FILE *f = fopen(CHAIN_FILE, "r");
        if (!f) {
            if (block_genesis(CHAIN_FILE, BLOCK_LOG) == 0) { log_proc("[Init] Genesis = ok"); printf("Genesis created\n"); }
            else { printf("Init failed\n"); }
        } else { fclose(f); printf("[Genesis] Chain exists\n"); }
        return 0;
    }

    if (strcmp(argv[1], "add-block") == 0) {
        if (argc < 3) { printf("missing data\n"); return 1; }
        if (block_append(CHAIN_FILE, BLOCK_LOG, argv[2]) == 0) {
            char buf[256]; snprintf(buf, sizeof(buf), "[Append] %s", argv[2]);
            log_proc(buf); printf("Block appended\n");
        } else { printf("Append failed\n"); }
        return 0;
    }

    if (strcmp(argv[1], "list") == 0) { list_chain(); return 0; }

    if (strcmp(argv[1], "verify") == 0) {
        int rc = verify_chain(CHAIN_FILE);
        printf("verify: %s\n", rc==0 ? "OK" : "BROKEN");
        return rc==0 ? 0 : 1;
    }

    if (strcmp(argv[1], "peers") == 0) {
        if (argc < 3) { printf("Peers subcmd\n"); return 1; }
        if (strcmp(argv[2], "add") == 0 && argc >= 4) { int rc = peers_add(PEERS_FILE, argv[3]); printf(rc==0 ? "peer added\n" : "peer add failed\n"); return 0; }
        if (strcmp(argv[2], "rm") == 0 && argc >= 4) { int rc = peers_remove(PEERS_FILE, argv[3]); printf(rc==0 ? "peer removed\n" : "peer rm failed\n"); return 0; }
        if (strcmp(argv[2], "ls") == 0) { peers_list(PEERS_FILE); return 0; }
        printf("Unknown peers subcmd\n"); return 1;
    }

    if (strcmp(argv[1], "tokens") == 0) {
        if (argc < 3) { printf("Missing event\n"); return 1; }
        int rc = tokenize_to_log(argv[2], TOK_LOG);
        printf(rc==0 ? "Tokenized\n" : "Tokenize failed\n");
        return 0;
    }

    if (strcmp(argv[1], "p2p") == 0) {
        if (argc < 3) { printf("P2P subcmd\n"); return 1; }
        if (strcmp(argv[2], "server") == 0 && argc >= 4) {
            unsigned short port = (unsigned short)atoi(argv[3]);
            return p2p_server(port);
        }
        if (strcmp(argv[2], "send") == 0 && argc >= 6) {
            const char *host = argv[3]; unsigned short port = (unsigned short)atoi(argv[4]);
            const char *data = argv[5];
            return p2p_client(host, port, data);
        }
        printf("Unknown P2P subcmd\n"); return 1;
    }

    if (strcmp(argv[1], "clean") == 0) {

        char ans1[8], ans2[8];
        printf("This will overwrite and remove local files. Type YES: ");
        if (scanf("%7s", ans1) != 1 || strcmp(ans1, "YES") != 0) { printf("Cancel\n"); return 0; }
        printf("Please confirm again by typing YES.: ");
        if (scanf("%7s", ans2) != 1 || strcmp(ans2, "YES") != 0) { printf("Cancel\n"); return 0; }

        overwrite_file("logs/chain.csv"); remove("logs/chain.csv");
        overwrite_file("logs/peers.txt"); remove("logs/peers.txt");
        overwrite_file("logs/blocks.log"); remove("logs/blocks.log");
        overwrite_file("logs/process.log"); remove("logs/process.log");
        overwrite_file("logs/tokens.log"); remove("logs/tokens.log");
        printf("Nuked!\n");
        return 0;
    }

    printf("Not found command\n");
    return 1;
}
