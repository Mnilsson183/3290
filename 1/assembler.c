#include <stdlib.h>
#include <stdio.h>

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
