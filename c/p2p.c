#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int block_append(const char *chain_path, const char *log_path, const char *data);

static const char *CHAIN_FILE = "logs/chain.csv";
static const char *BLOCK_LOG  = "logs/blocks.log";

int p2p_server(unsigned short port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return -1; }
    int opt = 1; setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr; memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(port);
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); close(s); return -2; }
    if (listen(s, 8) < 0) { perror("listen"); close(s); return -3; }

    printf("[P2P] listening on: %u\n", port);

    while (1) {
        int c = accept(s, NULL, NULL);
        if (c < 0) { perror("accept"); continue; }
        char buf[512]; int n = read(c, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            if (strncmp(buf, "APPEND,", 7) == 0) {
                const char *data = buf + 7;
                int rc = block_append(CHAIN_FILE, BLOCK_LOG, data);
                if (rc == 0) write(c, "OK\n", 3);
                else write(c, "ERR\n", 4);
            } else {
                write(c, "UNKNOWN\n", 8);
            }
        }
        close(c);
    }
    close(s);
    return 0;
}

int p2p_client(const char *host, unsigned short port, const char *payload) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return -1; }
    struct sockaddr_in addr; memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) { perror("inet_pton"); close(s); return -2; }
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("connect"); close(s); return -3; }

    char msg[512]; snprintf(msg, sizeof(msg), "APPEND,%s", payload);
    write(s, msg, strlen(msg));
    char resp[64]; int n = read(s, resp, sizeof(resp)-1);
    if (n > 0) { resp[n] = '\0'; printf("[P2P] Resp: %s", resp); }
    close(s);
    return 0;
}
