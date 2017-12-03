#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[]) {
    int w, h;
    for (w=10; w<=400; w+=10) {
        for (h=10; h<=400; h+=10) {
            char cmd[1024];
            sprintf(cmd, "./test-bitblt -w %d -h %d -f 0 -t 3000", w, h);
            printf("%s\n", cmd);
        }
    }

    for (w=10; w<=400; w+=10) {
        for (h=10; h<=400; h+=10) {
            char cmd[1024];
            sprintf(cmd, "./test-bitblt -w %d -h %d -f 0 -t 3000 -s", w, h);
            printf("%s\n", cmd);
        }
    }

    return 0;
}
