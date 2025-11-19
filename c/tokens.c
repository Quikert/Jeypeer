#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int is_delim(char c) { return c==' ' || c=='|' || c==',' || c=='\n' || c=='\t' || c=='='; }

int tokenize_to_log(const char *str, const char *log_path) {
    FILE *l = fopen(log_path, "a"); if (!l) return -1;
    const char *p = str; char tok[128]; int ti = 0;
    while (*p) {
        if (!is_delim(*p)) {
            tok[ti++] = (char)tolower((unsigned char)*p);
            if (ti >= 127) { tok[ti] = '\0'; fprintf(l, "[Token] %s\n", tok); ti = 0; }
        } else {
            if (ti > 0) { tok[ti] = '\0'; fprintf(l, "[Token] %s\n", tok); ti = 0; }
        }
        p++;
    }
if (ti > 0) { tok[ti] = '\0'; fprintf(l, "[Token] %s\n", tok); }
    fclose(l); return 0;
}
