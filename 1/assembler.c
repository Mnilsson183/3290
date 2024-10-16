#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char* convert(const char[] instrBuf, const char[] destBuf, const char[] srcBuf1, const char[] srcBuf2) {
    if (!strcmp("mov", ))
}

char* parseLine(char* line) {
    int line_len = strlen(line);
    if (line_len == 0) return NULL;
    if (line[1] == '\0') return NULL;
    // first word
    char instrBuf[32];
    int idx = 0;
    while(idx < sizeof(instrBuf) && idx < line_len && line[idx] != '\0') {
        instrBuf[idx] = line[idx];
        instrBuf[idx + 1] = '\0';
        idx++;
    }
    char destBuf[256];
    while(idx < sizeof(destBuf) && idx < line_len && line[idx] != '\0') {
        destBuf[idx] = line[idx];
        destBuf[idx + 1] = '\0';
        idx++;
    }
    char srcBuf1[256];
    while(idx < sizeof(srcBuf1) && idx < line_len && line[idx] != '\0') {
        srcBuf1[idx] = line[idx];
        srcBuf1[idx + 1] = '\0';
        idx++;
    }
    char srcBuf2[256];
    while(idx < sizeof(srcBuf2) && idx < line_len && line[idx] != '\0') {
        srcBuf2[idx] = line[idx];
        srcBuf2[idx + 1] = '\0';
        idx++;
    }
    return 
}

int main(int argc, char** args) {
    if (argc != 2) {
        printf("Please pass a file\n");
        return 1;
    }

    FILE* src = fopen(args[1], "r");
    if (src == NULL) {
        printf("Cannot open File %s\n", args[1]);
        return -1;
    }

    FILE* dest = fopen("out", "w");
    if (dest == NULL) {
        printf("Cannot open File\n");
        return -1;
    }

    char line[256];
    while(fgets(line, sizeof(line), src)) {
        if (line[0] == '\0') continue;
        char* buf = line;


        fwrite(buf, sizeof(buf), 1, dest);
        fwrite("\n", sizeof("\n"), 1, dest);
    }
    fclose(src);
    fclose(dest);
}
